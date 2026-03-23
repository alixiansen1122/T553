/*
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description:  Application Core Main function for Test Suite.
 * Author:
 * Create: 2021-03-09
 */
#include "main.h"
#include "cmsis_os2.h"
#include "string.h"
#include "ipc.h"
#include "irmalloc.h"
#include "uart.h"
#include "rtc.h"
#include "gpio.h"
#include "tcxo.h"
#include "debug_print.h"
#include "test_suite_uart.h"
#include "test_suite.h"
#include "test_auxiliary.h"
#include "flash.h"
#include "application_version.h"
#include "pinctrl_porting.h"
#include "pm.h"
#include "pmu.h"
#include "pm_control.h"
#include "otp.h"
#include "securec.h"
#include "test_flash.h"
#include "test_timer.h"
#include "test_systick.h"
#include "test_tcxo.h"
#include "test_spi.h"
#include "test_dma.h"
#include "test_pwm.h"
#include "test_i2c.h"
#include "test_xip.h"
#include "test_mipi_tx.h"
#include "soc_lcd_api.h"
#include "test_uart.h"
#include "test_calendar.h"
#include "test_pwm.h"
#include "test_rtc_unified.h"
#ifdef SUPPORT_DHRY_TEST
#include "test_dhry.h"
#endif
#ifdef SUPPORT_MATH_TEST
#include "test_math.h"
#endif
#ifdef SUPPORT_GPU_TEST
#include "test_gpu_comm.h"
#endif
#ifdef __LITEOS__
#ifdef FS_TEST
#ifdef CFG_DRIVERS_MMC
#include "test_emmc.h"
#endif
#endif
#include "test_cmsis.h"
#include "los_task_pri.h"
#include "los_printf_pri.h"
#endif
#include "test_update.h"
#include "test_partition.h"
#include "test_lcd_api.h"
#include "test_qspi_display.h"
#include "lcd_qspi.h"
#include "lcd_mipi.h"
#include "lcd_bus.h"
#include "test_nv.h"
#include "test_gpio.h"
#include "pmp_config.h"
#include "test_efuse.h"
#include "panic.h"

#if defined(SUPPORT_AUDIO_TEST)
#include "test_audio_case.h"
#endif
#if ENABLE_LOW_POWER == YES
#include "test_pm.h"
#endif

#ifdef __FREERTOS__
#include "FreeRTOS.h"
#include "task.h"
#include "print.h"
#if defined(XLTCFG_SUPPORT_MEMMNG)
#include "xlt_memory.h"
#endif
#if defined(FREERTOS_DEBUG)
#include "debug_cmd.h"
#endif
#endif

#include "exception.h"
#include "soc_osal.h"
#include "common_def.h"

#define EMMC_BASE_ADDR         0x52062000

#define DELAY_TIME_120US            120
#define DELAY_TIME_30US             30
#define TESTSUIT_VER_STR_MAX        128

struct rt_mmcsd_host *video_mmc = NULL;

extern uint32_t __l2m_gpu_text_start;
extern uint32_t __l2m_gpu_text_load;
extern uint32_t __l2m_gpu_text_end;
extern uint32_t __l2m_gpu_bss_start;
extern uint32_t __l2m_gpu_bss_end;
extern uint32_t __l2m_gpu_data_start;
extern uint32_t __l2m_gpu_data_load;
extern uint32_t __l2m_gpu_data_end;
extern uint32_t __l2m_dsp_text_start;
extern uint32_t __l2m_dsp_text_load;
extern uint32_t __l2m_dsp_text_end;
extern uint32_t __l2m_dsp_bss_start;
extern uint32_t __l2m_dsp_bss_end;
extern uint32_t __l2m_dsp_data_start;
extern uint32_t __l2m_dsp_data_load;
extern uint32_t __l2m_dsp_data_end;

extern char __heap_start;
extern void *g_intheap_size;
extern char __extend_heap_start;
extern void *g_extend_heap_size;

