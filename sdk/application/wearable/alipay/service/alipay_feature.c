/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay feature.
 * Author:
 * Create:
 */

#include "alipay_feature.h"
#include "stdlib.h"
#include "securec.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "aid_code.h"
#include "alipay_account_manage.h"
#include "alipay_bind.h"
#include "alipay_pay.h"
#include "alipay_pay_result.h"
#include "soc_errno.h"
#include "iotsec.h"
#include "IIC_Master_Driver.h"
#include "product.h"
#include "gpio.h"
#include "alipay_common.h"
#include "osal_msgqueue.h"
#include "thread_init.h"
#include "alipay_transit.h"
#include "vendor_file.h"
#if defined(__FREERTOS__)
#include "FreeRTOS.h"
#endif

#define ALIPAY_BIND_STR_MAX_NUM 256
#define ALIPAY_QUEUE_MAX_SIZE 32
#define ALIPAY_MAX_SIZE 0x80
#define ALIPAY_ID_LEN sizeof(uint16_t)
#define ALIPAY_MSG_LEN sizeof(uint16_t)
#define ALIPAT_MSG_STACK_SIZE 0x6000
#define TASK_PRIORITY_ALIPAY_APP (osPriority_t)(18)
#define ALIPAT_ERROR_MSG_LEN 256

typedef struct alipay_ctrl_info {
    mc_payment_state_t payment_state;
    uint8_t wear_state;
    bool bt_connected;
} alipay_ctrl_info_t;

typedef struct {
    osSemaphoreAttr_t attr;
    uint32_t maxcount;
    uint32_t initialcount;
} semaphoreex_t;

/* os semaphore id */
static osSemaphoreId_t g_alipay_sem_id;
semaphoreex_t g_alipay_sem = { { "AlipaySem", 0, NULL, 0 }, 1, 1 };
static bool g_alipay_init_flag = false;
static unsigned long g_alipay_queue_id = 0xFFFFFFFF;
static uint32_t *g_alipay_task_handle = NULL;
static uint32_t g_alipay_alow_read_write = 0;

static uint32_t g_alipay_full_msgnum = 0;
static uint32_t g_alipay_writeback_msgnum = 0;
static uint32_t g_alipay_recv_msgnum = 0;
osThreadAttr_t g_alipay_attr = {
    "alipay_task", 0, NULL, 0, NULL, ALIPAT_MSG_STACK_SIZE, TASK_PRIORITY_ALIPAY_APP, 0, 0
};

static mc_payment_state_t svr_alipay_get_paystate(void)
{
    return PAYMENT_UNSUPPORTED;
}

static int32_t svr_alipay_notify_bt(bool value)
{
    if (value) {
        svr_alipay_ble_adv_start();
    } else {
        svr_alipay_ble_adv_stop();
    }
    return 0;
}

static int32_t svr_alipay_get_logonid(uint8_t *alipay_logonid_buf, uint32_t *alipay_logonid_len)
{
    if ((alipay_logonid_buf == NULL) || (alipay_logonid_len == NULL)) {
        return APP_FAILURE;
    }
    retval_e ret = alipay_get_logon_ID(alipay_logonid_buf, alipay_logonid_len);
    if (ret != ALIPAY_RV_OK) {
        return APP_FAILURE;
    }
    return APP_SUCCESS;
}

static int32_t svr_alipay_get_nickname(uint8_t *alipay_nickname_buf, uint32_t *alipay_nickname_len)
{
    if ((alipay_nickname_buf == NULL) || (alipay_nickname_len == NULL)) {
        return APP_FAILURE;
    }

    retval_e ret = alipay_get_nick_name(alipay_nickname_buf, alipay_nickname_len);
    if (ret != ALIPAY_RV_OK) {
        return APP_FAILURE;
    }
    return APP_SUCCESS;
}

static bool svr_alipay_get_bind_state(void)
{
    bool state = alipay_get_binding_status();
    return state;
}

