/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay api module.
 * Author:
 * Create:
 */

#ifndef ALIPAY_FEATURE_H
#define ALIPAY_FEATURE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <stdint.h>
#include <stdio.h>
#include "stdbool.h"
#include "errcode.h"
#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG)
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#else
#include "debug_print.h"
#endif

#define APP_SUCCESS 0
#define APP_FAILURE (-1)

#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG)
#define ALIPAY_PRINT_ERR(log_num, fmt, arg...) uapi_diag_error_log(0, fmt, ##arg)
#define ALIPAY_PRINT_WARN(log_num, fmt, arg...) uapi_diag_warning_log(0, fmt, ##arg)
#define ALIPAY_PRINT_INFO(log_num, fmt, arg...) uapi_diag_info_log(0, fmt, ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define ALIPAY_PRINT_DEBUG(log_num, fmt, arg...) uapi_diag_debug_log(0, fmt, ##arg)
#else
#define ALIPAY_PRINT_DEBUG(log_num, fmt, arg...)
#endif
#else
#define ALIPAY_PRINT_ERR(log_num, fmt, arg...) PRINT("[ALIPAY SVR E/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#define ALIPAY_PRINT_WARN(log_num, fmt, arg...) PRINT("[ALIPAY SVR W/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#define ALIPAY_PRINT_INFO(log_num, fmt, arg...) PRINT("[ALIPAY SVR I/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define ALIPAY_PRINT_DEBUG(log_num, fmt, arg...) PRINT("[ALIPAY SVR D/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#else
#define ALIPAY_PRINT_DEBUG(log_num, fmt, arg...)
#endif
#endif

#define ALIPAY_INTER_STATUS_BINDING_FAIL 0  /* bind failure */
#define ALIPAY_INTER_STATUS_UNBINDED 1      /* unbind */
#define ALIPAY_INTER_STATUS_START_BINDING 2 /* start binding */
#define ALIPAY_INTER_STATUS_BINDING_OK 0xa5 /* bind successfully */
#define ALIPAY_INTER_STATUS_UNKNOWN 0xff    /* unknown state */

#define ALIPAY_TRANS_CARD_MEMBER_LEN 40 /* car str size */
#define ALIPAY_TRANS_CARD_SHIFT 1
#define ALIPAY_TRANS_CODE_SHIFT 2
#define ALIPAY_TRANS_CODE_MAX_LEN 512 /* car str size */
#define ALIPAY_TRANS_CARD_MAX_LENGTH 20
#define ALIPAY_TRANS_CARD_MIN_VALID_NUM 3

typedef enum {
    PAYMENT_UNSUPPORTED = 1,  /* NOT support payment */
    PAYMENT_UNBIND,           /* support but not bind */
    PAYMENT_UNBIND_BT_CHANGE, /* the state change to unbind because of changing of bt bind */
    PAYMENT_BIND_LOCKED,      /* bind but locked by security reason */
    PAYMENT_BIND_ACTIVE,      /* bind and active */
    PAYMENT_UNKNOWN           /* invalid state */
} mc_payment_state_t;

typedef enum {
    PAY_BIND_HISOTRY_STATE = 0x01,
    PAY_BIND_RST
} mc_pay_bind_data_t;

typedef enum {
    WD_ALG_WEAR_OFF = 0x00,
    WD_ALG_WEAR_ON = 0x01,
    WD_ALG_WEAR_UNKNOWN = 0xF,
} mc_app_wear_status;

typedef struct {
    char card_no[ALIPAY_TRANS_CARD_MEMBER_LEN];   /* card number */
    char card_type[ALIPAY_TRANS_CARD_MEMBER_LEN]; /* card type */
    char title[ALIPAY_TRANS_CARD_MEMBER_LEN];     /* card title */
} mc_alipay_trans_card_info_t;

typedef struct {
    bool card_exits;
    int32_t expire_timestamp;
    int32_t remain_use_count; /* remaining trips for the next 12 hours */
} mc_alipay_tans_card_status_t;