const lcd_bus_api g_display_api_qspi = {
    .bus_init = lcd_qspi_init,
    .bus_deinit = lcd_qspi_deinit,
    .bus_enable = lcd_qspi_enable,
    .bus_disable = lcd_qspi_disable,
    .bus_write_cmd = lcd_qspi_write_cmd,
    .bus_read_cmd = lcd_qspi_read_cmd,
    .bus_send_frame = lcd_qspi_send_frame,
    .bus_register_te_isr = lcd_qspi_register_te_isr,
};

static const lcd_bus_api g_display_api_mipi = {
    .bus_init = mipi_bus_init,
    .bus_deinit = mipi_bus_deinit,
    .bus_enable = mipi_bus_enable,
    .bus_disable = mipi_bus_disable,
    .bus_write_cmd = mipi_bus_write,
    .bus_read_cmd = mipi_bus_read,
    .bus_lp_ctrl = mipi_bus_lp_ctrl,
    .bus_register_panel_info = mipi_bus_register_panel_info,
    .bus_get_panel_timing = mipi_bus_get_panel_timing,
    .bus_get_partial_display_cmd = mipi_bus_get_partial_display_cmd,
    .bus_update_panel = mipi_bus_update_panel,
    .bus_refresh_area = mipi_bus_refresh_area,
    .bus_suspend = mipi_bus_suspend,
    .bus_resume = mipi_bus_resume,
};

typedef void (*CreateQueueFunction_t)(void);

typedef struct {
    osThreadAttr_t attr;
    osThreadFunc_t func;
    uint32_t *task_handle;
    CreateQueueFunction_t create_queue_fn_p;
} app_task_definition_t;

static void cmd_processor_entry(const void *args);
static void test_suite_main_add_functions(void);

static app_task_definition_t g_test_suite_tasks[] = {
    { { "cmd_loop", 0, NULL, 0, NULL, 0x1000, (osPriority_t)(17), 0, 0 },
      (osThreadFunc_t)cmd_processor_entry, NULL, NULL },
};

#ifdef CFG_DRIVERS_MMC
static int emmc_init(void)
{
    /**
     * FPGA解复位, 设置CMD_CONFLICT_CHECK
     * 之后才能设置管脚
     */
    writew(GLB_CTL_M_RB_BASE + 0xc8, 0x1);
    writew(GLB_CTL_M_RB_BASE + 0xc8, 0x3);
    writew(EMMC_BASE_ADDR + 0x508, 0);

    uapi_pm_add_sleep_veto(PM_ID_BSP);
    init_emmc_pinmux();
    int ret = emmc_drv_context_init();
    if (ret != ENOERR) {
        printf("emmc init error:[%d]\r\n", ret);
    }
    uapi_pm_remove_sleep_veto(PM_ID_BSP);
    return 0;
}
#endif

static void cmd_processor_entry(const void *args)
{
    unused(args);
#ifdef FS_TEST
    los_vfs_init();
#ifdef CFG_DRIVERS_MMC
    emmc_init();
#endif
#ifdef CFG_DRIVERS_NANDFLASH
    nandflash_filesystem_init();
#endif
#endif
    uapi_test_suite_init();
    test_suite_main_add_functions();
#if defined(__FREERTOS__) && defined(FREERTOS_DEBUG)
    debug_cmd_reg();
#endif
}

static void app_os_init(void)
{
    for (uint32_t i = 0; i < (sizeof(g_test_suite_tasks) / sizeof(g_test_suite_tasks[0])); i++) {
        if (g_test_suite_tasks[i].create_queue_fn_p) {
            g_test_suite_tasks[i].create_queue_fn_p();
        }
        g_test_suite_tasks[i].task_handle = osThreadNew(g_test_suite_tasks[i].func, NULL, &g_test_suite_tasks[i].attr);
        if (g_test_suite_tasks[i].task_handle == NULL) {
            panic(PANIC_TESTSUIT, __LINE__);
        }
    }
}

