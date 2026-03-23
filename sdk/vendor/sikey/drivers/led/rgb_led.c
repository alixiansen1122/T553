#include "thread_init.h"
#include "tcxo.h"
#include "log_common.h"
#include "log_def.h"
#include "log_uart.h"
#ifdef __LITEOS__
#include "los_status.h"
#endif
#include "pinctrl_porting.h"
#include "gpio.h"
#include "pinctrl.h"
#include "hal_gpio.h"
#include <time.h>
#include <sys/time.h>
#include "pinctrl_porting.h"
#include "soc_osal.h"
#include "rgb_led.h"

// GPIO引脚配置 - 根据实际硬件修改
#define GPIO_RED    S_AGPIO_L0
#define GPIO_GREEN  S_AGPIO_L1
#define GPIO_BLUE   S_AGPIO_L2

osal_task *rgb_led_blink_task_id = NULL;
osal_semaphore led_sem;
osal_timer led_blink_timer={0};

#define LED_BLINK_TIME 500
// 当前LED状态
static led_state_t current_state = {false, false, false};

// 闪烁任务相关变量
static bool blink_task_running = false;

// GPIO初始化
static void gpio_init(void)
{
    uapi_pin_set_mode(GPIO_RED, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(GPIO_RED, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_RED, GPIO_LEVEL_LOW);

    uapi_pin_set_mode(GPIO_GREEN, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(GPIO_GREEN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_GREEN, GPIO_LEVEL_LOW);

    uapi_pin_set_mode(GPIO_BLUE, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(GPIO_BLUE, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_BLUE, GPIO_LEVEL_LOW);
}



void rgb_led_set_red(bool state)
{
    uapi_pin_set_mode(GPIO_RED, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(GPIO_RED, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_RED, state ? 1 : 0);
    current_state.red = state;
}

void rgb_led_set_green(bool state)
{
    uapi_pin_set_mode(GPIO_GREEN, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(GPIO_GREEN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_GREEN, state ? 1 : 0);
    current_state.green = state;
}

void rgb_led_set_blue(bool state)
{
    uapi_pin_set_mode(GPIO_BLUE, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(GPIO_BLUE, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_BLUE, state ? 1 : 0);
    current_state.blue = state;
}



void rgb_led_set_rgb(bool red, bool green, bool blue)
{
    rgb_led_set_red(red);
    rgb_led_set_green(green);
    rgb_led_set_blue(blue);
}

void rgb_led_toggle_red(void)
{
    rgb_led_set_red(!current_state.red);
}

void rgb_led_toggle_green(void)
{
    rgb_led_set_green(!current_state.green);
}

void rgb_led_toggle_blue(void)
{
    rgb_led_set_blue(!current_state.blue);
}

led_state_t rgb_led_get_state(void)
{
    return current_state;
}
void led_blue_blink_start(void)
{
    if(blink_task_running == false)
    {
    osal_timer_start(&led_blink_timer);
    blink_task_running = true;
    }
}
void led_blue_blink_stop(void)
{
    if(blink_task_running == true)
    {
        osal_timer_stop(&led_blink_timer);
        blink_task_running = false;
    }
    rgb_led_set_blue(false);
}


void led_blink_timer_handler(void)
{

	osal_sem_up(&led_sem);


	osal_timer_mod(&led_blink_timer,LED_BLINK_TIME);
}

static int blink_task_handler(void *arg)
{
    int64_t ret = 0;

    (void)arg;

    while (1) {

            osal_sem_down(&led_sem);
            rgb_led_toggle_blue();
    }

    return 0;
}

void rgb_led_init(void)
{
    int ret = 0;
    gpio_init();
    rgb_led_set_rgb(false, false, false);
    current_state.red = false;
    current_state.blue = false;
    current_state.green = false;

    led_blink_timer.handler = led_blink_timer_handler;
    led_blink_timer.interval = 500;
	ret=osal_timer_init(&led_blink_timer);
    if(ret!=OSAL_SUCCESS)
    {
        printf("osal_timer_init failed\n");
    }
	else
	{
		//osal_timer_start(&led_blink_timer);
	}


    osThreadAttr_t threadAttr={0};
	memset(&threadAttr, 0, sizeof(threadAttr));
    threadAttr.name = "rgb_led_blink_task";
    threadAttr.stack_size = 0x2000;
    threadAttr.priority = 17;
    threadAttr.stack_mem = memalign(16, threadAttr.stack_size);

    rgb_led_blink_task_id = osThreadNew(blink_task_handler, NULL, &threadAttr);
    if ( rgb_led_blink_task_id ==  NULL) {
        printf("osal_kthread_create rgb_led_blink_task failed\r\n");

    }
    osal_sem_init(&led_sem, 0);
}