typedef enum {
    ALIPAY_TRANS_SUCCESS = 0,
    ALIPAY_TRANS_CARD_LIST_START = ALIPAY_TRANS_CARD_SHIFT << 8,
    ALIPAY_TRANS_CARD_LIST_UNBIND = ALIPAY_TRANS_CARD_LIST_START, // 设备已经在服务端解绑，设备端也需要调用解绑接口
    ALIPAY_TRANS_CARD_LIST_NET_ERROR,                             // 网络错误
    ALIPAY_TRANS_CARD_LIST_OWN_TOO_MANY_CARD,                     // 用户开通了太多乘车码
    ALIPAY_TRANS_CARD_LIST_FUNC_NOT_SUPPPORT,                     // 不支持该功能
    ALIPAY_TRANS_CARD_LIST_DEV_ERROR,                             // 不该出现的错误
    ALIPAY_TRANS_CARD_LIST_BUF_TOO_SHORT,                         // 入参的缓存太小
    ALIPAY_TRANS_CODE_START = ALIPAY_TRANS_CODE_SHIFT << 8,
    ALIPAY_TRANS_CODE_UNBIND = ALIPAY_TRANS_CODE_START, // 设备已经在服务端解绑，设备端也需要调用解绑接口
    ALIPAY_TRANS_CODE_NET_ERROR,                        // 网络错误
    ALIPAY_TRANS_CODE_DATA_LIMIT,                       // 因安全原因限制生码
    ALIPAY_TRANS_CODE_NOT_SUPPORT,                      // 该乘车码不支持
    ALIPAY_TRANS_CODE_SERVER_ERROR,                     // 服务端报错
    ALIPAY_TRANS_CODE_DEV_ERROR,                        // 不该出现的错误

} mc_alipay_trans_errcode_t;

typedef enum {
    ALIPAY_MSG_BT = 0,
    ALIPAY_MSG_UPDATA_TRANS_LIST,
    ALIPAY_MSG_UPDATA_SPECIAL_CARD,
    ALIPAY_MSG_UPDATA_ALL_CARDS,

} mc_alipay_msg_type_t;

typedef struct {
    mc_alipay_trans_card_info_t* card_info;
    uint32_t card_size;
    uint32_t card_len;
} mc_alipay_trans_context_t;

/**
 * @brief 消息事件回调函数
 */
typedef void (*alipay_msgf_event_callback_func)(uint32_t msgid, uint32_t retcode);

