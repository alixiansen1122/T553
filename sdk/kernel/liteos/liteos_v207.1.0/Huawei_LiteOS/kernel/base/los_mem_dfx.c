#include "los_task_pri.h"
#include "los_memstat_pri.h"
#include "los_memory_pri.h"
#include "los_slab_pri.h"
#include "mem/bestfit/los_memory_internal.h"

#ifdef LOSCFG_MEM_TASK_STAT
STATIC BOOL OsMemNodeIsUsed(LosMemDynNode *node)
{
    if (node == NULL) {
        return FALSE;
    }
    return ((OS_MEM_NODE_GET_USED_FLAG(node->selfNode.sizeAndFlag) == OS_MEM_NODE_USED_FLAG) ? TRUE : FALSE);
}
 
STATIC UINT32 OsHeapTaskHeapInfoGet(VOID *pool, UINT8 taskId, VOID (*print)(const char *fmt, ...))
{
    UINT32 totalPoolUsedSize = 0;
    UINT32 totalSlabUsedSize = 0;
    UINT32 totalCostSize = OS_MEM_NODE_HEAD_SIZE;   // record endNode cost first
    UINT32 idx = 0;
    UINTPTR callerRa = 0;
    LosTaskCB *taskCB = OS_TCB_FROM_TID(taskId);
 
    LosMemDynNode *tmpNode = NULL;
    LosMemPoolInfo *poolInfo = (LosMemPoolInfo *)pool;
    LosMemDynNode *endNode = NULL;
 
    if (poolInfo == NULL || taskId > TASK_NUM - 1) {
        return LOS_NOK;
    }
 
    if (taskId == TASK_NUM - 1) {
        print("Other malloc details:\r\n");
    } else {
        print("Task:%s(taskId=%02d) malloc details:\r\n", taskCB->taskName, taskId);
    }
    endNode = OS_MEM_END_NODE(pool, poolInfo->poolSize);
    tmpNode = endNode->selfNode.preNode;
 
    // get total slab used mem
    totalSlabUsedSize = OsSlabMemInfoGet(pool, taskId);
 
    while (tmpNode != endNode) {
        totalCostSize += OS_MEM_NODE_HEAD_SIZE;
        if (tmpNode->selfNode.taskId == taskId && OsMemNodeIsUsed(tmpNode)) {
            totalPoolUsedSize += OS_MEM_NODE_GET_SIZE(tmpNode->selfNode.sizeAndFlag);
            idx++;
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
            callerRa = tmpNode->selfNode.callerRa;
#endif
            print("mem[%03d]: size=%08d Bytes, addr=0x%08x, callerRA=0x%08x\r\n", idx, \
                  (OS_MEM_NODE_GET_SIZE(tmpNode->selfNode.sizeAndFlag)), (UINTPTR)(tmpNode + OS_MEM_NODE_HEAD_SIZE), callerRa);
        }
        tmpNode = tmpNode->selfNode.preNode;
    }
    print("Current total pool alloc size=%d Bytes\r\n", totalPoolUsedSize);
    print("Current total slab alloc size=%d Bytes\r\n", totalSlabUsedSize);
    print("Current total cost size=%d Bytes\r\n", totalCostSize);
 
    return LOS_OK;
}
 
STATIC UINT32 OsHeapFreeInfoGet(VOID *pool, VOID (*print)(const char *fmt, ...))
{
    LosMemDynNode *tmpNode = NULL;
    LosMemPoolInfo *poolInfo = (LosMemPoolInfo *)pool;
    LosMemDynNode *endNode = NULL;
 
    UINT32 totalFreeSize = 0;
    UINT32 idx = 0;
 
    if (poolInfo == NULL) {
        return LOS_NOK;
    }
 
    endNode = OS_MEM_END_NODE(pool, poolInfo->poolSize);
    tmpNode = endNode->selfNode.preNode;
 
    print("Current mem free info:\r\n");
    while (tmpNode != endNode) {
        if (!OsMemNodeIsUsed(tmpNode)) {
            UINT32 mem_size = (OS_MEM_NODE_GET_SIZE(tmpNode->selfNode.sizeAndFlag));
            UINTPTR mem_from = (UINTPTR)(tmpNode + OS_MEM_NODE_HEAD_SIZE);
            UINTPTR mem_end = (UINTPTR)(tmpNode + OS_MEM_NODE_HEAD_SIZE + mem_size);
            print("free mem[%03d]: size=%08d Bytes, from=0x%08x, end=0x%08x\r\n", idx, mem_size, mem_from, mem_end);
            totalFreeSize += mem_size;
            idx++;
        }
        tmpNode = tmpNode->selfNode.preNode;
    }
 
    print("total free mem: %d Bytes\r\n", totalFreeSize);
    return LOS_OK;
}

