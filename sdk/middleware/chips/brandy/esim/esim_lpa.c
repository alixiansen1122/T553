/*
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Application core board init function for standard
 * Author:
 * Create:
 */

#include "errcode.h"
#include "common_def.h"
#include "httpclient.h"
#include "ril_interface.h"
#include "lwip_public.h"
#include "debug_print.h"
#include "esim_lpa.h"

/* @brief http request buffer */
#define ESIM_REQ_BUF_SIZE 2048
static char *g_esim_http_req_buf = NULL;

/* @brief http response buffer */
#define ESIM_RSP_BUF_SIZE 50000
static char *g_esim_http_rsp_buf = NULL;

#define ESIM_RX_BUF_SIZE 25000
static unsigned char *g_esim_rx_buf = NULL;

#define HTTP_HEADER_BUF_SIZE 2048
#define URL_HEADER_BUF_SIZE 1024
#define MAX_HTTP_PROT 65535
#define ESIM_HTTP_NOCONTENT		204	/**< request does not have content */
#define HEX_STR_LEN 3
static char g_net_ready_flag = false;

char g_activeCode[ACTIVE_CODE_BUF_SIZE] = {0};
char g_iccid[ICCID_BUF_SIZE] = {0};

char *esim_itoa(char *p, uint32_t x)
{
    uint32_t data = x;
    // number of digits in a uint32_t + NUL
    p += 11; // 11：数字最大长度
    *--p = 0;
    do {
        *--p = '0' + data % 10; // 10:十进制
        data /= 10; // 10:十进制
    } while (data);
    return p;
}