static int32_t svr_alipay_get_bind_status(int32_t *status)
{
    if (status == NULL) {
        return APP_FAILURE;
    }
    retval_e ret = alipay_query_binding_result(status);
    if (ret != ALIPAY_RV_OK) {
        return ret;
    }
    return APP_SUCCESS;
}

static int32_t svr_alipay_remove_bind(void)
{
    retval_e ret = alipay_unbinding();
    if (ret != ALIPAY_RV_OK) {
        return APP_FAILURE;
    }
    (void)alipay_clear_rsvd_part();
    return APP_SUCCESS;
}

static void svr_alipay_start_bind(void)
{
    /* init */
    alipay_pre_init();
    svr_alipay_notify_bt(true);
    return;
}

static void svr_alipay_bind_finish(void)
{
    /* close ble service */
    svr_alipay_notify_bt(false);

    return;
}

static int32_t svr_alipay_proc_packet(uint8_t *pbuf, uint32_t len)
{
    if ((pbuf == NULL) || (len == 0)) {
        return APP_FAILURE;
    }
    alipay_ble_recv_data_handle(pbuf, len);
    return APP_SUCCESS;
}

static int32_t svr_alipay_get_bind_str(uint8_t *str_buf, int32_t *str_len)
{
    if ((str_buf == NULL) || (str_len == NULL)) {
        return APP_FAILURE;
    }

    retval_e ret = alipay_get_binding_code((char *)str_buf, str_len);
    if (ret != ALIPAY_RV_OK) {
        return APP_FAILURE;
    }

    return APP_SUCCESS;
}

static int32_t svr_alipay_get_pay_str(uint8_t *str_buf, uint32_t *str_len)
{
    if ((str_buf == NULL) || (str_len == NULL)) {
        return APP_FAILURE;
    }

    retval_e ret = alipay_get_paycode(str_buf, str_len);
    if (ret != ALIPAY_RV_OK) {
        return APP_FAILURE;
    }

    return APP_SUCCESS;
}

static int32_t svr_alipay_get_aid_str(int8_t *str_buf, uint32_t *str_len)
{
    if ((str_buf == NULL) || (str_len == NULL)) {
        return APP_FAILURE;
    }

    retval_e ret = alipay_get_aid_code((char *)str_buf, str_len);
    if (ret != ALIPAY_RV_OK) {
        return APP_FAILURE;
    }

    return APP_SUCCESS;
}

void svr_alipay_enter_lpm(void)
{
    uint32_t data;
    csi_enter_lpm((void *)&data);
    return;
}

void svr_alipay_exit_lpm(void)
{
    uint32_t data;
    csi_exit_lpm((void *)&data);
    return;
}

int32_t svr_alipay_trans_get_last_code(char *title, uint8_t *transitcode, uint32_t *len)
{
    if (title == NULL || transitcode == NULL || len == NULL) {
        ALIPAY_PRINT_ERR(0, "inparam exit nullptr");
        return APP_FAILURE;
    }
    char *trans_err_msg = (char *)malloc(ALIPAT_ERROR_MSG_LEN);
    if (trans_err_msg == NULL) {
        ALIPAY_PRINT_ERR(0, "error msg alloc fail");
        return APP_FAILURE;
    }
    int32_t ret = 0;
    ret = alipay_transit_get_the_last_transitCode(title, transitcode, len, trans_err_msg, ALIPAT_ERROR_MSG_LEN);
    ALIPAY_PRINT_DEBUG(0, "retcode:%u", ret);
    if (ret != 0) {
        ALIPAY_PRINT_ERR(0, "error msg:%s", trans_err_msg);

        transit_card_list_result_e request_result;
        alipay_get_last_card_list_result(&request_result);
        if (request_result != TRANSIT_CODE_SUCCESS) {
            free(trans_err_msg);
            return request_result + ALIPAY_TRANS_CODE_START;
        }
    }
    free(trans_err_msg);
    return ret;
}