VOID OsMemTaskUsageShow(VOID *memPool, UINT32 taskId,  VOID (*print)(const char *fmt, ...))
{
    LosMemPoolInfo *pool = NULL;
    Memstat *stat = NULL;
    TaskMemUsedInfo *taskMemInfo = NULL;
    LosTaskCB *taskCB = OS_TCB_FROM_TID(taskId);
    CHAR *name = NULL;
 
    pool = (LosMemPoolInfo *)memPool;
    stat = &(pool->stat);
    taskMemInfo = &(stat->taskMemstats[taskId]);
 
    name = (taskId == TASK_NUM - 1) ? " " : taskCB->taskName;
    if (taskCB->taskStatus != OS_TASK_STATUS_UNUSED) {
        print("%02d      %-14s  %08d        %08d\r\n", \
              taskId, name, taskMemInfo->memUsed, taskMemInfo->memPeak);
    }
    return ;
}
 
VOID LOS_MemTaskHeapInfoGet(VOID *pool, UINT8 taskId, VOID (*print)(const char *fmt, ...))
{
    UINT32 intSave;
 
    if ((pool == NULL) || (print == NULL)) {
        return;
    }
 
    MEM_LOCK(intSave);
    OsHeapTaskHeapInfoGet(pool, taskId, print);
    MEM_UNLOCK(intSave);
    return;
}
 
VOID LOS_MemFreeInfoGet(VOID *pool, VOID (*print)(const char *fmt, ...))
{
    UINT32 intSave;
 
    if ((pool == NULL) || (print == NULL)) {
        return;
    }
 
    MEM_LOCK(intSave);
    OsHeapFreeInfoGet(pool, print);
    MEM_UNLOCK(intSave);
    return;
}

UINT32 OsSlabMemInfoGet(const VOID *pool, UINT32 taskId)
{
    struct LosSlabControlHeader *slabMem = OsSlabCtrlHdrGet(pool);
    UINT32 taskTotalUsedSlabMem = 0;
    for (UINT32 clsidx = 0; clsidx < SLAB_MEM_COUNT; clsidx++) {
#ifdef LOSCFG_KERNEL_MEM_SLAB_AUTO_EXPANSION_MODE
#else
        OsSlabAllocator *allocator = slabMem->slabClass[clsidx].alloc;
        UINT32 numBits = allocator->bitset->numBits;
        for (UINT32 didx = 0; didx < numBits; didx++) {
            OsSlabBlockNode *blockNode = (OsSlabBlockNode *)(allocator->dataChunks + allocator->itemSz * (UINT32)didx);
            if (OS_SLAB_BLOCK_ID_GET(blockNode) == taskId) {
                taskTotalUsedSlabMem += OS_SLAB_BLOCK_SIZE_GET(blockNode);
            }
        }
#endif
    }
    return taskTotalUsedSlabMem;
}
#else
VOID LOS_MemTaskHeapInfoGet(VOID *pool, UINT8 taskId, VOID (*print)(const char *fmt, ...))
{
    return;
}
 
VOID LOS_MemFreeInfoGet(VOID *pool, VOID (*print)(const char *fmt, ...))
{
    return;
}
#endif

#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
UINTPTR g_callerRA[TASK_NUM];

STATIC UINT32 OsGetCurTaskId(VOID)
{
    UINT32 taskId;
 
 
    if ((OsCurrTaskGet() != NULL) && OS_INT_INACTIVE) {
        /*
         * after OsTaskInit, OsCurrTaskGet() is not null, but id is the same
         * as (LOSCFG_BASE_CORE_TSK_LIMIT + 1), so it will be recorded into
         * the last one of the array.
         */
        taskId = LOS_CurTaskIDGet();
    } else {
        taskId = TASK_NUM - 1;
    }
    return taskId;
}
 
VOID OsSaveCallerRa(UINTPTR callerRa)
{
    UINT32 taskId =  OsGetCurTaskId();
    g_callerRA[taskId] = callerRa;
}
 
BOOL OsIsSetCallerRa(VOID)
{
    UINT32 taskId =  OsGetCurTaskId();
    return (g_callerRA[taskId] != (UINTPTR)0);
}

VOID LOS_SaveCallerRa(UINTPTR callerRa)
{
    return OsSaveCallerRa(callerRa);
}

INT32 LOS_TaskMemInfoShow(VOID *pool, UINT32  taskId,  VOID (*print)(const char *fmt, ...))
{
    if (pool == NULL || print == NULL || taskId > g_taskMaxNum) {
        return -1;
    }
 
    OsMemTaskUsageShow(pool, taskId, print);
    return 0;
}
 
BOOL LOS_IsSetCallerRa(VOID)
{
    return OsIsSetCallerRa();
}

#else 
VOID LOS_SaveCallerRa(UINTPTR callerRa)
{
    return;
}
 
BOOL LOS_IsSetCallerRa(VOID)
{
    return TRUE;
}

INT32 LOS_TaskMemInfoShow(VOID *pool, UINT32 taskId,  VOID (*print)(const char *fmt, ...))
{
    return 0;
}
#endif