static int testsuit_get_version(int argc, const char *argv[])
{
    unused(argc);
    unused(argv);

    char str[TESTSUIT_VER_STR_MAX];
#if defined(BUILD_TIME) && defined(TARGET_NAME)
    if (sprintf_s(str, TESTSUIT_VER_STR_MAX, "\r\nVersion:%s %s %s\r\n",
                  get_version_string(), TARGET_NAME, BUILD_TIME) < 0) {
        return -1;
    }
#else
    if (sprintf_s(str, TESTSUIT_VER_STR_MAX, "\r\nVersion:%s\r\n", get_version_string()) < 0) {
        return -1;
    }
#endif
    PRINT("version is = %s\r\n", str);

    return 0;
}

static void add_testsuit_get_version(void)
{
    uapi_test_suite_add_function("get_version", "Test API: get version. Params: <version>", testsuit_get_version);
}

static void test_suite_main_add_functions(void)
{
    add_auxiliary_functions();
    add_testsuit_get_version();
    add_adc_test_case();
#ifdef SUPPORT_GPU_TEST
    test_gpu_add_case();
#endif
    add_test_partition();
    add_test_update();
    add_efuse_test_case();
    add_sdio_host_case();
    add_kv_functions();
    add_test_touch_case();
    add_test_mipi_tx();
    add_qspi_test_case();
    add_test_lcd();
#if ENABLE_LOW_POWER == YES
    add_pm_test_case();
#endif
    /* testsuit版本测试开始 */
    add_dma_test_case();
    add_flash_test_case();
    add_gpio_test_case();
    add_i2c_test_case();
    add_pinctrl_test_case();
    add_pmp_test_case();
    add_pwm_test_case();
    add_systick_test_case();
    add_tcxo_test_case();
    add_spi_test_case();
    add_uart_test_case();
    add_timer_test_case();
    add_watchdog_test_case();
    add_xip_test_case();
    add_rtc_test_case();
    /* testsuit版本测试结束 */
    /* 其他日常调试请在下面添加测试用例 */
    add_calendar_test_case();
#ifdef __LITEOS__
    add_cmsis_test_case();
#endif
#ifdef SUPPORT_MATH_TEST
    add_math_test_case();
#endif
#ifdef SUPPORT_DHRY_TEST
    add_dhry_test_case();
#endif
    add_test_reboot();
}

static void log_exception_dump(uint32_t int_id, uint32_t reason, const uint32_t *para1, const uint32_t *para2,
                               uint32_t addr)
{
    unused(reason);
    PRINT("Exection[%d] Occured" NEWLINE, int_id);
    PRINT("********Regs in Stack before entering Interrupt********" NEWLINE);
    PRINT("R0  = %x" NEWLINE, para1[0]);
    PRINT("R1  = %x" NEWLINE, para1[1]);
    PRINT("R2  = %x" NEWLINE, para1[2]);
    PRINT("R3  = %x" NEWLINE, para1[3]);
    PRINT("R4  = %x" NEWLINE, para2[0]);
    PRINT("R5  = %x" NEWLINE, para2[1]);
    PRINT("R6  = %x" NEWLINE, para2[2]);
    PRINT("R7  = %x" NEWLINE, para2[3]);
    PRINT("R8  = %x" NEWLINE, para2[4]);
    PRINT("R9  = %x" NEWLINE, para2[5]);
    PRINT("R10 = %x" NEWLINE, para2[6]);
    PRINT("R11 = %x" NEWLINE, para2[7]);
    PRINT("R12 = %x" NEWLINE, para1[4]);
    PRINT("LR  = %x" NEWLINE, para1[5]);
    PRINT("PC  = %x" NEWLINE, para1[6]);
    PRINT("PSR = %x" NEWLINE, para1[7]);
    PRINT("Fault Address = 0x%x" NEWLINE, addr);
    while (1) { //lint !e716
    }
}

static void init_h0_pinmux(void)
{
    writew(0x52008000 + 0x84, 0x1);
    writew(0x52008000 + 0x88, 0x1);
    writew(0x52008000 + 0x8c, 0x1);
    writew(0x52008000 + 0x90, 0x1);
    reg_setbit(0x5700c800, 0x284, 1);
    reg_setbit(0x5700c800, 0x288, 1);
    reg_setbit(0x5700c800, 0x28c, 1);
    reg_setbit(0x5700c800, 0x290, 1);
}