typedef struct alipay_svr_api {
    /*
     * @ingroup service api
     * @brief get payment status
     * @param [input] NA
     * @return current mc_payment_state_t
     */
    mc_payment_state_t (*get_paystate)(void);

    /*
     * @ingroup service api
     * @brief UI notify BT open or close alipay broadcast
     * @param[in] value: 1-open;0-close.
     * @return 0-success; other-failure.
     */
    int32_t (*set_bt_broadcast)(bool value);

    /*
     * @ingroup service api
     * @brief get logonId from alipay.
     * @param [out] alipay_logon_id_buf: logonId address,recommended buffer is 128.
     * @param [inout]alipay_logon_id_len:Maximum allowable read length at input, actual length at output.
     * @return 0-success; other-failure.
     */
    int32_t (*get_logon_id)(uint8_t *alipay_logon_id_buf, uint32_t *alipay_logon_id_len);

    /*
     * @ingroup service api
     * @brief get Nickname from alipay.
     * @param [out] alipay_nickname_buf: nickname address. recommended buffer is 128.
     * @param [inout] alipay_nickname_len:Maximum allowable read length at input, actual length at output.
     * @return 0-success; other-failure.
     */
    int32_t (*get_nickname)(uint8_t *alipay_nickname_buf, uint32_t *alipay_nickname_len);

    /*
     * @ingroup service api
     * @brief Gets whether it is bound.
     * @param[in] NA
     * @return true, false
     */
    bool (*get_bind_state)(void);

    /*
     * @ingroup service api
     * @brief Get the current binding status
     * @param[status]
     * ALIPAY_INTER_STATUS_BINDING_FAIL :bind failure
     * ALIPAY_INTER_STATUS_UNBINDED :unbind
     * ALIPAY_INTER_STATUS_START_BINDING :start binding
     * ALIPAY_INTER_STATUS_BINDING_OK :bind successfully
     * ALIPAY_INTER_STATUS_UNKNOWN :unknown state
     * @return @see retval_e:function is mainly concerned with status.
     */
    int32_t (*get_bind_status)(int32_t *status);

    /*
     * @ingroup service api
     * @brief bind start
     * @param[in] NA
     * @return NULL.
     */
    void (*bind_start)(void);

    /*
     * @ingroup service api
     * @brief bind finish
     * @param[in] NA
     * @return NULL.
     */
    void (*bind_finish)(void);

    /*
     * @ingroup service api
     * @brief get bind code string,for generating QR codes.
     * @param [out] str_buf: bind code string address. recommended buffer is 256.
     * @param [inout] str_len: Maximum allowable read length at input, actual length at output.
     * @return 0-success; other-failure.
     */
    int32_t (*get_bind_str)(uint8_t *str_buf, int32_t *str_len);

    /*
     * @ingroup service api
     * @brief get pay code string,for generating QR codes and barcodes.
     * @param [out] str_buf: bind code string address. recommended buffer is 20.
     * @param [inout] str_len: Maximum allowable read length at input, actual length at output.
     * @return 0-success; other-failure.
     */
    int32_t (*get_pay_str)(uint8_t *str_buf, uint32_t *str_len);

    /*
     * @ingroup service api
     * @brief get aid code string,for generating QR codes.
     * @param [out] str_buf: bind code string address. recommended buffer is 256.
     * @param [inout] str_len: Maximum allowable read length at input, actual length at output.
     * @return 0-success; other-failure.
     */
    int32_t (*get_aid_str)(int8_t *str_buf, uint32_t *str_len);

    /*
     * @ingroup service api
     * @brief remove bind
     * @param[in] NA
     * @return APP_SUCCESS, APP_FAILURE
     */
    int32_t (*remove_bind)(void);

    /*
     * @ingroup service api
     * @brief handling ble message interface
     * @param [in] pbuf: ble msg address.
     * @param [in] len: ble msg len.
     * @return APP_SUCCESS, APP_FAILURE
     */
    int32_t (*proc_packet)(uint8_t *pbuf, uint32_t len);
    /*
     * @ingroup service api
     * @brief The Alipay sdk is forced to do a wakeup and then use it when using the crypto chip,
     * so there is generally no need to explicitly call the interface.
     * @return NULL
     */
    void (*exit_lpm)(void);

    /*
     * @ingroup service api
     * @brief Low power mode is required when not using the Alipay service.
     * @return NULL
     */
    void (*enter_lpm)(void);
    /*
     * @ingroup service api
     * @param [out] title: Card Title, Cache Size Recommendation 40.
     * @param [out] transitcode: Cache for storing code values, buffer size recommended 512, bytecode, not string.
     * @param [inout] len: Input is the maximum length allowed to be fetched, and output is the actual length fetched.
     * @brief Get the most recently used ride code code value, which will be cleared after power failure.
     * @return @see retval_e
     */
    int32_t (*get_last_transcode)(char *title, uint8_t *transitcode, uint32_t *len);
    /*
     * @ingroup service api
     * @brief Get a list of locally cached cities.
     * @param [out] card_info: City list info, @see mc_alipay_trans_card_info_t.
     * @param [inout] card_size:The maximum length allowed to be read when passed in, and the actual length obtained
     * when passed out.
     * @param [out] card_len: The actual number of cards, the return value is RV_BUF_TOO_SHORT, this parameter is still
     * valid for the actual number of cards.
     * @return @see retval_e
     */
    int32_t (*get_local_translist)(mc_alipay_trans_card_info_t *card_info, uint32_t *card_size, uint32_t *card_len);
    /*
     * @ingroup service api
     * @brief Get the list of cities on the server side.
     * @param [out] card_info: City list info, @see mc_alipay_trans_card_info_t.
     * @param [inout] card_size:The maximum length allowed to be read when passed in, and the actual length obtained
     * when passed out.
     * @param [out] card_len: The actual number of cards, the return value is RV_BUF_TOO_SHORT, this parameter is still
     * valid for the actual number of cards.
     * @return @see retval_e
     */
    int32_t (*get_online_translist)(mc_alipay_trans_card_info_t *card_info, uint32_t *card_size, uint32_t *card_len);
    /*
     * @ingroup service api
     * @brief Update the data of the specified card.
     * @param [in] card_info: specified card info, @see mc_alipay_trans_card_info_t.
     * @return @see retval_e
     */
    int32_t (*update_trans_cardinfo)(mc_alipay_trans_card_info_t *card_info);
    /*
     * @ingroup service api
     * @brief Check the status information of the specified card.
     * @param [in] card_info: specified card info, @see mc_alipay_trans_card_info_t.
     * @param [out] card_status: Current status of the ride card, @see mc_alipay_tans_card_status_t.
     * @return @see retval_e
     */
    int32_t (*check_trans_cartstatus)(mc_alipay_trans_card_info_t *card_info, mc_alipay_tans_card_status_t *card_status);
    /*
     * @ingroup service api
     * @brief Get the ride code code value of the specified card.
     * @param [in] card_info: specified card info, @see mc_alipay_trans_card_info_t.
     * @param [out] trans_code: Cache for storing code values, buffer size recommended 512, bytecode, not string.
     * @param [inout] len_trans_code: Input is the maximum length allowed to be fetched, and output is the actual length
     * fetched.
     * @return @see retval_e
     */
    int32_t (*get_transcode)(mc_alipay_trans_card_info_t *card_info, uint8_t *trans_code, uint32_t *len_trans_code);
} alipay_svr_api_t;

