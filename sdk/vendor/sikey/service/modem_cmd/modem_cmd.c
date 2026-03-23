/* at_cmd.c */
#include "modem_cmd.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "ril_interface.h"
#include "errcode.h"
#include <time.h>
#include <sys/time.h>
#include "location_service.h"
#include "soc_osal.h"
#include "errcode.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include "osal_list.h"
#include "sk_ota.h"
#include "dev_storage.h"
#include "net_data_process.h"
#include "osal_timer.h"
#include "dfx_reboot.h"
#include "paqichong_debug_info.h"
#define MAX_AP 30
#define MAX_MODEM_AT_PROCESS_QUEUE_NUM 10

extern unsigned char get_download_file_state(void);
uint8_t g_facflag = 0;
WifiAPInfo ap_info_list[MAX_AP];
int ap_count = 0;
WATCH_CELL_INFO cell_info;
static char ncell_process_flag = 0;
static char net_auto_reg_flag = 0;
static char net_cfun0_set_flag = 0;
static char net_cfun1_set_flag = 0;
bool net_auto_reg_done = false;
bool mt_cfun0_flag = false;
int cfun_state_current = 0;
uint8_t net_debug_flag = 0;
static uint8_t g_debug_cmd_buf[64] = {0};
osal_timer timer_cmd_result_report;
char cmd_result[4096] = {0};
size_t g_offset = 0;

#define MODEM_CMD_BACK_DATA_SIZE     512
osal_task *modem_at_process_task_id;
unsigned long modem_at_process_queue_id;

#define MINUTES_PER_TZ_UNIT 15  // 每个时区单位代表15分钟
char sk_iccid[32] = {0};
char g_autoconnect_value[8] = {0};  // 假设值不会超过63个字符
typedef struct {
    const char* format;     // AT命令格式字符串
    bool has_parameter;     // 是否需要参数
} AT_Command;

static const AT_Command at_commands[] = {
    [CMD_LOCALTIME]={"AT+CCLK?\r\n",false},
    [CMD_WIFISCAN]={"AT+NWIFISCAN=3fff,24,1,10,1\r\n",false},
    [CMD_SERVINGCELL]={"AT+NUESTATS=SERVINGCELL\r\n",false},
    [CMD_NEIGHBOURCELL]={"AT+NUESTATS=NEIGHBOURCELL,1\r\n",false},
    [CMD_NNMPSD]={"AT+NNMPSD=1\r\n",false},
    [CMD_CSCON]={"AT+CSCON?\r\n",false},
    [CMD_CFUN0]={"AT+CFUN=0\r\n",false},
    [CMD_CFUN1]={"AT+CFUN=5\r\n",false},
    [CMD_CFUNGET]={"AT+CFUN?\r\n",false},
    [CMD_GET_ICCID]={"AT+ICCID?\r\n",false},
    [CMD_NET_AUTO_REG]={"AT+NCONFIG=AUTOCONNECT,APP\r\n",false},
    [CMD_IMSSWITCH]={"AT+IMSSWITCH=0\r\n",false},
    [CMD_GET_NCONFIG]={"AT+NCONFIG?\r\n",false},
    [CMD_CLOSE_HEAD_COMPRESS]={"AT+NCONFIG=HEAD_COMPRESS,FALSE\r\n",false},
    [CMD_MODEM_SLEEP_CHECK]={"AT+NQSTATE=1,1\r\n",false},
    [CMD_MODEM_RESET]={"AT+RESET\r\n",false},
    [CMD_CLOSE_CELL_RESELECTION]={"AT+NCONFIG=CELL_RESELECTION,FALSE\r\n",false},
    [CMD_OPEN_CELL_RESELECTION]={"AT+NCONFIG=CELL_RESELECTION,TRUE\r\n",false},
};

/**
 * @brief 从AT命令字符串中提取+cmd命令
 * @param input 输入的AT命令字符串
 * @param cmd 输出的命令缓冲区
 * @param cmd_size 缓冲区大小
 * @return 成功返回0，失败返回-1
 */
static int extract_at_command(const char *input, char *cmd, size_t cmd_size)
{
    if (input == NULL || cmd == NULL || cmd_size == 0)
    {
        return -1;
    }

    // 跳过开头的 "AT" 或 "at"
    const char *start = input;
    if (strncasecmp(start, "AT", 2) == 0)
    {
        start += 2;
    }

    // 查找 "+" 符号
    const char *plus_pos = strchr(start, '+');
    if (plus_pos == NULL)
    {
        return -1; // 没有找到 +
    }

    // 移动到 + 后面
    plus_pos++;

    // 查找命令结束位置
    const char *end_pos = plus_pos;
    while (*end_pos != '\0' && *end_pos != '?' && *end_pos != ' ' &&
           *end_pos != '\t' && *end_pos != '\r' && *end_pos != '\n' && *end_pos != '=')
    {
        end_pos++;
    }

    // 计算命令长度
    size_t cmd_length = end_pos - plus_pos;
    if (cmd_length == 0)
    {
        return -1; // 命令为空
    }

    if (cmd_length >= cmd_size)
    {
        cmd_length = cmd_size - 1; // 截断
    }

    // 复制命令
    strncpy(cmd, plus_pos, cmd_length);
    cmd[cmd_length] = '\0';

    return 0;
}

/**
 * @brief 通用分隔符提取函数
 * @param input 输入字符串
 * @param start_delim 开始分隔符
 * @param end_delim 结束分隔符
 * @param output 输出缓冲区
 * @param output_size 缓冲区大小
 * @return 成功返回0，失败返回-1
 */
