#include "sk_ota.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "ril_interface.h"
#include "errcode.h"
#include <time.h>
#include <sys/time.h>
#include "soc_osal.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include "osal_list.h"
#include "rgb_led.h"
/* ----------------- 任务/队列命名与参数 ----------------- */
#define SK_OTA_TASK_NAME       "sk_ota_task"
#define SK_OTA_TASK_STACK_SIZE 0x3000
#define SK_OTA_TASK_PRIO       17
#define SK_OTA_CFG_MAX_TIMEOUT 60

#define WAIT_ERASE_MAX_TIME 18

#define OTA_USE_AT_CMD  0
#define audio_ota_check_time 5000

static osThreadId_t sk_ota_task_id = NULL;
unsigned long ota_cmd_queue_id = 0;
unsigned char update_flag = 0;

/* 消息类型：由任务自身驱动下一片 */
typedef enum {
    OTA_CMD_NONE = 0,
    OTA_CMD_START,
    OTA_CMD_SEND_NEXT,
    OTA_CMD_END,
    OTA_CMD_ABORT
} ota_cmd_e;

typedef struct {
    ota_cmd_e cmd;
} ota_cmd_msg_t;

/* 全局 OTA 状态（任务上下文共享） */
static volatile int g_ota_running = 0;
static volatile int g_ota_mcu_running = 0;
static volatile int g_ota_abort_flag = 0;
static sk_ota_callback_t g_ota_cb = NULL;

/* 文件上下文保存在静态变量中，任务处理各命令时共用 */
static FILE *g_pkg_fp = NULL;
static uint32_t g_chunk_size = SK_OTA_CFG_CHUNK_SIZE;
static uint32_t g_sn = 0;
static uint32_t g_modem_ota_timeout_count = 0;
osal_timer audio_ota_check_timer={0};
/* ----------------- CRC8（CRC-8 poly=0x07, init=0x00） ----------------- */
uint8_t sk_ota_crc8(const uint8_t *data, size_t len)
{
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
    }
    return crc;
}

#if OTA_USE_AT_CMD
/* ----------------- 辅助：二进制转大写HEX字符串 ----------------- */
static void _bin_to_hex_upper(const uint8_t *bin, size_t len, char *out, size_t out_size)
{
    static const char hex[] = "0123456789ABCDEF";
    if (!out || out_size < (len * 2 + 1)) {
        if (out && out_size > 0) out[0] = '\0';
        return;
    }
    for (size_t i = 0; i < len; ++i) {
        out[i * 2]     = hex[(bin[i] >> 4) & 0x0F];
        out[i * 2 + 1] = hex[bin[i] & 0x0F];
    }
    out[len * 2] = '\0';
}

/* ----------------- 内部：发 AT START（擦除区） ----------------- */
static void _ota_do_start_at(void)
{
    /* AT+NFWUPD=0 */
    ril_at_cmd_exc("AT+NFWUPD=0\r\n");
}

/* ----------------- 内部：发 AT END（执行升级） ----------------- */
static void _ota_do_end_at(void)
{
    /* AT+NFWUPD=5 */
    ril_at_cmd_exc("AT+NFWUPD=5\r\n");
}