int esim_atoi(const char *str)
{
    return (int)strtol(str, NULL, 10); // 10:十进制
}
int esim_atoi_hex(const char *str)
{
    return (int)strtol(str, NULL, 16); // 16:十六进制
}
void esim_get_header(struct Header_list* hlist, char* customer_header)
{
    char* data = hlist->data;
    int32_t ret;
    struct Header_list* list = hlist;
    while (1) {
        ret = strcat_s(customer_header, HTTP_HEADER_BUF_SIZE, data);
        ret += strcat_s(customer_header, HTTP_HEADER_BUF_SIZE, "\r\n");
        if (ret != EOK) {
            printf("esim_get_header strcat_s fail!");
            return;
        }
        if (list->next == NULL) {
            return;
        }
        list = list->next;
        data = list->data;
    }
}
int32_t esim_http_hostreq(char *url, int32_t port, struct Header_list *hlist, char *txbuf, int32_t txLen, char **rxbuf)
{
    char *req_url = NULL;
    char port_buf[16] = {0}; // 16:端口数字转字符串长度
    // char *port_str;
    char *customer_header = NULL;
    int32_t ret;
    int32_t responseCode;
    HttpClient client = {0};
    HttpClientData client_data = {0};

    wstp_bt_print("esim_http_hostreq \r\n");
    if (port > MAX_HTTP_PROT || port < 0 || txLen <= 0) {
        return ERRCODE_FAIL;
    }
    if (url == NULL || hlist == NULL || txbuf == NULL || rxbuf == NULL) {
        return ERRCODE_FAIL;
    }
    if (esim_buf_malloc(&req_url, URL_HEADER_BUF_SIZE, &customer_header, HTTP_HEADER_BUF_SIZE) != OK) {
        return ERRCODE_FAIL;
    }
    wstp_bt_print("url:%s,port:%d \r\n", url, port);
    if (memcpy_s(req_url, URL_HEADER_BUF_SIZE, url, strlen(url) + 1) != EOK) {
        return ERRCODE_FAIL;
    }
    esim_get_header(hlist, customer_header);

    memset_s(g_esim_http_req_buf, ESIM_REQ_BUF_SIZE, 0, ESIM_REQ_BUF_SIZE);
    client_data.headerBuf = g_esim_http_req_buf; // 服务返回值
    client_data.headerBufLen = ESIM_REQ_BUF_SIZE; // 服务返回值

    memset_s(g_esim_http_rsp_buf, ESIM_RSP_BUF_SIZE, 0, ESIM_RSP_BUF_SIZE);
    client_data.responseBuf = g_esim_http_rsp_buf; // 服务返回值
    client_data.responseBufLen = ESIM_RSP_BUF_SIZE; // 服务返回值

    client_data.postBuf = txbuf;
    client_data.postBufLen = strlen(client_data.postBuf) + 1;

    wstp_bt_print("esim http request %s,header:%s\r\n", req_url, customer_header);
    printf("txbuf_res:len:%d, data:%s\r\n", txLen, client_data.postBuf);
    HttpClientSetCustomHeader(&client, customer_header);
    ret = HttpClientPostRequest(&client, req_url, &client_data);
    responseCode = HttpClientGetResponseCode(&client);
    if (ret >= HTTP_SUCCESS || responseCode == ESIM_HTTP_NOCONTENT) {
        wstp_print("esim_http_hostreq success!\r\n");
        ret = HTTP_SUCCESS;
        *rxbuf = client_data.responseBuf;
    } else {
        wstp_print("esim_http_hostreq fail!, resposeCode=%d, ret=0x%x\r\n", responseCode, ret);
    }
    free(req_url);
    free(customer_header);
    return ret;
}
void printf_Hex(char *head, unsigned char *buf, unsigned short len)
{
    if (head != NULL) {
        printf("datalen = %d --%s ", len, head);
    }
    for (size_t i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}
void esim_strHex(char *dest, unsigned char *src, unsigned short srcLen)
{
    char temBuffer[HEX_STR_LEN];
    int32_t ret;
    ret = memset_s(dest, AT_CSIM_CMD_LEN, 0, 2 * (srcLen)); // 2: 2个字符表示1字节
    for (int32_t i = 0; i < srcLen; i++) {
        if (sprintf_s(temBuffer, HEX_STR_LEN, "%02X", src[i]) < 0) {
            printf("esim_strHex sprintf_s fail!");
        }
        ret += strcat_s(dest, AT_CSIM_CMD_LEN, temBuffer);
    }
    if (ret != EOK) {
        printf("esim_strHex memset_s sprintf_s strcat_s fail!");
    }
}

void hexStr2Ascii(unsigned char *ascii_str, char *hex_str)
{
    int32_t len = strlen((const char *)hex_str);
    int32_t i, j;
    
    // 确保输入长度是偶数（每个字节由两个十六进制字符表示）
    if (len % 2 != 0) { // 2: 2个字符表示1字节
        return; // 输入无效，直接返回
    }
    
    // 计算ASCII字符串的长度
    int ascii_len = len / 2;
    for (i = 0, j = 0; i < len; i += 2, j++) { // 2: 2个字符表示1字节
        // 将两个十六进制字符转换为一个字节
        char c1 = toupper(hex_str[i]);
        char c2 = toupper(hex_str[i +1 ]);
        
        // 将每个字符转换为对应的4位二进制值
        unsigned char byte = 0;
        if (c1 >= '0' && c1 <= '9') {
            byte += (c1 - '0') << 4; // 4:16进制的高位
        } else if (c1 >= 'A' && c1 <= 'F') {
            byte += (c1 - 'A' + 10) << 4; // 10:十进制 4:16进制的高位
        }
        
        if (c2 >= '0' && c2 <= '9') {
            byte += (c2 - '0');
        } else if (c2 >= 'A' && c2 <= 'F') {
            byte += (c2 - 'A' + 10); // 10:十进制
        }
        
        ascii_str[j] = byte;
    }
    ascii_str[j] = '\0'; // 添加字符串结束符
}

int32_t AT_Transmit(char *atCmd, char *resData)
{
    int32_t ret = 0;
    char channel_open[] = "AT+CSIM=10,0070000001\r\n";
    char channel_close[] = "AT+CSIM=10,0070800100\r\n";
    if (atCmd == NULL || resData == NULL) {
        return ERR_API_PARAMETER;
    }
    
    osDelay(500); // 500:等待2131处理时间
    ret = ril_at_cmd_sync_get_info(atCmd, (uint8_t *)resData, AT_BUFFER, AT_CMD_TIMEOUT);
    /* 如果开通道失败，就关通道 */
    if ((ret == RIL_RET_FAIL_RECIVE_TIMEOUT) && (strcmp(atCmd, channel_open) == 0)) {
        ril_at_cmd_sync_get_info(channel_close, (uint8_t *)resData, AT_BUFFER, AT_CMD_TIMEOUT);
    }
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }
    return OK;
}

