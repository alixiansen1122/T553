/*
 * Copyright (c) CompanyNameMagicTag. 2019-2020. All rights reserved.
 * Description: implementation for mbox, ticks and memory adaptor
 * Author: none
 * Create: 2020
 */

#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
#include "arch/sys_arch.h"
#include "string.h"
#include "stdlib.h"
#if LWIP_LITEOS_COMPAT
#include "los_sem.h"
#include "los_mux.h"
#include "los_config.h"
#include "pthread.h"
#include "linux/wait.h"
#endif

#define SYS_ARCH_ERROR 0xffffffffU

#if !LWIP_FREERTOS_COMPAT
#ifndef UNUSED
#define UNUSED(a) ((void)(a))
#endif

#define LWIP_SEM_ID(x) (u32_t)(uintptr_t)((x)->sem)

#define MBOX_EXPAND_MULTIPLE_SIZE   2
#if LWIP_LITEOS_COMPAT
#if defined(LOSCFG_KERNEL_SMP) && defined(YES) && (LOSCFG_KERNEL_SMP == YES)
SPIN_LOCK_INIT(arch_protect_spin);
static u32_t lwprot_thread = LOS_ERRNO_TSK_ID_INVALID;
static int lwprot_count = 0;
#endif /* LOSCFG_KERNEL_SMP == YES */
#else
static pthread_mutex_t lwprot_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t lwprot_thread = (pthread_t)0xDEAD;
static int lwprot_count = 0;
#endif /* LWIP_LITEOS_COMPAT */

err_t sys_mbox_new_ext(struct sys_mbox **mb, int size, unsigned char is_auto_expand)
{
  struct sys_mbox *mbox = NULL;
  int ret;

  if (size <= 0) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_new: mbox size must bigger than 0\n"));
    return ERR_VAL;
  }

  mbox = (struct sys_mbox *)mem_malloc(sizeof(struct sys_mbox));
  if (mbox == NULL) {
    goto err_handler;
  }

  (void)memset_s(mbox, sizeof(struct sys_mbox), 0, sizeof(struct sys_mbox));

  mbox->msgs = (void **)mem_malloc(sizeof(void *) * size);
  if (mbox->msgs == NULL) {
    goto err_handler;
  }

  (void)memset_s(mbox->msgs, (sizeof(void *) * size), 0, (sizeof(void*) * size));

  mbox->mbox_size = size;

  mbox->first = 0;
  mbox->last = 0;
  mbox->is_full = 0;
  mbox->is_empty = 1;
  mbox->is_autoexpand = is_auto_expand;

  ret = pthread_mutex_init(&(mbox->mutex), NULL);
  if (ret != 0) {
    goto err_handler;
  }

  ret = pthread_cond_init(&(mbox->not_empty), NULL);
  if (ret != 0) {
    (void)pthread_mutex_destroy(&(mbox->mutex));
    goto err_handler;
  }

  ret = pthread_cond_init(&(mbox->not_full), NULL);
  if (ret != 0) {
    (void)pthread_mutex_destroy(&(mbox->mutex));
    (void)pthread_cond_destroy(&(mbox->not_empty));
    goto err_handler;
  }

  SYS_STATS_INC_USED(mbox);
  *mb = mbox;
  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_new: mbox created successfully 0x%p\n", (void *)mbox));
  return ERR_OK;

err_handler:
  if (mbox != NULL) {
    if (mbox->msgs != NULL) {
      mem_free(mbox->msgs);
      mbox->msgs = NULL;
    }
    mem_free(mbox);
  }
  return ERR_MEM;
}

/*-----------------------------------------------------------------------------------*/
void sys_mbox_free(struct sys_mbox **mb)
{
  if ((mb != NULL) && (*mb != SYS_MBOX_NULL)) {
    struct sys_mbox *mbox = *mb;
    int ret;
    SYS_STATS_DEC(mbox.used);

    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_free: going to free mbox 0x%p\n", (void *)mbox));

    ret = pthread_mutex_lock(&(mbox->mutex));
    if (ret != 0) {
      return;
    }

    (void)pthread_cond_destroy(&(mbox->not_empty));
    (void)pthread_cond_destroy(&(mbox->not_full));

    (void)pthread_mutex_unlock(&(mbox->mutex));

    (void)pthread_mutex_destroy(&(mbox->mutex));

    mem_free(mbox->msgs);
    mbox->msgs = NULL;
    mem_free(mbox);
    *mb = NULL;

    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_free: freed mbox\n"));
  }
}