/* ----------------- 内部：发送单片（通过 AT+NFWUPD=1,...） -----------------
   fire-and-go：不等待响应（按你选择 A）
   打包格式：
   AT+NFWUPD=1,1,<sn>,<len>,<data_hex>,<crc_hex>
   data_hex: 连续大写HEX字符（每字节2位）
   crc_hex: 单字节HEX（两位）
--------------------------------------------------------------- */
static int _ota_do_send_piece_at(uint32_t sn, const uint8_t *data, uint32_t len, uint8_t crc)
{
    if (!data || len == 0) return -1;

    /* data -> hex string */
    /* max len 512 -> hex length 1024, +1 for null */
    size_t hex_needed = (size_t)len * 2 + 1;
    char *hexbuf = (char *)malloc(hex_needed);
    if (!hexbuf) return -1;
    _bin_to_hex_upper(data, len, hexbuf, hex_needed);

    /* crc hex (2 chars) */
    char crc_hex[3];
    const char *hexchars = "0123456789ABCDEF";
    crc_hex[0] = hexchars[(crc >> 4) & 0x0F];
    crc_hex[1] = hexchars[crc & 0x0F];
    crc_hex[2] = '\0';

    /* 构造AT命令字符串（足够大的缓冲） */
    /* AT+NFWUPD=1,1,<sn>,<len>,<data_hex>,<crc> */
    size_t cmd_buf_size = 16 + 4 + hex_needed + 8; /* 预估，实际分配更大 */
    char *cmd = (char *)malloc(cmd_buf_size + 64);
    if (!cmd) {
        free(hexbuf);
        return -1;
    }

    /* snprintf 可能受限，这里使用 snprintf 安全构造 */
    snprintf(cmd, cmd_buf_size + 64, "AT+NFWUPD=1,1,%u,%u,%s,%s\r\n", (unsigned)sn, (unsigned)len, hexbuf, crc_hex);

    // /* 打印调试信息（HEX+ASCII 格式）在此处：先打印 CRC 与头信息 */
    // printf("sk_ota: packet sn=%u raw_len=%u crc=0x%02X\n", sn, len, crc);

    // const uint8_t *p = data;
    // uint32_t offset = 0;
    // while (offset < len) {
    //     uint32_t line_len = (len - offset > 16) ? 16 : (len - offset);

    //     /* 打印十六进制部分 */
    //     printf("%08X  ", offset);
    //     for (uint32_t i = 0; i < line_len; ++i) {
    //         printf("%02X ", p[offset + i]);
    //     }
    //     for (uint32_t i = line_len; i < 16; ++i) {
    //         printf("   ");
    //     }

    //     /* 打印 ASCII 部分 */
    //     printf(" |");
    //     for (uint32_t i = 0; i < line_len; ++i) {
    //         uint8_t c = p[offset + i];
    //         if (c >= 32 && c <= 126) putchar((char)c);
    //         else putchar('.');
    //     }
    //     printf("|\n");

    //     offset += line_len;
    // }
    printf("%s", cmd);
    /* 发送 AT 命令（fire-and-go） */
    ril_at_cmd_exc(cmd);

    free(cmd);
    free(hexbuf);
    return 0; /* 发送成功（表示命令已发出） */
}

#endif

/* ----------------- 指数退避睡眠（attempt 从 0 开始） ----------------- */
static void _backoff_sleep(int attempt)
{
    if (attempt < 0) attempt = 0;
    if (attempt > 8) attempt = 8;
    unsigned int delay = SK_OTA_CFG_RETRY_BASE_DELAY_MS << attempt;
    osDelay(delay);
}

/* ----------------- 内部：安全关闭并清理文件句柄 ----------------- */
static void _cleanup_pkg_fp(void)
{
    if (g_pkg_fp) {
        fclose(g_pkg_fp);
        g_pkg_fp = NULL;
    }
}

/* ----------------- 内部：发送 END（若 RIL 返回错误也继续清理） ----------------- */
static void _send_end_and_cleanup(void)
{
    int32_t ret = 0;
    ret = ril_systerm_updated_end();
    if (ret != 0) {
        printf("sk_ota: ril end returned %d\n", ret);
    }
    _cleanup_pkg_fp();
}

/* ----------------- 任务内部：处理 START 命令 ----------------- */
static void _handle_start(void)
{
    if (g_ota_running) {
        printf("sk_ota: start requested but OTA already running\n");
        return;
    }

    /* 校验 chunk 大小 */
    switch (g_chunk_size) {
        case 32: case 64: case 128: case 256: case 512:
            break;
        default:
            g_chunk_size = 512;
            break;
    }

    /* 打开升级包文件 */
    g_pkg_fp = fopen(SK_OTA_PKG_PATH, "rb");
    if (!g_pkg_fp) {
        printf("sk_ota: failed to open package %s\n", SK_OTA_PKG_PATH);
        if (g_ota_cb) g_ota_cb(SK_OTA_RESULT_FILE_OPEN_FAIL, -1);
        return;
    }

#if OTA_USE_AT_CMD
    /* 发送 AT 擦除命令 */
    _ota_do_start_at();
#else
    /* 发送 START 指令 */
    int32_t ret = ril_systerm_updated_start();
    if (ret != 0) {
        printf("sk_ota: ril start failed %d\n", ret);
        fclose(g_pkg_fp);
        g_pkg_fp = NULL;
        if (g_ota_cb) g_ota_cb(SK_OTA_RESULT_START_FAIL, ret);
        return;
    }
#endif
    /* 初始化序号并标记正在运行 */
    g_sn = 0;
    g_ota_abort_flag = 0;
    g_ota_running = 1;

    osDelay(WAIT_ERASE_MAX_TIME * 1000);
    /* 触发第一次发送（任务向自己发消息） */
    ota_cmd_msg_t next = { .cmd = OTA_CMD_SEND_NEXT };
    if (osal_msg_queue_write_copy(ota_cmd_queue_id, &next, sizeof(next), 0) != OSAL_SUCCESS) {
        /* 如果写队列失败，直接结束并回调错误 */
        printf("sk_ota: failed to post SEND_NEXT\n");
        _send_end_and_cleanup();
        g_ota_running = 0;
        if (g_ota_cb) g_ota_cb(SK_OTA_RESULT_INTERNAL_ERR, -2);
    }
}