int32_t svr_alipay_trans_get_local_cache(mc_alipay_trans_card_info_t *card_info, uint32_t *card_size,
    uint32_t *card_len)
{
    if (card_info == NULL || card_size == NULL || card_len == NULL) {
        ALIPAY_PRINT_ERR(0, "inparam exit nullptr");
        return APP_FAILURE;
    }
    int32_t ret = 0;
    ret = alipay_transit_get_card_list_offline((alipay_tansit_CardBaseVO_t *)card_info, card_size, card_len);
    ALIPAY_PRINT_DEBUG(0, "retcode:%u", ret);
    if (ret != 0) {
        transit_card_list_result_e request_result;
        alipay_get_last_card_list_result(&request_result);
        if (request_result == TRANSIT_CARD_LIST_SUCCESS) {
            return APP_SUCCESS;
        }
        return request_result + ALIPAY_TRANS_CARD_LIST_START;
    }
    return APP_SUCCESS;
}

int32_t svr_alipay_trans_get_online(mc_alipay_trans_card_info_t *card_info, uint32_t *card_size, uint32_t *card_len)
{
    if (card_info == NULL || card_size == NULL || card_len == NULL) {
        ALIPAY_PRINT_ERR(0, "inparam exit nullptr");
        return APP_FAILURE;
    }
    int32_t ret = 0;
    ret = alipay_transit_get_card_list_online((alipay_tansit_CardBaseVO_t *)card_info, card_size, card_len);
    ALIPAY_PRINT_DEBUG(0, "retcode:%u", ret);
    if (ret != 0) {
        transit_card_list_result_e request_result;
        alipay_get_last_card_list_result(&request_result);
        if (request_result == TRANSIT_CARD_LIST_SUCCESS) {
            return APP_SUCCESS;
        }
        return request_result + ALIPAY_TRANS_CARD_LIST_START;
    }
    return APP_SUCCESS;
}

int32_t svr_alipay_trans_update_card_info(mc_alipay_trans_card_info_t *card_info)
{
    if (card_info == NULL) {
        ALIPAY_PRINT_ERR(0, "inparam exit nullptr");
        return APP_FAILURE;
    }
    char *trans_err_msg = (char *)malloc(ALIPAT_ERROR_MSG_LEN);
    if (trans_err_msg == NULL) {
        ALIPAY_PRINT_ERR(0, "error msg alloc fail");
        return APP_FAILURE;
    }
    int32_t ret = 0;
    ret =
        alipay_transit_update_card_data(card_info->card_no, card_info->card_type, trans_err_msg, ALIPAT_ERROR_MSG_LEN);
    ALIPAY_PRINT_DEBUG(0, "card_no:%s, card_type:%s, ret:%u", card_info->card_no, card_info->card_type, ret);
    if (ret != 0) {
        ALIPAY_PRINT_ERR(0, "error msg:%s", trans_err_msg);
        transit_card_list_result_e request_result;
        alipay_get_last_card_list_result(&request_result);
        if (request_result != TRANSIT_CODE_SUCCESS) {
            free(trans_err_msg);
            return request_result + ALIPAY_TRANS_CODE_START;
        }
    }
    free(trans_err_msg);
    return ret;
}

int32_t svr_alipay_trans_get_trancode(mc_alipay_trans_card_info_t *card_info, uint8_t *trans_code,
    uint32_t *len_trans_code)
{
    if (card_info == NULL || trans_code == NULL || len_trans_code == NULL) {
        ALIPAY_PRINT_ERR(0, "inparam exit nullptr");
        return APP_FAILURE;
    }
    char *trans_err_msg = (char *)malloc(ALIPAT_ERROR_MSG_LEN);
    if (trans_err_msg == NULL) {
        ALIPAY_PRINT_ERR(0, "error msg alloc fail");
        return APP_FAILURE;
    }
    int32_t ret = 0;
    ret = alipay_transit_get_TransitCode(card_info->card_no, card_info->card_type, trans_code, len_trans_code,
        trans_err_msg, ALIPAT_ERROR_MSG_LEN);
    ALIPAY_PRINT_DEBUG(0, "retcode:%u", ret);
    if (ret != 0) {
        ALIPAY_PRINT_ERR(0, "error msg:%s", trans_err_msg);

        transit_card_list_result_e request_result;
        alipay_get_last_card_list_result(&request_result);
        if (request_result != TRANSIT_CODE_SUCCESS) {
            free(trans_err_msg);
            return request_result + ALIPAY_TRANS_CODE_START;
        }
    }
    free(trans_err_msg);
    return ret;
}

