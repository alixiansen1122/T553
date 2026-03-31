#include "soc_osal.h"
#include "osal_timer.h"
#include "paqichong_web_service.h"
#include "cmsis_os2.h"
#include "modem_cmd.h"
#include "sk_ota.h"
#include "dev_storage.h"
#include "dfx_reboot.h"
#include "paqichong_debug_info.h"

extern uint8_t g_facflag;
extern uint8_t modem_get_reg_status(void);
extern uint8_t g_ipv4_status;

osal_timer timer_connect_ref;
char version_2131[64] = {0};
static bool found_version = 0;
char imei[32] = {0};
static int retry_count = 0;

void extract_first_parts(const char *input)
{
    char *copy = strdup(input);
    if (copy == NULL)
        return;

    char *line = strtok(copy, "\n");
    while (line != NULL)
    {
        if (strstr(line, "APPLICATION"))
        {
            char *ver = strchr(line, ',');
            if (ver != NULL)
            {
                int len = strlen(ver + 1) - 1;//做减1，是因为字符结尾有个\r
                strncpy(version_2131, ver + 1, len);
                version_2131[len] = '\0';
                found_version = 1;
                WS_PRINT("get 2131 version %s\n", version_2131);
            }
            break;
        }
        line = strtok(NULL, "\n");
    }
    free(copy);
}

static void connect_timer_handler(unsigned long data)
{
    (void)data;
    uint8_t status = 0;
    int ret = 0;
    char buf[256] = {0};

    /* 获取2131版本号 */
    if (found_version == 0)
    {
        ret = ril_get_software_version(buf, 255);
        if (ret == 0)
        {
            extract_first_parts(buf);
        }
        else
        {
            WS_PRINT("get 2131 version failed\n");
        }
    }
    watch_storage_get(STORAGE_IMEI, imei, 16);
    modem_get_iccid();
    cfun_check_cmd();
    osDelay(100);
    if((cfun_get_state() != 1) && (sk_ota_get_state() != 1)) {
        cfun1_state();
    }
    status = modem_get_reg_status();
    WS_PRINT("modem get reg status = %u, ipv4 status = %u, retry_count = %d\n", status, g_ipv4_status, retry_count);
    if (status && g_ipv4_status)
    {
        paqichong_send_cmd(PQC_WEB_EVENT_CONNECT);
    }
    else
    {
        retry_count++;
        /*时间太短可能会注不上网，160*8，大概20分钟会重启*/
        if (retry_count > 160)
        {
           WS_PRINT("reset system\n");
           paqichong_save_log("reset system");
           uapi_system_reboot(SYSTEM_SOFT_REBOOT);
        }
        osal_timer_mod(&timer_connect_ref, 8 * 1000);
    }
}

int32_t start_connect_paqichong(void)
{
    int ret = 0;
    // if(g_facflag == 0)
    //    return 0;
    /* 添加定时任务 */
    timer_connect_ref.handler = connect_timer_handler;
    timer_connect_ref.interval = 10 * 1000;
    ret = osal_timer_init(&timer_connect_ref);
    if (ret == OSAL_FAILURE)
    {
        WS_PRINT("create connect timer fail, ret %d\n", ret);
        return ret;
    }
    osal_timer_start(&timer_connect_ref);
    return ret;
}