static unsigned char
sys_mbox_auto_expand(struct sys_mbox *mbox)
{
  unsigned char is_expanded = 0;
  void **realloc_msgs = NULL;
  if ((mbox->is_autoexpand == MBOX_AUTO_EXPAND) &&
      (MBOX_EXPAND_MULTIPLE_SIZE * (u32_t)mbox->mbox_size) <= MAX_MBOX_SIZE) {
    realloc_msgs = mem_malloc(MBOX_EXPAND_MULTIPLE_SIZE * sizeof(void *) * (u32_t)mbox->mbox_size);
    if (realloc_msgs != NULL) {
      /* memcpy_s */
      if (mbox->first > 0) {
        (void)memcpy_s(realloc_msgs,
                       sizeof(void *) * (mbox->mbox_size - mbox->first),
                       mbox->msgs + mbox->first,
                       sizeof(void *) * (mbox->mbox_size - mbox->first));
        (void)memcpy_s(realloc_msgs + (mbox->mbox_size - mbox->first),
                       sizeof(void *) * (mbox->last),
                       mbox->msgs,
                       sizeof(void *) * (mbox->last));
      } else {
        (void)memcpy_s(realloc_msgs, sizeof(void *) * mbox->mbox_size,
                       mbox->msgs, sizeof(void *) * mbox->mbox_size);
      }
      mem_free(mbox->msgs);
      mbox->msgs = realloc_msgs;
      mbox->first = 0;
      mbox->last = mbox->mbox_size;
      mbox->mbox_size *= MBOX_EXPAND_MULTIPLE_SIZE;
      LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post : mbox 0x%p is auto expanded\n", (void *)mbox));
      is_expanded = 1;
    }
  }
  return is_expanded;
}

/*
 * Routine:  sys_mbox_post
 *
 * Description:
 *      Post the "msg" to the mailbox.
 * Inputs:
 *      sys_mbox_t mbox        -- Handle of mailbox
 *      void *msg              -- Pointer to data to post
 */
void sys_mbox_post(struct sys_mbox **mb, void *msg)
{
  struct sys_mbox *mbox = NULL;
  int ret;
  if ((mb == NULL) || (*mb == NULL)) {
    return;
  }
  mbox = *mb;
  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post: mbox 0x%p msg 0x%p\n", (void *)mbox, (void *)msg));

  ret = pthread_mutex_lock(&(mbox->mutex));
  if (ret != 0) {
    return;
  }

  while (mbox->is_full == 1) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post : mbox 0x%p mbox 0x%p, queue is full\n", (void *)mbox, (void *)msg));
    ret = pthread_cond_wait(&(mbox->not_full), &(mbox->mutex));
    if (ret != 0) {
      (void)pthread_mutex_unlock(&(mbox->mutex));
      return;
    }
  }

  mbox->msgs[mbox->last] = msg;

  mbox->last++;
  if (mbox->last == mbox->mbox_size) {
    mbox->last = 0;
  }

  if (mbox->first == mbox->last) {
    if (sys_mbox_auto_expand(mbox) == 0) {
      mbox->is_full = 1;
    }
  }

  if (mbox->is_empty == 1) {
    mbox->is_empty = 0;
    (void)pthread_cond_signal(&(mbox->not_empty)); /* if signal failed, anyway it will unlock and go out */
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post : mbox 0x%p msg 0x%p, signalling not empty\n", (void *)mbox, (void *)msg));
  }
  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post: mbox 0x%p msg 0%p posted\n", (void *)mbox, (void *)msg));
  (void)pthread_mutex_unlock(&(mbox->mutex));
}

/*
 * Routine:  sys_mbox_trypost
 *
 * Description:
 *      Try to post the "msg" to the mailbox.  Returns immediately with
 *      error if cannot.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void *msg               -- Pointer to data to post
 * Outputs:
 *      err_t                   -- ERR_OK if message posted, else ERR_MEM
 *                                  if not.
 */
err_t sys_mbox_trypost(struct sys_mbox **mb, void *msg)
{
  struct sys_mbox *mbox = NULL;
  int ret;
  mbox = *mb;

  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox 0x%p msg 0x%p \n", (void *)mbox, (void *)msg));

  ret = pthread_mutex_lock(&(mbox->mutex));
  if (ret != 0) {
    return ERR_MEM;
  }

  if (mbox->is_full == 1) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost : mbox 0x%p msgx 0x%p,queue is full\n", (void *)mbox, (void *)msg));
    (void)pthread_mutex_unlock(&(mbox->mutex));
    return ERR_MEM;
  }

  mbox->msgs[mbox->last] = msg;

  mbox->last = (mbox->mbox_size - 1 - mbox->last) ? (mbox->last + 1) : 0;

  if (mbox->first == mbox->last) {
    if (sys_mbox_auto_expand(mbox) == 0) {
      mbox->is_full = 1;
    }
  }

  if (mbox->is_empty) {
    mbox->is_empty = 0;
    (void)pthread_cond_signal(&(mbox->not_empty));
  }
  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox 0x%p msg 0x%p posted\n", (void *)mbox, (void *)msg));
  (void)pthread_mutex_unlock(&(mbox->mutex));
  return ERR_OK;
}