int32_t svr_alipay_trans_check_card_status(mc_alipay_trans_card_info_t *card_info,
    mc_alipay_tans_card_status_t *card_status)
{
    if (card_info == NULL || card_status == NULL) {
        ALIPAY_PRINT_ERR(0, "inparam exit nullptr");
        return APP_FAILURE;
    }
    int32_t ret = 0;
    ret = alipay_transit_check_card_status(card_info->card_no, card_info->card_type,
        (alipay_transit_card_status_t *)card_status);
    ALIPAY_PRINT_DEBUG(0, "retcode:%u", ret);
    return ret;
}

static alipay_svr_api_t g_alipay_svr_api = {
    .get_paystate = svr_alipay_get_paystate,
    .set_bt_broadcast = svr_alipay_notify_bt,
    .get_logon_id = svr_alipay_get_logonid,
    .get_nickname = svr_alipay_get_nickname,
    .get_bind_state = svr_alipay_get_bind_state,
    .get_bind_status = svr_alipay_get_bind_status,
    .bind_start = svr_alipay_start_bind,
    .bind_finish = svr_alipay_bind_finish,
    .get_bind_str = svr_alipay_get_bind_str,
    .get_pay_str = svr_alipay_get_pay_str,
    .get_aid_str = svr_alipay_get_aid_str,
    .remove_bind = svr_alipay_remove_bind,
    .proc_packet = svr_alipay_proc_packet,
    .exit_lpm = svr_alipay_exit_lpm,
    .enter_lpm = svr_alipay_enter_lpm,
    .get_last_transcode = svr_alipay_trans_get_last_code,
    .get_local_translist = svr_alipay_trans_get_local_cache,
    .get_online_translist = svr_alipay_trans_get_online,
    .update_trans_cardinfo = svr_alipay_trans_update_card_info,
    .check_trans_cartstatus = svr_alipay_trans_check_card_status,
    .get_transcode = svr_alipay_trans_get_trancode,

};

const alipay_svr_api_t *alipay_svr_get_api(void)
{
    return &g_alipay_svr_api;
}

static mc_alipay_trans_context_t g_alipay_trans_context = { 0 };
static alipay_msgf_event_callback_func g_alipay_msg_cb = NULL;

uint32_t svr_alipay_init_trans_context(void)
{
    g_alipay_trans_context.card_info = NULL;
    g_alipay_trans_context.card_info =
        (mc_alipay_trans_card_info_t *)malloc(sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH);
    if (g_alipay_trans_context.card_info == NULL) {
        return -1;
    }
    return 0;
}

void svr_alipay_deinit_trans_context(void)
{
    if (g_alipay_trans_context.card_info != NULL) {
        free(g_alipay_trans_context.card_info);
        g_alipay_trans_context.card_info = NULL;
    }
    return;
}

mc_alipay_trans_context_t *svr_alipay_get_trans_context(void)
{
    return &g_alipay_trans_context;
}

uint32_t svr_alipay_register_cb(alipay_msgf_event_callback_func callback)
{
    if (g_alipay_msg_cb != NULL) {
        return APP_FAILURE;
    }
    g_alipay_msg_cb = callback;
    return APP_SUCCESS;
}

static int32_t svr_alipay_proc_trans_list(uint32_t msg_id, uint8_t *pbuf, uint32_t len)
{
    if ((pbuf == NULL) || (len == 0)) {
        return APP_FAILURE;
    }
    g_alipay_trans_context.card_len = ALIPAY_TRANS_CARD_MAX_LENGTH;
    g_alipay_trans_context.card_size = sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH;
    int32_t ret = svr_alipay_trans_get_online(g_alipay_trans_context.card_info, &g_alipay_trans_context.card_size,
        &g_alipay_trans_context.card_len);
    if (g_alipay_msg_cb != NULL) {
        g_alipay_msg_cb(msg_id, ret);
    }
    return APP_SUCCESS;
}

