/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: action display func.
 * Author:
 * Create:
 */

#include "power_display_service.h"
#include "display_action.h"
#include "common_def.h"
#include "pm_definition.h"
#include "pm.h"

#include "lcd_bus.h"
#include "soc_vau_type.h"
#include "soc_dpu_type.h"
#include "soc_mipi_tx.h"
#include "pmu_ldo.h"
#include "pm_lp_stub.h"
#include "hal_pmu_ldo.h"
#include "cmsis_os2.h"
#ifdef CFG_DRIVERS_NANDFLASH
#include "nandflash_to_os.h"
#endif
#ifdef SUPPORT_OHOSFWK
#include "screennotify/ui_screennotify.h"
#endif
#include "sys/time.h"
#include "touch_screen_def.h"
#include "time.h"
#include "tcxo.h"
/* GBL_CTL_M中SOFT_RST_4寄存器 */
#define DISPLAY_LOGIC_CRG_RESET 0x570000c0
/**
 * @brief  PMU1_CTL中DISPLAY_PWR_EN_CFG寄存器.
 * 0   display_sub power gating控制：0,下电; 1，上电.
 * 1:3 reserve
 * 4   display_sub power sts：0,下电; 1，上电.
 * 5:7 reserve
 * 8   display_sub isolation控制：0,取消iso; 1，使能iso
 */
#define DISPLAY_PWR_EN_CFG 0x57004034
#define DISPLAY_PWR_CLOSE_POWER 0x100

#ifdef CONFIG_LOW_POWER_TEST
static bool g_pms_dpu_ready = false;
#else
static bool g_pms_dpu_ready = true;
#endif

#if defined(SUPPORT_OHOSFWK)
bool g_pms_lcd_on_first_flag = true;
#else
bool g_pms_lcd_on_first_flag = false;
#endif

uint32_t last_screen_on_timestamp;
uint32_t last_screen_off_timestamp;

uint32_t screen_on_time;
uint32_t screen_off_time;

uint32_t get_display_timestamp(void)
{
    uint32_t result;
    struct timeval tv = {0};
    if (gettimeofday(&tv, NULL)) {
        return ERRCODE_FAIL;
    }
    return (uint32_t)tv.tv_sec;
}

static void action_display_lcd_on_pre_handle(void)
{
    display_resume_power_on();
#ifdef QSPI_DISPLAY
    uapi_pin_set_mode(DISPLAY_TE_GPIO, HAL_PIO_FUNC_GPIO); // sagpio_l22 set gpio mode
#else
    uapi_pin_set_mode(DISPLAY_TE_GPIO, HAL_PIO_FUNC_TE); // sagpio_l22 set te mode
#endif
    uapi_pin_set_ie(DISPLAY_TE_GPIO, PIN_IE_ENABLE); // sagpio_l22 open ie
#ifndef QSPI_DISPLAY
    pmu_ldo_power_on(PMU_LDO_AUXLDO3, TURN_ON);
#endif
    return;
}

static void action_display_lcd_off_handle(void)
{
    writew(DISPLAY_LOGIC_CRG_RESET, 0x0);                // display logic crg reset
    writew(DISPLAY_PWR_EN_CFG, DISPLAY_PWR_CLOSE_POWER); // display close power
#ifndef QSPI_DISPLAY
    pmu_ldo_power_on(PMU_LDO_AUXLDO3, TURN_OFF);
    hal_pmu_auxldo_enable_high_resistance(HAL_PMU_AUXLDO3, TURN_ON);
#endif
#ifdef CONFIG_CLDO1PD_SUPPORT
    pmu_control_cldo1_sleep_bypass(TURN_OFF);
#endif
    return;
}

static void caculate_screen_off_time()
{
    if (last_screen_off_timestamp == 0) {
        return;
    }
    screen_off_time = last_screen_on_timestamp - last_screen_off_timestamp; //灭屏时间间隔等于本次亮屏的时间点减去上次灭屏的时间点。
    screen_context_t *sc = power_display_get_screen_context();
    sc->screen_off_time_interval = screen_off_time;
}