static int extract_between_delimiters(const char *input, const char *start_delim,
                                      const char *end_delim, char *output, size_t output_size)
{
    if (input == NULL || start_delim == NULL || end_delim == NULL ||
        output == NULL || output_size == 0)
    {
        return -1;
    }

    // 查找开始分隔符
    const char *start = strstr(input, start_delim);
    if (start == NULL)
    {
        return -1;
    }

    // 移动到开始分隔符之后
    start += strlen(start_delim);

    // 查找结束分隔符
    const char *end = strstr(start, end_delim);
    if (end == NULL)
    {
        // 如果没有找到结束分隔符，使用字符串结束
        end = start + strlen(start);
    }

    // 计算提取内容的长度
    size_t length = end - start;
    if (length == 0 || length >= output_size)
    {
        return -1;
    }

    // 复制内容
    strncpy(output, start, length);
    output[length] = '\0';

    return 0;
}

static void cmd_result_timer_handler(unsigned long data)
{
    (void)data;
    ws_report_debug_info(cmd_result);
}

void SendATCommand(AT_CMD cmd, const char* param)
{
    int32_t ret;
    char buffer[256] = {0};
    if (cmd >= CMD_MAX || cmd < 0) {
        return;
    }

    int state = sk_ota_get_state();
    if (state) {
        return;
    }
    const AT_Command* current_cmd = &at_commands[cmd];

    if (current_cmd->has_parameter) {
        if (!param) {
            // 处理参数缺失错误
            return;
        }
        snprintf(buffer, sizeof(buffer), current_cmd->format, param);
    } else {
        strncpy(buffer, current_cmd->format, sizeof(buffer));
    }

    ret = ril_at_cmd_exc(buffer);
    if (ret!= 0) {
        printf("modem AT command fail\n");
        return;
    }
    printf("Sending: %s\n", buffer); // 示例输出
}

void SendnetCommand(uint8_t *cmd_buffer, const char* param)
{
    (void)param;
    int32_t ret;
    if(!cmd_buffer) {
        return;
    }
    g_offset = 0;
    memset(cmd_result, 0, 4096);
    /* 提取Command信息, 例如+NCONFIG字符，用于过滤上报信息 */
    memset(g_debug_cmd_buf, 0, 64);
    extract_at_command((char *)cmd_buffer, (char *)g_debug_cmd_buf, 64);

    ret = ril_at_cmd_exc(cmd_buffer);
    if (ret!= 0) {
        printf("modem AT command fail\n");
        return;
    }
    printf("Sending: %s\n", cmd_buffer); // 示例输出
}

int g_tz_hours = 0;
int parse_timeqry_string(const char *str, struct timeval *tv, struct timezone *tz) {
    const char *s = str;
    int timezone=0;
    while (*s == '\r' || *s == '\n') {
        s++;
    }
    const char *prefix = "+CCLK: ";
    const size_t prefix_len = strlen(prefix);
    if (strncmp(s, prefix, prefix_len) != 0) {
        return -1;  // 前缀不匹配
    }
    s += prefix_len; // 跳过前缀
    struct tm tm = {0};
    int tz_offset, dummy;

    // 解析日期、时间及时区（忽略dt字段）
    if (sscanf(s, "\"%02d/%02d/%02d,%02d:%02d:%02d+%d\"",
               &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
               &tm.tm_hour, &tm.tm_min, &tm.tm_sec,
               &tz_offset) != 7) {
        printf(" parse_timeqry_string data err \r\n");
        return -1;  // 字段数量不匹配
    }
    // 调整tm结构
    tm.tm_year += 100;    // 25 -> 2025年（tm_year=125）
    tm.tm_mon -= 1;       // 月份从0开始（02 -> 1月）
    tm.tm_isdst = -1;     // 自动判断夏令时
    uint32_t sec = (uint32_t)__tm_to_secs(&tm);
    tv->tv_usec = 0;

    // 填充timezone（可选）
    // if (tz != NULL) {
    //     tz->tz_minuteswest = tz_offset * MINUTES_PER_TZ_UNIT;  // 转换为分钟
    //     tz->tz_dsttime = 0;  // 假设无夏令时
    // }
    // timezone= tz->tz_minuteswest /60;
    // // watch_set_timezone(timezone);
    // tv->tv_sec = sec + tz_offset*MINUTES_PER_TZ_UNIT*60-timezone*3600;
    return 0;
}

int parse_time_string(const char *str, struct timeval *tv) {
    const char *s = str;

    // 跳过前缀和空白字符
    while (*s == '\r' || *s == '\n' || *s == ' ') s++;

    const char *prefix = "+CCLK:";
    if (strncmp(s, prefix, strlen(prefix)) != 0) {
        return -1;
    }
    s += strlen(prefix);
    while (*s == ' ') s++; // 跳过可能存在的空格

    struct tm tm = {0};
    int tz_offset_quarters;

    // 解析字符串
    if (sscanf(s, "\"%02d/%02d/%02d,%02d:%02d:%02d+%d\"",
               &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
               &tm.tm_hour, &tm.tm_min, &tm.tm_sec,
               &tz_offset_quarters) != 7) {
        return -1;
    }

    // 调整tm结构
    tm.tm_year += 100;    // 25 -> 2025年
    tm.tm_mon -= 1;       // 月份从1-12调整为0-11
    tm.tm_isdst = -1;     // 自动判断夏令时

    // 计算时区偏移（分钟）
    int tz_minutes = tz_offset_quarters * 15; // 32 * 15 = 480分钟
    int tz_hours = tz_minutes / 60;           // 480 / 60 = 8小时
    int tz_remaining_minutes = tz_minutes % 60;
    g_tz_hours = tz_hours;
    printf("timezone: UTC+%02d:%02d\n", tz_hours, tz_remaining_minutes);

    // 方法1：将本地时间转换为UTC时间
    // 由于这是UTC+8的时间，减去8小时得到UTC时间
    time_t local_secs = mktime(&tm); // 先将tm转换为本地时间的时间戳
    //time_t utc_secs = local_secs + (tz_hours * 3600 + tz_remaining_minutes * 60);

    tv->tv_sec = local_secs;
    tv->tv_usec = 0;
    printf("UTC time: %d\n", tv->tv_sec);
    return 0;
}

