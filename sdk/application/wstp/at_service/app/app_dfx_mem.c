/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: at perf \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-20， Create file. \n
 */

#include "app_test_perf.h"
#include "app_at_process.h"
#include "common_def.h"

#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
#include "los_memory.h"
#include "los_task.h"
#include "los_task_pri.h"

#undef OS_SYS_MEM_ADDR
#undef OS_EXTEND_MEM_ADDR
#define OS_SYS_MEM_ADDR     ((void *)&g_intheap_begin)
#define OS_EXTEND_MEM_ADDR  ((void *)&g_extend_heap_begin)

static void osal_print_os_sys_task_heap(void *pool, unsigned int tid)
{
    osal_printk("========== [task_id: %d heap] pool start: 0x%x ==========\n", tid, pool);
    LOS_MemTaskHeapInfoGet(pool, tid, osal_printk);
}

static void dfx_print_os_all_sys_task_heap(void *pool)
{
    osal_printk("========== [all task heap] pool start: 0x%x ==========\n", pool);
    for (uint8_t idx = 0; idx < g_taskMaxNum + 1; idx++) {
        osal_print_os_sys_task_heap(pool, idx);
    }
}

static void dfx_print_os_sys_mem_free(void *pool)
{
    osal_printk("========== [mem free] pool start: 0x%x ==========\n", pool);
    LOS_MemFreeInfoGet(pool, osal_printk);
}

static void dfx_print_heap_statistics_riscv(void *pool)
{
    osal_printk("========== [statistics heap] pool start: 0x%x ==========\n", pool);
    LOS_MEM_POOL_STATUS status;

    LOS_MemInfoGet(pool, &status);
#if defined(PM_MCPU_MIPS_STATISTICS_ENABLE) && (PM_MCPU_MIPS_STATISTICS_ENABLE == YES)
    oml_pf_log_print4(LOG_BCORE_PLT_INFO_HEAP, LOG_NUM_INFO_HEAP, LOG_LEVEL_INFO,
                      "[HEAP_STAT1] total:0x%x, used:0x%x, free:0x%x, usage waterline:0x%x",
                      (uint32_t)(status.uwTotalFreeSize + status.uwTotalUsedSize),
                      (uint32_t)(status.uwTotalUsedSize), (uint32_t)(status.uwTotalFreeSize),
                      (uint32_t)(status.uwUsageWaterLine));
#endif
    osal_printk("[SysHeap stat] total:0x%x, used:0x%x, current free:0x%x, peak usage:0x%x, peak free:0x%x\r\n", \
                (uint32_t)(status.uwTotalFreeSize + status.uwTotalUsedSize), \
                (uint32_t)(status.uwTotalUsedSize), (uint32_t)(status.uwTotalFreeSize), \
                (uint32_t)(status.uwUsageWaterLine), \
                (uint32_t)(status.uwTotalFreeSize + status.uwTotalUsedSize - status.uwUsageWaterLine));

    /* print all task heap usage info */
    osal_printk("Idx     TaskName        current malloc  peak malloc\r\n");
    for (uint8_t idx = 0; idx < g_taskMaxNum + 1; idx++) {
        if (idx == g_taskMaxNum) {
            osal_printk("---------non-task alloc(e.g. startup stage, interrupt)----------\r\n");
        }
        LOS_TaskMemInfoShow((void *)pool, idx, osal_printk);
    }
    osal_printk("Done\r\n");
}

static void dfx_print_os_task_id_and_name(void)
{
    TSK_INFO_S taskinfo;
    uint32_t ret = 0;
    for (uint32_t loop = 0; loop < g_taskMaxNum + 1; loop++) {
        ret = LOS_TaskInfoGet(loop, &taskinfo);
        if (ret != LOS_OK) {
            continue;
        }
        osal_printk("task_id: %d, task_name: %s\n", taskinfo.uwTaskID, taskinfo.acName);
    }
}