static u32_t
sys_arch_mbox_handler_timeouts(struct sys_mbox *mbox, u32_t timeout, u8_t ignore_timeout)
{
  struct timespec tmsp;
  int ret;
#if LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT
  struct timeval tv;
#endif /* LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT */

  if (mbox->is_empty && ignore_timeout) {
    return SYS_ARCH_TIMEOUT;
  }

  while ((mbox->is_empty == 1) && (ignore_timeout == 0)) {
    if (timeout != 0) {
#if LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT
      gettimeofday(&tv, NULL);
      tmsp.tv_sec = tv.tv_sec + (timeout / MS_PER_SECOND);
      tmsp.tv_nsec = (tv.tv_usec * NS_PER_USECOND) + ((timeout % US_PER_MSECOND) * US_PER_MSECOND * NS_PER_USECOND);
      tmsp.tv_sec += tmsp.tv_nsec / (NS_PER_USECOND * US_PER_MSECOND * MS_PER_SECOND);
      tmsp.tv_nsec %= (NS_PER_USECOND * US_PER_MSECOND * MS_PER_SECOND);
#else
      tmsp.tv_sec = (timeout / MS_PER_SECOND);
      tmsp.tv_nsec = ((timeout % US_PER_MSECOND) * US_PER_MSECOND * NS_PER_USECOND);
#endif

      ret = pthread_cond_timedwait(&(mbox->not_empty), &(mbox->mutex), &tmsp);
      if (ret != 0) {
        LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: mbox 0x%p,timeout in cond wait\n", (void *)mbox));
        return SYS_ARCH_TIMEOUT;
      }
    } else {
      LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: cond wait\n"));
      ret = pthread_cond_wait(&(mbox->not_empty), &(mbox->mutex));
      if (ret != 0) {
        return SYS_ARCH_TIMEOUT;
      }
    }
  }
  return ERR_OK;
}

u32_t sys_arch_mbox_fetch_ext(struct sys_mbox **mb, void **msg, u32_t timeout, u8_t ignore_timeout)
{
  struct sys_mbox *mbox = NULL;
#if LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT
  struct timeval tv;
#endif /* LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT */
  int ret;
  mbox = *mb;

  LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: mbox 0x%p msg 0x%p\n", (void *)mbox, (void *)msg));

  /* The mutex lock is quick so we don't bother with the timeout stuff here. */
  ret = pthread_mutex_lock(&(mbox->mutex));
  if (ret != 0) {
    return SYS_ARCH_TIMEOUT;
  }
  u32_t timeouts = sys_arch_mbox_handler_timeouts(mbox, timeout, ignore_timeout);
  if (timeouts != ERR_OK) {
    (void)pthread_mutex_unlock(&(mbox->mutex));
    return SYS_ARCH_TIMEOUT;
  }

  if (msg != NULL) {
    *msg = mbox->msgs[mbox->first];
    LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: mbox 0x%p msg 0x%p\n", (void *)mbox, (void *)*msg));
  } else {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: mbox 0x%p, null msg\n", (void *)mbox));
  }

  mbox->first = (mbox->mbox_size - 1 - mbox->first) ? (mbox->first + 1) : 0;

  if (mbox->first == mbox->last) {
    mbox->is_empty = 1;
  }

  if (mbox->is_full) {
    mbox->is_full = 0;
    (void)pthread_cond_signal(&(mbox->not_full));
  }
  LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: mbox 0x%p msg 0x%p fetched\n", (void *)mbox, (void *)msg));
  (void)pthread_mutex_unlock(&(mbox->mutex));

  return 0;
}

/*
 * Routine:  sys_init
 *
 * Description:
 *      Initialize sys arch
 */
void sys_init(void)
{
  /* set rand seed to make random sequence diff on every startup */
  extern VOID LOS_GetCpuCycle(UINT32 *puwCntHi, UINT32 *puwCntLo);
  u32_t seedhsb, seedlsb;
  LOS_GetCpuCycle(&seedhsb, &seedlsb);
  srand(seedlsb);
}


/*
 * Routine:  sys_arch_protect
 *
 * Description:
 *      This optional function does a "fast" critical region protection and
 *      returns the previous protection level. This function is only called
 *      during very short critical regions. An embedded system which supports
 *      ISR-based drivers might want to implement this function by disabling
 *      interrupts. Task-based systems might want to implement this by using
 *      a mutex or disabling tasking. This function should support recursive
 *      calls from the same task or interrupt. In other words,
 *      sys_arch_protect() could be called while already protected. In
 *      that case the return value indicates that it is already protected.
 *
 *      sys_arch_protect() is only required if your port is supporting an
 *      OS.
 * Outputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 */
sys_prot_t sys_arch_protect(void)
{
#if LWIP_LITEOS_COMPAT
#if defined(LOSCFG_KERNEL_SMP) && defined(YES) && (LOSCFG_KERNEL_SMP == YES)
  /* Note that we are using spinlock instead of mutex for LiteOS-SMP here:
   * 1. spinlock is more effective for short critical region protection.
   * 2. this function is called only in task context, not in interrupt handler.
   *    so it's not needed to disable interrupt.
   */
  if (lwprot_thread != LOS_CurTaskIDGet()) {
    /* We are locking the spinlock where it has not been locked before
     * or is being locked by another thread */
    LOS_SpinLock(&arch_protect_spin);
    lwprot_thread = LOS_CurTaskIDGet();
    lwprot_count = 1;
  } else {
    /* It is already locked by THIS thread */
    lwprot_count++;
  }
#else
  LOS_TaskLock();
#endif /* LOSCFG_KERNEL_SMP == YES */
#else
  int ret;

  /* Note that for the UNIX port, we are using a lightweight mutex, and our
   * own counter (which is locked by the mutex). The return code is not actually
   * used. */
  if (lwprot_thread != pthread_self()) {
    /* We are locking the mutex where it has not been locked before *
    * or is being locked by another thread */
    ret = pthread_mutex_lock(&lwprot_mutex);
    LWIP_ASSERT("pthread_mutex_lock failed", (ret == 0));
    lwprot_thread = pthread_self();
    lwprot_count = 1;
  } else {
    /* It is already locked by THIS thread */
    lwprot_count++;
  }
#endif /* LWIP_LITEOS_COMPAT */

    return 0;
}


