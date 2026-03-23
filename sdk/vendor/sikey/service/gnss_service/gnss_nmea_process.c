
#include "gnss_service.h"
#include "gnss_nmea_process.h"
#include "gnss_log.h"
#include "location_service.h"

static gnss_parse_info_t g_gnss_parse_info = {0};

//static COMMON_SNR_INFO_T g_BDDataInfoBuf = {0};
//static COMMON_SNR_INFO_T g_GADataInfoBuf = {0};
//static COMMON_SNR_INFO_T g_GQDataInfoBuf = {0};
//static COMMON_SNR_INFO_T g_GLDataInfoBuf = {0};

//static gnss_location_info_t g_CurGPSLocInfo = {0};
static uint32_t seg_count = 0;
static uint32_t byte_count;
static char cmd[5] = {0};
static uint32_t seg_id = 0;
static GPS_CMD_ID_E Gps_Cmd_ID = GPS_CMD_UNKNOW;
static RECEIVE_MODE_E ReceiveMode = RECEIVE_CMD_MODE;
//static uint8_t watch_gnss_cn_buf[WATCH_GPS_CN_NUM] = {0};
//static uint32_t maxcnr_seg = 0;
//static uint32_t maxcnr_rec = 0;
extern uint64_t get_utc_time(void);
bool gnss_is_fixed = 0;

void gnss_info_init(void)
{
    gnss_is_fixed = 0;
    memset(&g_gnss_parse_info, 0, sizeof(gnss_parse_info_t));
}

#if 0
void watch_set_gnss_cnr(int sat_no, uint8_t snr, int clean_flag, uint8_t type)
{
    uint32_t count = sizeof(watch_gnss_cn_buf);
    int no = sat_no % count;
    if (clean_flag)
    {
        gnss_is_fixed = 0;
        memset(watch_gnss_cn_buf, 0, count);
        memset(&g_gnss_parse_info, 0, sizeof(gnss_parse_info_t));
        memset(&g_BDDataInfoBuf, 0, sizeof(COMMON_SNR_INFO_T));
        memset(&g_GADataInfoBuf, 0, sizeof(COMMON_SNR_INFO_T));
        memset(&g_GLDataInfoBuf, 0, sizeof(COMMON_SNR_INFO_T));
        memset(&g_GQDataInfoBuf, 0, sizeof(COMMON_SNR_INFO_T));
        return;
    }
    switch (type)
    {
    case GNSS_TYPE_GPS:
        no = no + GNSS_PRN_OFFSET_GPS;
        break;
    case GNSS_TYPE_BDS:
        no = no + GNSS_PRN_OFFSET_BDS;
        break;
    case GNSS_TYPE_GLO:
        no = no + GNSS_PRN_OFFSET_GLO;
        break;
    case GNSS_TYPE_GAL:
        no = no + GNSS_PRN_OFFSET_GAL;
        break;
    case GNSS_TYPE_QZS:
        no = no + GNSS_PRN_OFFSET_QZS;
        break;
    default:
        break;
    }
    watch_gnss_cn_buf[no] = snr;
    GNSS_LOG_INFO("[LOC]snr[%d:%d]:%d\n", sat_no % count, no, snr);
}

uint32_t watch_gps_get_maxcnr(void)
{
    GNSS_LOG_INFO("maxcnr_rec:%d\n", maxcnr_rec);
    return maxcnr_rec;
}
#endif