static void init_h1_pinmux(void)
{
    writew(0x52008000 + 0x4c, 0x1);
    writew(0x52008000 + 0x50, 0x1);
    writew(0x52008000 + 0x54, 0x1);
    writew(0x52008000 + 0x58, 0x1);
    reg_setbit(0x5700c800, 0x250, 1);
    reg_setbit(0x5700c800, 0x258, 1);
}

#if defined(__FREERTOS__)
static void OsCacheInit(void)
{
    dsb();
    ArchICacheInvAll();
    ArchDCacheInvAll();
    dsb();

    (void)ArchICacheEnable(CACHE_32KB);
    (void)ArchICachePrefetchEnable(CACHE_PREF_1_LINES);

    (void)ArchDCacheEnable(CACHE_32KB);
    (void)ArchDCachePrefetchEnable(CACHE_PREF_1_LINES, CACHE_PREF_1_LINES);
}
#endif

static void copy_from_flash_to_l2ram(void)
{
    errno_t ret;
    uint32_t size = (uint32_t)(&__l2m_gpu_text_end) - (uint32_t)(&__l2m_gpu_text_start);
    ret = memcpy_s((void *)((uint32_t)(&__l2m_gpu_text_start)), size, (void *)(uint32_t)(&__l2m_gpu_text_load), size);
    if (ret != EOK) {
        PRINT("l2m gpu text memcpy fail."NEWLINE);
    }

    size = (uint32_t)(&__l2m_gpu_data_end) - (uint32_t)(&__l2m_gpu_data_start);
    ret = memcpy_s((void *)((uint32_t)(&__l2m_gpu_data_start)), size, (void *)(uint32_t)(&__l2m_gpu_data_load), size);
    if (ret != EOK) {
        PRINT("l2m gpu data memcpy fail."NEWLINE);
    }

    size = (uint32_t)(&__l2m_gpu_bss_end) - (uint32_t)(&__l2m_gpu_bss_start);
    ret = memset_s((void *)((uint32_t)(&__l2m_gpu_bss_start)), size, 0, size);
    if (ret != EOK) {
        PRINT("l2m gpu bss memset fail."NEWLINE);
    }
#if DSP_L2RAM_AREA_SIZE < DSP_L2RAM_AREA_TOTAL
    size = (uint32_t)(&__l2m_dsp_text_end) - (uint32_t)(&__l2m_dsp_text_start);
    ret = memcpy_s((void *)((uint32_t)(&__l2m_dsp_text_start)), size, (void *)(uint32_t)(&__l2m_dsp_text_load), size);
    if (ret != EOK) {
        PRINT("l2m dsp text memcpy fail."NEWLINE);
    }

    size = (uint32_t)(&__l2m_dsp_data_end) - (uint32_t)(&__l2m_dsp_data_start);
    ret = memcpy_s((void *)((uint32_t)(&__l2m_dsp_data_start)), size, (void *)(uint32_t)(&__l2m_dsp_data_load), size);
    if (ret != EOK) {
        PRINT("l2m dsp data memcpy fail."NEWLINE);
    }

    size = (uint32_t)(&__l2m_dsp_bss_end) - (uint32_t)(&__l2m_dsp_bss_start);
    ret = memset_s((void *)((uint32_t)(&__l2m_dsp_bss_start)), size, 0, size);
    if (ret != EOK) {
        PRINT("l2m dsp bss memset fail."NEWLINE);
    }
#endif
}

static void service_init(void)
{
    uapi_pm_control(PM_DISPLAY_POWER_UP_AND_RUN);
    uapi_lcd_init();

#if ENABLE_LOW_POWER == YES
    uapi_pm_init();
#endif
#if defined(SUPPORT_AUDIO_TEST)
    add_test_audio();
#endif
}

static void flash_spi_init(void)
{
    spi_attr_t attr = {0};
    spi_extra_attr_t extra_attr = {0};
    uapi_spi_init(SPI_BUS_3, &attr, &extra_attr);
}