/*
 * Routine:  sys_arch_unprotect
 *
 * Description:
 *      This optional function does a "fast" set of critical region
 *      protection to the value specified by pval. See the documentation for
 *      sys_arch_protect() for more information. This function is only
 *      required if your port is supporting an OS.
 * Inputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 */
void sys_arch_unprotect(sys_prot_t pval)
{
#if LWIP_LITEOS_COMPAT
  LWIP_UNUSED_ARG(pval);
#if defined(LOSCFG_KERNEL_SMP) && defined(YES) && (LOSCFG_KERNEL_SMP == YES)
  if (lwprot_thread == LOS_CurTaskIDGet()) {
    lwprot_count--;
    if (lwprot_count == 0) {
      lwprot_thread = LOS_ERRNO_TSK_ID_INVALID;
      LOS_SpinUnlock(&arch_protect_spin);
    }
  }
#else
  LOS_TaskUnlock();
#endif /* LOSCFG_KERNEL_SMP == YES */
#else
  int ret;

  LWIP_UNUSED_ARG(pval);
  if (lwprot_thread == pthread_self()) {
    if (--lwprot_count == 0) {
      lwprot_thread = (pthread_t)0xDEAD;
      ret = pthread_mutex_unlock(&lwprot_mutex);
      LWIP_ASSERT("pthread_mutex_unlock failed", (ret == 0));
      LWIP_UNUSED_ARG(ret);
    }
  }
#endif
}

u32_t sys_now(void)
{
    /* Lwip docs mentioned like wraparound is not a problem in this funtion */
    return (u32_t)((LOS_TickCountGet() * 1000) / LOSCFG_BASE_CORE_TICK_PER_SECOND);
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
{
  TSK_INIT_PARAM_S task;
  UINT32 taskid, ret;
  (void)memset_s(&task, sizeof(task), 0, sizeof(task));

  /* Create host Task */
  task.pfnTaskEntry = (TSK_ENTRY_FUNC)function;
  task.uwStackSize  = 4096;
  task.pcName = (char *)name;
  task.usTaskPrio = prio;
  task.uwResved   = LOS_TASK_STATUS_DETACHED;
#ifdef BR_HW_LITEOS_VERSION
  LOS_TASK_PARAM_INIT_ARG(task, arg);
#else
  task.auwArgs[0] = (UINTPTR)arg;
#endif
  ret = LOS_TaskCreate(&taskid, &task);
  if (ret != OS_OK) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_thread_new: LOS_TaskCreate error %u\n", ret));
    return SYS_ARCH_ERROR;
  }
  (void)stacksize;
  return taskid;
}

#ifdef LWIP_DEBUG
/* \brief  Displays an error message on assertion

    This function will display an error message on an assertion
    to the dbg output.

    \param[in]    msg   Error message to display
    \param[in]    line  Line number in file with error
    \param[in]    file  Filename with error
 */
void assert_printf(char *msg, int line, char *file)
{
  if (msg != NULL) {
    LWIP_DEBUGF(LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("%s:%d in file %s", msg, line, file));
    return;
  } else {
    LWIP_DEBUGF(LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("LWIP ASSERT"));
    return;
  }
}
#endif /* LWIP_DEBUG */


/*
 * Routine:  sys_sem_new
 *
 * Description:
 *      Creates and returns a new semaphore. The "ucCount" argument specifies
 *      the initial state of the semaphore.
 *      NOTE: Currently this routine only creates counts of 1 or 0
 * Inputs:
 *      sys_sem_t sem         -- Handle of semaphore
 *      u8_t count            -- Initial count of semaphore
 * Outputs:
 *      err_t                 -- ERR_OK if semaphore created
 */
err_t sys_sem_new(sys_sem_t *sem,  u8_t count)
{
  u32_t sem_handle;
  u32_t ret;

  if (sem == NULL) {
    return -1;
  }

  LWIP_ASSERT("in sys_sem_new count exceeds the limit", (count < 0xFF));

  ret = LOS_SemCreate(count, &sem_handle);
  if (ret != ERR_OK) {
    return -1;
  }

  sem->sem = (void *)(uintptr_t)(sem_handle);

  return ERR_OK;
}