// 静态变量或外部变量，用于记录当前数组中已存储的AP数量
static int current_count = 0;
/**
 * @brief 解析一行NWIFISCAN数据并填充到结构体数组中
 * @param line 以'+NWIFISCAN: '开头的字符串行
 * @param ap_list 存储解析结果的WifiAPInfo结构体数组
 * @param list_size 结构体数组的大小（最大容量）
 * @return int 成功解析并存储的AP数量（正值），或错误码（负值）
 *         -1: 行首格式错误
 *         -2: 索引为0，扫描结束
 *         -3: 字段解析失败（sscanf返回值不符）
 *         -4: 数组已满
 */
int parse_nwifiscan_line(const char *line, WifiAPInfo *ap_list, int list_size) {
    int idx, rssi, channel;
    char bssid_str[13] = {0}; // 临时存储BSSID字符串
    int parsed_count = 0; // 记录当前已解析的AP条目数（静态变量或通过参数管理）

    while (*line == '\r' || *line == '\n') {
        line++;
    }
    // 1. 检查行首是否是"+NWIFISCAN: "
    if (strncmp(line, "+NWIFISCAN: ", 12) != 0) {
        printf("1 modem wifi scan line parse err, line: %s \n", line);
        return -1; // 行首格式错误
    }

    // 2. 跳过"+NWIFISCAN: "，解析后面的字段
    // 字段格式：索引,预留,预留,RSSI,BSSID,信道
    int fields_parsed = sscanf(line + 12, "%d,,,%d,%12[^,],%d",
                               &idx, &rssi, bssid_str, &channel);

    // 3. 检查解析出的字段数量是否为4（索引、RSSI、BSSID、信道）
    if (fields_parsed < 4) {
        // 尝试解析索引为0的特殊情况（扫描结束）
        if (sscanf(line + 12, "%d", &idx) == 1 && idx == 0) {
            printf("modem wifi scan end \n");
            return -2; // 扫描结束标识
        }
        printf("2 modem wifi scan line parse err, line: %s \n", line);
        return -3; // 字段解析失败
    }

    // 4. 检查索引是否为0（扫描结束）
    if (idx == 0) {
        //此处需要处理上报扫描结束
        printf("modem wifi scan end \n");
        return -2;
    }

    // 5. 检查数组是否已满
    if (current_count >= list_size) {
        printf("modem wifi scan line is full \n");
        return -4;
    }

    // 6. 将解析出的数据存入结构体
    ap_list[current_count].index = idx;
    ap_list[current_count].rssi = rssi;
    // 确保BSSID字符串拷贝不会溢出，并手动添加结束符
    strncpy(ap_list[current_count].bssid, bssid_str, sizeof(ap_list[current_count].bssid) - 1);
    ap_list[current_count].bssid[sizeof(ap_list[current_count].bssid) - 1] = '\0';
    ap_list[current_count].channel_num = channel;
    ap_count++;
    current_count++; // 指向下一个空闲位置
    return current_count; // 返回当前已存储的数量
}

/**
 * @brief 打印 WiFi AP 信息列表中的有效数据
 * @param ap_list WifiAPInfo 结构体数组
 * @param list_size 数组的最大容量
 * @param print_header 是否打印表头（1打印，0不打印）
 */
void print_wifi_ap_list(const WifiAPInfo *ap_list, int list_size, int print_header) {
    int valid_count = 0;

    // 打印表头
    if (print_header) {
        printf("Idx | RSSI(dBm) | BSSID         | Channel\n");
        printf("----+-----------+---------------+--------\n");
    }

    // 遍历数组，打印有效条目
    for (int i = 0; i < list_size; i++) {
        // 假设索引为0的条目无效（或表示结束）
        if (ap_list[i].index == 0) {
            continue; // 跳过无效条目
        }
        valid_count++;
        printf("%-3d | %-9d | %s | %-7d\n",
               ap_list[i].index,
               ap_list[i].rssi,
               ap_list[i].bssid,
               ap_list[i].channel_num);
    }

    if (valid_count == 0) {
        printf("No valid AP information found.\n");
    } else {
        printf("Total: %d AP(s)\n", valid_count);
    }
}