/**
 * @brief Get Alipay service API interface.
 *
 * @return Alipay service API interface.
 */
const alipay_svr_api_t *alipay_svr_get_api(void);

/**
 * @brief Pre-application launch messaging task creation.
 *
 * @return APP_SUCCESS, APP_FAILURE.
 */

errcode_t svr_alipay_create_task(void);

/**
 * @brief End-of-application message task destruction.
 *
 * @return @see errcode_t.
 */
errcode_t svr_alipay_destroy_task(void);

/*
 * @brief Send a message to Alipay Messaging Tasks.
 * @param [in] msg_id: msg id.
 * @param [in] msg: message address.
 * @param [in] msg_len: message length.
 * @param [in] wait: Whether write blocking is required.
 * @return  @see errcode_t
 */
errcode_t svr_alipay_msg_write(uint16_t msg_id, uint8_t *msg, uint16_t msg_len, bool wait);

/*
 * @brief Send a message to gatt client.
 * @param [in] data: data buff.
 * @param [in] data_len: data length.
 */
void svr_alipay_bt_gatts_noyify(uint8_t *data, uint16_t data_len);

/*
 * @brief alipay BLE module initialization.
 * @return  NULL.
 */
void svr_alipay_bluetooth_init(void);

/*
 * @brief alipay BLE adv start.
 * @return  NULL.
 */
void svr_alipay_ble_adv_start(void);

/*
 * @brief alipay BLE adv stop.
 * @return  NULL.
 */
void svr_alipay_ble_adv_stop(void);

/*
 * @brief alipay svr init.
 * @return @see errcode_t.
 */
errcode_t uapi_alipay_svr_init(void);

/*
 * @brief alipay svr reset.
 * @return @see errcode_t.
 */
errcode_t uapi_alipay_svr_reset(void);

/*
 * @brief alipay init trans context.
 * @return @see errcode_t.
 */
uint32_t svr_alipay_init_trans_context(void);

/*
 * @brief alipay deinit trans context.
 * @return NULL.
 */
void svr_alipay_deinit_trans_context(void);

/*
 * @brief get alipay trans context.
 * @return @see mc_alipay_trans_context_t.
 */
mc_alipay_trans_context_t * svr_alipay_get_trans_context(void);

/*
 * @brief Registration alipay message completion notification callback
 * @param [in] callback: callback func.
 * @return @see mc_alipay_trans_context_t.
 */
uint32_t svr_alipay_register_cb(alipay_msgf_event_callback_func callback);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* FEATURE_ALIPAY_API_H */