/*
 * Routine:  sys_arch_sem_wait
 *
 * Description:
 *      Blocks the thread while waiting for the semaphore to be
 *      signaled. If the "timeout" argument is non-zero, the thread should
 *      only be blocked for the specified time (measured in
 *      milliseconds).
 *
 *      If the timeout argument is non-zero, the return value is the number of
 *      milliseconds spent waiting for the semaphore to be signaled. If the
 *      semaphore wasn't signaled within the specified time, the return value is
 *      SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
 *      (i.e., it was already signaled), the function may return zero.
 *
 *      Notice that lwIP implements a function with a similar name,
 *      sys_sem_wait(), that uses the sys_arch_sem_wait() function.
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to wait on
 *      u32_t timeout           -- Number of milliseconds until timeout
 * Outputs:
 *      u32_t                   -- Time elapsed or SYS_ARCH_TIMEOUT.
 */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  u32_t retval;
  u64_t start_tick;
  u64_t end_tick;

  if ((sem == NULL) || (sem->sem == NULL)) {
    return SYS_ARCH_ERROR;
  }

  start_tick = LOS_TickCountGet();

  if (timeout == 0) {
    timeout = LOS_WAIT_FOREVER;
  } else {
    timeout = LOS_MS2Tick(timeout);
    if (!timeout) {
      timeout = 1;
    }
  }
  retval = LOS_SemPend(LWIP_SEM_ID(sem), timeout);
  if (retval == LOS_ERRNO_SEM_TIMEOUT) {
    return SYS_ARCH_TIMEOUT;
  }
  if (retval != ERR_OK) {
    return SYS_ARCH_ERROR;
  }

  end_tick = LOS_TickCountGet();
  /* Here milli second will not come more than 32 bit because timeout received as 32 bit millisecond only */
  return (u32_t)(((end_tick - start_tick) * 1000) / LOSCFG_BASE_CORE_TICK_PER_SECOND);
}


/*
 * Routine:  sys_sem_signal
 *
 * Description:
 *      Signals (releases) a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to signal
 */
void sys_sem_signal(sys_sem_t *sem)
{
  u32_t ret;

  if ((sem == NULL) || (sem->sem == NULL)) {
    return;
  }

  ret = LOS_SemPost(LWIP_SEM_ID(sem));
  LWIP_ASSERT("LOS_SemPost failed", (ret == 0));
  UNUSED(ret);

  return;
}


/*
 * Routine:  sys_sem_free
 *
 * Description:
 *      Deallocates a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to free
 */
void sys_sem_free(sys_sem_t *sem)
{
  u32_t ret;

  if ((sem == NULL) || (sem->sem == NULL)) {
    return;
  }

  ret = LOS_SemDelete(LWIP_SEM_ID(sem));
  sem->sem = NULL;
  LWIP_ASSERT("LOS_SemDelete failed", (ret == 0));
  UNUSED(ret);
  return;
}

#if !LWIP_COMPAT_MUTEX
/* Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
  u32_t ret;
#ifdef LWIP_LITEOS_A_COMPAT
  ret = pthread_mutex_init(mutex, NULL);
#else
  ret = LOS_MuxCreate(mutex);
#endif
  LWIP_ASSERT("sys_mutex_new failed", (ret == OS_OK));
  return ret;
}
/* Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock(sys_mutex_t *mutex)
{
  u32_t ret;
#ifdef LWIP_LITEOS_A_COMPAT
  ret = pthread_mutex_lock(mutex);
#else
  ret = LOS_MuxPend(*mutex, LOS_WAIT_FOREVER);
#endif
  LWIP_ASSERT("sys_mutex_lock failed", (ret == OS_OK));
  UNUSED(ret);
  return;
}
/* Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
  u32_t ret;
#ifdef LWIP_LITEOS_A_COMPAT
  ret = pthread_mutex_unlock(mutex);
#else
  ret = LOS_MuxPost(*mutex);
#endif
  LWIP_ASSERT("sys_mutex_unlock failed", (ret == OS_OK));
  UNUSED(ret);
  return;
}
/* Delete a semaphore
 * @param mutex the mutex to delete */
void sys_mutex_free(sys_mutex_t *mutex)
{
  u32_t ret;
#ifdef LWIP_LITEOS_A_COMPAT
  ret = pthread_mutex_destroy(mutex);
#else
  ret = LOS_MuxDelete(*mutex);
#endif
  LWIP_ASSERT("sys_mutex_free failed", (ret == OS_OK));
  UNUSED(ret);
  return;
}
#endif

#else /* LWIP_FREERTOS_COMPAT */
#include <time.h>
#define  NANO_TO_MS   (1000 * 1000)
#define  SEC_TO_MS     1000

/* Mailbox functions. */
/**  Create a new mbox of specified size
 *   @param  mbox pointer to the mbox to create
 *   @param  size (miminum) number of messages in this mbox
 *   @return ERR_OK if successful, another err_t otherwise
 */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  if (size == 0) {
    SYS_STATS_INC(mbox.err);
    return ERR_VAL;
  }

  *mbox = osMessageQueueNew(size, sizeof(void*), NULL);
  if (!*mbox) {
    LWIP_DEBUGF(SYS_DEBUG, ("[%s]: Failed to created sys_mbox_t of size %d\n", __FUNCTION__, size));
    SYS_STATS_INC(mbox.err);
    return ERR_MEM;
  }

  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: sys_mbox_t 0x%08x created\n", __FUNCTION__, *mbox));
  SYS_STATS_INC_USED(mbox);
  return ERR_OK;
}

