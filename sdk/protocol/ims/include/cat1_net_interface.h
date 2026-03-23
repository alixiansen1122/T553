/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 * Description: 4G NETWORK module.
 */

#ifndef CAT1_NET_INTERFACE_H
#define CAT1_NET_INTERFACE_H

#include "ril_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief Enum of 4G net state.
 * @else
 * @brief CAT1 网络状态
 * @endif
 */
typedef enum : uint8_t {
    CAT1_NET_STATE_OFF,                      /*!< @if Eng Offline. @else 离线 @endif */
    CAT1_NET_STATE_ON,                       /*!< @if Eng Online. @else 在线 @endif */
} cat1_net_state_t;

/**
 * @if Eng
 * @brief Enum of 4G call state.
 * @else
 * @brief CAT1 呼叫状态
 * @endif
 */
typedef enum : uint8_t {
    CAT1_CALL_STATE_OFF,                      /*!< @if Eng HangUp. @else 挂断 @endif */
    CAT1_CALL_STATE_ON,                       /*!< @if Eng Calling. @else 呼叫 @endif */
} cat1_call_state_t;

/**
 * @if Eng
 * @brief  Enum of bt status error code.
 * @else
 * @brief  定义4g cat1 network 状态类型。
 * @endif
 */
typedef enum : uint8_t {
    ERRCODE_CAT1_SUCCESS = 0x00,                /*!< @if Eng error code of success
                                                   @else   执行成功错误码 @endif */
    ERRCODE_CAT1_FAIL,                          /*!< @if Eng error code of failure
                                                   @else   执行失败错误码 @endif */
    ERRCODE_CAT1_MALLOC_FAIL,                   /*!< @if Eng error code of memmery not enough
                                                   @else   内存不足错误码 @endif */
    ERRCODE_CAT1_MEMCPY_FAIL,                   /*!< @if Eng error code of memcpy fail
                                                   @else   内存拷贝错误错误码 @endif */
    ERRCODE_CAT1_RECV_FAIL,                     /*!< @if Eng error code of system is busy
                                                   @else   接收错误码 @endif */
    ERRCODE_CAT1_PARAM_ERR,                     /*!< @if Eng error code of invalid param
                                                   @else   无效参数错误码 @endif */
    ERRCODE_CAT1_TIMEOUT,                       /*!< @if Eng error code of state error
                                                   @else   超时错误 @endif */
    ERRCODE_CAT1_SEND_FAIL,                     /*!< @if Eng error code of unhandle
                                                   @else   发送错误错误码 @endif */
    ERRCODE_CAT1_CONNECT_FAIL,                  /*!< @if Eng error code of remote device down
                                                   @else   连接错误错误码 @endif */
    ERRCODE_CAT1_MAX                            /*!< @if Eng the max of bt error code
                                                   @else   CAT1错误码最大值 @endif */
} errcode_cat1_net_t;

/**
 * @if Eng
 * @brief Open the 4g cat1 network service.
 * @par Description:
 *            Open the 4g cat1 network service.
 * @retval #ERRCODE_CAT1_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_cat1_net_t
 * @else
 * @brief 打开4g cat1 network服务
 * @par 说明:
 *           打开4g cat1 network服务
 * @retval #ERRCODE_CAT1_SUCCESS   成功.
 * @retval Other                失败. 参考 @ref errcode_cat1_net_t
 * @endif
 */
int cat1_network_service_open(void);

/**
 * @if Eng
 * @brief Close the 4g cat1 network service.
 * @par Description:
 *            Close the 4g cat1 network service.
 * @retval #ERRCODE_CAT1_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_cat1_net_t
 * @else
 * @brief 关闭4g cat1 network服务
 * @par 说明:
 *           关闭4g cat1 network服务

 * @retval #ERRCODE_CAT1_SUCCESS   成功.
 * @retval Other                失败. 参考 @ref errcode_cat1_net_t
 * @endif
 */
int cat1_network_service_close(void);

/**
 * @if Eng
 * @brief Send the ethernet.
 * @par Description:
 *            Send the ethernet.
 * @param  [in]  ip_packet A poniter of the ethernet data.
 * @param  [in]  ip_length The length of the ethernet data.
 * @retval #ERRCODE_CAT1_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_cat1_net_t
 * @else
 * @brief 发送网络数据包
 * @par 说明:
 *           发送网络数据包
 * @param  [in]  ip_packet 网络数据包指针
 * @param  [in]  ip_length 网络数据包长度
 * @retval #ERRCODE_CAT1_SUCCESS   成功.
 * @retval Other                失败. 参考 @ref errcode_cat1_net_t
 * @endif
 */