/* ----------------- 任务内部：处理 SEND_NEXT 命令（每次只发送一片） ----------------- */
static void _handle_send_next(void)
{
    if (!g_pkg_fp) {
        printf("sk_ota: send_next but pkg_fp is NULL\n");
        /* 直接结束 */
        if (g_ota_cb) g_ota_cb(SK_OTA_RESULT_INTERNAL_ERR, -3);
        g_ota_running = 0;
        return;
    }

    if (g_ota_abort_flag) {
        printf("sk_ota: abort flag set before sending next\n");
        /* 触发 END 流程 */
        ota_cmd_msg_t endm = { .cmd = OTA_CMD_END };
        (void)osal_msg_queue_write_copy(ota_cmd_queue_id, &endm, sizeof(endm), 0);
        return;
    }

    /* 读取一片数据 */
    uint8_t *buf = (uint8_t *)malloc(g_chunk_size);
    if (!buf) {
        printf("sk_ota: malloc failed\n");
        ota_cmd_msg_t endm = { .cmd = OTA_CMD_END };
        (void)osal_msg_queue_write_copy(ota_cmd_queue_id, &endm, sizeof(endm), 0);
        return;
    }
    memset(buf, 0, g_chunk_size);

    size_t r = fread(buf, 1, g_chunk_size, g_pkg_fp);
    if (r == 0) {
        free(buf);
        if (feof(g_pkg_fp)) {
            /* 文件发送完毕，发 END */
            printf("sk_ota: EOF reached, total packets=%u\n", g_sn);
            // osDelay(1000);
            ota_cmd_msg_t endm = { .cmd = OTA_CMD_END };
            (void)osal_msg_queue_write_copy(ota_cmd_queue_id, &endm, sizeof(endm), 0);
            return;
        } else {
            /* 读错误 */
            printf("sk_ota: fread error\n");
            ota_cmd_msg_t endm = { .cmd = OTA_CMD_END };
            (void)osal_msg_queue_write_copy(ota_cmd_queue_id, &endm, sizeof(endm), 0);
            return;
        }
    }

#if OTA_USE_AT_CMD
    /* 计算 crc（xor8） */
        uint8_t crc = sk_ota_crc8(buf, r);

        /* 发送分片（构造 AT 命令并发送） - 带重试，注意 fire-and-go：重试仅代表再次发 AT 命令 */
        int attempt;
        int success = 0;
        for (attempt = 0; attempt < SK_OTA_CFG_MAX_RETRY && !g_ota_abort_flag; ++attempt) {
            int rc = _ota_do_send_piece_at(g_sn, buf, (uint32_t)r, crc);
            if (rc == 0) {
                success = 1;
                break; /* 因为 fire-and-go，认为已发送成功 */
            }
            printf("sk_ota: send piece sn=%u failed attempt=%d\n", g_sn, attempt);
            _backoff_sleep(attempt);
        }
        osDelay(3500);
#else
    /* 组装包并发送（含重试） */
    ril_upadated_package_t pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.download_type = 1;
    pkg.sn = g_sn;
    pkg.len = (uint32_t)r;
    pkg.data = buf;
    pkg.crc = sk_ota_crc8(buf, pkg.len);

    /* HEX+ASCII dump */
    printf("sk_ota: packet sn=%u raw_len=%u crc=0x%02X\n",
        g_sn, pkg.len, pkg.crc);

    const uint8_t *p = buf;
    uint32_t offset = 0;
    while (offset < pkg.len) {
        uint32_t line_len = (pkg.len - offset > 16) ? 16 : (pkg.len - offset);

        printf("%08X  ", offset);
        for (uint32_t i = 0; i < line_len; i++) {
            printf("%02X ", p[offset + i]);
        }
        for (uint32_t i = line_len; i < 16; i++) {
            printf("   ");
        }

        printf(" |");
        for (uint32_t i = 0; i < line_len; i++) {
            uint8_t c = p[offset + i];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("|\n");

        offset += line_len;
    }
    int attempt = 0;
    int success = 0;
    for (attempt = 0; attempt < SK_OTA_CFG_MAX_RETRY && !g_ota_abort_flag; ++attempt) {
        int32_t ret = ril_systerm_updated_download_package(&pkg);
        if (ret == 0) {
            success = 1;
            printf("sk_ota: send packet sn=%u sucess ret=%d attempt=%d\n", g_sn, ret, attempt);
            break;
        }
        printf("sk_ota: send packet sn=%u failed ret=%d attempt=%d\n", g_sn, ret, attempt);
        _backoff_sleep(attempt);
    }
    osDelay(1000);
#endif
    free(buf);

    if (g_ota_abort_flag) {
        printf("sk_ota: aborted during packet send\n");
        ota_cmd_msg_t endm = { .cmd = OTA_CMD_END };
        (void)osal_msg_queue_write_copy(ota_cmd_queue_id, &endm, sizeof(endm), 0);
        return;
    }

    if (!success) {
        printf("sk_ota: packet sn=%u final failed, will end OTA\n", g_sn);
        ota_cmd_msg_t endm = { .cmd = OTA_CMD_END };
        (void)osal_msg_queue_write_copy(ota_cmd_queue_id, &endm, sizeof(endm), 0);
        return;
    }

    /* 成功发送当前包：sn++，并再次给自己发 SEND_NEXT（推进下一包） */
    g_sn++;
    ota_cmd_msg_t next = { .cmd = OTA_CMD_SEND_NEXT };
    if (osal_msg_queue_write_copy(ota_cmd_queue_id, &next, sizeof(next), 0) != OSAL_SUCCESS) {
        /* 如果写队列失败，直接结束并回调错误 */
        printf("sk_ota: failed to post next SEND_NEXT\n");
        ota_cmd_msg_t endm = { .cmd = OTA_CMD_END };
        (void)osal_msg_queue_write_copy(ota_cmd_queue_id, &endm, sizeof(endm), 0);
    }
}

/* ----------------- 任务内部：处理 END 命令 ----------------- */
static void _handle_end(void)
{
#if OTA_USE_AT_CMD
    /* 发送 AT 结束命令 */
    _ota_do_end_at();
    printf("sk_ota: at update end\n");
    _cleanup_pkg_fp();
#else
    /* 发送 END 指令并清理 */
    int32_t ret = ril_systerm_updated_end();
    if (ret != 0) {
        printf("sk_ota: ril end failed %d\n", ret);
        _cleanup_pkg_fp();
        if (g_ota_cb) g_ota_cb(SK_OTA_RESULT_END_FAIL, ret);
        return;
    }
#endif
    osal_timer_mod(&audio_ota_check_timer, audio_ota_check_time);
    if (g_ota_abort_flag) {
        if (g_ota_cb) g_ota_cb(SK_OTA_RESULT_ABORTED, 0);
    } else {
        if (g_ota_cb) g_ota_cb(SK_OTA_RESULT_OK, (int)g_sn);
    }
}

/* ----------------- 任务内部：处理 ABORT 命令（外部直接设标记也可） ----------------- */
static void _handle_abort(void)
{
    g_ota_abort_flag = 1;
    /* 如果正在进行，会在下一处理点自行转为 END */
}

/* ----------------- OTA 任务入口 ----------------- */
static void sk_ota_task_entry(void *arg)
{
    (void)arg;
    ota_cmd_msg_t msg;
    unsigned int msg_len = sizeof(msg);

    for (;;) {
        if (osal_msg_queue_read_copy(ota_cmd_queue_id, &msg, &msg_len, (unsigned int)0xFFFFFFFF) == OSAL_SUCCESS) {
            switch (msg.cmd) {
                case OTA_CMD_START:
                    _handle_start();
                    break;
                case OTA_CMD_SEND_NEXT:
                    _handle_send_next();
                    break;
                case OTA_CMD_END:
                    _handle_end();
                    break;
                case OTA_CMD_ABORT:
                    _handle_abort();
                    break;
                default:
                    break;
            }
        }
        /* 重置 msg_len 以便下一次读取 */
        msg_len = sizeof(msg);
    }
}

void ota_check_timer_handler(void)
{
    printf("{%s():%d} ota_check_timer_handler in\r\n", __FUNCTION__, __LINE__);
    char ver[256] = {0};
    unsigned int size = 255;
    ril_get_software_version(ver, size);
    if(strlen(ver) != 0){
        printf("+SW_VERSION_MODEM:size = %d, ver = %s\n", size, ver);
        g_ota_running = 0;
        if(update_flag >= SK_UPDATE_MODEM_AND_MCU){
            g_ota_mcu_running = 1;
            mcu_update(NULL, 0);
        }
        osal_timer_stop(&audio_ota_check_timer);
        mcu_os_sys_reset(NULL, 0);
    }
    else{
        osal_timer_mod(&audio_ota_check_timer, audio_ota_check_time);
    }
    g_modem_ota_timeout_count++;
    if(g_modem_ota_timeout_count >= SK_OTA_CFG_MAX_TIMEOUT){
        g_modem_ota_timeout_count = 0;
        printf("sk_ota: modem ota timeout\n");
        mcu_os_sys_reset(NULL, 0);
    }
}

/* ----------------- 对外 API 实现 ----------------- */
int sk_ota_init(void)
{
    static int inited = 0;
    if (inited) return ERRCODE_SUCC;

    const char queue_name[] = "msgqueue_sk_ota";

    /* 创建任务 */
    osThreadAttr_t threadAttr;
    memset(&threadAttr, 0, sizeof(threadAttr));
    threadAttr.name = SK_OTA_TASK_NAME;
    threadAttr.stack_size = SK_OTA_TASK_STACK_SIZE;
    threadAttr.priority = SK_OTA_TASK_PRIO;
    threadAttr.stack_mem = memalign(16, threadAttr.stack_size); /* 若需要手动分配栈，设置此字段 */

    sk_ota_task_id = osThreadNew(sk_ota_task_entry, NULL, &threadAttr);
    if (sk_ota_task_id == NULL) {
        printf("sk_ota: create task failed\n");
        return ERRCODE_FAIL;
    }

    /* 创建消息队列 */
    int rc = osal_msg_queue_create(queue_name, SK_OTA_CMD_QUEUE_SIZE,
                                   &ota_cmd_queue_id, 0, sizeof(ota_cmd_msg_t));
    if (rc != ERRCODE_SUCC) {
        printf("sk_ota: create msgqueue failed %d\n", rc);
        return ERRCODE_FAIL;
    }

    audio_ota_check_timer.handler = ota_check_timer_handler;
    audio_ota_check_timer.interval = audio_ota_check_time;
	int ret=osal_timer_init(&audio_ota_check_timer);
    if(ret!=OSAL_SUCCESS)
    {
        printf("osal_timer_init failed\n");
    }

    inited = 1;
    return ERRCODE_SUCC;
}

void sk_ota_register_callback(sk_ota_callback_t cb)
{
    g_ota_cb = cb;
}

/* 启动 OTA（异步） */
int sk_ota_start_update_async(sk_update_mode_t update_type)
{
    int max_retries = 5;
    int retry_count = 0;
    led_blue_blink_stop();
    rgb_led_set_blue(1);
    if (access("user/paqichong/model.bin", F_OK) == 0)
    {
        // 文件存在，尝试删除（最多重试5次）
        for (retry_count = 0; retry_count < max_retries; retry_count++)
        {
            if (remove("user/paqichong/model.bin") == 0)
            {
                printf("file user/paqichong/model.bin deleted\n");
                break; // 删除成功，跳出循环
            }
            else
            {
                printf("remove user/paqichong/model.bin failed (attempt %d/%d)\n", 
                    retry_count + 1, max_retries);

                // 如果不是最后一次尝试，等待后重试
                if (retry_count < max_retries - 1)
                {
                    osDelay(10);
                }
            }
        }

        // 检查是否所有尝试都失败了
        if (retry_count == max_retries)
        {
            printf("remove user/paqichong/model.bin failed after %d attempts\n", max_retries);
            return ERRCODE_FAIL;
        }
    }
    if (sk_ota_init() != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if(update_type >= SK_UPDATE_MAX){
        return ERRCODE_FAIL;
    }
    update_flag = update_type;

    if(update_type >= SK_UPDATE_MCU_ONLY){
        g_ota_mcu_running = 1;
        mcu_update(NULL, 0);
        return ERRCODE_SUCC;
    }
    cfun0_state();
    osDelay(5000);
    ota_cmd_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.cmd = OTA_CMD_START;

    if (osal_msg_queue_write_copy(ota_cmd_queue_id, &msg, sizeof(msg), 0) != OSAL_SUCCESS) {
        cfun1_state();
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/* 中断 OTA；会尽快在任务内结束（在下一处理点触发 END） */
void sk_ota_abort(void)
{
    /* 设置标志并尝试发送一个 ABORT 消息以尽快被任务处理 */
    g_ota_abort_flag = 1;
    ota_cmd_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.cmd = OTA_CMD_ABORT;
    (void)osal_msg_queue_write_copy(ota_cmd_queue_id, &msg, sizeof(msg), 0);
}

int sk_ota_get_state(void)
{
    return g_ota_running;
}

int sk_ota_get_mcu_state(void)
{
    return g_ota_mcu_running;
}