/** Delete an sys mbox
 * @param mbox mbox to delete
 */
void sys_mbox_free(sys_mbox_t *mbox)
{
  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: sys_mbox_t 0x%08x deleted\n", __FUNCTION__, *mbox));
  osMessageQueueDelete(*mbox);
  SYS_STATS_DEC(mbox.used);
  *mbox = NULL;
}

/** Post a message to an mbox - may not fail
 * -> blocks if full, only used from tasks not from ISR
 *    @param mbox mbox to posts the message
 *    @param msg message to post (ATTENTION: can be NULL)
 */
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: sys_mbox_t 0x%08x has 0x%08x posted\n", __FUNCTION__, *mbox, msg));
  while (osMessageQueuePut(*mbox, &msg, 0, osWaitForever) != osOK) {
    LWIP_DEBUGF(SYS_DEBUG, ("[%s]: sys_mbox_t 0x%08x has 0x%08x posted (timed out, trying again)\n",
        __FUNCTION__, *mbox, msg));
    SYS_STATS_INC(mbox.err);
  }
}

/** Try to post a message to an mbox - may fail if full or ISR
 *   @param mbox mbox to posts the message
 *   @param msg message to post (ATTENTION: can be NULL)
 */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  if (osMessageQueuePut(*mbox, &msg, 0, osNoWait) != osOK) {
      LWIP_DEBUGF(SYS_DEBUG, ("[%s]: sys_mbox_t 0x%08x posting 0x%08x would block\n", __FUNCTION__, *mbox, msg));
      SYS_STATS_INC(mbox.err);
      return ERR_WOULDBLOCK;
  }

  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: sys_mbox_t 0x%08x 0x%08x posted to it\n",__FUNCTION__, *mbox, msg));
  return ERR_OK;
}

/**
 * @ingroup sys_mbox
 * Blocks the thread until a message arrives in the mailbox, but does
 * not block the thread longer than "timeout" milliseconds (similar to
 * the sys_arch_sem_wait() function). If "timeout" is 0, the thread should
 * be blocked until a message arrives. The "msg" argument is a result
 * parameter that is set by the function (i.e., by doing "*msg =
 * ptr"). The "msg" parameter maybe NULL to indicate that the message
 * should be dropped.
 * The return values are the same as for the sys_arch_sem_wait() function:
 * SYS_ARCH_TIMEOUT if there was a timeout, any other value if a messages
 * is received.
 *
 * Note that a function with a similar name, sys_mbox_fetch(), is
 * implemented by lwIP.
 *
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message (0 = wait forever)
 * @return SYS_ARCH_TIMEOUT on timeout, any other value if a message has been received
 */
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  void *dummy = NULL;
  if (msg == NULL) {
    msg = &dummy;
  }

  osStatus_t status;
  u32_t time2tick;
  
  if (timeout) {
    LWIP_DEBUGF(SYS_DEBUG, ("[%s]: sys_mbox_t 0x%08x timed out %u ms\n",__FUNCTION__, *mbox, timeout));
    time2tick = (u32_t)osal_msecs_to_jiffies(timeout);
    if (!time2tick) {
      time2tick = 1;
    }
    
    status = osMessageQueueGet(*mbox, msg, 0, time2tick);
    if (status != osOK) {
      return SYS_ARCH_TIMEOUT;
    }
  } else {
    osMessageQueueGet(*mbox, msg, 0, osWaitForever);
  }

  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: sys_mbox_t 0x%08x, got message 0x%08x\n", __FUNCTION__,
    *mbox, *msg));

  /* don't care about the reality of the actual waiting */
  return 0;
}

/**
 * @ingroup sys_mbox
 * This is similar to sys_arch_mbox_fetch, however if a message is not
 * present in the mailbox, it immediately returns with the code
 * SYS_MBOX_EMPTY. On success 0 is returned.
 * To allow for efficient implementations, this can be defined as a
 * function-like macro in sys_arch.h instead of a normal function. For
 * example, a naive implementation could be:
 * \#define sys_arch_mbox_tryfetch(mbox,msg) sys_arch_mbox_fetch(mbox,msg,1)
 * although this would introduce unnecessary delays.
 *
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty
 */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  void *dummy = NULL;

  if (msg == NULL) {
    msg = &dummy;
  }
      
  if (osMessageQueueGet(*mbox, msg, 0, 0) != osOK) {
    LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: sys_mbox_t 0x%08x is empty.\n", __FUNCTION__, *mbox));
    return SYS_MBOX_EMPTY;
  }

  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: sys_mbox_t 0x%08x, got message 0x%08x\n",
      __FUNCTION__, *msg, *mbox));
  return 0;
}

/**
 * @ingroup sys_misc
 * sys_init() must be called before anything else.
 * Initialize the sys_arch layer.
 */
