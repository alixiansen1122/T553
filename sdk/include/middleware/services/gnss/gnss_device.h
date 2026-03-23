/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

/**
 * @defgroup middleware_services_gnss GNSS API
 * @ingroup  middleware_services
 * @{
 */

#ifndef GNSS_DEVICE_H
#define GNSS_DEVICE_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Definition of GNSS callback types.
 * @else
 * @brief  GNSS回调函数类型定义。
 * @endif
 */
typedef enum {
    GNSS_CALLBACK_MSG = 0,          /*!< @if Eng Binary Message data type
                                     @else   二进制数据类型         @endif */
    GNSS_CALLBACK_NMEA,             /*!< @if Eng NMEA data type
                                     @else   NMEA数据类型           @endif */
} gnss_callback_enum_t;

#pragma pack(1)

/**
 * @if Eng
 * @brief  Definition of GNSS message data.
 * @note   Data should be aligned in 1-bytes
 * @else
 * @brief  GNSS数据类型定义。
 * @note   数据需要按1字节对齐
 * @endif
 */
typedef struct gnss_message {
    uint16_t  message_type;         /*!< @if Eng Message type, please see chip manual for reference
                                         @else   消息类型，请参考芯片手册                                @endif */
    uint16_t  message_chksum;       /*!< @if Eng Message checksum for .payload field, not include header 8-bytes
                                         @else   消息载荷的校验和，不包括消息头8字节                      @endif */
    uint16_t  message_sequence;     /*!< @if Eng Message sequence, default 0
                                         @else   消息序号，默认为0                                       @endif */
    uint16_t  payload_length;       /*!< @if Eng Message length for .payload field, not include header 8-bytes
                                         @else   消息载荷长度，不包括消息头8字节                          @endif */
    uint8_t   payload[0];           /*!< @if Eng Message payload
                                         @else   消息载荷                                               @endif */
} gnss_message_t;

#pragma pack()

/**
 * @if Eng
 * @brief  Prototype declaration of the callback function for receiving GNSS messages.
 * @param  [in]  buff Message buff.
 * @param  [in]  len Message len.
 * @note   Messages need to be localized before processing, avoiding time-consuming operations in callback.
 * @else
 * @brief  接收GNSS消息的回调函数的原型声明。
 * @param  [in]  buff 消息缓存。
 * @param  [in]  len 消息长度。
 * @note   需要将消息本地化后再处理，避免在回调中调用处理耗时操作。
 * @endif
 */
typedef void (*gnss_callback_func)(uint8_t *buff, uint32_t len);

/**
 * @if Eng
 * @brief  Interface for registering the GNSS message callback function.
 * @param  [in]  callback_type Callback message type, see @ref gnss_callback_enum_t.
 * @param  [in]  callback_func Callback function address, see @ref gnss_callback_func.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  GNSS消息回调函数注册接口。
 * @param  [in]  callback_type 回调消息类型，参考 @ref gnss_callback_enum_t。
 * @param  [in]  callback_func 回调函数地址，参考 @ref gnss_callback_func。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gnss_register_callback(uint32_t callback_type, gnss_callback_func callback_func);

/**
 * @if Eng
 * @brief  GNSS initialization interface, create GNSS task thread.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  GNSS初始化接口，创建GNSS任务线程。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gnss_init(void);

/**
 * @if Eng
 * @brief  GNSS open interface, open device node and load firmware.
 * @note   Ensure that the GNSS chip is powered on on the board before invoking the interface.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  GNSS加载接口，打开设备节点并加载固件。
 * @note   需要确保调用前板级已给GNSS芯片上电。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gnss_open(void);

/**
 * @if Eng
 * @brief  GNSS configuration interface.
 * @param  [in]  msg Pointer to the configuration item message, see @ref gnss_message_t.
 * @note   This interface needs to be called after the GNSS open interface is called.
 *         Some configuration items need to be called before the startup interface is called.
 *         For details, see the GNSS chip manual.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  GNSS配置接口。
 * @param  [in]  msg 配置项消息指针，参考 @ref gnss_message_t
 * @note   需在调用GNSS加载接口后调用此接口，部分配置项需在调用GNSS启动接口前调用，请参考GNSS芯片手册。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gnss_config(gnss_message_t *msg);

/**
 * @if Eng
 * @brief  GNSS startup interface, GNSS starts to report location information.
 * @note   This interface needs to be called after the initialization interface is called.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  GNSS启动接口，GNSS开始上报定位信息。
 * @note   需在调用初始化接口后调用。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gnss_start(void);

/**
 * @if Eng
 * @brief  GNSS stop interface, GNSS stop to report location information.
 * @note   This interface needs to be called after the startup interface is called.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  GNSS停止接口，GNSS停止上报定位信息。
 * @note   需在调用启动接口后调用。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gnss_stop(void);

/**
 * @if Eng
 * @brief  GNSS close interface, close device node.
 * @note   Board-level power-off of the GNSS chip can be performed only after this interface is called.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  GNSS卸载接口，关闭GNSS设备节点。
 * @note   调用此接口后方可执行GNSS芯片板级下电。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gnss_close(void);

/**
 * @if Eng
 * @brief  GNSS deinitialization interface，quit GNSS task thread.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  GNSS去初始化接口，退出任务线程。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gnss_deinit(void);

/**
 * @if Eng
 * @brief  GNSS power on interface.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  GNSS上电接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gnss_power_on(void);

/**
 * @if Eng
 * @brief  GNSS power off interface.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  GNSS下电接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gnss_power_off(void);

#ifdef __cplusplus
}
#endif
#endif

/**
 * @}
 */