// 解析 SERVINGCELL 字符串到 WATCH_CELL_INFO 结构体
int parse_serving_cell(const char* data, WATCH_CELL_INFO* cell_info) {
    if (data == NULL || cell_info == NULL) {
        printf("Input parameter is NULL. data: %p, cell_info: %p\n", data, cell_info);
        return -1;
    }
    while (*data == '\r' || *data == '\n') {
        data++;
    }
    // 清空结构体
    memset(cell_info, 0, sizeof(WATCH_CELL_INFO));
    printf("Function started. WATCH_CELL_INFO struct initialized to zero.\n");

    // 解析 SERVINGCELL 格式:
    // +NUESTATS: SERVINGCELL,NOCONN,LTE,TDD,460,00,1B3A02A,232,36275,34,4,4,180B,-102,-7,-69,8,29
    // 对应格式: SERVINGCELL,<state>,LTE,<is_tdd>,<MCC>,<MNC>,<cellID>,<PCI>,<EARFCN>,<freq_band_ind>,
    // <UL_bandwidth>,<DL_bandwidth>,<TAC>,<RSRP>,<RSRQ>,<RSSI>,<SINR>,<srxlev>

    char state[16] = {0}, tech[8] = {0}, is_tdd[8] = {0};
    unsigned int cellid_hex = 0, tac_hex = 0;
    int pci = 0, freq_band_ind = 0, ul_bw = 0, dl_bw = 0;
    int rsrq = 0, rssi = 0, sinr = 0, srxlev = 0;

    // 打印原始输入数据
    printf("Raw input data: %s\n", data);

    int result = sscanf(data,
        "+NUESTATS: SERVINGCELL,%15[^,],%7[^,],%7[^,],%hu,%hu,%x,%d,%hu,%d,%d,%d,%x,%d,%d,%d,%d,%d,%d",
        state, tech, is_tdd,
        &cell_info->resident_cell_info.mcc,
        &cell_info->resident_cell_info.mnc,
        &cellid_hex,
        &pci,
        &cell_info->resident_cell_info.euarfcn,
        &freq_band_ind,
        &ul_bw,
        &dl_bw,
        &tac_hex,
        &cell_info->resident_cell_info.strength,
        &rsrq,
        &rssi,
        &sinr,
        &srxlev);

    printf("sscanf parsed %d fields (expected at least 11).\n", result);
    printf("Parsed basic info - state: '%s', tech: '%s', TDD/FDD: '%s'\n", state, tech, is_tdd);
    printf("Parsed identifiers - MCC: %hu, MNC: %hu, PCI: %d, EARFCN: %hu\n",
                cell_info->resident_cell_info.mcc,
                cell_info->resident_cell_info.mnc,
                pci,
                cell_info->resident_cell_info.euarfcn);
    printf("Parsed hex strings - cellid_hex: 0x%x, tac_hex: 0x%x\n", cellid_hex, tac_hex);
    printf("Parsed measurements - RSRP: %d, RSRQ: %d, RSSI: %d, SINR: %d, srxlev: %d\n",
                cell_info->resident_cell_info.strength, rsrq, rssi, sinr, srxlev);
    printf("Parsed band info - freq_band_ind: %d, UL_bw: %d, DL_bw: %d\n",
                freq_band_ind, ul_bw, dl_bw);

    if (result >= 11) { // 至少需要解析出前11个必要字段
        cell_info->resident_cell_info.valid = 1;
        cell_info->resident_cell_info.cellid = (unsigned long long)cellid_hex;
        cell_info->resident_cell_info.tac = (unsigned long)tac_hex;

        // 拷贝网络类型
        strncpy(cell_info->network, tech, sizeof(cell_info->network) - 1);
        cell_info->network[sizeof(cell_info->network) - 1] = '\0'; // 确保字符串终止

        printf("Successfully parsed serving cell information:\n");
        printf("  Final values - MCC: %hu, MNC: %hu, TAC: 0x%lx\n",
                    cell_info->resident_cell_info.mcc,
                    cell_info->resident_cell_info.mnc,
                    cell_info->resident_cell_info.tac);
        printf("  Final values - CellID: 0x%llx, EARFCN: %hu, RSRP: %d\n",
                    cell_info->resident_cell_info.cellid,
                    cell_info->resident_cell_info.euarfcn,
                    cell_info->resident_cell_info.strength);
        printf("  Final values - Network type: '%s', Valid flag: %u\n",
                    cell_info->network, cell_info->resident_cell_info.valid);

        printf("Serving cell parsing completed successfully.\n");
        return 0;
    }

    printf("Error: Expected at least 11 fields, but only parsed %d. Parsing failed.\n", result);
    printf("This usually indicates a format mismatch between the string and sscanf pattern.\n");
    return -1;
}

// 解析 NEIGHBOURCELL 字符串到 WATCH_NEIGHBOR_CELL_INFO 结构体
static void trim_inplace(char *s) {
    if (!s) return;
    // trim left
    char *p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    // trim right
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len-1])) {
        s[len-1] = '\0';
        len--;
    }
}

