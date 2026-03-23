
#ifndef __SH366102_H
#define __SH366102_H

//#include "sys.h"
#include <string.h>
#include "stdbool.h"
#include <bits/alltypes.h>
#include "i2c.h"
#include "debug_print.h"
#include "errcode.h"
#include "tcxo.h"
#include "soc_osal.h"

#define SH366102_I2C_BUS I2C_BUS_1
#define FG_SUCCESS 1
#define FG_FAIL 0
#define FG_ENABLE_DEBUG 1
#if  FG_ENABLE_DEBUG
#define FG_PRINT(fmt, ...)      printf("[FG] " fmt, ##__VA_ARGS__)
#define FG_INFO(fmt, ...)		FG_PRINT("[FG] [INFO] "fmt, ##__VA_ARGS__)
#define FG_ERR(fmt, ...)		FG_PRINT("[FG] [ERR] "fmt, ##__VA_ARGS__)
#else
#define FG_PRINT(fmt, ...)
#define FG_INFO(fmt, ...)
#define FG_ERR(fmt, ...)
#endif
#define  CNTL_102    0x00  // 0x00/0x01 R/W       (control) ×éºÏÖ¸Áî£¬Ð´Èë²»Í¬Ö¸Áî·µ»Ø²»Í¬Êý¾Ý
#define  TEMP_102    0x02  // 0x02/0x03 R    			(temperature)
#define  VOLT_102    0x04  // 0x04/0x05 R    			(voltage)

#define  FLAG_102    0x06
#define  CC_102      0x08
#define  SOH_102     0x0A

#define  CUR_102     0x10
#define  AVECUR_102  0x12
#define  SOC_102     0x1C  // 0x1C/0x1D R    			(relative state of charge)Ê£Óà/×ÜÁ¿°Ù·Ö±È

#define  INTTEMP_102 0x1E  // 0x1E/0x1F R    			(internal temp)           ·µ»ØÄÚ²¿ÎÂ¶ÈÖµ
#define  AltMAC_102  0x3E  // 0x3E/0x3F R/W
#define  AltMDA_102  0x40  // 0x40/0x5F  R/W
#define  AltMAC_Chk_102 0x60 //0x60
#define  AltMAC_Len_102 0x61 //0x61
#define  ChgVB_102   0x62

#define  Check_AFI_102 0x66
#define  Check_OCV_102 0x68
#define  Check_NTC_102 0x7E

#define  IAP_StatusCheck_102    0xA0

// ×ÓÃüÁî
#define  CONTROL_STATUS_102    ((uint16_t)0x0000)  //report the status word  »ñÈ¡×´Ì¬×Ö(control_status)
#define  DEVICE_TYPE_102      ((uint16_t)0x0001)  //report the information in subclass 122 offset 1
#define  FW_VERSION_102        ((uint16_t)0x0002)  //report the information in subclass 122 offset 3

//////////////////¼ÓÃÜ²»¿É²Ù×÷//////////////
#define  DF_CHECKSUM_102       ((uint16_t)0x0004)  //enables a data flash checksum to be generated and reports on a read
#define  PREV_MACWRITE_102     ((uint16_t)0x0007)  //returns previous MAC command code

//////////////////¼ÓÃÜ¿É²Ù×÷(³ý0x0010)
#define  CHEM_ID_102           ((uint16_t)0x0008)  //reports the chemical identifier
#define  BAT_INSERT_102        ((uint16_t)0x000C)  //
#define  BAT_REMOVE_102        ((uint16_t)0x000D)  //
/////////////////¼ÓÃÜ²»¿É²Ù×÷////////////
/*

*/
#define  SEALED_102            ((uint16_t)0x0020)  //force the SH366002 into SEALED mode

#define  PULSE_SOC_INT_102     ((uint16_t)0x0023)
#define  RESET_102             ((uint16_t)0x0041)  //force a full reset of the SH366002
#define  SOFT_RESET_102        ((uint16_t)0x0042)

#define  BLOCKA_102            ((uint16_t)0x0070)
#define  BLOCKB_102            ((uint16_t)0x007A)

#define  COMM_RESULT_102       ((uint16_t)0x00C0)
#define  COMM_CHECKSUM_102     ((uint16_t)0x00C2)
#define  RAND_KEY_102          ((uint16_t)0x00C4)
#define  CALI_INFO_102         ((uint16_t)0xE014)

