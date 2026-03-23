/*
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Application core board init function for standard
 * Author:
 * Create:
 */

#include "../../../chips/brandy/nv/nv_config/include/common.h"
#include "errcode.h"
#include "nv.h"
#include "debug_print.h"
#include "pm_definition.h"
#include "pm.h"
#include "esim_lpa.h"
#include "esim_bind.h"

#define CSIM_STATUS_LEN 5
#define CSIM_STATUS_STR_LEN 5
#define CSIM_BIND_STATUS_STR_LEN 3
uint32_t g_verify_flag = false; // 上电后是否认证过，用于切卡后重新再次认证
errcode_t uapi_esim_get_bind_info(esim_bind_info_t *bind_info)
{
    if (bind_info == NULL) {
        PRINT("esim get bind info param is null\r\n");
        return ERRCODE_ESIM_INVALID_PARAM;
    }
    memset_s(bind_info, sizeof(esim_bind_info_t), 0, sizeof(esim_bind_info_t));
    uint16_t nvsize = 0;
    errcode_t ret = uapi_nv_read(NV_ID_SYS_ESIM_BIND_INFO, sizeof(esim_bind_info_t), &nvsize, (uint8_t *)bind_info);
    uapi_print_bind_info(bind_info);
    return ret;
}

void print_info(char *info, uint32_t length)
{
    for (int i = 0; i < length; i++) {
        PRINT("%02X", info[i]);  // 打印每个元素
    }
}

void uapi_print_bind_info(esim_bind_info_t *bind_info)
{
    if (bind_info == NULL) {
        return;
    }
    PRINT("bind info\r\n valid:%x\r\n", bind_info->valid);
    PRINT("imei:");
    print_info((char *)bind_info->imei, IMEI_LENGTH);
    PRINT("eid:");
    print_info((char *)bind_info->eid, ESIM_BIND_EID_LENGTH);
    PRINT("share_key:");
    print_info((char *)bind_info->share_key, ESIM_BIND_SHARE_KEY_LENGTH);
}

errcode_t uapi_esim_write_bind_info(esim_bind_info_t *bind_info)
{
    esim_bind_info_t *info_before;

    info_before = malloc(sizeof(esim_bind_info_t));
    if (info_before == NULL || bind_info == NULL) {
        return ERRCODE_ESIM_INVALID_PARAM;
    }
    if (uapi_esim_get_bind_info(info_before) != ERRCODE_SUCC) {
        free(info_before);
        return ERRCODE_ESIM_INVALID_PARAM;
    }
    if (info_before->valid == 1) {
        free(info_before);
        PRINT("esim write duplicate\r\n");
        return ERRCODE_ESIM_DUPLICATE_BINDING_INFORMATION_WRITING;
    }
    free(info_before);
    errcode_t ret = uapi_nv_write(NV_ID_SYS_ESIM_BIND_INFO, (uint8_t *)bind_info, sizeof(esim_bind_info_t));
    if (ret != ERRCODE_SUCC) {
        free(info_before);
        PRINT("esim nv write bind info fail ret=%x", ret);
        return ret;
    }
    return ret;
}