int parse_neighbor_cell(const char* data, WATCH_NEIGHBOR_CELL_INFO* neighbor_cell) {
    if (data == NULL || neighbor_cell == NULL) {
        printf("Input parameter is NULL.\n");
        return -1;
    }

    memset(neighbor_cell, 0, sizeof(WATCH_NEIGHBOR_CELL_INFO));

    // 找到 NEIGHBOURCELL 段并定位到第一个字段
    const char *pstart = strstr(data, "NEIGHBOURCELL");
    if (!pstart) {
        printf("Cannot find NEIGHBOURCELL in data: %s\n", data);
        return -1;
    }
    const char *fields = strchr(pstart, ',');
    if (!fields) {
        printf("No fields after NEIGHBOURCELL: %s\n", data);
        return -1;
    }
    fields++; // 指向第1个字段的首字符（可能是双引号、逗号、空格或结束）

    printf("Adjusted parser started. Data: %s\n", data);

    // 解析字段（最多解析 MAX_TOKENS 个）
    #define MAX_TOKENS 16
    #define TOKEN_SZ 128
    char tokens[MAX_TOKENS][TOKEN_SZ];
    for (int i=0;i<MAX_TOKENS;i++) tokens[i][0] = '\0';
    int tok_count = 0;

    const char *s = fields;
    // 先计算预期字段数（逗号数量 + 1），但要忽略引号内的逗号
    int expected_fields = 0;
    if (*s != '\0') {
        expected_fields = 1;
        int in_quote = 0;
        for (const char *q = s; *q; ++q) {
            if (*q == '"') in_quote = !in_quote;
            else if (*q == ',' && !in_quote) expected_fields++;
        }
        if (expected_fields > MAX_TOKENS) expected_fields = MAX_TOKENS;
    }

    // 抽取字段，支持空字段 ,, 以及 "quoted,with,commas"
    while (tok_count < expected_fields && *s != '\0') {
        char tmp[TOKEN_SZ];
        tmp[0] = '\0';

        if (*s == '"') {
            // quoted field
            s++; // skip opening quote
            const char *end = strchr(s, '"');
            if (!end) {
                // 没有闭合引号：取到行尾
                size_t len = strlen(s);
                if (len >= sizeof(tmp)) len = sizeof(tmp)-1;
                memcpy(tmp, s, len);
                tmp[len] = '\0';
                s += len;
            } else {
                size_t len = (size_t)(end - s);
                if (len >= sizeof(tmp)) len = sizeof(tmp)-1;
                memcpy(tmp, s, len);
                tmp[len] = '\0';
                s = end + 1; // 跳过闭合引号
                if (*s == ',') s++; // 跳过字段分隔符
            }
        } else {
            // non-quoted: 直到下一个未被引号包围的逗号或行尾
            const char *end = strchr(s, ',');
            if (!end) {
                size_t len = strlen(s);
                if (len >= sizeof(tmp)) len = sizeof(tmp)-1;
                memcpy(tmp, s, len);
                tmp[len] = '\0';
                s += len;
            } else {
                size_t len = (size_t)(end - s);
                if (len >= sizeof(tmp)) len = sizeof(tmp)-1;
                memcpy(tmp, s, len);
                tmp[len] = '\0';
                s = end + 1; // 跳到分隔符后
            }
        }

        trim_inplace(tmp);
        strncpy(tokens[tok_count], tmp, TOKEN_SZ-1);
        tokens[tok_count][TOKEN_SZ-1] = '\0';
        tok_count++;
        // 如果现在 s 指向 '\0'，循环会在条件处结束
    }

    // 特殊情况：当 fields 非空但字符串以单个逗号结束（例如 "...,"），上面的循环可能不会添加最后一个空字段
    // 用 expected_fields 去补齐（expected_fields 已经按忽略引号的逗号计好）
    for (int i = tok_count; i < expected_fields && i < MAX_TOKENS; ++i) {
        tokens[i][0] = '\0';
        tok_count++;
    }

    printf("Adjusted token count: %d (expected %d)\n", tok_count, expected_fields);
    for (int i = 0; i < tok_count; ++i) {
        printf(" token[%d] = '%s'\n", i+1, tokens[i]);
    }

    // 映射字段（依据说明：1 cell_type,2 tech,3 EARFCN,4 PCI,5 RSRP,6 RSRQ,7 srxlev,
    // 8 thresh_low,9 thresh_high,10 cell_resel_priority,11 cellID,12 TAC,13 freqBandIndicator,14 plmns）
    char cell_type[32] = {0}, tech[16] = {0};
    char cellid_str[64] = {0}, tac_str[64] = {0}, plmns_str[128] = {0};
    unsigned long long cellid = 0;
    unsigned long tac_hex = 0;
    int pci = 0, rsrq = 0, srxlev = 0;
    int thresh_low = 0, thresh_high = 0, cell_resel_priority = 0;
    int freq_band_ind = 0;

    if (tok_count > 0) strncpy(cell_type, tokens[0], sizeof(cell_type)-1);
    if (tok_count > 1) strncpy(tech, tokens[1], sizeof(tech)-1);
    if (tok_count > 2 && tokens[2][0] != '\0') neighbor_cell->euarfcn = (unsigned short)strtoul(tokens[2], NULL, 10);
    if (tok_count > 3 && tokens[3][0] != '\0') pci = atoi(tokens[3]);
    if (tok_count > 4 && tokens[4][0] != '\0') neighbor_cell->strength = atoi(tokens[4]); // RSRP
    if (tok_count > 5 && tokens[5][0] != '\0') rsrq = atoi(tokens[5]);
    if (tok_count > 6 && tokens[6][0] != '\0') srxlev = atoi(tokens[6]);
    if (tok_count > 7 && tokens[7][0] != '\0') thresh_low = atoi(tokens[7]);
    if (tok_count > 8 && tokens[8][0] != '\0') thresh_high = atoi(tokens[8]);
    if (tok_count > 9 && tokens[9][0] != '\0') cell_resel_priority = atoi(tokens[9]);
    if (tok_count > 10) strncpy(cellid_str, tokens[10], sizeof(cellid_str)-1);
    if (tok_count > 11) strncpy(tac_str, tokens[11], sizeof(tac_str)-1);
    if (tok_count > 12 && tokens[12][0] != '\0') freq_band_ind = atoi(tokens[12]);
    if (tok_count > 13) strncpy(plmns_str, tokens[13], sizeof(plmns_str)-1);

    printf("Parsed raw - cell_type:'%s', tech:'%s'\n", cell_type, tech);
    printf("Parsed numeric - EARFCN:%hu, PCI:%d, RSRP:%d, RSRQ:%d\n", neighbor_cell->euarfcn, pci, neighbor_cell->strength, rsrq);
    printf("Parsed strings - cellid:'%s', tac:'%s', plmns:'%s'\n", cellid_str, tac_str, plmns_str);

    // 解析 cellID (十六进制) 与 TAC (十六进制)
    if (cellid_str[0] != '\0') {
        // 支持带/不带 0x 前缀
        neighbor_cell->cellid = strtoull(cellid_str, NULL, 16);
        printf("CellID converted: '%s' -> 0x%llx\n", cellid_str, (unsigned long long)neighbor_cell->cellid);
    }
    if (tac_str[0] != '\0') {
        neighbor_cell->tac = (unsigned long)strtoul(tac_str, NULL, 16);
        printf("TAC converted: '%s' -> 0x%lx\n", tac_str, neighbor_cell->tac);
    }

// 解析 PLMNs 字段（可能是 "46000" 或 "46000,46015"）
if (plmns_str[0] != '\0') {
    char plmn_copy[128];
    strncpy(plmn_copy, plmns_str, sizeof(plmn_copy)-1);
    plmn_copy[sizeof(plmn_copy)-1] = '\0';

    // 取第一个 PLMN（例如 "46000"）
    char *tok = strtok(plmn_copy, ",");
    if (tok) {
        size_t len = strlen(tok);
        if (len >= 5) {  // 至少要有 MCC(3) + MNC(2)
            neighbor_cell->mcc = (unsigned short)atoi(tok);      // 先取前3位
            neighbor_cell->mcc /= 100;                           // 保证只有 MCC 部分

            // 提取 MCC
            char mcc_str[4] = {0};
            strncpy(mcc_str, tok, 3);
            neighbor_cell->mcc = (unsigned short)atoi(mcc_str);

            // 提取 MNC（可能是2位或3位）
            char mnc_str[4] = {0};
            strncpy(mnc_str, tok + 3, len - 3);
            neighbor_cell->mnc = (unsigned short)atoi(mnc_str);

            printf("PLMN parsed: '%s' -> MCC:%hu, MNC:%hu\n",
                   tok, neighbor_cell->mcc, neighbor_cell->mnc);
        } else {
            printf("Invalid PLMN format: '%s'\n", tok);
        }
    }
} else {
    printf("PLMNs string is empty.\n");
}

    // 判定是否成功：至少应包含 EARFCN 和 PCI 和 RSRP（你可以根据需要调整）
    if (tok_count >= 5) {
        printf("Neighbor cell parsing completed successfully (tokens=%d).\n", tok_count);
        return 0;
    } else {
        printf("Error: not enough fields parsed (tokens=%d). Data may not match expected format.\n", tok_count);
        return -1;
    }
}