static uint8_t GGT_GPS_data_decode(char data)
{
    switch (data)
    {
    case '$':
    {
        Gps_Cmd_ID = GPS_CMD_UNKNOW;
        ReceiveMode = RECEIVE_CMD_MODE;
        byte_count = 0;
    }
    break;
    case ',':
    {
        seg_count++;
        byte_count = 0;
    }
    break;
    case '*':
    {
        switch (Gps_Cmd_ID)
        {
        #if 0
        case GPS_CMD_GPGSV:
        {
            int snr = 0;
            int sat_no = 0;
            int maxcnr = 0;
            for (uint8_t i = 0; i < 4; i++)
            {
                if (strlen(g_gnss_parse_info.Satellites_snr_table[i]) > 0)
                {
                    snr = atoi(&g_gnss_parse_info.Satellites_snr_table[i][0]);
                    sat_no = atoi(&g_gnss_parse_info.Satellites_list[i][0]);
                    if (sat_no > 0 && snr > 0)
                    {
                        watch_set_gnss_cnr(sat_no, snr, 0, GNSS_TYPE_GPS);
                        if (maxcnr <= snr)
                        {
                            maxcnr = snr;
                        }
                    }
                }
            }
            if (maxcnr > maxcnr_seg)
            {
                maxcnr_seg = maxcnr;
            }
            if (seg_id == 1)
            {
                if (maxcnr_seg > 0)
                {
                    maxcnr_rec = maxcnr_seg;
                }
                maxcnr_seg = 0;
            }
            GNSS_LOG_INFO("gps seg_id:%d,maxcnr:%d,maxcnr_seg:%d,maxcnr_rec:%d\n", seg_id, maxcnr, maxcnr_seg, maxcnr_rec);
        }
        break;
        case GPS_CMD_GBGSV:
        {
            int snr = 0;
            int sat_no = 0;
            int maxcnr = 0;
            for (uint8_t i = 0; i < 4; i++)
            {
                if (strlen(g_BDDataInfoBuf.Satellites_snr_table[i]) > 0)
                {
                    snr = atoi(&g_BDDataInfoBuf.Satellites_snr_table[i][0]);
                    sat_no = atoi(&g_BDDataInfoBuf.Satellites_list[i][0]);
                    if (sat_no > 0 && snr > 0)
                    {
                        watch_set_gnss_cnr(sat_no, snr, 0, GNSS_TYPE_BDS);
                        if (maxcnr <= snr)
                        {
                            maxcnr = snr;
                        }
                    }
                }
            }
            if (maxcnr > maxcnr_seg)
            {
                maxcnr_seg = maxcnr;
            }
            if (seg_id == 1)
            {
                if (maxcnr_seg > 0)
                {
                    maxcnr_rec = maxcnr_seg;
                }
                maxcnr_seg = 0;
            }
            GNSS_LOG_INFO("bds seg_id:%d,maxcnr:%d,maxcnr_seg:%d,maxcnr_rec:%d\n", seg_id, maxcnr, maxcnr_seg, maxcnr_rec);
        }
        break;
        case GPS_CMD_GAGSV:
        {
            int snr = 0;
            int sat_no = 0;
            int maxcnr = 0;
            for (uint8_t i = 0; i < 4; i++)
            {
                if (strlen(g_GADataInfoBuf.Satellites_snr_table[i]) > 0)
                {
                    snr = atoi(&g_GADataInfoBuf.Satellites_snr_table[i][0]);
                    sat_no = atoi(&g_GADataInfoBuf.Satellites_list[i][0]);
                    if (sat_no > 0 && snr > 0)
                    {
                        watch_set_gnss_cnr(sat_no, snr, 0, GNSS_TYPE_GAL);
                        if (maxcnr <= snr)
                        {
                            maxcnr = snr;
                        }
                    }
                }
            }
            if (maxcnr > maxcnr_seg)
            {
                maxcnr_seg = maxcnr;
            }
            if (seg_id == 1)
            {
                if (maxcnr_seg > 0)
                {
                    maxcnr_rec = maxcnr_seg;
                }
                maxcnr_seg = 0;
            }
            GNSS_LOG_INFO("gal seg_id:%d,maxcnr:%d,maxcnr_seg:%d,maxcnr_rec:%d\n", seg_id, maxcnr, maxcnr_seg, maxcnr_rec);
        }
        break;
        case GPS_CMD_GLGSV:
        {
            int snr = 0;
            int sat_no = 0;
            int maxcnr = 0;
            for (uint8_t i = 0; i < 4; i++)
            {
                if (strlen(g_GLDataInfoBuf.Satellites_snr_table[i]) > 0)
                {
                    snr = atoi(&g_GLDataInfoBuf.Satellites_snr_table[i][0]);
                    sat_no = atoi(&g_GLDataInfoBuf.Satellites_list[i][0]);
                    if (sat_no > 0 && snr > 0)
                    {
                        watch_set_gnss_cnr(sat_no, snr, 0, GNSS_TYPE_GLO);
                        if (maxcnr <= snr)
                        {
                            maxcnr = snr;
                        }
                    }
                }
            }
            if (maxcnr > maxcnr_seg)
            {
                maxcnr_seg = maxcnr;
            }
            if (seg_id == 1)
            {
                if (maxcnr_seg > 0)
                {
                    maxcnr_rec = maxcnr_seg;
                }
                maxcnr_seg = 0;
            }
            GNSS_LOG_INFO("glo seg_id:%d,maxcnr:%d,maxcnr_seg:%d,maxcnr_rec:%d\n", seg_id, maxcnr, maxcnr_seg, maxcnr_rec);
        }
        break;
        case GPS_CMD_GQGSV:
        {
            int snr = 0;
            int sat_no = 0;
            int maxcnr = 0;
            for (uint8_t i = 0; i < 4; i++)
            {
                if (strlen(g_GQDataInfoBuf.Satellites_snr_table[i]) > 0)
                {
                    snr = atoi(&g_GQDataInfoBuf.Satellites_snr_table[i][0]);
                    sat_no = atoi(&g_GQDataInfoBuf.Satellites_list[i][0]);
                    if (sat_no > 0 && snr > 0)
                    {
                        watch_set_gnss_cnr(sat_no, snr, 0, GNSS_TYPE_QZS);
                        if (maxcnr <= snr)
                        {
                            maxcnr = snr;
                        }
                    }
                }
            }
            if (maxcnr > maxcnr_seg)
            {
                maxcnr_seg = maxcnr;
            }
            if (seg_id == 1)
            {
                if (maxcnr_seg > 0)
                {
                    maxcnr_rec = maxcnr_seg;
                }
                maxcnr_seg = 0;
            }
            GNSS_LOG_INFO("qzs seg_id:%d,maxcnr:%d,maxcnr_seg:%d,maxcnr_rec:%d\n", seg_id, maxcnr, maxcnr_seg, maxcnr_rec);
        }
        break;
        #endif
        case GPS_CMD_GPGSA:
        {
            // watch_set_gnss_cnr(0, 0, 1,GNSS_TYPE_MAX);
        }
        break;
        case GPS_CMD_GPRMC:
        case GPS_CMD_GPGGA:
        case GPS_CMD_GPTXT:
        {
            // if(!gnss_is_test){
            //     watch_set_gnss_cnr(0, 0, 1, GNSS_TYPE_MAX);
            // }
        }
        break;
        default:
            break;
        }
        ReceiveMode = RECEIVE_END_MODE;
    }
    break;
    default:
        if (ReceiveMode == RECEIVE_CMD_MODE)
        {
            if (byte_count <= 4)
            {
                cmd[byte_count] = data;
            }
            if (byte_count >= 4)
            {
                if (((cmd[0] == 'G') && ((cmd[1] == 'P') || (cmd[1] == 'N') || (cmd[1] == 'B') || (cmd[1] == 'A') || (cmd[1] == 'L') || (cmd[1] == 'Q'))) || ((cmd[0] == 'B') && (cmd[1] == 'D')))
                {
                    if (cmd[2] == 'G')
                    {
                        if (cmd[3] == 'G')
                        {
                            if (cmd[4] == 'A')
                            {
                                Gps_Cmd_ID = GPS_CMD_GPGGA;
                                ReceiveMode = RECEIVE_DATA_MODE;
                                seg_count = 0;
                                byte_count = 0;
                            }
                        }
                        else if (cmd[3] == 'S')
                        {
                            if (cmd[4] == 'V')
                            {
                                if ((cmd[0] == 'G' && cmd[1] == 'P') || (cmd[0] == 'G' && cmd[1] == 'N') || (cmd[0] == 'G' && cmd[1] == 'B'))
                                {
                                    Gps_Cmd_ID = GPS_CMD_GPGSV;
                                }
                                else if (cmd[0] == 'G' && cmd[1] == 'B')
                                {
                                    Gps_Cmd_ID = GPS_CMD_GBGSV;
                                }
                                else if (cmd[0] == 'G' && cmd[1] == 'A')
                                {
                                    Gps_Cmd_ID = GPS_CMD_GAGSV;
                                }
                                else if (cmd[0] == 'G' && cmd[1] == 'Q')
                                {
                                    Gps_Cmd_ID = GPS_CMD_GQGSV;
                                }
                                else if (cmd[0] == 'G' && cmd[1] == 'L')
                                {
                                    Gps_Cmd_ID = GPS_CMD_GLGSV;
                                }
                                else if (cmd[0] == 'B' && cmd[1] == 'D')
                                {
                                    Gps_Cmd_ID = GPS_CMD_BDGSV;
                                }
                                ReceiveMode = RECEIVE_DATA_MODE;
                                seg_count = 0;
                                byte_count = 0;
                            }
                            else if (cmd[4] == 'A')
                            {
                                Gps_Cmd_ID = GPS_CMD_GPGSA;
                                ReceiveMode = RECEIVE_DATA_MODE;
                                seg_count = 0;
                                byte_count = 0;
                            }
                        }
                    }
                    else if (cmd[2] == 'R')
                    {
                        if (cmd[3] == 'M')
                        {
                            if (cmd[4] == 'C')
                            {
                                Gps_Cmd_ID = GPS_CMD_GPRMC;
                                ReceiveMode = RECEIVE_DATA_MODE;
                                seg_count = 0;
                                byte_count = 0;
                                g_gnss_parse_info.Speed_num = '0';
                                g_gnss_parse_info.Angle_num = '0';
                            }
                        }
                    }
                    else if (cmd[2] == 'T')
                    {
                        if (cmd[3] == 'X')
                        {
                            if (cmd[4] == 'T')
                            {
                                Gps_Cmd_ID = GPS_CMD_GPTXT;
                                ReceiveMode = RECEIVE_DATA_MODE;
                                seg_count = 0;
                                byte_count = 0;
                            }
                        }
                    }
                }
                else
                {
                    Gps_Cmd_ID = GPS_CMD_UNKNOW;
                    ReceiveMode = RECEIVE_DATA_MODE;
                    seg_count = 0;
                    byte_count = 0;
                }
            }
            else if ((cmd[0] == 'O') && (cmd[1] == 'K') && byte_count == 1)
            {
                Gps_Cmd_ID = GPS_CMD_ACK;
                ReceiveMode = RECEIVE_DATA_MODE;
                seg_count = 0;
                byte_count = 0;
            }
        }
        else if (ReceiveMode == RECEIVE_DATA_MODE)
        {
            switch (Gps_Cmd_ID)
            {
            #if 0
            case GPS_CMD_GPGGA:
                switch (seg_count)
                {
                case 7:
                    if (byte_count < 2)
                    {
                        g_gnss_parse_info.Satellites_Used[byte_count] = data;
                    }
                    break;
                case 8:
                    if (byte_count < 4)
                    {
                        g_gnss_parse_info.HDOP[byte_count] = data;
                    }
                    break;
                default:
                    break;
                }

                break;
            case GPS_CMD_GPGSV:
            {
                switch (seg_count)
                {
                case 2:
                    if (byte_count < 2)
                    {
                        seg_id = data - '0';
                    }
                case 3:
                    if (byte_count < 2)
                    {
                        g_gnss_parse_info.Satellites_in_View[byte_count] = data;
                    }
                    break;
                case 4:
                    if (byte_count < 3)
                    {
                        g_gnss_parse_info.Satellites_list[0][byte_count] = data;
                    }
                    break;
                case 7:
                    if (byte_count < 2)
                    {
                        g_gnss_parse_info.Satellites_snr_table[0][byte_count] = data;
                    }
                    break;
                case 8:
                    if (byte_count < 3)
                    {
                        g_gnss_parse_info.Satellites_list[1][byte_count] = data;
                    }
                    break;
                case 11:
                    if (byte_count < 2)
                    {
                        g_gnss_parse_info.Satellites_snr_table[1][byte_count] = data;
                    }
                    break;
                case 12:
                    if (byte_count < 3)
                    {
                        g_gnss_parse_info.Satellites_list[2][byte_count] = data;
                    }
                    break;
                case 15:
                    if (byte_count < 2)
                    {
                        g_gnss_parse_info.Satellites_snr_table[2][byte_count] = data;
                    }
                    break;
                case 16:
                    if (byte_count < 3)
                    {
                        g_gnss_parse_info.Satellites_list[3][byte_count] = data;
                    }
                    break;
                case 19:
                    if (byte_count < 2)
                    {
                        g_gnss_parse_info.Satellites_snr_table[3][byte_count] = data;
                    }
                    break;
                default:
                    break;
                }
            }
            break;
            case GPS_CMD_GBGSV:
            case GPS_CMD_BDGSV:
                switch (seg_count)
                {
                case 3:
                    if (byte_count < 2)
                    {
                        g_BDDataInfoBuf.Satellites_in_View[byte_count] = data;
                    }
                    break;
                case 4:
                    if (byte_count < 3)
                    {
                        g_BDDataInfoBuf.Satellites_list[0][byte_count] = data;
                    }
                    break;
                case 7:
                    if (byte_count < 2)
                    {
                        g_BDDataInfoBuf.Satellites_snr_table[0][byte_count] = data;
                    }
                    break;
                case 8:
                    if (byte_count < 3)
                    {
                        g_BDDataInfoBuf.Satellites_list[1][byte_count] = data;
                    }
                    break;
                case 11:
                    if (byte_count < 2)
                    {
                        g_BDDataInfoBuf.Satellites_snr_table[1][byte_count] = data;
                    }
                    break;
                case 12:
                    if (byte_count < 3)
                    {
                        g_BDDataInfoBuf.Satellites_list[2][byte_count] = data;
                    }
                    break;
                case 15:
                    if (byte_count < 2)
                    {
                        g_BDDataInfoBuf.Satellites_snr_table[2][byte_count] = data;
                    }
                    break;
                case 16:
                    if (byte_count < 3)
                    {
                        g_BDDataInfoBuf.Satellites_list[3][byte_count] = data;
                    }
                    break;
                case 19:
                    if (byte_count < 2)
                    {
                        g_BDDataInfoBuf.Satellites_snr_table[3][byte_count] = data;
                    }
                    break;
                default:
                    break;
                }
                break;
            case GPS_CMD_GQGSV:
                switch (seg_count)
                {
                case 3:
                    if (byte_count < 2)
                    {
                        g_GQDataInfoBuf.Satellites_in_View[byte_count] = data;
                    }
                    break;
                case 4:
                    if (byte_count < 3)
                    {
                        g_GQDataInfoBuf.Satellites_list[0][byte_count] = data;
                    }
                    break;
                case 7:
                    if (byte_count < 2)
                    {
                        g_GQDataInfoBuf.Satellites_snr_table[0][byte_count] = data;
                    }
                    break;
                case 8:
                    if (byte_count < 3)
                    {
                        g_GQDataInfoBuf.Satellites_list[1][byte_count] = data;
                    }
                    break;
                case 11:
                    if (byte_count < 2)
                    {
                        g_GQDataInfoBuf.Satellites_snr_table[1][byte_count] = data;
                    }
                    break;
                case 12:
                    if (byte_count < 3)
                    {
                        g_GQDataInfoBuf.Satellites_list[2][byte_count] = data;
                    }
                    break;
                case 15:
                    if (byte_count < 2)
                    {
                        g_GQDataInfoBuf.Satellites_snr_table[2][byte_count] = data;
                    }
                    break;
                case 16:
                    if (byte_count < 3)
                    {
                        g_GQDataInfoBuf.Satellites_list[3][byte_count] = data;
                    }
                    break;
                case 19:
                    if (byte_count < 2)
                    {
                        g_GQDataInfoBuf.Satellites_snr_table[3][byte_count] = data;
                    }
                    break;
                default:
                    break;
                }
                break;
            case GPS_CMD_GLGSV:
                switch (seg_count)
                {
                case 3:
                    if (byte_count < 2)
                    {
                        g_GLDataInfoBuf.Satellites_in_View[byte_count] = data;
                    }
                    break;
                case 4:
                    if (byte_count < 3)
                    {
                        g_GLDataInfoBuf.Satellites_list[0][byte_count] = data;
                    }
                    break;
                case 7:
                    if (byte_count < 2)
                    {
                        g_GLDataInfoBuf.Satellites_snr_table[0][byte_count] = data;
                    }
                    break;
                case 8:
                    if (byte_count < 3)
                    {
                        g_GLDataInfoBuf.Satellites_list[1][byte_count] = data;
                    }
                    break;
                case 11:
                    if (byte_count < 2)
                    {
                        g_GLDataInfoBuf.Satellites_snr_table[1][byte_count] = data;
                    }
                    break;
                case 12:
                    if (byte_count < 3)
                    {
                        g_GLDataInfoBuf.Satellites_list[2][byte_count] = data;
                    }
                    break;
                case 15:
                    if (byte_count < 2)
                    {
                        g_GLDataInfoBuf.Satellites_snr_table[2][byte_count] = data;
                    }
                    break;
                case 16:
                    if (byte_count < 3)
                    {
                        g_GLDataInfoBuf.Satellites_list[3][byte_count] = data;
                    }
                    break;
                case 19:
                    if (byte_count < 2)
                    {
                        g_GLDataInfoBuf.Satellites_snr_table[3][byte_count] = data;
                    }
                    break;
                default:
                    break;
                }
                break;
            case GPS_CMD_GAGSV:
                switch (seg_count)
                {
                case 3:
                    if (byte_count < 2)
                    {
                        g_GADataInfoBuf.Satellites_in_View[byte_count] = data;
                    }
                    break;
                case 4:
                    if (byte_count < 3)
                    {
                        g_GADataInfoBuf.Satellites_list[0][byte_count] = data;
                    }
                    break;
                case 7:
                    if (byte_count < 2)
                    {
                        g_GADataInfoBuf.Satellites_snr_table[0][byte_count] = data;
                    }
                    break;
                case 8:
                    if (byte_count < 3)
                    {
                        g_GADataInfoBuf.Satellites_list[1][byte_count] = data;
                    }
                    break;
                case 11:
                    if (byte_count < 2)
                    {
                        g_GADataInfoBuf.Satellites_snr_table[1][byte_count] = data;
                    }
                    break;
                case 12:
                    if (byte_count < 3)
                    {
                        g_GADataInfoBuf.Satellites_list[2][byte_count] = data;
                    }
                    break;
                case 15:
                    if (byte_count < 2)
                    {
                        g_BDDataInfoBuf.Satellites_snr_table[2][byte_count] = data;
                    }
                    break;
                case 16:
                    if (byte_count < 3)
                    {
                        g_GADataInfoBuf.Satellites_list[3][byte_count] = data;
                    }
                    break;
                case 19:
                    if (byte_count < 2)
                    {
                        g_GADataInfoBuf.Satellites_snr_table[3][byte_count] = data;
                    }
                    break;
                default:
                    break;
                }
                break;
            #endif
            case GPS_CMD_GPRMC:
                switch (seg_count)
                {
                case 1:
                    if (byte_count < 6)
                    {
                        g_gnss_parse_info.utc_time[byte_count] = data;
                    }
                    break;
                case 2:
                    if (byte_count < 1)
                    {
                        g_gnss_parse_info.fixed_status = data;
                    }
                    break;
                case 3:
                    if (byte_count < 9)
                    {
                        g_gnss_parse_info.Latitude[byte_count] = data;
                    }
                    break;
                case 4:
                    if (byte_count < 1)
                    {
                        g_gnss_parse_info.Latitude_Dir = data;
                    }
                    break;
                case 5:
                    if (byte_count < 10)
                    {
                        g_gnss_parse_info.Longitude[byte_count] = data;
                    }
                    break;
                case 6:
                    if (byte_count < 1)
                    {
                        g_gnss_parse_info.Longitude_Dir = data;
                    }
                    break;
                case 7:
                    if (byte_count < 5)
                    {
                        g_gnss_parse_info.Speed[byte_count] = data;
                        g_gnss_parse_info.Speed_num++;
                    }
                    break;
                case 8:
                    if (byte_count < 5)
                    {
                        g_gnss_parse_info.Angle[byte_count] = data;
                        g_gnss_parse_info.Angle_num++;
                    }
                    break;
                case 9:
                    if (byte_count < 6)
                    {
                        g_gnss_parse_info.utc_date[byte_count] = data;
                    }
                    break;
                default:
                    break;
                }
                break;
            case GPS_CMD_GPGSA:
                switch (seg_count)
                {
                case 15:
                    if (byte_count < 4)
                    {
                        g_gnss_parse_info.PDOP[byte_count] = data;
                    }
                    break;
                case 16:
                    if (byte_count < 4)
                    {
                        g_gnss_parse_info.HDOP[byte_count] = data;
                    }
                    break;
                case 17:
                    if (byte_count < 4)
                    {
                        g_gnss_parse_info.VDOP[byte_count] = data;
                    }
                    break;
                default:
                    break;
                }
                break;
            case GPS_CMD_GPTXT:
            case GPS_CMD_ACK:
            default:
                break;
            }
        }
        byte_count++;
        break;
    }
    if (ReceiveMode == RECEIVE_END_MODE)
    {
        return 1;
    }
    return 0;
}