void sys_init(void)
{
  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]:enter now .\n", __FUNCTION__));

  /* Use System Tick as Seed for secure random number */
  u32_t seed = (u32_t)osKernelGetTickCount();
  srand(seed);
}

/*
 * Routine:  sys_arch_protect
 *
 * Description:
 *      This optional function does a "fast" critical region protection and
 *      returns the previous protection level. This function is only called
 *      during very short critical regions. An embedded system which supports
 *      ISR-based drivers might want to implement this function by disabling
 *      interrupts. Task-based systems might want to implement this by using
 *      a mutex or disabling tasking. This function should support recursive
 *      calls from the same task or interrupt. In other words,
 *      sys_arch_protect() could be called while already protected. In
 *      that case the return value indicates that it is already protected.
 *
 *      sys_arch_protect() is only required if your port is supporting an
 *      OS.
 * Outputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 */
sys_prot_t sys_arch_protect(void)
{
  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]\n", __FUNCTION__));
  osal_kthread_lock();    
  return 0;
}

/*
 * Routine:  sys_arch_unprotect
 *
 * Description:
 *      This optional function does a "fast" set of critical region
 *      protection to the value specified by pval. See the documentation for
 *      sys_arch_protect() for more information. This function is only
 *      required if your port is supporting an OS.
 * Inputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 */
void sys_arch_unprotect(sys_prot_t pval)
{
  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]\n", __FUNCTION__));
  LWIP_UNUSED_ARG(pval);
  osal_kthread_unlock();
}

/**
 * @ingroup sys_time
 * Returns the current time in milliseconds,
 * may be the same as sys_jiffies or at least based on it.
 * Don't care for wraparound, this is only used for time diffs.
 * Not implementing this function means you cannot use some modules (e.g. TCP
 * timestamps, internal timeouts for NO_SYS==1).
 */
u32_t sys_now(void)
{
  u32_t current_time = 0;
  struct timespec tp;
  if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0) {
    LWIP_DEBUGF(SYS_DEBUG, ("[%s]: clock_gettime failed.\n", __FUNCTION__));
  }

  current_time = (u32_t)((u64_t)tp.tv_sec * SEC_TO_MS + (u64_t)tp.tv_nsec / NANO_TO_MS);
  return current_time;
}

/**
 * @ingroup sys_misc
 * The only thread function:
 * Starts a new thread named "name" with priority "prio" that will begin its
 * execution in the function "thread()". The "arg" argument will be passed as an
 * argument to the thread() function. The stack size to used for this thread is
 * the "stacksize" parameter. The id of the new thread is returned. Both the id
 * and the priority are system dependent.
 * ATTENTION: although this function returns a value, it MUST NOT FAIL (ports have to assert this!)
 *
 * @param name human-readable name for the thread (used for debugging purposes)
 * @param thread thread-function
 * @param arg parameter passed to 'thread'
 * @param stacksize stack size in bytes for the new thread (may be ignored by ports)
 * @param prio priority of the new thread (may be ignored by ports) */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
  osThreadId_t thread_id;
  osThreadAttr_t attr;
  osThreadFunc_t func;
  if (memset_s(&attr, sizeof(osThreadAttr_t), 0, sizeof(osThreadAttr_t)) != EOK) {
      return NULL;
  }
  
  /* All the rest should be NULL or 0 - at the time of writing. */
  attr.name = name;
  attr.stack_size = stacksize;
  attr.priority = (osPriority_t) prio;
  
  func = thread;
  thread_id = osThreadNew(func, arg,  &attr);
  
  if (!thread_id) {
      LWIP_DEBUGF(SYS_DEBUG, ("[%s]: Failed to create thread for '%s'.\n", __FUNCTION__, name));
      return SYS_ARCH_ERROR;
  }
  
  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: Created thread 0x%08x name %s stacksize %d prio %d.\n", __FUNCTION__,
      thread_id, name, stacksize, prio));
  
  return (sys_thread_t)thread_id; 
}

/**
 * @ingroup sys_sem
 * Create a new semaphore
 * Creates a new semaphore. The semaphore is allocated to the memory that 'sem'
 * points to (which can be both a pointer or the actual OS structure).
 * The "count" argument specifies the initial state of the semaphore (which is
 * either 0 or 1).
 * If the semaphore has been created, ERR_OK should be returned. Returning any
 * other error will provide a hint what went wrong, but except for assertions,
 * no real error handling is implemented.
 *
 * @param sem pointer to the semaphore to create
 * @param count initial count of the semaphore
 * @return ERR_OK if successful, another err_t otherwise
 */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  /* Check that count is not larger than 1 */
  LWIP_ASSERT("sys_sem_new() count parameter too large.", count <= 1);
  *sem = osSemaphoreNew(1, count, NULL);
  if (!*sem) {
      LWIP_DEBUGF(SYS_DEBUG, ("%s(): Failed to create sys_sem_t (count = %u)\n", __FUNCTION__, count));
      SYS_STATS_INC(sem.err);
      return ERR_MEM;
  }

  return ERR_OK; 
}