#define  Code_Version_YM_102   ((uint16_t)0x00F5) //°æ±¾ºÅ-ÄêÔÂ
#define  Code_Version_D_102    ((uint16_t)0x00F6) //°æ±¾ºÅ-ÈÕ
#define  TS_Version_102        ((uint16_t)0x00CC)

#define  FWVersion_MAIN_102    ((uint16_t)0x00F1)
#define  TSVersion_102         ((uint16_t)0x00CC)
#define  FW_DATE_MASK_102      ((uint16_t)0x00FF)
#define  SECTOR_FLAG_102       ((uint16_t)0x00C7)

typedef enum {
	CHECK_IAP_MODE_ERR = -1,
	CHECK_IAP_MODE_IN = 5,
	CHECK_IAP_MODE_NO = 6,
}CHECK_IAP_MODE;

typedef enum {
	CHECK_VERSION_ERR = -1,
	CHECK_VERSION_OK  = 0,
	CHECK_VERSION_FW  = 1,
	CHECK_VERSION_AFI = 2,
	CHECK_VERSION_TS  = 4,
	CHECK_VERSION_WHOLE_CHIP = (CHECK_VERSION_FW|CHECK_VERSION_AFI|CHECK_VERSION_TS),
}CHECK_VERSION_MODE;

typedef enum {
	DTSI_BIN = 0,
	DTSI_NTC = 1,
	DTSI_AFI = 2,
}DTSI_FILE_TYPE;

typedef enum {
	CHARGE_STATUS_DISCHARGING = 0,
	CHARGE_STATUS_CHARGING = 1,
	CHARGE_STATUS_FULL = 2,
	CHARGE_STATUS_UNKNOWN = 3,
}chg_st_type;

typedef struct
{
	uint16_t Frame_Count;
	uint16_t Data_Stream;
}SH366102_DownLoadProcess;

typedef struct
{
	uint8_t  Length;
	uint8_t  Sysmode;
	uint16_t StartChgSOC;
	uint16_t SOC_Value;
	uint16_t RSOC_Value;
	uint16_t CapRated;
	uint16_t CycleCount;

	uint32_t ChgCapSum;
	uint32_t PassC;
	uint32_t FCC;
	uint32_t RC;
	uint32_t RealRC;

}SH366102_GuageBlock1_Struct;



extern unsigned char SH366102_Address;
extern unsigned short DeviceType;
extern CHECK_IAP_MODE  IAP_Status;

#define OPERATE_WRITE               0x02
#define OPERATE_COMPARE             0x03
#define OPERATE_WAIT                0x04
#define OPERATE_WAIT_LENGTH         0x02


#define FILE_WRITE_RETRY            0x05
#define FILE_READ_RETRY             0x05
#define FILE_LAST_RETRY             0x04

errcode_t SH366102_Read_SBS_Conmmand_HalfWord(uint8_t address , uint8_t command , uint16_t * value );
errcode_t SH366102_Read_Sub_Conmmand_HalfWord(uint8_t address, uint16_t subcommand , uint16_t * value);
errcode_t SH366102_Read_Sub_Conmmand_Block(uint8_t address, uint16_t ID , uint8_t * value ,uint8_t length);

errcode_t SH366102_GuageBlock1_Read(uint8_t address, SH366102_GuageBlock1_Struct *GuageBlock1);
uint16_t SH366102_ReadVoltage(uint8_t address);
int16_t SH366102_ReadCurrent(uint8_t address);
uint16_t SH366102_ReadSOC(uint8_t address);
float SH366102_ReadTemperature(uint8_t address);
uint16_t SH366102_ReadDeviceType(uint8_t address);
CHECK_IAP_MODE SH366102_CheckIAP(uint8_t address);
CHECK_VERSION_MODE SH366102_CheckFWVersion(uint8_t address);
CHECK_VERSION_MODE SH366102_CheckAFIVersion(uint8_t address);
CHECK_VERSION_MODE SH366102_CheckNTCVersion(uint8_t address);
errcode_t SH366102_Updata(DTSI_FILE_TYPE binfile);
bool SH366102_Update_Check(void);
int fg_init(void);
int chg_state_get(void);
uint8_t get_battery_level(void);
uint16_t get_battery_voltage(void);
extern osal_timer g_batt_check_timer;
extern uint8_t g_chargestatus;
extern uint16_t last_battery_level;






#endif