int cat1_network_service_write_data(const unsigned char *ip_packet, const unsigned short ip_length);

/**
 * @if Eng
 * @brief Get the net state.
 * @par Description:
 *            Get the net state.
 * @retval #CAT1_NET_STATE_OFF   offline
 * @retval #CAT1_NET_STATE_ON    online
 * @else
 * @brief  获取网络连接状态
 * @par 说明:
 *           获取网络连接状态
 * @retval #CAT1_NET_STATE_OFF   离线
 * @retval #CAT1_NET_STATE_ON    在线
 * @endif
 */
unsigned char cat1_network_service_net_state_get(void);

/**
 * @if Eng
 * @brief The call back of net state change.
 * @par Description:
 *           The call back of net state change.
 * @param  [in]  net_state    See @ref cat1_net_state_t
 * @retval No return value. See @ref void
 * @else
 * @brief  4g cat1 网络连接状态通知
 * @par 说明:
 *           注册该回调函数之后，4G服务网络连接状态反馈上层应用。
 * @param  [in]  net_state    参考 @ref cat1_net_state_t
 * @retval 无返回值。参考 @ref void
 * @endif
 */
typedef void (*cat1_net_state_callback)(unsigned char net_state);

/**
 * @if Eng
 * @brief The call back of ethernet.
 * @par Description:The call back of ethernet.
 * @param  [in]  packet  a pointer to net data.
 * @param  [in]  packet_length  the length of net data.
 * @retval No return value. See @ref void
 * @else
 * @brief  读取网络数据包通知
 * @par 说明:注册该回调函数之后，读取网络数据包结果反馈上层应用。
 * @param  [in]  packet  指针，指向网络数据包所在内存地址.
 * @param  [in]  packet_length  网络数据包长度.
 * @retval 无返回值。参考 @ref void
 * @endif
 */
typedef void (*cat1_rpt_data_callback)(unsigned char *packet, unsigned short packet_length);

/**
 * @if Eng
 * @brief The call back of call state change.
 * @par Description:
 *           The call back of call state change.
 * @param  [in]  call_state    See @ref cat1_call_state_t
 * @retval No return value. See @ref void
 * @else
 * @brief  4g cat1 呼叫状态通知
 * @par 说明:
 *           注册该回调函数之后，4G呼叫状态反馈上层应用。
 * @param  [in]  call_state    参考 @ref cat1_call_state_t
 * @retval 无返回值。参考 @ref void
 * @endif
 */
typedef void (*cat1_call_state_callback)(unsigned char call_state);

/**
 * @if Eng
 * @brief Struct of 4g callback function.
 * @else
 * @brief 4g cat1回调接口定义。
 * @endif
 */
typedef struct {
    cat1_net_state_callback net_state_cb;
    cat1_rpt_data_callback rpt_data_cb;
    cat1_call_state_callback call_state_cb;
} cat1_net_callbacks_t;

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of framework.
 * @par Description:
 *           Use this funtion to register callback function of framework.
 * @param  [in]  func   A poniter of the callback function. See @ref cat1_net_callbacks_t
 * @retval #ERRCODE_CAT1_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_cat1_net_t
 * @else
 * @brief  注册上层应用的回调。
 * @par 说明:
 *           注册上层应用的回调。
 * @param  [in]  func   回调函数指针。参考 @ref cat1_net_callbacks_t
 * @retval #ERRCODE_CAT1_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_cat1_net_t
 * @endif
 */
int cat1_net_register_callbacks(cat1_net_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of framework.
 * @par Description:
 *           Use this funtion to deregister callback function of framework.
 * @param  [in]  func   A poniter of the callback function. See @ref cat1_net_callbacks_t
 * @retval #ERRCODE_CAT1_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_cat1_net_t
 * @else
 * @brief  去注册上层应用的回调。
 * @par 说明:
 *           去注册上层应用的回调。
 * @param  [in]  func   回调函数指针。参考 @ref cat1_net_callbacks_t
 * @retval #ERRCODE_CAT1_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_cat1_net_t
 * @endif
 */
int cat1_net_deregister_callbacks(void);

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* end of cat1_net_interface.h */