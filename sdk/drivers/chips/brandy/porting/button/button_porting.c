/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides button port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-15， Create file. \n
 */

#include "button_porting.h"
#include "chip_core_irq.h"
#include "chip_io.h"
#include "gpio.h"
#include "pm_veto.h"
#include "debug_print.h"
#include "tcxo.h"
#include "clock_calibration.h"
#include "hal_gpio.h"
#include "pinctrl_porting.h"
#include "rgb_led.h"
#include "sh366102.h"

bool  g_startup_finish_flag = false;
uint16_t g_ulp_wkup_flag = 0;
static uint32_t g_clock_ulp = 3276800;
static uint8_t g_button_vetoed = 0;
uint8_t g_chargestatus = 0;

void set_startup_finish_flag(void)
{
    g_startup_finish_flag = true;
}

void uapi_ulp_wkup_type_record(void)
{
    if (g_startup_finish_flag == false) {
        g_ulp_wkup_flag = 0;
        g_ulp_wkup_flag = readw(ULP_WKUP_INT_STS);
        g_startup_finish_flag = true;
        PRINT("startup reason flag is 0x%x\r\n", g_ulp_wkup_flag);
    }
}

startup_reason_t uapi_get_startup_reason(void)
{
    startup_reason_t startup_type = STARTUP_OTHER;

    if (((g_ulp_wkup_flag >> WKUP_SYSTICK_MASK) & 1) == 1) {
        startup_type = STARTUP_SYSTICK;
    }
    if (((g_ulp_wkup_flag >> WKUP_PWRON_MASK) & 1) == 1) {
        startup_type = STARTUP_PWRON;
    }
    if (((g_ulp_wkup_flag >> WKUP_VBUS_EN_MASK) & 1) == 1) {
        startup_type = STARTUP_VBUS_EN;
    }
    return startup_type;
}