static void dfx_print_stack_waterline_riscv(void)
{
    TSK_INFO_S taskinfo;
    uint32_t ret = 0;

    osal_printk("task_id  taskName          stackTop   stackLen   peakUsage    sp       peakRatio\r\n");
    for (uint32_t loop = 0; loop < g_taskMaxNum + 1; loop++) {
        ret = LOS_TaskInfoGet(loop, &taskinfo);
        if (ret != LOS_OK) {
            continue;
        }
        if ((taskinfo.usTaskStatus & OS_TASK_STATUS_UNUSED) != 0) {
            continue;
        }
#if defined(PM_MCPU_MIPS_STATISTICS_ENABLE) && (PM_MCPU_MIPS_STATISTICS_ENABLE == YES)
        oml_pf_log_print_alter(LOG_BCORE_PLT_INFO_STACK, LOG_NUM_INFO_STACK, LOG_LEVEL_INFO, \
            "[STACK] id:%d, top:0x%x, size:0x%x, task usage peak:0x%x, sp:0x%x", FIVE_ARG, \
            (uint32_t)(taskinfo.uwTaskID), (uint32_t)(taskinfo.uwTopOfStack), \
            (uint32_t)(taskinfo.uwStackSize), (uint32_t)(taskinfo.uwPeakUsed), (uint32_t)((uintptr_t)(taskinfo.uwSP)));
#endif
        osal_printk("%02d      %-18s 0x%08X 0x%08X 0x%08X 0x%08X %02d%%\r\n", \
            taskinfo.uwTaskID, taskinfo.acName, \
            taskinfo.uwTopOfStack, taskinfo.uwStackSize, taskinfo.uwPeakUsed, (uint32_t)((uintptr_t)(taskinfo.uwSP)), \
            taskinfo.uwPeakUsed * 100 / taskinfo.uwStackSize); // * 100 for calculate percent.
    }
}

uint32_t dfx_show_task_heap(uint32_t tid)
{
    osal_print_os_sys_task_heap(OS_EXTEND_MEM_ADDR, tid);
    osal_print_os_sys_task_heap(OS_SYS_MEM_ADDR, tid);
    return ERRCODE_SUCC;
}

uint32_t dfx_show_all_task_heap(void)
{
    dfx_print_os_all_sys_task_heap(OS_EXTEND_MEM_ADDR);
    dfx_print_os_all_sys_task_heap(OS_SYS_MEM_ADDR);
    return ERRCODE_SUCC;
}

uint32_t dfx_show_mem_free(void)
{
    dfx_print_os_sys_mem_free(OS_EXTEND_MEM_ADDR);
    dfx_print_os_sys_mem_free(OS_SYS_MEM_ADDR);
    return ERRCODE_SUCC;
}

uint32_t dfx_show_heap_statistic(void)
{
    dfx_print_heap_statistics_riscv(OS_EXTEND_MEM_ADDR);
    dfx_print_heap_statistics_riscv(OS_SYS_MEM_ADDR);
    return ERRCODE_SUCC;
}

uint32_t dfx_show_mem_info(void)
{
    dfx_print_stack_waterline_riscv();
    dfx_print_heap_statistics_riscv(OS_EXTEND_MEM_ADDR);
    dfx_print_os_all_sys_task_heap(OS_EXTEND_MEM_ADDR);
    dfx_print_os_sys_mem_free(OS_EXTEND_MEM_ADDR);
    dfx_print_heap_statistics_riscv(OS_SYS_MEM_ADDR);
    dfx_print_os_all_sys_task_heap(OS_SYS_MEM_ADDR);
    dfx_print_os_sys_mem_free(OS_SYS_MEM_ADDR);
    return ERRCODE_SUCC;
}

uint32_t dfx_show_task_info(void)
{
    dfx_print_os_task_id_and_name();
    return ERRCODE_SUCC;
}

uint32_t dfx_show_stack_waterline(void)
{
    dfx_print_stack_waterline_riscv();
    return ERRCODE_SUCC;
}
#endif