void print_watch_cell_info(const WATCH_CELL_INFO *cell_info) {
    if (cell_info == NULL) {
        printf("Error: NULL cell info pointer\n");
        return;
    }

    printf("=========== WATCH_CELL_INFO ===========\n");

    // 打印服务小区信息
    printf("[Serving Cell Info]\n");
    printf("  Network: %s\n", cell_info->network);
    printf("  Valid: %u\n", cell_info->resident_cell_info.valid);
    printf("  MCC: %u\n", cell_info->resident_cell_info.mcc);
    printf("  MNC: %u\n", cell_info->resident_cell_info.mnc);
    printf("  TAC: 0x%lX\n", cell_info->resident_cell_info.tac);
    printf("  EARFCN: %u\n", cell_info->resident_cell_info.euarfcn);
    printf("  CellID: 0x%llX\n", cell_info->resident_cell_info.cellid);
    printf("  Strength: %d\n", cell_info->resident_cell_info.strength);
    printf("  CAGE: %d\n", cell_info->resident_cell_info.cage);

    // 打印邻区小区信息
    printf("\n[Neighbor Cells] Count: %u\n", cell_info->neighbor_cell_num);

    for (int i = 0; i < cell_info->neighbor_cell_num; i++) {
        printf("  Neighbor %d:\n", i + 1);
        printf("    MCC: %u\n", cell_info->neighbor_cell_list[i].mcc);
        printf("    MNC: %u\n", cell_info->neighbor_cell_list[i].mnc);
        printf("    TAC: 0x%lX\n", cell_info->neighbor_cell_list[i].tac);
        printf("    EARFCN: %u\n", cell_info->neighbor_cell_list[i].euarfcn);
        printf("    CellID: 0x%llX\n", cell_info->neighbor_cell_list[i].cellid);
        printf("    Strength: %d\n", cell_info->neighbor_cell_list[i].strength);
        printf("    CAGE: %d\n", cell_info->neighbor_cell_list[i].cage);
    }

    printf("=======================================\n");
}

int parse_cfun_string(const char* input_str) {
    // 检查输入指针是否有效
    if (input_str == NULL) {
        printf("input_str is NULL\n");
        return -1;
    }
    while (*input_str == '\r' || *input_str == '\n') {
        input_str++;
    }

    // 定义匹配的模式："+CFUN: "
    const char* pattern = "+CFUN: ";
    size_t pattern_len = strlen(pattern);

    // 移动到数字部分开始的位置
    const char* number_start = input_str + pattern_len;

    // 检查数字部分是否为空
    if (*number_start == '\0') {
        printf("have no number\n");
        return -1;
    }

    // 使用strtol将数字部分转换为整数，并进行错误检查[2](@ref)
    char* endptr;
    long number = strtol(number_start, &endptr, 10);  // 10表示十进制

    // 检查转换是否成功[2](@ref)
    if (endptr == number_start) {
        printf("number is invalid\n");
        return -1;
    }

    // 检查数字是否在int范围内（根据编译器）
    if (number < -2147483648 || number > 2147483647) {
        printf("number is out of int range\n");
        return -1;
    }

    return (int)number;
}

//modem 不休眠的问题规避方案，!!!!!!必须保证此接口的调用周期是5分钟的周期，因为modem的重启动作直接在此接口中做了处理!!!!!
static unsigned int last_sleep_times = 0;
int parse_sleep_time(const char* message) {
    if (message == NULL) {
        printf("err: input string is NULL\n");
        return -1;
    }

    const char* sleep_times_start = strstr(message, "soc_sleep=");
    if (sleep_times_start != NULL) {
        int sleep_time_value;
        int num_matched = sscanf(sleep_times_start, "soc_sleep=%d", &sleep_time_value);

        if (num_matched == 1) {
            printf("sleep time parsed: %d\n", sleep_time_value);
            if(sleep_time_value != last_sleep_times) {
                last_sleep_times = sleep_time_value;
            }
            else {
                if((!chg_state_get()) || (sk_ota_get_mcu_state()) || (sk_ota_get_state())){
                    printf("sleep time not changed and not in charge state\n");
                    paqichong_save_log("sleep time not changed and not in charge state");
                    uapi_system_reboot(SYSTEM_SOFT_REBOOT);
                }
            }
        } else {
            printf("err: parse sleep time failed\n");
        }
    } else {
        printf("err: sleep time field not found in message\n");
    }
    return -1;
}

