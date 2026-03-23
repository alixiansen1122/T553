/* at_cmd.h */
#ifndef MODEM_CMD_H
#define MODEM_CMD_H
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

#define WATCH_NCELL_MAX_NUM                     10

typedef enum {
    CMD_LOCALTIME,
    CMD_WIFISCAN,
    CMD_SERVINGCELL,
    CMD_NEIGHBOURCELL,
    CMD_NNMPSD,
    CMD_CSCON,
    CMD_CFUN0,
    CMD_CFUN1,
    CMD_CFUNGET,
    CMD_GET_ICCID,
    CMD_NET_AUTO_REG,
    CMD_IMSSWITCH,
    CMD_GET_NCONFIG,
    CMD_CLOSE_HEAD_COMPRESS,
    CMD_MODEM_SLEEP_CHECK,
    CMD_MODEM_RESET,
    CMD_CLOSE_CELL_RESELECTION,
    CMD_OPEN_CELL_RESELECTION,
    CMD_MAX
} AT_CMD;

typedef struct {
    int index;          // AP信息索引（从1开始，0代表扫描结束）
    int rssi;           // 信号强度（dBm）
    char bssid[13];     // 十六进制BSSID（MAC地址，12字符+1结束符）
    int channel_num;    // 信道号
} WifiAPInfo;

typedef struct {
    unsigned short mcc;
    unsigned short mnc;
    unsigned long tac;
    unsigned short euarfcn;
    unsigned long long cellid;
    int strength;
    int cage;
    unsigned char valid;
} WATCH_RESIDENT_CELL_INFO;

typedef struct {
    unsigned short mcc;
    unsigned short mnc;
    unsigned long tac;
    unsigned short euarfcn;
    unsigned long long cellid;
    int strength;
    int cage;
} WATCH_NEIGHBOR_CELL_INFO;

typedef struct _WATCH_CELL_INFO {
    WATCH_RESIDENT_CELL_INFO resident_cell_info;
    char network[8];
    unsigned char neighbor_cell_num;
    WATCH_NEIGHBOR_CELL_INFO neighbor_cell_list[WATCH_NCELL_MAX_NUM];
} WATCH_CELL_INFO;

typedef struct {
    unsigned char *data;
    unsigned int size;
} sk_modem_at_process_cmd_msg_t;

void sync_time_from_modem(void);

void modem_callback_init(void);

void wifi_scan(void);

void get_cell_info(void);

void RRC_release(void);

void cacon_state(void);

void cfun0_state(void);

void cfun1_state(void);

int cfun_get_state(void);

void cfun_check_cmd(void);

void modem_get_iccid(void);

void modem_auto_connect(void);
void modem_info_init(void);
void modem_get_nconfig(void);
void close_head_compress(void);
void net_debug_flag_set(unsigned char flag);
void SendnetCommand(unsigned char *cmd_buffer, const char* param);
void modem_sleep_check(void);
void modem_reset(void);
void cell_reselection_enable(char enable);

extern char sk_iccid[32];
extern char g_autoconnect_value[8];
extern bool net_auto_reg_done;
extern bool mt_cfun0_flag;
extern int g_tz_hours;

#ifdef __cplusplus
}
#endif

#endif // AT_CMD_H