static void board_init(uint32_t flash_id, uint32_t flash_unique_id)
{
    test_suite_uart_init();
    gpio_select_core(DISPLAY_TE_GPIO, CORE);
#ifdef QSPI_DISPLAY
    PRINT("qspi**************\r\n");
    uapi_pin_set_mode(DISPLAY_TE_GPIO, HAL_PIO_FUNC_GPIO);
    lcd_bus_register_api(&g_display_api_qspi, BUS_DISPLAY_QSPI);
#else
    PRINT("mipi**************\r\n");
    uapi_pin_set_mode(DISPLAY_TE_GPIO, 1);  /* DSI_TE */
    lcd_bus_register_api(&g_display_api_mipi, BUS_DISPLAY_MIPI);
#endif

    /* flash init */
    flash_spi_init();
    flash_funcreg_adapt(0);
    flash_save_manufacturer(0, flash_id);
    uapi_flash_init(0);
    uapi_flash_config_cmd_at_xip_mode(0, (uint8_t)flash_unique_id);
    uapi_flash_switch_to_cache_mode(0);

    PRINT("Flash id is %x, init success"NEWLINE, flash_id);
}

static void enable_ulp_ram(void)
{
    writew(0x5702c50c, 0x1);
    writew(0x5700450c, 0x1);
}

static void chip_init(void)
{
    clocks_pll_init_vote();
    uapi_pin_init();
    uapi_tcxo_init();
    uapi_systick_init();

    hal_xip_init();

    /* Initialise malloc */
    irmalloc_init_default();
    init_h0_pinmux();
    init_h1_pinmux();

    uapi_timer_init();
    uapi_timer_adapter(0, TIMER_0_IRQN, irq_prio(TIMER_0_IRQN));

    uapi_rtc_init();
    uapi_rtc_adapter(0, RTC_0_IRQN, irq_prio(RTC_0_IRQN));

    ipc_init();

    log_memory_region_init();
    log_init();
#if CHIP_ASIC
    uapi_pmu_init();
    uapi_clocks_init();
#endif

    uapi_dma_init();
    uapi_dma_open();
    uapi_efuse_init();
}

static void liteos_kernel_init(void)
{
#ifdef __LITEOS__
    /* enable icache and dcahce */
    pmp_init();
    OsCacheInit();

    ArchSetExcHook((EXC_PROC_FUNC)do_fault_handler);
    ArchSetNMIHook((NMI_PROC_FUNC)do_hard_fault_handler);
    OsSetMainTask();
    OsCurrTaskSet(OsGetMainTask());

    (void)osKernelInitialize();
#endif
}

static void freertos_kernel_init(void)
{
#ifdef __FREERTOS__
    /* os kernel adapt config */
    vPortHwiInit();

    /* enable icache and dcahce */
    pmp_init();
    OsCacheInit();

    /* memory pool init */
#if defined(XLTCFG_SUPPORT_MEMMNG)
    g_heap = (void *)&__heap_start;
    if (xlt_mem_init(g_heap, (uint32_t)&g_intheap_size)) {
        panic(PANIC_MEMORY, __LINE__);
    }

    g_extend_heap = g_heap;
#endif

    osal_irq_request(NON_MASKABLE_INT_IRQN, (osal_irq_handler)do_fault_handler_freertos, NULL, NULL, NULL);
    osal_irq_enable(NON_MASKABLE_INT_IRQN);
    (void)osKernelInitialize();
    vTimerTickInit();
#endif
}

static void kernel_init(void)
{
    enable_ulp_ram();
    copy_from_flash_to_l2ram();

    panic_init();

    freertos_kernel_init();
    liteos_kernel_init();
}

static startup_table ssb_to_app_table;

void sec_main(const void *startup_details_table)
{
    if (memcpy_s(&ssb_to_app_table, sizeof(ssb_to_app_table), startup_details_table, sizeof(ssb_to_app_table)) != EOK) {
        panic(PANIC_MEMORY, __LINE__);
        return;
    }

    kernel_init();

    chip_init();

    board_init(ssb_to_app_table.ssb_to_application_flash_id, ssb_to_app_table.ssb_to_application_flash_unique_id);

    service_init();

    app_os_init();

    (void)osKernelStart();
    for (;;) {
    }
}
