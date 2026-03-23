#ifndef __SK_OTA_H__
#define __SK_OTA_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------ 配置宏（可在编译时覆盖） ------------ */
#ifndef SK_OTA_CFG_CHUNK_SIZE
#define SK_OTA_CFG_CHUNK_SIZE     512   /* 允许值：32/64/128/256/512 */
#endif

#ifndef SK_OTA_CFG_MAX_RETRY
#define SK_OTA_CFG_MAX_RETRY      10     /* 每片最大重试次数 */
#endif

#ifndef SK_OTA_CFG_RETRY_BASE_DELAY_MS
#define SK_OTA_CFG_RETRY_BASE_DELAY_MS 200 /* 指数退避基值(ms) */
#endif

#ifndef SK_OTA_PKG_PATH
#define SK_OTA_PKG_PATH "/update/Hi2131EV100.fwpkg.bin"
#endif

#ifndef SK_OTA_CMD_QUEUE_SIZE
#define SK_OTA_CMD_QUEUE_SIZE 8
#endif




/* 线程 API：使用 CMSIS-RTOS2 类型，若不同请替换 */
#include <cmsis_os2.h>


/* ------------ 对外类型与 API ------------ */
typedef enum {
    SK_OTA_RESULT_OK = 0,
    SK_OTA_RESULT_START_FAIL,
    SK_OTA_RESULT_SEND_FAIL,
    SK_OTA_RESULT_END_FAIL,
    SK_OTA_RESULT_FILE_OPEN_FAIL,
    SK_OTA_RESULT_FILE_READ_FAIL,
    SK_OTA_RESULT_ABORTED,
    SK_OTA_RESULT_INTERNAL_ERR
} sk_ota_result_e;

typedef enum
{
    SK_UPDATE_MODEM_ONLY = 1,
    SK_UPDATE_MODEM_AND_MCU = 2,
    SK_UPDATE_MCU_ONLY = 3,
    SK_UPDATE_MAX = 4,
}sk_update_mode_t;

/* 升级结果回调：升级完成或失败都会调用一次 */
typedef void (*sk_ota_callback_t)(sk_ota_result_e result, int detail);

/* 初始化（创建任务与队列） */
int sk_ota_init(void);

/* 注册回调（可选） */
void sk_ota_register_callback(sk_ota_callback_t cb);

/* 异步启动 OTA（向任务发送 START 指令，立即返回） */
int sk_ota_start_update_async(sk_update_mode_t update_type);

/* 中途取消当前升级（线程安全） */
void sk_ota_abort(void);

/* CRC8 工具（可复用） */
uint8_t sk_ota_crc8(const uint8_t *data, size_t len);

int sk_ota_get_state(void);
int sk_ota_get_mcu_state(void);

#ifdef __cplusplus
}
#endif

#endif /* __SK_OTA_H__ */