void esim_toupper(char* data)
{
    uint8_t cnt;
    for (cnt = 0; cnt < strlen(data); cnt ++) {
        data[cnt] = toupper(data[cnt]);
    }
}
static void esim_at_csim_cmd_process(char* apdu, char* at_csim_cmd)
{
    int32_t apdu_len;
    char apdu_str_len[11] = {0};
    int32_t ret = 0;

    ret += strcat_s(at_csim_cmd, AT_BUFFER, "AT+CSIM=");
    apdu_len = strlen(apdu);
    ret += strcat_s(at_csim_cmd, AT_BUFFER, esim_itoa(apdu_str_len, apdu_len));
    ret += strcat_s(at_csim_cmd, AT_BUFFER, ",");
    ret += strcat_s(at_csim_cmd, AT_BUFFER, apdu);
    ret += strcat_s(at_csim_cmd, AT_BUFFER, "\r\n");
    if (ret != EOK) {
        printf("esim_at_csim_cmd_process: strcat_s faile:%d", ret);
    }
}
errcode_t SendAPDU(char* apdu, char* expected_status_code, char* response)
{
    char *at_csim_cmd = NULL;
    char *returndata = NULL;
    int32_t returndata_len, length, ret;
    char status_code[CSIM_STATUS_LEN] = {0};
    char response_len_str[CSIM_STATUS_STR_LEN] = {0};
    char *start_index, *comma_index;

    ret = ERRCODE_SUCC;
    if (esim_buf_malloc(&returndata, AT_BUFFER, &at_csim_cmd, AT_CSIM_CMD_LEN) != OK) {
        return ERRCODE_FAIL;
    }

    esim_at_csim_cmd_process(apdu, at_csim_cmd);
    printf("at_cmd: %s", at_csim_cmd);
    ret = ril_at_cmd_sync_get_info(at_csim_cmd, (uint8_t *)returndata, AT_BUFFER, AT_CMD_TIMEOUT);
    if (ret != ERRCODE_SUCC) {
        free(returndata);
        free(at_csim_cmd);
        return ret;
    }
    printf("returndata: %s", returndata);
    memset_s(response, AT_BUFFER, 0, AT_BUFFER);
    if (strstr(returndata, "+CSIM: ") == NULL) {
        free(returndata);
        free(at_csim_cmd);
        return ERRCODE_FAIL;
    }
    comma_index = strchr(returndata, ',');
    start_index = strstr(returndata, "+CSIM: ");
    length = comma_index - (start_index + 7); // "+CSIM: " 长度为 7
    strncpy_s(response_len_str, CSIM_STATUS_STR_LEN, start_index + 7, length);
    returndata_len = esim_atoi(response_len_str);
    if (returndata_len >= 4) { // 4:status code为最后4位
        ret = memcpy_s(status_code, CSIM_STATUS_LEN, comma_index + returndata_len - 3, 4); // 3开始复制位置,4status code
        ret += memcpy_s(response, AT_BUFFER, comma_index + 1, (returndata_len - 4)); // 4status code
    } else {
        printf("String is too short.\n");
        ret = ERRCODE_FAIL;
    }

    if (ret != EOK || strcmp(expected_status_code, status_code) != 0) {
        ret = ERRCODE_FAIL;
    }
    free(returndata);
    free(at_csim_cmd);
    return ret;
}
static void esim_get_status_judge(esim_bind_status_t* status, char *data, uint8_t *bind_cnt, uint8_t *verify_cnt)
{
    char status_str[CSIM_BIND_STATUS_STR_LEN] = {0};
    char bind_str[CSIM_BIND_STATUS_STR_LEN] = {0};
    char verify_str[CSIM_BIND_STATUS_STR_LEN] = {0};

    *status = ESIM_BIND_BUTT;
    strncpy_s(status_str, CSIM_BIND_STATUS_STR_LEN, data, 2); // 2:前2个字符是status
    strncpy_s(bind_str, CSIM_BIND_STATUS_STR_LEN, data + 2, 2); // 2:前2个字符是status
    strncpy_s(verify_str, CSIM_BIND_STATUS_STR_LEN, data + 4, 2); // 4,2:前2个字符是status， 第3个字节表示verify次数
    if (strcmp(status_str, "ff") == 0) {
        *status = ESIM_BIND_NEED_PPK;
    }

    if (strcmp(status_str, "00") == 0) {
        *status = ESIM_BIND_NEED_BIND;
    }

    if (strcmp(status_str, "01") == 0) {
        *status = ESIM_BIND_NEED_VERIFY;
    }

    if (strcmp(status_str, "02") == 0) {
        *status = ESIM_BIND_VERIFY_SUCC;
    }

    if (strcmp(status_str, "04") == 0) {
        *status = ESIM_BIND_LOCKED;
    }

    *bind_cnt = esim_atoi_hex(bind_str);
    *verify_cnt = esim_atoi_hex(verify_str);
}
int32_t esim_get_card_status(esim_bind_status_t* status, uint8_t *bind_cnt, uint8_t *verify_cnt, char *ccho_response)
{
    char ccho_cmd[] = "AT+CCHO=A000000533C000FF8600000004270001\r\n";
    char ccho_close[] = "AT+CCHC=1\r\n";
    int32_t ret, channel, data_len;
    char *begin_idx, *comma1_index, *comma2_index;
    char channel_str[CSIM_BIND_STATUS_STR_LEN] = {0};
    char *data_str = NULL;
    char data_len_str[CSIM_STATUS_STR_LEN] = {0};

    ret = ril_at_sync_multiple_get_info(ccho_cmd, (uint8_t *)ccho_response, AT_BUFFER, 2, AT_CMD_TIMEOUT); // 2:取2次上报
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    printf("ccho_response:%s", ccho_response);
    ret = ril_at_cmd_exc(ccho_close);
    osDelay(1000); // 1000：等待1s
    begin_idx = strstr(ccho_response, "OK\r\n\r\n");
    if (begin_idx == NULL) {
        printf("at cmd fail!");
        return ERRCODE_FAIL;
    }
    begin_idx += 6; // 6: "OK\r\n\r\n"长度为6
    printf("status:%s\n", begin_idx);

    comma1_index = strchr(begin_idx, ',');
    strncpy_s(channel_str, CSIM_BIND_STATUS_STR_LEN, begin_idx, 1);
    channel = esim_atoi(channel_str);

    comma2_index = strchr(comma1_index + 1, ',');
    strncpy_s(data_len_str, CSIM_STATUS_STR_LEN, comma1_index + 1, (comma2_index - comma1_index - 1));
    data_len = esim_atoi(data_len_str);
    if (data_len < 3) { // 3:东信上板起码有3字节
        printf("data_len is not enough\n");
        return ERRCODE_FAIL;
    }
    data_str = (char *)malloc(AT_CMD_LEN);
    if (data_str == NULL) {
        return ERRCODE_FAIL;
    }
    if (strncpy_s(data_str, AT_CMD_LEN, comma2_index + 1, data_len * 2) != EOK) { // 2:2个字符串表示一个字节
        free(data_str);
        return ERRCODE_FAIL;
    }

    printf("channel:%s, data_len:%s, data_str:%s\n", channel_str, data_len_str, data_str);
    esim_get_status_judge(status, data_str, bind_cnt, verify_cnt);
    free(data_str);
    return ERRCODE_SUCC;
}