/**
 * @ingroup sys_sem
 *  Blocks the thread while waiting for the semaphore to be signaled. If the
 * "timeout" argument is non-zero, the thread should only be blocked for the
 * specified time (measured in milliseconds). If the "timeout" argument is zero,
 * the thread should be blocked until the semaphore is signalled.
 *
 * The return value is SYS_ARCH_TIMEOUT if the semaphore wasn't signaled within
 * the specified time or any other value if it was signaled (with or without
 * waiting).
 * Notice that lwIP implements a function with a similar name,
 * sys_sem_wait(), that uses the sys_arch_sem_wait() function.
 *
 * @param sem the semaphore to wait for
 * @param timeout timeout in milliseconds to wait (0 = wait forever)
 * @return SYS_ARCH_TIMEOUT on timeout, any other value on success
 */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  u32_t ret;
  u64_t start_tick;
  u64_t end_tick;

  if (sem == NULL) {
    return SYS_ARCH_ERROR;
  }

  if(timeout == 0) {
    timeout = osWaitForever;
  } else {
    timeout = (u32_t)osal_msecs_to_jiffies(timeout);
    if (!timeout) {
      timeout = 1;
    }
  }
  
  start_tick = sys_now();
  
  osStatus_t status = osSemaphoreAcquire(*sem, timeout);
  if (status != osOK) {
    LWIP_DEBUGF(SYS_DEBUG, ("[%s]: Failed to Acquire sem %d.\n", __FUNCTION__, status));
    return SYS_ARCH_TIMEOUT;
  }

  end_tick = sys_now();

  /* Here milli second will not come more than 32 bit because timeout received as 32 bit millisecond only */
  return (u32_t)(end_tick - start_tick);
}

/**
 * @ingroup sys_sem
 * Signals a semaphore
 * @param sem the semaphore to signal
 */
void sys_sem_signal(sys_sem_t *sem)
{
  if(sem == NULL) {
    return;
  }
  
  osSemaphoreRelease(*sem);
}

/**
 * @ingroup sys_sem
 * Deallocates a semaphore.
 * @param sem semaphore to delete
 */
void sys_sem_free(sys_sem_t *sem)
{
  LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: sys_sem_t 0x%08x free\n", __FUNCTION__, *sem));
  
  SYS_STATS_DEC(sem.used);
  osSemaphoreDelete (*sem);
  *sem = NULL;
}

/**
 * @ingroup sys_mutex
 * Create a new mutex.
 * Note that mutexes are expected to not be taken recursively by the lwIP code,
 * so both implementation types (recursive or non-recursive) should work.
 * The mutex is allocated to the memory that 'mutex'
 * points to (which can be both a pointer or the actual OS structure).
 * If the mutex has been created, ERR_OK should be returned. Returning any
 * other error will provide a hint what went wrong, but except for assertions,
 * no real error handling is implemented.
 *
 * @param mutex pointer to the mutex to create
 * @return ERR_OK if successful, another err_t otherwise
 */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
  LWIP_DEBUGF(SYS_DEBUG, ("[%s]: enter now.\n", __FUNCTION__));
  *mutex = osMutexNew(NULL);
  if (!*mutex) {
    SYS_STATS_INC(mutex.err);
    return ERR_MEM;
  }

  LWIP_DEBUGF(SYS_DEBUG, ("[%s]: out now *mutex [%p].\n", __FUNCTION__, *mutex));
  SYS_STATS_INC_USED(mutex);
  return ERR_OK;
}

/**
 * @ingroup sys_mutex
 * Blocks the thread until the mutex can be grabbed.
 * @param mutex the mutex to lock
 */
void sys_mutex_lock(sys_mutex_t *mutex)
{
  if(mutex == NULL) {
    return;
  }

  LWIP_DEBUGF(SYS_DEBUG, ("[%s]: mutex %p *mutex %p.\n", __FUNCTION__, mutex, *mutex));
  osStatus_t status = osMutexAcquire(*mutex, osWaitForever);

  if (status != osOK) {
    LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: sys_mutex_lock fialed\n", __FUNCTION__));  
  }
}

/**
 * @ingroup sys_mutex
 * Releases the mutex previously locked through 'sys_mutex_lock()'.
 * @param mutex the mutex to unlock
 */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
  LWIP_DEBUGF(SYS_DEBUG, ("[%s]: enter now.\n", __FUNCTION__));
  if(mutex == NULL) {
    return;
  }
  
  osStatus_t status = osMutexRelease(*mutex);
  if (status != osOK) {
    LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_TRACE, ("[%s]: sys_mutex_unlock fialed\n", __FUNCTION__));  
  }
}

/**
 * @ingroup sys_mutex
 * Deallocates a mutex.
 * @param mutex the mutex to delete
 */
void sys_mutex_free(sys_mutex_t *mutex)
{
  LWIP_DEBUGF(SYS_DEBUG, ("[%s]: enter now.\n", __FUNCTION__));
  if(mutex == NULL) {
    return;
  }
  
  osMutexDelete(*mutex);
}
#endif /* LWIP_FREERTOS_COMPAT */