static uint32_t action_display_lcd_on()
{
    last_screen_on_timestamp = get_display_timestamp();
    caculate_screen_off_time();
    ext_errno ret;
#ifdef CFG_DRIVERS_NANDFLASH
    nand_lock();
#endif
    uapi_pm_request_mode(PM_ID_VIDEO, PM_MODE_PERFORMACE);
#ifdef CFG_DRIVERS_NANDFLASH
    nand_unlock();
#endif
    if (g_pms_lcd_on_first_flag == false) {
        if (lcd_get_status()) {
            POWER_DISPLAY_PRINT_ERR(0, "\r\nLCD has benn on.\r\n");
            return ERRCODE_SUCC;
        }
        action_display_lcd_on_pre_handle();
        lcd_bus_api *bus_api = lcd_get_bus_api();
        ret = bus_api->bus_resume();
        if (ret != 0) {
            POWER_DISPLAY_PRINT_ERR(0, "bus resume error ret=%x.\r\n", ret);
            return ERRCODE_FAIL;
        }
#ifndef QSPI_DISPLAY
        if (g_pms_dpu_ready == false) {
            ret = drv_dpu_resume(NULL);
            if (ret != 0) {
                POWER_DISPLAY_PRINT_ERR(0, "drv dpu resume error ret=%x\r\n", ret);
                return ERRCODE_FAIL;
            }
        }
#endif
#ifdef SUPPORT_GPU_JPEG
        (void)drv_jpeg_resume(NULL);
#endif
        ret = drv_vau_resume(NULL);
        if (ret != 0) {
            POWER_DISPLAY_PRINT_ERR(0, "drv vau resume error ret=%x\r\n", ret);
            return ERRCODE_FAIL;
        }
        ret = uapi_lcd_init();
        if (ret != 0 && ret != EXT_ERR_MIPI_ALREADY_INIT) {
            POWER_DISPLAY_PRINT_ERR(0, "LCD init error ret=%x\r\n", ret);
            return ERRCODE_FAIL;
        }
    }
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
    NotifyScreenOn(); // Instruct the uikit to start the display operation.
#endif
    if ((g_pms_lcd_on_first_flag == false) && (g_pms_dpu_ready == false)) {
        touch_screen_resume(); // open touch
    }
#ifdef CONFIG_CLDO1PD_SUPPORT
    pmu_control_cldo1_sleep_bypass(TURN_ON);
#endif
    lcd_set_status(true);
    g_pms_lcd_on_first_flag = false;
    return ERRCODE_SUCC;
}

static void caculate_screen_on_time()
{
    if (last_screen_on_timestamp == 0) {
        return;
    }
    screen_on_time = last_screen_off_timestamp - last_screen_on_timestamp; //亮屏时间间隔等于本次灭屏的时间点减去上次亮屏的时间点。
    screen_context_t *sc = power_display_get_screen_context();
    sc->screen_on_time_interval = screen_on_time;
}

static uint32_t action_display_lcd_off()
{
    if (!lcd_get_status()) {
        POWER_DISPLAY_PRINT_ERR(0, "\r\nLCD has benn off\r\n");
        return ERRCODE_SUCC;
    }
    last_screen_off_timestamp = get_display_timestamp();
    caculate_screen_on_time();
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
    NotifyScreenOff(); // Instruct the uikit to close the display operation.
#endif

    osDelay(100);        // Wait 100ms for the uikit to complete display.
    touch_screen_suspend(); // close touch
    ext_errno ret = drv_vau_suspend(NULL);
    if (ret != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "drv vau suspend error ret=%x\r\n", ret);
    }
#ifdef SUPPORT_GPU_JPEG
    (void)drv_jpeg_suspend(NULL);
#endif
#ifndef QSPI_DISPLAY
    ret = drv_dpu_suspend(NULL);
    if (ret != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "drv dpu suspend error ret=%x\r\n", ret);
    }
#endif

    ret = uapi_lcd_deinit();
    if (ret != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "lcd deinit error ret=%x\r\n", ret);
        return ERRCODE_FAIL;
    }
    action_display_lcd_off_handle();
    lcd_set_status(false);
    g_pms_dpu_ready = false;
    display_suspend_power_off();
#ifdef CFG_DRIVERS_NANDFLASH
    nand_lock();
#endif
    uapi_pm_request_mode(PM_ID_VIDEO, PM_MODE_LP);
#ifdef CFG_DRIVERS_NANDFLASH
    nand_unlock();
#endif
    return ERRCODE_SUCC;
}

int32_t action_display_execute_entry(uint16_t action_bitmap, uint16_t new_state, event_type_t type, uint32_t event)
{
    unused(type);
    unused(event);

    screen_context_t *sc = power_display_get_screen_context();
    uint32_t ret;

    if (new_state == sc->current_state) {
        return 0;
    } else {
        POWER_DISPLAY_PRINT_DEBUG(0, "entry:action_bitmap:0x%x,current state:%u. \r\n", action_bitmap, sc->current_state);
    }
    if ((action_bitmap & GET_ACTION_MASK(SET_DISPLAY_ON)) != 0) {
        uapi_pm_add_sleep_veto(PM_ID_SCREEN);
        ret = action_display_lcd_on();
        uapi_pm_remove_sleep_veto(PM_ID_SCREEN);
        if (ret != 0) {
            POWER_DISPLAY_PRINT_ERR(0, "power_display_turn_on_screen fail, ret:%u.", ret);
            return ret;
        }
        POWER_DISPLAY_PRINT_DEBUG(0, "turn on success.");
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_DISPLAY_TO_IDLE_MODE)) != 0) {
        notify_screen_aod_on_event();
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_DISPLAY_TO_NORMAL_MODE)) != 0) {
        notify_screen_aod_off_event();
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_TE_ON)) != 0) {
        // rerserve
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_TE_OFF)) != 0) {
        // rerserve
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_DISPLAY_OFF)) != 0) {
        uapi_pm_add_sleep_veto(PM_ID_SCREEN);
        ret = action_display_lcd_off();
        uapi_pm_remove_sleep_veto(PM_ID_SCREEN);
        if (ret != 0) {
            POWER_DISPLAY_PRINT_ERR(0, "power_display_turn_on_screen fail, ret:%u.", ret);
            return ret;
        }
        POWER_DISPLAY_PRINT_DEBUG(0, "turn off success.");
    }
    sc->press_state = false;
    sc->current_state = new_state;

    return ret;
}