errcode_t uapi_power_set_mode(power_mode_t pdmode)
{
    if (pdmode >= PDMODE_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    if (pdmode == PDMODE_RE_UP) {
        reg16_clrbit(PWRON_PD_MODE_REG, PWRON_PD_MODE_POS4);
    } else if (pdmode == PDMODE_DOWN) {
        reg16_setbit(PWRON_PD_MODE_REG, PWRON_PD_MODE_POS4);
    }
    return ERRCODE_SUCC;
}

uint64_t uapi_ulp_systick_get_count(void)
{
    uint64_t count = 0;

    count = (uint64_t)readw(ULP_SYSTICK_VAL3) << 48U;
    count |= (uint64_t)readw(ULP_SYSTICK_VAL2) << 32U;
    count |= (uint64_t)readw(ULP_SYSTICK_VAL1) << 16U;
    count |= (uint64_t)readw(ULP_SYSTICK_VAL0);

    return count;
}

uint32_t uapi_ulp_systick_soc_calibration(void)
{
    unsigned int irq_sts = osal_irq_lock();
    uint16_t val = readw(CLK_32K_32M_EN_SEL_REG);
    reg16_clrbit(CLK_32K_32M_EN_SEL_REG, PMU_RC_512K_XO_SEL_MASK);
    reg16_clrbit(CLK_32K_32M_EN_SEL_REG, RCXO_32K_SEL_MASK);
    g_clock_ulp = calibration_get_clock_frq(CALIBRATION_CYCLE_MUL);
    writew(CLK_32K_32M_EN_SEL_REG, val);
    osal_irq_restore(irq_sts);
    return g_clock_ulp;
}

uint32_t uapi_ulp_systick_get_frequency(void)
{
    return g_clock_ulp;
}

uint64_t uapi_ulp_systick_get_ms(void)
{
    uint64_t count = 0;
    count = uapi_ulp_systick_get_count();
    uint64_t ms = count * MS_PER_S * CALIBRATION_CLOCK_MUL / g_clock_ulp;

    return ms;
}

errcode_t uapi_enable_alarm(uint64_t timeout_ms)
{
    uint64_t count = 0;
    count = uapi_ulp_systick_get_count();
    count += (timeout_ms * g_clock_ulp / (MS_PER_S * CALIBRATION_CLOCK_MUL));

    // alarm enable
    reg16_setbit(ULP_WKUP_EVT_EN, POS_2);
    reg16_setbit(ULP_WKUP_INT_EN, POS_2);

    writew(ULP_SYSTICK_SET3, (uint16_t)(count >> 48U));
    writew(ULP_SYSTICK_SET2, (uint16_t)(count >> 32U));
    writew(ULP_SYSTICK_SET1, (uint16_t)(count >> 16U));
    writew(ULP_SYSTICK_SET0, (uint16_t)count);

    return ERRCODE_SUCC;
}

errcode_t uapi_disable_alarm(void)
{
    reg16_clrbit(ULP_WKUP_EVT_EN, POS_2);
    reg16_clrbit(ULP_WKUP_INT_EN, POS_2);

    writew(ULP_SYSTICK_SET3, 0);
    writew(ULP_SYSTICK_SET2, 0);
    writew(ULP_SYSTICK_SET1, 0);
    writew(ULP_SYSTICK_SET0, 0);

    return ERRCODE_SUCC;
}

errcode_t uapi_power_time_mode(power_time_t time_mode)
{
    if (time_mode >= PDTIME_LEVELMAX) {
        return ERRCODE_INVALID_PARAM;
    }

    if (time_mode == PDTIME_LEVEL0) {
        reg16_setbit(PWRON_PD_MODE_REG, PWRON_TM_MODE_POS0);
        reg16_clrbit(PWRON_PD_MODE_REG, PWRON_TM_MODE_POS1);
    }
    if (time_mode == PDTIME_LEVEL1) {
        reg16_clrbit(PWRON_PD_MODE_REG, PWRON_TM_MODE_POS0);
        reg16_clrbit(PWRON_PD_MODE_REG, PWRON_TM_MODE_POS1);
    }
    if (time_mode == PDTIME_LEVEL2) {
        reg16_clrbit(PWRON_PD_MODE_REG, PWRON_TM_MODE_POS0);
        reg16_setbit(PWRON_PD_MODE_REG, PWRON_TM_MODE_POS1);
    }
    if (time_mode == PDTIME_LEVEL3) {
        reg16_setbit(PWRON_PD_MODE_REG, PWRON_TM_MODE_POS0);
        reg16_setbit(PWRON_PD_MODE_REG, PWRON_TM_MODE_POS1);
    }
    return ERRCODE_SUCC;
}

static button_press_state_t porting_button_get_state(pin_t pin)
{
    if (pin == PIN_NONE) {
        if ((readl(ANA_STS) & 0x3) == 0) {
            return PRESSED;
        } else {
            return RELEASED;
        }
    } else {
        if (uapi_gpio_get_val(pin) == GPIO_LEVEL_LOW) {
            return PRESSED;
        } else {
            return RELEASED;
        }
    }
}

static errcode_t porting_button_irq_register(pin_t pin, osal_irq_handler thread_handler)
{
    unused(pin);
    osal_irq_request(ULP_TO_MCU_IRQn, thread_handler, NULL, NULL, NULL);
    osal_irq_enable(ULP_TO_MCU_IRQn);

    writel(ULP_WKUP_INT_EN, ULP_WKUP_INT_EN_MASK);
    return ERRCODE_SUCC;
}

static errcode_t porting_button_unregister(pin_t pin)
{
    if (pin == PIN_NONE) {
        osal_irq_free(ULP_TO_MCU_IRQn, NULL);
    } else {
        uapi_gpio_unregister_isr_func(pin);
    }
    return ERRCODE_SUCC;
}

static bool porting_button_vbus_event_check(void)
{
    if (reg32_getbit(ULP_WKUP_INT_STS, VBUS_INSERT_INTERRUPT_POS) == 1 ||
        reg32_getbit(ULP_WKUP_INT_STS, VBUS_UNPLUG_INNTERRUPT_POS) == 1) {
        return true;
    }

    return false;
}

#define VBUS_DETECT_INTERVAL_MS 1000  // 检测间隔
#define TOTAL_VBUS_DETECT_INTERVAL_MS 5000  // 检测总时间
// 状态跟踪变量
static int plug_unplug_count = 0;        // 连续插拔次数计数
static long long last_event_time = 0;     // 上次事件时间戳（毫秒）
static int waiting_for_unplug = 0;        // 等待拔除状态（0：待拔除，1：待插入）
static long long first_event_time = 0;     // 第一次有效事件时间戳

// 重置检测状态
void reset_detection(void) {
    plug_unplug_count = 0;
    last_event_time = 0;
    waiting_for_unplug = 0;
    first_event_time = 0;
}

// 关机触发函数（您需要实现具体关机逻辑）
void trigger_shutdown(void) {
    // TODO: 在此处调用您准备的关机接口
    // 示例: system_shutdown();
    PRINT("trigger_shutdown~");

    // 关机后重置检测状态
    reset_detection();

    uapi_system_shutdown(0);
}

void insert_vbus(void)
{
    gpio_level_t state=GPIO_LEVEL_LOW;
    PRINT("insert_vbus");  // add user insert usb notify function here to change ui
	
    rgb_led_set_rgb(true,false,false);
    uapi_pin_set_mode(ULP_GPIO0, (pin_mode_t)HAL_PIO_FUNC_GPIO);
	uapi_gpio_set_dir(ULP_GPIO0, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(ULP_GPIO0, GPIO_LEVEL_HIGH);
    g_chargestatus = CHARGE_STATUS_CHARGING;
    ws_report_chg_status(CHARGE_STATUS_CHARGING);
    osal_timer_mod(&g_batt_check_timer,1000*10);
    long long current_time = uapi_ulp_systick_get_ms();

// 必须是等待插入状态（即上次是拔除事件）
    if (waiting_for_unplug) {
        // 检查时间间隔是否在VBUS_DETECT_INTERVAL_MS内
        if (last_event_time > 0 && (current_time - last_event_time) > VBUS_DETECT_INTERVAL_MS) {
            // 超过VBUS_DETECT_INTERVAL_MS，重置计数
            reset_detection();
            plug_unplug_count = 1;
            first_event_time = current_time;
        } else {
            // 有效连续事件，但计数在拔除时增加
        }

        // 更新最后事件时间
        last_event_time = current_time;

        // 如果是第一次事件，记录起始时间
        if (first_event_time == 0) {
            first_event_time = current_time;
        }

        // 切换为等待拔除状态
        waiting_for_unplug = 0;

        PRINT("insert_vbus %d,time:%lld", plug_unplug_count, current_time);
    } else {
        // 非预期插入事件（连续两次插入），重置状态
        reset_detection();
        // 重新开始计数，当前插入作为第一次
        plug_unplug_count = 0;
        waiting_for_unplug = 0;
        last_event_time = current_time;
        first_event_time = current_time;
    }
}

void unplug_vbus(void)
{
    PRINT("unplug_vbus");  // add user unplug usb notify function here to change ui
	
     rgb_led_set_rgb(false,false,false);
     g_chargestatus = CHARGE_STATUS_DISCHARGING;
     ws_report_chg_status(CHARGE_STATUS_DISCHARGING);
     osal_timer_mod(&g_batt_check_timer,1000*60);

     long long current_time = uapi_ulp_systick_get_ms();

    // 必须是等待拔除状态（即上次是插入事件）
    if (!waiting_for_unplug) {
        // 检查时间间隔是否在VBUS_DETECT_INTERVAL_MS内（非首次事件）
        if (last_event_time > 0 && (current_time - last_event_time) > VBUS_DETECT_INTERVAL_MS) {
            // 超过VBUS_DETECT_INTERVAL_MS，重置计数
            reset_detection();
            return;
        }

        // 增加插拔计数（在拔除时计数）
        plug_unplug_count++;

        // 更新最后事件时间
        last_event_time = current_time;

        // 切换为等待插入状态
        waiting_for_unplug = 1;

        PRINT("unplug_vbus %d,time:%lld", plug_unplug_count, current_time);

        // 检查是否达到5次且在2秒时间窗口内
        if (plug_unplug_count >= 5) {
            if (first_event_time > 0 && (current_time - first_event_time) <= TOTAL_VBUS_DETECT_INTERVAL_MS) {
                // 满足条件：2秒内连续5次，触发关机
                for(int i = 0;i<10;i++)
                {
                    rgb_led_toggle_blue();
                    uapi_tcxo_delay_ms(500);
                }
                trigger_shutdown();
            } else {
                // 超过2秒时间窗口，重置计数但保留当前状态
                if (first_event_time > 0 && (current_time - first_event_time) > TOTAL_VBUS_DETECT_INTERVAL_MS) {
                    reset_detection();
                }
            }
        }
    } else {
        // 非预期拔除事件（连续两次拔除），重置状态
        reset_detection();
    }
}

static void deal_with_vbus_event(void)
{
    if (reg32_getbit(ULP_WKUP_INT_STS, VBUS_INSERT_INTERRUPT_POS) == 1) {
        insert_vbus();
        return;
    }

    if (reg32_getbit(ULP_WKUP_INT_STS, VBUS_UNPLUG_INNTERRUPT_POS) == 1) {
        unplug_vbus();
        return;
    }
    return;
}

static bool porting_button_check_int(pin_t pin)
{
    unused(pin);
    if (reg32_getbit(ULP_WKUP_INT_STS, WKUP_PWRON_MASK) == 1 ||
        reg32_getbit(ULP_WKUP_INT_STS, VBUS_INSERT_INTERRUPT_POS) == 1 ||
        reg32_getbit(ULP_WKUP_INT_STS, VBUS_UNPLUG_INNTERRUPT_POS) == 1) {
        return true;
    }

    return false;
}

static bool porting_tick_check_int(pin_t pin)
{
    unused(pin);
    if (reg32_getbit(ULP_WKUP_INT_STS, 0x2) == 1) {
        return true;
    }

    return false;
}

static errcode_t porting_button_clear_int(pin_t pin)
{
    unused(pin);
    osal_irq_clear(ULP_TO_MCU_IRQn);
    writel(ULP_WKUP_INT_CLR, ULP_WKUP_INT_CLR_MASK);

    while (porting_button_check_int(pin) == true) {
        uapi_tcxo_delay_us(ULP_INT_CLEAR_DELAY);
        continue;
    }
    while (porting_button_vbus_event_check() == true) {
        uapi_tcxo_delay_us(ULP_INT_CLEAR_DELAY);
        continue;
    }
    while (porting_tick_check_int(pin) == true) {
        uapi_tcxo_delay_us(ULP_INT_CLEAR_DELAY);
        continue;
    }
    return ERRCODE_SUCC;
}

static errcode_t porting_button_pmu_pwr_cfg(pin_t pin)
{
    unused(pin);
    return ERRCODE_SUCC;
}

static void porting_button_add_veto(void)
{
    if (g_button_vetoed == 0) {
        uapi_pm_add_sleep_veto(PM_ID_BUTTON);
        g_button_vetoed = 1;
    }
}

static void porting_button_remove_veto(void)
{
    if (g_button_vetoed == 1) {
        uapi_pm_remove_sleep_veto(PM_ID_BUTTON);
        g_button_vetoed = 0;
    }
}

button_peripheral_api g_brandy_button_api = {
    .button_pmu_pwr_cfg = porting_button_pmu_pwr_cfg,
    .button_check_interrupt = porting_button_check_int,
    .button_check_vbus_interrupt = porting_button_vbus_event_check,
    .button_clear_interrupt = porting_button_clear_int,
    .button_deal_vbus_event = deal_with_vbus_event,
    .button_get_state = porting_button_get_state,
    .button_register_callback = porting_button_irq_register,
    .button_unregister_callback = porting_button_unregister,
    .button_add_veto = porting_button_add_veto,
    .button_remove_veto = porting_button_remove_veto,
};

button_peripheral_api *button_port_get_api(void)
{
    return &g_brandy_button_api;
}
