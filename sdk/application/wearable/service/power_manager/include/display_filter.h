/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: Power display fliter header file.
 * Author:
 * Create:
 */

#ifndef POWERMGR_DISPLAY_FILTER_H
#define POWERMGR_DISPLAY_FILTER_H

#include "power_display_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief 过滤器定义
 */
typedef struct event_filter {
    /* * 系统定义规则，用于定义系统默认行为 */
    execute_rule system_rule;
    /* * 产品自定义规则，用于每个产品实现独有特性 */
    execute_rule product_rule;
} event_filter_t;

/**
 * @brief 获取图形用户界面过滤转换接口.
 *
 * @return 图形用户界面过滤转换接口.
 */
event_filter_t *display_get_gui_filter(void);

/**
 * @brief 获取应用过滤转换接口.
 *
 * @return 应用过滤转换接口.
 */
event_filter_t *display_get_slice_filter(void);

/**
 * @brief 获取定时器过滤转换接口.
 *
 * @return 定时器过滤转换接口.
 */
event_filter_t *display_get_timer_filter(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