static bool gnss_is_nmea_sentence_legal(char *nmea, uint32_t len)
{
    uint32_t i = 0, j = 0;
    uint8_t checksum = 0;
    char cs_string[3] = {0};
    bool result = false;
    int sentence_begin = -1;
    int sentence_end = -1;
    if (NULL == nmea || len < 0)
    {
        return result;
    }
    for (i = 0; i < len; i++)
    {
        if (nmea[i] == '$')
        {
            sentence_begin = i;
        }
        if (nmea[i] == '*' && sentence_begin != -1 && i + 2 < len)
        {
            sentence_end = i;
        }
        if (sentence_end != -1 && len - i >= 2)
        {
            for (j = sentence_begin + 1; j < sentence_end; j++)
            {
                checksum ^= nmea[j];
            }
            sprintf(cs_string, "%02X", checksum);
            if (cs_string[0] == nmea[j + 1] && cs_string[1] == nmea[j + 2])
            {
                result = true;
            }
            sentence_begin = -1;
            sentence_end = -1;
            checksum = 0;
        }
    }
    return result;
}

void gnss_process_nmea(uint8_t *nmea)
{
    uint8_t result = 0;
    uint32_t i = 0;
    uint32_t nLenToParse = strlen((char *)nmea);
    gnss_parse_info_t *pGPSInfo = &g_gnss_parse_info;
    gnss_location_info_t *g_CurGPSLocInfo = NULL;

    if (nLenToParse <= 0)
    {
        GNSS_LOG_INFO("gnss nmea length is invalid\n");
        return;
    }

    if (!gnss_is_nmea_sentence_legal((char *)nmea, nLenToParse))
    {
        GNSS_LOG_INFO("gnss nmea sentence is invalid\n");
        return;
    }

    for (i = 0; i < nLenToParse; i++)
    {
        GGT_GPS_data_decode(nmea[i]);
    }

    g_CurGPSLocInfo = (gnss_location_info_t *)malloc(sizeof(gnss_location_info_t));
    if (!g_CurGPSLocInfo)
    {
        return;
    }
    memset(g_CurGPSLocInfo, 0, sizeof(gnss_location_info_t));

    memcpy(g_CurGPSLocInfo->utc_date, pGPSInfo->utc_date + 4, 2);
    memcpy(g_CurGPSLocInfo->utc_date + 2, pGPSInfo->utc_date + 2, 2);
    memcpy(g_CurGPSLocInfo->utc_date + 4, pGPSInfo->utc_date, 2);

    if (g_CurGPSLocInfo->status != 'A')
    {
        g_CurGPSLocInfo->status = pGPSInfo->fixed_status;
    }
    memcpy(g_CurGPSLocInfo->latitude, pGPSInfo->Latitude, 9);
    memcpy(g_CurGPSLocInfo->longitude, pGPSInfo->Longitude, 10);
    g_CurGPSLocInfo->latitude_dir = pGPSInfo->Latitude_Dir;
    g_CurGPSLocInfo->longitude_dir = pGPSInfo->Longitude_Dir;
    sprintf(g_CurGPSLocInfo->speed, "%05.1f", atof(pGPSInfo->Speed));

    memcpy(g_CurGPSLocInfo->utc_time, pGPSInfo->utc_time, 6);
    memcpy(g_CurGPSLocInfo->angle, pGPSInfo->Angle, 5);
    //memcpy(g_CurGPSLocInfo->satellites_view, pGPSInfo->Satellites_in_View, 2);
    //memcpy(g_CurGPSLocInfo->satellites_use, pGPSInfo->Satellites_Used, 2);
    memcpy(g_CurGPSLocInfo->hdop, pGPSInfo->HDOP, 4);
    memcpy(g_CurGPSLocInfo->pdop, pGPSInfo->PDOP, 4);
    memcpy(g_CurGPSLocInfo->vdop, pGPSInfo->VDOP, 4);
    //memcpy(pGPSInfo->gps_cn_buf, watch_gnss_cn_buf, sizeof(watch_gnss_cn_buf));

    //pGPSInfo->all_satellites_view = atoi(pGPSInfo->Satellites_in_View) + atoi(g_BDDataInfoBuf.Satellites_in_View) + atoi(g_GADataInfoBuf.Satellites_in_View) + atoi(g_GQDataInfoBuf.Satellites_in_View) + atoi(g_GLDataInfoBuf.Satellites_in_View);
    //pGPSInfo->all_satellites_use = atoi(pGPSInfo->Satellites_Used);

    if ((g_CurGPSLocInfo->status == 'A') && (gnss_is_fixed == 0))
    {
        GNSS_LOG_INFO("gnss locate successful %lu\n", get_utc_time());
        gnss_is_fixed = 1;
        if (gnss_test_mode != 0)
        {
            gnss_test_send_msg(MSG_GNSS_STOP_TEST, 0, NULL);
        }
        else
        {
            /* 发送gnss info到定位 */
            location_send_msg(MSG_GNSS_SCAN_DONE, sizeof(gnss_location_info_t), g_CurGPSLocInfo);
        }
        memset(&g_gnss_parse_info, 0, sizeof(gnss_parse_info_t));
    }
    free(g_CurGPSLocInfo);
    g_CurGPSLocInfo = NULL;
    return;
}