void parse_modem_rsp(char *rsp_str)
{
    if (strstr(rsp_str, "+CCLK:")) {
        printf("modem time: %s", rsp_str);
        struct timeval tv;
        struct timezone tz;
        if (parse_time_string(rsp_str, &tv) == 0) {
            // 调用settimeofday
            int ret = settimeofday(&tv, NULL);
            if (ret == 0) {
                // sync_time=1;
                // watch_set_sync_time_state(sync_time);
                printf("Time set successfully.\n");
            } else {
                printf("settimeofday failed");
            }
        } else {
            printf("Invalid time format\n");
        }
    } else if (strstr(rsp_str, "+NWIFISCAN:")) {
        printf("modem wifi scan line: %s \n", rsp_str);
        if(-2 == parse_nwifiscan_line(rsp_str, ap_info_list, MAX_AP)) {
            //print_wifi_ap_list(ap_info_list, MAX_AP, 1);
            /* TODO: */
            if (ap_count > 0) {
                uint32_t length = 0;
                if (ap_count >= 24) {
                    length = 24 * sizeof(WifiAPInfo);
                } else {
                    length = ap_count * sizeof(WifiAPInfo);
                }
                location_send_msg(MSG_WIFI_SCAN_DONE, length, ap_info_list);
            }
        }
        printf("modem wifi scan line end \n");
    }else if (strstr(rsp_str, "+NUESTATS: SERVINGCELL") != NULL) {
        parse_serving_cell(rsp_str, &cell_info);
    } else if (strstr(rsp_str, "+NUESTATS: NEIGHBOURCELL") != NULL) {
        // 对于邻区，需要添加到邻区列表中
        if (cell_info.neighbor_cell_num < WATCH_NCELL_MAX_NUM) {
            int result = parse_neighbor_cell(rsp_str,
                &cell_info.neighbor_cell_list[cell_info.neighbor_cell_num]);
            if (result == 0) {
                cell_info.neighbor_cell_num++;
            }
        }
    }else if((strstr(rsp_str, "OK")) && ncell_process_flag) {
        ncell_process_flag = 0;
        printf("cellinfo is ok \n");
        print_watch_cell_info(&cell_info);
        location_send_msg(MSG_CELL_GET_DONE, sizeof(WATCH_CELL_INFO), &cell_info);
    }
    else if(strstr(rsp_str,"+ICCID:")){

        const char *iccid_start = strstr(rsp_str, "+ICCID:");
        iccid_start += strlen("+ICCID:");

        // 跳过可能存在的空格和冒号后的空格
        while (*iccid_start == ' ' || *iccid_start == '\t' || *iccid_start == ':') {
            iccid_start++;
        }

        // 提取ICCID数字
        int i = 0;
        while (*iccid_start != '\0' && *iccid_start != '\r' && *iccid_start != '\n' &&
               i < (int)sizeof(sk_iccid)-1) {
            if ((*iccid_start >= '0' && *iccid_start <= '9') ||
                (*iccid_start >= 'a' && *iccid_start <= 'z')) {
                sk_iccid[i++] = *iccid_start;
            } else if (*iccid_start == ' ' || *iccid_start == '\t') {
                // 遇到空格停止提取（ICCID通常是连续数字）
                break;
            }
            iccid_start++;
        }

        sk_iccid[i] = '\0';  // 确保字符串结束

        if (strlen(sk_iccid) > 0) {
            printf("ICCID: %s\n", sk_iccid);
        } else {
            printf("Non valid ICCID\n");
        }

    }
    else if(strstr(rsp_str, "+NCONFIG: AUTOCONNECT,")) {
        const char *autoconnect_start = strstr(rsp_str, "+NCONFIG: AUTOCONNECT,");
        autoconnect_start += strlen("+NCONFIG: AUTOCONNECT,");

        while (*autoconnect_start == ' ' || *autoconnect_start == '\t') {
            autoconnect_start++;
        }

        int i = 0;

        while (*autoconnect_start != '\0' &&
            *autoconnect_start != '\r' &&
            *autoconnect_start != '\n' &&
            *autoconnect_start != ',' &&  // 防止意外情况
            i < (int)sizeof(g_autoconnect_value)-1) {

            g_autoconnect_value[i++] = *autoconnect_start;
            autoconnect_start++;
        }

        g_autoconnect_value[i] = '\0';  // 确保字符串结束

        if (strlen(g_autoconnect_value) > 0) {
            printf("AUTOCONNECT value: %s\n", g_autoconnect_value);
        } else {
            printf("No valid AUTOCONNECT value found\n");
        }
    }else if((strstr(rsp_str, "OK") && net_auto_reg_flag)) {
        net_auto_reg_flag = 0;
        net_auto_reg_done = true;
        printf("net_auto_reg is ok \n");
    }else if((strstr(rsp_str, "OK") && net_cfun0_set_flag)) {
        net_cfun0_set_flag = 0;
        mt_cfun0_flag = true;
        printf("net_cfun0 is ok \n");
    }else if(strstr(rsp_str, "+CFUN:")) {
        cfun_state_current = parse_cfun_string(rsp_str);
        printf("cfun_state_current: %d \n", cfun_state_current);
    }else if(strstr(rsp_str, "sleep_times")) {
        parse_sleep_time(rsp_str);
    }
}

void get_cell_info(void)
{
    memset(&cell_info, 0, sizeof(WATCH_CELL_INFO));
    SendATCommand(CMD_SERVINGCELL,NULL);
    osDelay(100);
    //SendATCommand(CMD_NEIGHBOURCELL,NULL);
    ncell_process_flag = 1;
}

void sync_time_from_modem(void)
{
    SendATCommand(CMD_LOCALTIME,NULL);
}

void wifi_scan(void)
{
    if (get_download_file_state() == 0)
    {
        ap_count = 0;
        current_count = 0;
        memset(ap_info_list, 0, sizeof(ap_info_list));
        SendATCommand(CMD_WIFISCAN, NULL);
    }
}

void RRC_release(void)
{
    SendATCommand(CMD_NNMPSD,NULL);
}

void cacon_state(void)
{
    SendATCommand(CMD_CSCON,NULL);
}

void cfun0_state(void)
{
    SendATCommand(CMD_CFUN0,NULL);
    net_cfun0_set_flag = 1;
}

void cfun1_state(void)
{
    SendATCommand(CMD_CFUN1,NULL);
    net_cfun1_set_flag = 1;
}

int cfun_get_state(void)
{
    return cfun_state_current;
}

void cfun_check_cmd(void)
{
    SendATCommand(CMD_CFUNGET,NULL);
}