int32_t esim_write_ppk(char *response, char *apdu, char *errtext, esim_bind_info_t *bind_info)
{
    int32_t ret;

    uapi_pm_request_mode(PM_ID_VIDEO, PM_MODE_PERFORMACE);
    uapi_watchdog_kick();
    // 1. 调用WritePPK接口获取指令(生成公私钥对)，n/d/e需要缓存，后续在Bind接口需要输入
    ret = WritePPK(1, apdu, (char *)bind_info->esim_n, (char *)bind_info->esim_d, (char *)bind_info->esim_e, errtext);
    uapi_watchdog_kick();
    printf("n:%s\n", bind_info->esim_n);
    printf("d:%s\n", bind_info->esim_d);
    printf("e:%s\n", bind_info->esim_e);
    // 2. 将WritePPK指令发送至卡端
    if ((ret != ERRCODE_SUCC) || (SendAPDU(apdu, "9000", response) != ERRCODE_SUCC) || strlen(errtext) != 0) {
        printf("esim_write_ppk ret:%d\n,errtext:%s\n", ret, errtext);
        return ERRCODE_FAIL;
    }
    if (uapi_esim_write_bind_info(bind_info) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

int32_t esim_bind_card(char *response, char *apdu, char *errtext, esim_bind_info_t *bind_info)
{
    int32_t ret, imei_len;

    // 1. 调用BindInit接口获取指令
    ret = BindInit(1, apdu, errtext);
    // 2. 将BindInit指令发送至卡端，获取返回值并保存
    if ((ret != ERRCODE_SUCC) || (SendAPDU(apdu, "9000", response) != ERRCODE_SUCC) || strlen(errtext) != 0) {
        printf("esim_bind_card BindInit ret:%d\n, errtext:%s\n", ret, errtext);
        return ERRCODE_FAIL;
    }

    memset_s(apdu, AT_CMD_LEN, 0, AT_CMD_LEN);
    if (esim_get_imei((char *)bind_info->imei, IMEI_LENGTH) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    imei_len = strlen((char *)bind_info->imei);
    if (imei_len % 2 != 0) { // 2:长度不大于 30 的 16 进制串且长度必为偶数，若为奇数应在前面补充一个“0”
        ret = memmove_s(bind_info->imei + 1, IMEI_LENGTH - 1, bind_info->imei, imei_len);
        bind_info->imei[0] = '0';
    }
    // 3. 调用Bind接口获取指令，传入前面的变量nde，IMEI，BindinitRes，输出的shardkey需要保存用于后续步骤
    ret = Bind(1, response, (char *)bind_info->imei, (char *)bind_info->esim_n, (char *)bind_info->esim_d,
        (char *)bind_info->esim_e, apdu, (char *)bind_info->share_key, errtext);
    printf("share_key:%s\n", bind_info->share_key);

    // 4. 将Bind指令发送至卡端
    if ((ret != ERRCODE_SUCC) || (SendAPDU(apdu, "6114", response) != ERRCODE_SUCC) || strlen(errtext) != 0) {
        printf("esim_bind_card Bind ret:%d\n, errtext:%s\n", ret, errtext);
        return ERRCODE_FAIL;
    }

    // 移除shardkey前后的特殊字符$，特殊字符$包含的数据需要持久化存储，绑定成功后每次需要使用该密钥校验
    ret = memmove_s(bind_info->share_key, ESIM_BIND_SHARE_KEY_LENGTH, bind_info->share_key + 1, 65); // 65 sharekey
    bind_info->share_key[64] = 0; // 64: share key固定格式 最后置0
    bind_info->share_key[65] = 0; // 65: share key固定格式 最后置0

    // BindResponse
    // 5. 将01C0000014指令发送至卡端获取返回值
    if (ret != EOK || SendAPDU("01C0000014", "9000", response) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    esim_toupper(response);
    printf("response:%s\n", response);
    // 6. 调用BindResponse接口处理卡端返回值获取eID，传入shardkey
    ret = BindResponse(response, (char *)bind_info->share_key, (char *)bind_info->eid, errtext);
    if (ret != ERRCODE_SUCC) {
        printf("esim_bind_card BindResponse ret:%d\n, errtext:%s\n", ret, errtext);
        return ERRCODE_FAIL;
    }
    printf("eid:%s\n", (char *)bind_info->eid);
    // 移除eID前后的特殊字符$，特殊字符$包含的数据需要持久化存储，绑定成功后每次需要使用该eID校验
    ret = memmove_s(bind_info->eid, ESIM_BIND_EID_LENGTH, bind_info->eid + 1, 33); // 33: eid固定格式
    if (ret != EOK) {
        return ERRCODE_FAIL;
    }
    bind_info->eid[32] = 0; // 32: eid固定格式
    bind_info->eid[33] = 0; // 33: eid固定格式

    if (uapi_esim_write_bind_info(bind_info) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

int32_t esim_verify_card(char *response, char *apdu, char *errtext, esim_bind_info_t *bind_info)
{
    int32_t ret;

    // 1. 调用VerifyInit接口获取指令
    ret = VerifyInit(1, apdu, errtext);
    // 2. 将VerifyInit指令发送至卡端，获取返回值并保存
    if ((ret != ERRCODE_SUCC) || (SendAPDU(apdu, "9000", response) != ERRCODE_SUCC) || strlen(errtext) != 0) {
        printf("esim_verify_card VerifyInit ret:%d\n, errtext:%s\n", ret, errtext);
        return ERRCODE_FAIL;
    }
    esim_toupper(response);
    // 3. 调用Verify接口获取指令,传入VerifyinitRes、shardkey、IMEI、eID
    ret = Verify(1, response, (char *)bind_info->imei, (char *)bind_info->eid, (char *)bind_info->share_key,
        apdu, errtext);
    // 4. 将Verify指令发送至卡端
    if ((ret != ERRCODE_SUCC) || (SendAPDU(apdu, "9000", response) != ERRCODE_SUCC) || strlen(errtext) != 0) {
        printf("esim_verify_card Verify ret:%d\n, errtext:%s\n", ret, errtext);
        ret = ERRCODE_FAIL;
    }
    if (uapi_esim_write_bind_info(bind_info) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    g_verify_flag = true;
    return ERRCODE_SUCC;
}
int32_t esim_unlock_bind(void)
{
    char cmd0[] = "AT+CSIM=38,00A404000EA000000533C000FF860000000427\r\n";
    char cmd1[] = "AT+CSIM=26,80500000083E5C8B863DD5BC63\r\n";
    char cmd2[] = "AT+CSIM=10,00C000001C\r\n";
    char cmd3[] = "AT+CSIM=42,848203001018C5D437FEE2A492FBCAFE7C99658575\r\n";
    char cmd4[] = "AT+CSIM=74,84AD00022039A86FDA56635F9BE6A4896728324BC95CD97E3699A92AEED94B5489E3A6BAF9\r\n";
    char cmd5[] = "AT+CSIM=74,8451880120838615CD822132BF5421806FB0BF92225252703C6DDA9A3E429950E8A5A0EB71\r\n";
    char *cmd_list[] = {cmd0, cmd1, cmd2, cmd3, cmd4, cmd5};
    char *response = NULL;
    int32_t ret, cnt;

    response = (char *)malloc(AT_BUFFER);
    if (response == NULL) {
        return ERRCODE_FAIL;
    }

    for (cnt = 0; cnt < 6; cnt++) { // 6: 总共6个命令
        ret = ril_at_cmd_sync_get_info(cmd_list[cnt], (uint8_t *)response, AT_BUFFER, AT_CMD_TIMEOUT);
        if (ret != ERRCODE_SUCC) {
            free(response);
            printf("esim_unlock_bind fail cnt:%d,ret:%d", cnt, ret);
            return ERRCODE_FAIL;
        }
        osDelay(500); // 500:每个命令等待500ms
    }
    free(response);
    ril_at_cmd_exc("AT+CFUN=0\r\n");
    osDelay(4000); // 4000:每个命令等待4000ms
    ril_at_cmd_exc("AT+CFUN=1\r\n");
    osDelay(4000); // 4000:每个命令等待4000ms
    ril_at_cmd_exc("AT+NUICC=1\r\n");
    osDelay(1000); // 1000:每个命令等待1000ms
    return ERRCODE_SUCC;
}
int32_t esim_verify_waitready_process(void)
{
    int32_t res;
    esim_bind_msg_t *esim_bind_msg = NULL;
    char ccho_cmd[] = "AT+CCHO=A000000533C000FF8600000004270001\r\n";

    if (g_verify_flag == false) {
        printf("dont need verify");
        return ERRCODE_SUCC;
    }

    esim_bind_msg = (esim_bind_msg_t *)malloc(sizeof(esim_bind_msg_t));
    if (esim_bind_msg == NULL) {
        return ERRCODE_FAIL;
    }
    memset_s(esim_bind_msg, sizeof(esim_bind_msg_t), 0, sizeof(esim_bind_msg_t));
    if (uapi_esim_get_bind_info(&esim_bind_msg->bind_info) != ERRCODE_SUCC) {
        free(esim_bind_msg);
        return ERRCODE_FAIL;
    }
    res = ril_at_cmd_sync_get_info(ccho_cmd, (uint8_t *)esim_bind_msg->response, AT_BUFFER, AT_CMD_TIMEOUT);
    if (res != ERRCODE_SUCC) {
        free(esim_bind_msg);
        printf("ERROR:ccho_cmd fail!res=%d!\n", res);
        return ERRCODE_FAIL;
    }

    res = esim_verify_card(esim_bind_msg->response, esim_bind_msg->apdu, esim_bind_msg->errtext,
        &esim_bind_msg->bind_info);
    osDelay(1000); // 1000：等待1s
    if (res != 0) {
        printf("ERROR:esim_verify_waitready_process fail!res=%d!\n", res);
        free(esim_bind_msg);
        return ERRCODE_FAIL;
    }
    free(esim_bind_msg);

    return ERRCODE_SUCC;
}

uint32_t uapi_get_esim_status(void)
{
    int32_t res;
    esim_bind_status_t status = ESIM_BIND_BUTT;
    char *response = NULL;
    uint8_t bind_cnt = 0xff;
    uint8_t verify_cnt = 0xff;

    response = (char *)malloc(AT_BUFFER);
    if (response == NULL) {
        return ERRCODE_FAIL;
    }
    memset_s(response, AT_BUFFER, 0, AT_BUFFER);
    res = esim_get_card_status(&status, &bind_cnt, &verify_cnt, response);
    if (res != 0) {
        wstp_bt_print("ERROR:esim_get_card_status fail!res=%d!\n", res);
        free(response);
        return ERRCODE_FAIL;
    }
    free(response);
    wstp_bt_print("uapi_get_esim_status:%d, bind_cnt:%d, verify_cnt:%d\n", status, bind_cnt, verify_cnt);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_write_ppk(void)
{
    int32_t res;
    esim_bind_msg_t *esim_bind_msg = NULL;
    char ccho_cmd[] = "AT+CCHO=A000000533C000FF8600000004270001\r\n";
    char ccho_close[] = "AT+CCHC=1\r\n";

    wstp_bt_print("uapi_esim_write_ppk\n");
    esim_bind_msg = (esim_bind_msg_t *)malloc(sizeof(esim_bind_msg_t));
    if (esim_bind_msg == NULL) {
        return ERRCODE_FAIL;
    }
    memset_s(esim_bind_msg, sizeof(esim_bind_msg_t), 0, sizeof(esim_bind_msg_t));
    if (uapi_esim_get_bind_info(&esim_bind_msg->bind_info) != ERRCODE_SUCC) {
        free(esim_bind_msg);
        return ERRCODE_FAIL;
    }
    res = ril_at_cmd_sync_get_info(ccho_cmd, (uint8_t *)esim_bind_msg->response, AT_BUFFER, AT_CMD_TIMEOUT);
    if (res != ERRCODE_SUCC) {
        free(esim_bind_msg);
        wstp_bt_print("ERROR:ccho_cmd fail!res=%d!\n", res);
        return ERRCODE_FAIL;
    }

    res = esim_write_ppk(esim_bind_msg->response, esim_bind_msg->apdu, esim_bind_msg->errtext,
        &esim_bind_msg->bind_info);
    res += ril_at_cmd_exc(ccho_close);
    osDelay(1000); // 1000：等待1s
    if (res != 0) {
        wstp_bt_print("ERROR:uapi_esim_write_ppk fail!res=%d!\n", res);
        free(esim_bind_msg);
        return ERRCODE_FAIL;
    }
    free(esim_bind_msg);
    return ERRCODE_SUCC;
}
uint32_t uapi_esim_bind_card(void)
{
    int32_t res;
    esim_bind_msg_t *esim_bind_msg = NULL;
    char ccho_cmd[] = "AT+CCHO=A000000533C000FF8600000004270001\r\n";
    char ccho_close[] = "AT+CCHC=1\r\n";

    wstp_bt_print("uapi_esim_bind_card\n");
    esim_bind_msg = (esim_bind_msg_t *)malloc(sizeof(esim_bind_msg_t));
    if (esim_bind_msg == NULL) {
        return ERRCODE_FAIL;
    }

    memset_s(esim_bind_msg, sizeof(esim_bind_msg_t), 0, sizeof(esim_bind_msg_t));
    if (uapi_esim_get_bind_info(&esim_bind_msg->bind_info) != ERRCODE_SUCC) {
        free(esim_bind_msg);
        return ERRCODE_FAIL;
    }

    res = ril_at_cmd_sync_get_info(ccho_cmd, (uint8_t *)esim_bind_msg->response, AT_BUFFER, AT_CMD_TIMEOUT);
    if (res != ERRCODE_SUCC) {
        free(esim_bind_msg);
        wstp_bt_print("ERROR:ccho_cmd fail!res=%d!\n", res);
        return ERRCODE_FAIL;
    }

    res = esim_bind_card(esim_bind_msg->response, esim_bind_msg->apdu, esim_bind_msg->errtext,
        &esim_bind_msg->bind_info);
    res += ril_at_cmd_exc(ccho_close);
    osDelay(1000); // 1000：等待1s
    if (res != 0) {
        wstp_bt_print("ERROR:uapi_esim_bind_card fail!res=%d!\n", res);
        free(esim_bind_msg);
        return ERRCODE_FAIL;
    }
    free(esim_bind_msg);
    return ERRCODE_SUCC;
}
uint32_t uapi_esim_verify_card(void)
{
    int32_t res;
    esim_bind_msg_t *esim_bind_msg = NULL;
    char ccho_cmd[] = "AT+CCHO=A000000533C000FF8600000004270001\r\n";
    char ccho_close[] = "AT+CCHC=1\r\n";

    wstp_bt_print("uapi_esim_verify_card\n");
    esim_bind_msg = (esim_bind_msg_t *)malloc(sizeof(esim_bind_msg_t));
    if (esim_bind_msg == NULL) {
        return ERRCODE_FAIL;
    }
    memset_s(esim_bind_msg, sizeof(esim_bind_msg_t), 0, sizeof(esim_bind_msg_t));
    if (uapi_esim_get_bind_info(&esim_bind_msg->bind_info) != ERRCODE_SUCC) {
        free(esim_bind_msg);
        return ERRCODE_FAIL;
    }
    res = ril_at_cmd_sync_get_info(ccho_cmd, (uint8_t *)esim_bind_msg->response, AT_BUFFER, AT_CMD_TIMEOUT);
    if (res != ERRCODE_SUCC) {
        free(esim_bind_msg);
        wstp_bt_print("ERROR:ccho_cmd fail!res=%d!\n", res);
        return ERRCODE_FAIL;
    }

    res = esim_verify_card(esim_bind_msg->response, esim_bind_msg->apdu, esim_bind_msg->errtext,
        &esim_bind_msg->bind_info);
    res += ril_at_cmd_exc(ccho_close);
    osDelay(1000); // 1000：等待1s
    if (res != 0) {
        wstp_bt_print("ERROR:uapi_esim_verify_card fail!res=%d!\n", res);
        free(esim_bind_msg);
        return ERRCODE_FAIL;
    }
    free(esim_bind_msg);

    return ERRCODE_SUCC;
}
uint32_t uapi_esim_bind_main(void)
{
    int32_t res;
    esim_bind_status_t status = ESIM_BIND_BUTT;
    char *response = NULL;
    uint8_t bind_cnt = 0xff;
    uint8_t verify_cnt = 0xff;

    response = (char *)malloc(AT_BUFFER);
    if (response == NULL) {
        return ERRCODE_FAIL;
    }

    res = esim_get_card_status(&status, &bind_cnt, &verify_cnt, response);
    if (res != 0) {
        free(response);
        wstp_bt_print("ERROR:esim_get_card_status fail!res=%d!\n", res);
        return ERRCODE_FAIL;
    }
    wstp_bt_print("uapi_esim_bind_main status:%d\n", status);
    switch (status) {
        case ESIM_BIND_NEED_PPK:
            uapi_esim_write_ppk();
            break;
        case ESIM_BIND_NEED_BIND:
            uapi_esim_bind_card();
            break;
        case ESIM_BIND_NEED_VERIFY:
            uapi_esim_verify_card();
            break;
        case ESIM_BIND_VERIFY_SUCC:
        case ESIM_BIND_LOCKED:
            wstp_bt_print("uapi_esim_bind_main status=%d, dont need process\n", status);
            break;
        default:
            wstp_bt_print("error:uapi_esim_bind_main status=%d\n", status);
            break;
    }
    free(response);
    return ERRCODE_SUCC;
}