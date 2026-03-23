#ifndef RGB_LED_H
#define RGB_LED_H

#include <stdint.h>
#include <stdbool.h>


// LED状态结构
typedef struct {
    bool red;
    bool green;
    bool blue;
} led_state_t;

// 初始化函数
void rgb_led_init(void);

// 基础控制函数
void rgb_led_set_red(bool state);
void rgb_led_set_green(bool state);
void rgb_led_set_blue(bool state);

// 高级控制函数
void rgb_led_set_rgb(bool red, bool green, bool blue);
void rgb_led_toggle_red(void);
void rgb_led_toggle_green(void);
void rgb_led_toggle_blue(void);

// 获取当前状态
led_state_t rgb_led_get_state(void);

// 闪烁控制函数
void led_blue_blink_start(void);
void led_blue_blink_stop(void);

#endif // RGB_LED_H