void modem_get_iccid(void)
{
    SendATCommand(CMD_GET_ICCID,NULL);
}

void modem_auto_connect(void)
{
    SendATCommand(CMD_NET_AUTO_REG,NULL);
    net_auto_reg_flag = 1;
}
void modem_get_nconfig(void)
{
    SendATCommand(CMD_GET_NCONFIG,NULL);
}
void ims_close(void)
{
    SendATCommand(CMD_IMSSWITCH,NULL);
}
void close_head_compress(void)
{
    SendATCommand(CMD_CLOSE_HEAD_COMPRESS,NULL);
}

void modem_sleep_check(void)
{
    SendATCommand(CMD_MODEM_SLEEP_CHECK,NULL);
}

void modem_reset(void)
{
    SendATCommand(CMD_MODEM_RESET,NULL);
}

void cell_reselection_enable(char enable)
{
    if (enable) {
        SendATCommand(CMD_OPEN_CELL_RESELECTION,NULL);
    } else {
        SendATCommand(CMD_CLOSE_CELL_RESELECTION,NULL);
    }
}

int32_t modem_at_rsp_fun(uint8_t *data, size_t size)
{
    sk_modem_at_process_cmd_msg_t modem_data_msg = {0};
    modem_data_msg.data = malloc(size);
    modem_data_msg.size = size;
    if (modem_data_msg.data == NULL) {
        printf("{%s():%d} malloc fail!\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }
    if (memcpy_s(modem_data_msg.data, size, data, size) != EOK) {
        printf("{%s():%d} memcpy_s fail!\r\n", __FUNCTION__, __LINE__);
        free(modem_data_msg.data);
        return ERRCODE_FAIL;
    }
    if (osal_msg_queue_write_copy(modem_at_process_queue_id, (void *)&modem_data_msg,
                                            sizeof(sk_modem_at_process_cmd_msg_t), OSAL_MSGQ_WAIT_FOREVER) == OSAL_SUCCESS) {
        printf("{%s():%d} modem_at_rsp_fun send sucess! size:%d.\r\n", __FUNCTION__, __LINE__,modem_data_msg.size);
        return ERRCODE_SUCC;
    }
    printf("{%s():%d} osal_msg_queue_write_copy fail!\r\n", __FUNCTION__, __LINE__);
    free(modem_data_msg.data);
    return ERRCODE_FAIL;
}

static int  modem_at_process_task_entry(void *data)
{
	unused(data);
    sk_modem_at_process_cmd_msg_t modem_data_msg = {0};
    unsigned int cmd_len = sizeof(sk_modem_at_process_cmd_msg_t);
    while(1)
    {
        if (osal_msg_queue_read_copy(modem_at_process_queue_id, (void *)&modem_data_msg,
                                            &cmd_len, OSAL_MSGQ_WAIT_FOREVER) == OSAL_SUCCESS) {
            if (net_debug_flag && strstr((char *)modem_data_msg.data, (char *)g_debug_cmd_buf))
            {
                size_t length = 0;
                char cmd[128] = {0};
                if (0 == extract_between_delimiters((char *)modem_data_msg.data, "\r\n", "\r\n", cmd, 128))
                {
                    /* 将cmd结果存到buf,再通过定时器一起上报，4096大小已经够大，不做超过4096大小的判断 */
                    length = snprintf(cmd_result + g_offset, 4096, "%s;", cmd);
                    g_offset += length;
                    osal_timer_mod(&timer_cmd_result_report, 6*1000);
                }
            }
            parse_modem_rsp((char *)modem_data_msg.data);
            free(modem_data_msg.data);
        }else {
            printf("{%s():%d} osal_msg_queue_read_copy fail!\r\n", __FUNCTION__, __LINE__);
        }
    }
}

void modem_callback_init(void)
{
    int32_t ret = ERRCODE_SUCC;
    const char modem_at_process_queue_name[] = "msgqueue_modem_at_process";
    osThreadAttr_t threadAttr={0};
	memset(&threadAttr, 0, sizeof(threadAttr));
    threadAttr.name = "modem_at_process_task";
    threadAttr.stack_size = 0x2000;
    threadAttr.priority = 17;
    threadAttr.stack_mem = memalign(16, threadAttr.stack_size);

    modem_at_process_task_id = osThreadNew(modem_at_process_task_entry, NULL, &threadAttr);
    if ( modem_at_process_task_id ==  NULL) {
        printf("osal_kthread_create modem_at_process_task_id failed\r\n");
        ret = ERRCODE_FAIL;
    }

    ret = osal_msg_queue_create(modem_at_process_queue_name, MAX_MODEM_AT_PROCESS_QUEUE_NUM,
                                &modem_at_process_queue_id,
                                0, sizeof(sk_modem_at_process_cmd_msg_t));
    if (ret != ERRCODE_SUCC) {
        printf("Create msgqueue %s failed: %d\n", modem_at_process_queue_name, ret);
        ret = ERRCODE_FAIL;
    }
    else
    {
        printf("Create msgqueue %s, queue_id: %d, sucess: %d\n", modem_at_process_queue_name, modem_at_process_queue_id, ret);
    }
    ril_register_at_reply_callback(modem_at_rsp_fun);

    timer_cmd_result_report.handler = cmd_result_timer_handler;
    timer_cmd_result_report.interval = 6*1000;
    timer_cmd_result_report.data = 0;
    osal_timer_init(&timer_cmd_result_report);
}


void modem_info_init(void)
{
    watch_storage_get(STORAGE_FAC_FLAG, &g_facflag, sizeof(uint8_t));
    printf("modem_info_init,g_facflag:%d\n",g_facflag);
    if(g_facflag == 1)
    {
        cfun0_state();
        osDelay(500);
        close_head_compress();
        osDelay(500);
        cfun1_state();
        osDelay(3000);
        ims_close();
        osDelay(100);
    }
}

void net_debug_flag_set(uint8_t flag)
{
    net_debug_flag = flag;
}