static int32_t svr_alipay_proc_trans_special_card(uint32_t msg_id, uint8_t *pbuf, uint32_t len)
{
    if ((pbuf == NULL) || (len == 0)) {
        return APP_FAILURE;
    }
    uint32_t index = *(uint32_t *)pbuf;
    ALIPAY_PRINT_DEBUG(0, "index:%u", index);
    int32_t ret = svr_alipay_trans_update_card_info(&g_alipay_trans_context.card_info[index]);

    if (g_alipay_msg_cb != NULL) {
        g_alipay_msg_cb(msg_id, ret);
    }
    return APP_SUCCESS;
}

static unsigned long alipay_get_msg_queue_id(void)
{
    return g_alipay_queue_id;
}

/* 发送消息接口 */
errcode_t svr_alipay_msg_write(uint16_t msg_id, uint8_t *msg, uint16_t msg_len, bool wait)
{
    g_alipay_writeback_msgnum++;
    if (g_alipay_alow_read_write == 0) {
        return 0;
    }
    unsigned int timeout = (wait) ? OSAL_MSGQ_WAIT_FOREVER : 0;
    uint8_t msg_data[ALIPAY_MAX_SIZE + ALIPAY_ID_LEN + ALIPAY_MSG_LEN];

    if (osal_msg_queue_is_full(alipay_get_msg_queue_id())) {
        g_alipay_full_msgnum++;
        return ERRCODE_FAIL;
    }

    *(uint16_t *)msg_data = msg_id;
    *(uint16_t *)&msg_data[ALIPAY_ID_LEN] = msg_len;

    if ((msg != NULL) && (memcpy_s(&msg_data[ALIPAY_ID_LEN + ALIPAY_MSG_LEN], ALIPAY_MAX_SIZE, msg, msg_len) != EOK)) {
        return ERRCODE_FAIL;
    }
    return osal_msg_queue_write_copy(alipay_get_msg_queue_id(), msg_data,
        ALIPAY_MAX_SIZE + ALIPAY_ID_LEN + ALIPAY_MSG_LEN, timeout);
}

static int32_t svr_alipay_msg_proc(uint32_t msg_id, uint8_t *data, uint16_t size)
{
    g_alipay_recv_msgnum++;
    switch (msg_id) {
        case ALIPAY_MSG_BT:
            svr_alipay_proc_packet(data, size);
            break;
        case ALIPAY_MSG_UPDATA_TRANS_LIST:
            svr_alipay_proc_trans_list(msg_id, data, size);
            break;
        case ALIPAY_MSG_UPDATA_SPECIAL_CARD:
            svr_alipay_proc_trans_special_card(msg_id, data, size);
            break;
        default:
            break;
    }

    return ERRCODE_SUCC;
}

static void svr_alipay_msg_thread(void *data)
{
    uint8_t msg_data[ALIPAY_MAX_SIZE + ALIPAY_ID_LEN + ALIPAY_MSG_LEN];
    uint32_t msg_data_size = sizeof(msg_data);
    uint16_t msg_id;
    uint16_t msg_len;

    unused(data);
    while (g_alipay_alow_read_write) {
        (void)memset_s(msg_data, sizeof(msg_data), 0, sizeof(msg_data));
        osal_msg_queue_read_copy(g_alipay_queue_id, msg_data, &msg_data_size, OSAL_MSGQ_WAIT_FOREVER);

        msg_id = *((uint16_t *)&msg_data[0]);
        if (msg_id == 0xfff) {
            break;
        }
        msg_len = *((uint16_t *)&msg_data[ALIPAY_ID_LEN]);
        ALIPAY_PRINT_DEBUG(0, "svr_alipay_msg_thread size:%u ,len %u.", msg_data_size, msg_len);
        svr_alipay_msg_proc(msg_id, &msg_data[ALIPAY_ID_LEN + ALIPAY_MSG_LEN], msg_len);
    }
    ALIPAY_PRINT_ERR(0, "svr_alipay_msg_thread exit");
    return;
}