int32_t esim_transmit(char *inData, char *outData, int32_t *ioLen)
{
    int32_t strLen;
    char *strP, *strCSim;
    char temp_buf[strlen(inData) + 20]; // 20:AT+CSIM命令长度预留
    char *temp_outbuf = NULL;
    int32_t ret = 0;

    temp_outbuf = (char *)malloc(AT_BUFFER);
    if (temp_outbuf == NULL) {
        return ERR_API_PARAMETER;
    }
    memset_s(temp_outbuf, AT_BUFFER, 0, AT_BUFFER);
    memset_s(temp_buf, strlen(inData) + 20, 0, strlen(inData) + 20); // 20:AT+CSIM命令长度预留
    // AT指令拼装
    if (sprintf_s(temp_buf, strlen(inData) + 20, "AT+CSIM=%d,%s\r\n", strlen(inData), inData) < 0) { // 20:AT+CSIM命令
        return ERR_SE_COMMUNICA;
    }
    printf("temp_buf[%s]\n", temp_buf);
    ret = AT_Transmit(temp_buf, temp_outbuf);
    if (ret != RIL_RET_SUCCESS) {
        free(temp_outbuf);
        printf("== AT_Transmit err code:%d==\n", ret);
        return ERR_SE_COMMUNICA;
    }
    printf("AllData[%d]:[%s]\n", strlen(temp_outbuf), temp_outbuf);

    // AT指令解析
    strCSim = strstr(temp_outbuf, "+CSIM:");
    if (strCSim == NULL) {
        free(temp_outbuf);
        printf("NOT FOUND \"+CSIM\"\n");
        return ERR_SE_COMMUNICA;
    }
    strP = strstr(strCSim, ",");
    *strP = '\0';
    strLen = atoi(strstr(strCSim, " ") + 1);
    if (strncpy_s(outData, AT_CSIM_CMD_LEN, strP + 1, strLen) != EOK) {
        return ERR_SE_COMMUNICA;
    }
    outData[strLen]= '\0';
    *ioLen = strLen;
    free(temp_outbuf);
    printf("outData[%d]:%s \n", strlen(outData), outData);
    return OK;
}
void esim_get_net_status(const uint8_t netif_type, const uint8_t net_connect_state)
{
    if (net_connect_state == CONNECTION_CONNECTED && netif_type == LWIP_NETIF_BT_PAN) {
        esim_set_net_flag(true);
    } else {
        esim_set_net_flag(false);
    }
    printf("netif_type[%d],net_connect_state[%d] \n", netif_type, net_connect_state);
}
int32_t esim_apdu_transmit_init()
{
    int32_t ret;

    g_esim_rx_buf = (unsigned char *)malloc(sizeof(char) * ESIM_RX_BUF_SIZE);
    g_esim_http_req_buf = (char *)malloc(sizeof(char) * ESIM_REQ_BUF_SIZE);
    g_esim_http_rsp_buf = (char *)malloc(sizeof(char) * ESIM_RSP_BUF_SIZE);
    if (g_esim_rx_buf == NULL || g_esim_http_req_buf == NULL || g_esim_http_rsp_buf == NULL) {
        return ERR_API_STATE;
    }

    lwip_register_connect_listener(esim_get_net_status);
    osDelay(1000); // 1000:等待2131处理时间
    printf("esim_apdu_transmit_init succ!");
    return OK;
}
/* 与esim_apdu_transmit_init对应使用，负责失败或退出时释放资源 */
void esim_apdu_transmit_deinit()
{
    if (g_esim_rx_buf != NULL) {
        free(g_esim_rx_buf);
    }

    if (g_esim_http_req_buf != NULL) {
        free(g_esim_http_req_buf);
    }

    if (g_esim_http_rsp_buf != NULL) {
        free(g_esim_http_rsp_buf);
    }
    lwip_unregister_connect_listener(esim_get_net_status);
    g_esim_rx_buf = NULL;
    g_esim_http_req_buf = NULL;
    g_esim_http_rsp_buf = NULL;
}