static errcode_t svr_alipay_task_init(void)
{
    errcode_t ret;
    ret = osal_msg_queue_create("alipay_msg", ALIPAY_QUEUE_MAX_SIZE, &g_alipay_queue_id, 0,
        ALIPAY_MAX_SIZE + ALIPAY_ID_LEN + ALIPAY_MSG_LEN);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

void svr_alipay_debug(void)
{
    ALIPAY_PRINT_ERR(0, "msg statisticians: write callback num:%u,full num:%u,proc num:%u.", g_alipay_writeback_msgnum,
        g_alipay_full_msgnum, g_alipay_recv_msgnum);
    g_alipay_writeback_msgnum = 0;
    g_alipay_full_msgnum = 0;
    g_alipay_recv_msgnum = 0;
    return;
}

errcode_t svr_alipay_create_task(void)
{
    ALIPAY_PRINT_DEBUG(0, "svr_alipay_create_task in");
    if (g_alipay_task_handle != NULL) {
        return ERRCODE_SUCC;
    }
    g_alipay_alow_read_write = 1;
    svr_alipay_task_init();
    g_alipay_attr.stack_mem = memalign(16, ALIPAT_MSG_STACK_SIZE);
#if defined(__FREERTOS__)
    g_alipay_attr.cb_size = sizeof(StaticTask_t);
    g_alipay_attr.cb_mem = memalign(16, sizeof(StaticTask_t)); // 16字节对齐
#endif
    g_alipay_task_handle = osThreadNew(svr_alipay_msg_thread, NULL, &g_alipay_attr);
    if (g_alipay_task_handle == NULL) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t svr_alipay_destroy_task(void)
{
    ALIPAY_PRINT_DEBUG(0, "svr_alipay_destroy_task in");
    if (g_alipay_task_handle == NULL) {
        return ERRCODE_SUCC;
    }
    uint32_t data = 0;
    osStatus_t ret = osThreadTerminate(g_alipay_task_handle);
    g_alipay_alow_read_write = 0;
    if (ret != osOK) {
        ALIPAY_PRINT_ERR(0, "osThreadTerminate faile ret %u", ret);
        return ERRCODE_FAIL;
    }
    if (g_alipay_attr.stack_mem != NULL) {
        free(g_alipay_attr.stack_mem);
        g_alipay_attr.stack_mem = NULL;
    }
#if defined(__FREERTOS__)
    if (g_alipay_attr.cb_mem != NULL) {
        free(g_alipay_attr.cb_mem);
        g_alipay_attr.cb_mem = NULL;
    }
#endif
    g_alipay_task_handle = NULL;
    osal_msg_queue_delete(g_alipay_queue_id);
    ALIPAY_PRINT_DEBUG(0, "svr_alipay_destroy_task %u", g_alipay_queue_id);
    return ERRCODE_SUCC;
}

errcode_t uapi_alipay_svr_init(void)
{
    if (g_alipay_init_flag == true) {
        return ERRCODE_SUCC;
    }
    ALIPAY_PRINT_DEBUG(0, "uapi_alipay_svr_init start.\r\n");
    uapi_gpio_set_dir(ALIPAY_SEC_GPIO, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(ALIPAY_SEC_GPIO, GPIO_LEVEL_HIGH);
    osDelay(30);
    svr_alipay_bluetooth_init();
    HS_IIC_Init();
    alipay_pre_init();
    svr_alipay_enter_lpm();
    ALIPAY_PRINT_DEBUG(0, "uapi_alipay_svr_init end.\r\n");
    g_alipay_init_flag = true;
    return ERRCODE_SUCC;
}

errcode_t uapi_alipay_svr_reset(void)
{
    if (g_alipay_init_flag) {
        int ret = alipay_reset_all();
        ALIPAY_PRINT_INFO(0, "uapi_alipay_svr_reset ret=%d.\r\n", ret);
    }
    return ERRCODE_SUCC;
}