int32_t esim_buf_malloc(char **buff1, int32_t buff1_len, char **buff2, int32_t buff2_len)
{
    *buff1 = (char *)malloc(buff1_len);
    if (*buff1 == NULL) {
        return ERR_API_PARAMETER;
    }
    *buff2 = (char *)malloc(buff2_len);
    if (*buff2 == NULL) {
        free(*buff1);
        return ERR_API_PARAMETER;
    }
    memset_s(*buff1, buff1_len, 0, buff1_len);
    memset_s(*buff2, buff2_len, 0, buff2_len);
    return OK;
}
int32_t esim_apdu_transmit_recv(unsigned char *txbuf, unsigned long txlen, unsigned char **rxbuf, unsigned long *rxlen)
{
    unsigned long currentOffset = 0;
    unsigned char getData[] = {0x00, 0xC0, 0x00, 0x00, 0x00};
    char *write_buf = NULL;
    char *readBuf = NULL;
    int32_t ioLen, ret;

    if (esim_buf_malloc(&write_buf, AT_CSIM_CMD_LEN, &readBuf, AT_CSIM_CMD_LEN) != OK) {
        return ERR_API_PARAMETER;
    }

    *rxbuf = g_esim_rx_buf;
    memset_s(g_esim_rx_buf, ESIM_RX_BUF_SIZE, 0, ESIM_RX_BUF_SIZE);

    // 写入数据到设备
    esim_strHex(write_buf, txbuf, txlen);
    printf("writeBuf:[%d]%s\n", strlen(write_buf), write_buf);

    ioLen = txlen * 2; // 2: 2个字符表示1字节
    if ((ret = esim_transmit(write_buf, readBuf, &ioLen)) != 0) {
        free(write_buf);
        free(readBuf);
        printf("== esim_transmit err ==\n");
        return ret;
    }
    printf("readBuf:[%d]%s\n", strlen(readBuf), readBuf);
    hexStr2Ascii(g_esim_rx_buf, readBuf);
    *rxlen = ioLen / 2; // 2: 2个字符表示1字节

    currentOffset = *rxlen - 2; // 2: 固定格式
    getData[0] = txbuf[0] & 0x03;
    while (g_esim_rx_buf[currentOffset] == 0x61) {
        *rxlen = ESIM_RX_BUF_SIZE;
        getData[4] = g_esim_rx_buf[currentOffset + 1];  // 4: 固定处理
        esim_strHex(write_buf, getData, 5);  // 5: 固定处理
        ioLen = 10;  // 10: 固定处理
        if ((ret = esim_transmit(write_buf, readBuf, &ioLen)) != 0) {
            free(write_buf);
            free(readBuf);
            return ret;
        }
        hexStr2Ascii(&g_esim_rx_buf[currentOffset], readBuf);
        *rxlen = ioLen / 2; // 2: 2个字符表示1字节
        currentOffset += (*rxlen - 2); // 2:固定处理
    }
    currentOffset += 2; // 2:固定处理
    *rxlen = currentOffset;
    printf("se_recv:len[%ld],outdata[%s][%s]\r\n", *rxlen, g_esim_rx_buf, *rxbuf);
    printf_Hex(NULL, *rxbuf, *rxlen);
    free(write_buf);
    free(readBuf);
    return OK;
}

int32_t esim_waitfor_ready()
{
    int32_t res;
    bool rsp = TRUE;
    res = ril_at_cmd_exc("AT+CFUN=0\r\n");
    osDelay(2000); // 2000:等待2131处理时间
    res += ril_at_cmd_exc("AT+CFUN=1\r\n");
    osDelay(4000); // 4000:等待2131处理时间
    res += ril_at_cmd_exc("AT+NUICC=1\r\n");
    osDelay(1000); // 1000:等待2131处理时间
    res += esim_verify_waitready_process(); // waitready后东信会关通道，因此无需关闭
    if (res != RIL_RET_SUCCESS) {
        printf("esim_waitfor_ready res:%d \r\n", res);
        return res;
    }
    printf("esim_waitfor_ready SUCC! \r\n");
    return OK;
}

int32_t esim_get_imei(char* imei, size_t len)
{
    int32_t res;
    res = ril_get_imei(imei, len);
    if (res != RIL_RET_SUCCESS) {
        return ERRCODE_FAIL;
    }
    printf("esim_get_imei %d \r\n", res);
    return ERRCODE_SUCC;
}

void esim_get_activecode(char* code_res)
{
    if (memcpy_s(code_res, ACTIVE_CODE_BUF_SIZE, g_activeCode, sizeof(g_activeCode)) != EOK) {
        printf("esim_get_activecode memcpy_s FAIL \r\n");
    }
}

void esim_get_iccid(char* iccid_res)
{
    if (memcpy_s(iccid_res, ICCID_BUF_SIZE, g_iccid, sizeof(g_iccid)) != EOK) {
        printf("esim_get_iccid memcpy_s FAIL \r\n");
    }
}

uint32_t esim_set_activecode(char* code_res, uint32_t para_len)
{
    memset_s(g_activeCode, ACTIVE_CODE_BUF_SIZE, 0, ACTIVE_CODE_BUF_SIZE);
    if (memcpy_s(g_activeCode, sizeof(g_activeCode), code_res, para_len) != EOK) {
        return ERRCODE_FAIL;
    };
    printf("g_activeCode:%s\n", g_activeCode);
    return ERRCODE_SUCC;
}

uint32_t esim_set_iccid(char* iccid_res, uint32_t para_len)
{
    memset_s(g_iccid, ICCID_BUF_SIZE, 0, ICCID_BUF_SIZE);
    if (memcpy_s(g_iccid, sizeof(g_iccid), iccid_res, para_len) != EOK) {
        return ERRCODE_FAIL;
    };
    printf("g_iccid:%s\n", g_iccid);
    return ERRCODE_SUCC;
}

int32_t esim_metadata_process(struct API_Metadata metadata)
{
    return OK;
}
void esim_set_net_flag(bool flag)
{
    g_net_ready_flag = flag;
}
char esim_get_net_flag()
{
    return g_net_ready_flag;
}