
#include "sh366102.h"

#include "stdbool.h"
#include "sk_board.h"
#include "systick.h"
#include "stdio.h"
#include "SH366102_UpdateFile.h"
#include "pinctrl_porting.h"
#include "hal_gpio.h"
//#include "charge_drv.h"
uint8_t BlockA_Write_102[32] = {'Z', 'e', 'n' , 'c' , 'h' , 'a' , 'n' , 't' , '-', '3' , '6' , '6' , '1' , '0' , '1'};
uint8_t BlockA_Read_102[32] = {0};
osal_timer g_batt_check_timer={0};
#define SH366102_CHECK_BATT_TIME 1000*60 //60S
unsigned char SH366102_Address = 0x55;
//unsigned short DeviceType = 0x0000;
//CHECK_IAP_MODE  IAP_Status = CHECK_IAP_MODE_ERR;
errcode_t I2C_Readbuff(uint8_t address, uint8_t command, uint8_t length, uint8_t *value)
{
	errcode_t ret = ERRCODE_FAIL;
	i2c_data_t data;
	char rty_time = 0;

	data.send_buf = &command;
	data.send_len = 1;
	data.receive_buf = value;
	data.receive_len = length;
	while(rty_time < 3)
	{
		ret = uapi_i2c_master_writeread(SH366102_I2C_BUS, address, &data);
		if(ret == ERRCODE_SUCC)
			break;
		else
			rty_time++;
	}
	if(ret == ERRCODE_SUCC)
		FG_INFO("sh366102 read %d byte from 0x%x\r\n", length, address);
	else
		FG_INFO("sh366102 read fail\r\n");
	return ret;

}

errcode_t I2C_WriteBuff(uint8_t address, uint8_t command, uint8_t length, uint8_t *value)
{
	errcode_t ret = ERRCODE_FAIL;
	i2c_data_t data;
	uint8_t send_data[50]={0};
	int i = 0;
	send_data[0] = command;
	for(i=0;i<length;i++)
	{
		send_data[i+1] = value[i];
	}

	data.send_buf = send_data;
	data.send_len = length+1;
	data.receive_buf = NULL;
	data.receive_len = 0;

	ret = uapi_i2c_master_write(SH366102_I2C_BUS, address, &data);
	if(ret == ERRCODE_SUCC)
	FG_INFO("sh366102 write %d byte for 0x%x\r\n", length, address);
	else
    FG_INFO("sh366102 write fail\r\n");
	return ret;

}





errcode_t SH366102_Read_SBS_Conmmand_HalfWord(uint8_t address , uint8_t command , uint16_t * value )
{
	errcode_t status = ERRCODE_FAIL;
	uint8_t recieve_data[2] = {0x00};
	status = I2C_Readbuff(address, command, 2, recieve_data);
	if(status == ERRCODE_SUCC){
		*value = recieve_data[0] + (((uint16_t)recieve_data[1])<<8);
		FG_INFO("SH366102_Read_SBS_Conmmand_HalfWord  Command %d= 0x%x\r\n", command ,*value);
	}
	else{
		*value = 0;
		FG_INFO("SH366102_Read_SBS_Conmmand_HalfWord  Command %d Read Fail\r\n", command);
	}
	return status;
}

errcode_t SH366102_Read_Sub_Conmmand_HalfWord(uint8_t address, uint16_t subcommand , uint16_t * value)
{
	uint8_t i = 0;
	uint8_t checksum = 0;
	uint8_t DF_Checksum = 0;
	uint8_t DF_ChecksumLength = 0;

	uint8_t subcommand_buff[2] = {0x00};
	uint8_t recieve_data[2] = {0x00};

	subcommand_buff[0] = (uint8_t)subcommand;
	subcommand_buff[1] = (uint8_t)(subcommand>>8);

	if(ERRCODE_SUCC != I2C_WriteBuff(address, 0x3E, 2, subcommand_buff)){
		*value = 0;
		FG_INFO("SH366102_Read_Sub_Conmmand_HalfWord  Command %x Read Fail\r\n", subcommand);
		return ERRCODE_FAIL;
	}
	uapi_tcxo_delay_ms(10);
	if(ERRCODE_SUCC != I2C_Readbuff(address,0x40,2,recieve_data) ){
		*value = 0;
		FG_INFO("SH366102_Read_Sub_Conmmand_HalfWord  Command %x Read Fail\r\n", subcommand);
		return ERRCODE_FAIL;
	}
	FG_INFO("SH366102 read 0x40 = 0x%x 0x%x\r\n", recieve_data[0], recieve_data[1]);
	checksum = checksum +recieve_data[0]+recieve_data[1];
	checksum = checksum +subcommand_buff[0]+subcommand_buff[1];//366102计算checksum需要加上 0x3E寄存器中的命令号数据
	checksum = 0xFF-checksum;

	*value = recieve_data[0] + (((uint16_t)recieve_data[1])<<8);
	FG_INFO("SH366102_Read_Sub_Conmmand_HalfWord  Command %x = 0x%x\r\n", subcommand, *value);

	return ERRCODE_SUCC;
}

/*
描述：SH366102 所有的SubCommand Read Block都是通过3E - 40读取
length一定要对用上该命令所返回的数据长度，不然checksuam对不上
参数：ID = 子命令号  value = 返回的读取数据，length = 读取数据字节长度
*/
errcode_t SH366102_Read_Sub_Conmmand_Block(uint8_t address, uint16_t subcommand, uint8_t * value, uint8_t length)
{;
	uint8_t i = 0;
	uint8_t checksum = 0;
	uint8_t DF_Checksum = 0;
	uint8_t DF_ChecksumLength = 0;
	uint8_t subcommand_buff[2] = {0x00};
	subcommand_buff[0] = (uint8_t)subcommand;
	subcommand_buff[1] = (uint8_t)(subcommand>>8);

	if(ERRCODE_SUCC != I2C_WriteBuff(address, 0x3E, 2, subcommand_buff)){
		FG_INFO("SH366102_Read_Sub_Conmmand_Block  Command %x Read Fail\r\n", subcommand);
		return ERRCODE_FAIL;
	}
	uapi_tcxo_delay_ms(10);
	if(ERRCODE_SUCC != I2C_Readbuff(address, 0x40, length, value) ){
		FG_INFO("SH366102_Read_Sub_Conmmand_Block  Command %x Read Fail\r\n", subcommand);
		return ERRCODE_FAIL;
	}
	for(i=0 ; i <length ; i++)
	{
		checksum += *(value+i);
	}
	checksum = checksum +subcommand_buff[0]+subcommand_buff[1];//366102计算checksum需要加上 0x3E寄存器中的命令号数据
	checksum = 0xFF-checksum;
	I2C_Readbuff(address,0x60,1,&DF_Checksum);
	I2C_Readbuff(address,0x61,1,&DF_ChecksumLength);
	if( (checksum == DF_Checksum) &&( (length+4) == DF_ChecksumLength ) )
		return ERRCODE_SUCC;
	else
		return ERRCODE_FAIL;
}

errcode_t SH366102_GuageBlock1_Read(uint8_t address, SH366102_GuageBlock1_Struct *GuageBlock1)
{
	errcode_t  status = ERRCODE_FAIL;
	uint16_t RemaningCapacity = 0;
	uint16_t FullChargeCapacity = 0;
	uint8_t value[32] = {0};
	status = SH366102_Read_Sub_Conmmand_Block(address, 0x00F8 , value, 32);

	if(status == ERRCODE_SUCC)
	{
		GuageBlock1->Length = value[0];
		GuageBlock1->Sysmode = value[1];
		GuageBlock1->StartChgSOC = value[3] + ( (uint16_t)value[2]<<8 );
		GuageBlock1->SOC_Value = value[5] + ( (uint16_t)value[4]<<8 );
		GuageBlock1->RSOC_Value = value[7] + ( (uint16_t)value[6]<<8 );
		GuageBlock1->CapRated = value[9] + ( (uint16_t)value[8]<<8 );
		GuageBlock1->CycleCount = value[11] + ( (uint16_t)value[10]<<8 );

		GuageBlock1->ChgCapSum = value[15] + ( (uint16_t)value[14]<<8 ) +( (uint16_t)value[13]<<16 ) +( (uint16_t)value[12]<<24 );
		GuageBlock1->PassC = value[19] + ( (uint16_t)value[18]<<8 ) +( (uint16_t)value[17]<<16 ) +( (uint16_t)value[16]<<24 );
		GuageBlock1->FCC = value[23] + ( (uint16_t)value[22]<<8 ) +( (uint16_t)value[21]<<16 ) +( (uint16_t)value[20]<<24 );
		GuageBlock1->RC = value[27] + ( (uint16_t)value[26]<<8 ) +( (uint16_t)value[25]<<16 ) +( (uint16_t)value[24]<<24 );
		GuageBlock1->RealRC = value[31] + ( (uint16_t)value[30]<<8 ) +( (uint16_t)value[29]<<16 ) +( (uint16_t)value[28]<<24 );

	}
	else
	{
		GuageBlock1->Length = 0;
		GuageBlock1->Sysmode = 0;
		GuageBlock1->StartChgSOC = 0;
		GuageBlock1->SOC_Value = 0;
		GuageBlock1->RSOC_Value = 0;
		GuageBlock1->CapRated = 0;
		GuageBlock1->CycleCount = 0;

		GuageBlock1->ChgCapSum = 0;
		GuageBlock1->PassC = 0;
		GuageBlock1->FCC = 0;
		GuageBlock1->RC = 0;
		GuageBlock1->RealRC = 0;
	}
	RemaningCapacity = GuageBlock1->RC/3600;
	FullChargeCapacity = GuageBlock1->FCC/3600;

	FG_INFO("SH366102_GuageBlock1_Read :: Length = %d \r\n",GuageBlock1->Length);
	FG_INFO("SH366102_GuageBlock1_Read :: SysMode = 0x%x \r\n",GuageBlock1->Sysmode);
	FG_INFO("SH366102_GuageBlock1_Read :: StartChgSOC = %d ‰ \r\n",GuageBlock1->StartChgSOC);
	FG_INFO("SH366102_GuageBlock1_Read :: SOC_Value = %d \r\n",GuageBlock1->SOC_Value);
	FG_INFO("SH366102_GuageBlock1_Read :: RSOC_Value = %d ‰ \r\n",GuageBlock1->RSOC_Value);
	FG_INFO("SH366102_GuageBlock1_Read :: CapRated = %d \r\n",GuageBlock1->CapRated);
	FG_INFO("SH366102_GuageBlock1_Read :: CycleCount = %d \r\n",GuageBlock1->CycleCount);

	FG_INFO("SH366102_GuageBlock1_Read :: ChgCapSum = %d \r\n",GuageBlock1->ChgCapSum);
	FG_INFO("SH366102_GuageBlock1_Read :: PassC = %d \r\n",GuageBlock1->PassC);
	FG_INFO("SH366102_GuageBlock1_Read :: FCC = %d \r\n",GuageBlock1->FCC);
	FG_INFO("SH366102_GuageBlock1_Read :: RC = %d \r\n",GuageBlock1->RC);
	FG_INFO("SH366102_GuageBlock1_Read :: RealRC = %d \r\n",GuageBlock1->RealRC);

	FG_INFO("SH366102_GuageBlock1_Read :: FullchargingCapacity = %d \r\n",FullChargeCapacity);
	FG_INFO("SH366102_GuageBlock1_Read :: RemaingCapacity = %d \r\n",RemaningCapacity);

	return status;
}


uint16_t SH366102_ReadVoltage(uint8_t address){
	uint8_t voltage_buff[2] = {0x00};
	uint16_t voltage = 0;
	if( ERRCODE_SUCC != I2C_Readbuff(address, VOLT_102, 2, voltage_buff) ){
		FG_INFO("SH366102_ReadVoltage : Voltage Read Fail\r\n");
	}
	else{
		voltage = voltage_buff[0] + (((uint16_t)voltage_buff[1])<<8);
		FG_INFO("SH366102_ReadVoltage : Voltage = %d\r\n", voltage);
	}
	return voltage;
}

uint32_t SH366102_Read_battery_level(void){
	uint32_t level = 0;
	level = SH366102_ReadSOC(SH366102_Address);
	return level;
}

int16_t SH366102_ReadCurrent(uint8_t address){
	uint8_t current_buff[2] = {0x00};
	int16_t current = 0;
	if( ERRCODE_SUCC != I2C_Readbuff(address, CUR_102, 2, current_buff) ){
		FG_INFO("SH366102_ReadCurrent : Current Read Fail\r\n");
	}
	else{
		current = current_buff[0] + (((uint16_t)current_buff[1])<<8);
		FG_INFO("SH366102_ReadCurrent : Current = %d\r\n", current);
	}
	return current;
}

int16_t SH366102_Read_chg_current(void){
	int16_t current = 0;
	current = SH366102_ReadCurrent(SH366102_Address);
	return current;
}


uint16_t SH366102_ReadSOC(uint8_t address){
	uint8_t soc_buff[2] = {0x00};
	uint16_t soc = 0;
	if( ERRCODE_SUCC != I2C_Readbuff(address, SOC_102, 2, soc_buff) ){
		FG_INFO("SH366102_ReadSOC : SOC Read Fail\r\n");
	}
	else{
		soc = soc_buff[0] + (((uint16_t)soc_buff[1])<<8);
		FG_INFO("SH366102_ReadSOC : SOC = %d\r\n", soc);
	}
	return soc;
}

float SH366102_ReadTemperature(uint8_t address){
	uint8_t temperature_buff[2] = {0x00};
	float temperature = 0;
	if( ERRCODE_SUCC != I2C_Readbuff(address, TEMP_102, 2, temperature_buff) ){
		FG_INFO("SH366102_ReadTemperature : Temperature Read Fail\r\n");
	}
	else{
		temperature = temperature_buff[0] + (((uint16_t)temperature_buff[1])<<8);
	}
	temperature = (temperature-2731)/10.0;
	FG_INFO("SH366102_ReadTemperature : Temperature = %f\r\n", temperature);
	return temperature;
}

uint16_t SH366102_ReadDeviceType(uint8_t address){
	uint16_t devicetype = 0;
	if(SH366102_Read_Sub_Conmmand_HalfWord(address, DEVICE_TYPE_102, &devicetype) != ERRCODE_SUCC)
		return 0;
	FG_INFO("SH366102_ReadDeviceType  DeviceType = 0x%x\r\n", devicetype);
	return devicetype;
}

CHECK_IAP_MODE SH366102_CheckIAP(uint8_t address){
	uint8_t iap_read2[4];
	uint8_t iap_read1[4];
	if(I2C_Readbuff(address ,0xA2,4,iap_read1) != ERRCODE_SUCC)
		return CHECK_IAP_MODE_ERR;
	if(I2C_Readbuff(address ,0xA2,4,iap_read2) != ERRCODE_SUCC)
		return CHECK_IAP_MODE_ERR;
	if((iap_read2[0]==iap_read1[0]) &&(iap_read2[1]==iap_read1[1])&&(iap_read2[2]==iap_read1[2]) &&(iap_read2[3]==iap_read1[3]) ){
		if((iap_read2[0]==0x02) &&(iap_read2[1]==0x03)&&(iap_read2[2]==00) &&(iap_read2[3]==0x61) )
			return CHECK_IAP_MODE_IN;
		else
			return CHECK_IAP_MODE_NO;
	}
	else{
		return CHECK_IAP_MODE_ERR;
	}
}

CHECK_VERSION_MODE SH366102_CheckFWVersion(uint8_t address){
	uint16_t ym_version1 = 0;
	uint16_t ym_version2 = 0;
	uint16_t day_version1 = 0;
	uint16_t day_version2 = 0;
	uint32_t fwversion = 0;
	//////////////////////Read 0x00F5 twice and then compare////////////////////////
	if(SH366102_Read_Sub_Conmmand_HalfWord(address, Code_Version_YM_102, &ym_version1) != ERRCODE_SUCC){
		if(SH366102_Read_Sub_Conmmand_HalfWord(address, Code_Version_YM_102, &ym_version1) != ERRCODE_SUCC)
			return CHECK_VERSION_ERR;
	}

	if(SH366102_Read_Sub_Conmmand_HalfWord(address, Code_Version_YM_102, &ym_version2) != ERRCODE_SUCC){
		if(SH366102_Read_Sub_Conmmand_HalfWord(address, Code_Version_YM_102, &ym_version2) != ERRCODE_SUCC)
			return CHECK_VERSION_ERR;
	}

	//////////////////////Read 0x00F6 twice and then compare////////////////////////
	if(SH366102_Read_Sub_Conmmand_HalfWord(address, Code_Version_D_102, &day_version1) != ERRCODE_SUCC){
		if(SH366102_Read_Sub_Conmmand_HalfWord(address, Code_Version_D_102, &day_version1) != ERRCODE_SUCC)
			return CHECK_VERSION_ERR;
	}

	if(SH366102_Read_Sub_Conmmand_HalfWord(address, Code_Version_D_102, &day_version2) != ERRCODE_SUCC){
		if(SH366102_Read_Sub_Conmmand_HalfWord(address, Code_Version_D_102, &day_version2) != ERRCODE_SUCC)
			return CHECK_VERSION_ERR;
	}

	//////////////////////if the two read results are different . this result is invalid ////////////////////////
	if( (ym_version1 != ym_version2)||(day_version1 != day_version2) )
		return CHECK_VERSION_ERR;
	fwversion = ((uint32_t)ym_version1<<16) + day_version1;
	FG_INFO("SH366102_CheckFWVersion : Read IC FW Version = : 0x%x-----File Bin Version = : 0x%x \r\n", fwversion, BIN_FW_Date);
	if(fwversion == BIN_FW_Date){
		return CHECK_VERSION_OK;
	}
	else{
		return CHECK_VERSION_FW;
//		return CHECK_VERSION_WHOLE_CHIP; //看情况，可选择将Code,AFI NTC都升级
	}
}

CHECK_VERSION_MODE SH366102_CheckAFIVersion(uint8_t address){
	uint16_t afi_version1 = 0;
	uint16_t afi_version2 = 0;
	uint16_t ocv_version1 = 0;
	uint16_t ocv_version2 = 0;
	//////////////////////Read 0x66 twice and then compare////////////////////////
	if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_AFI_102, &afi_version1 ) == ERRCODE_FAIL){
		if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_AFI_102, &afi_version1 ) == ERRCODE_FAIL)
			return CHECK_VERSION_ERR;
	}

	if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_AFI_102, &afi_version2 ) == ERRCODE_FAIL){
		if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_AFI_102, &afi_version2 ) == ERRCODE_FAIL)
			return CHECK_VERSION_ERR;
	}

	//////////////////////Read 0x68 twice and then compare////////////////////////
	if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_OCV_102, &ocv_version1 ) == ERRCODE_FAIL){
		if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_OCV_102, &ocv_version1 ) == ERRCODE_FAIL)
			return CHECK_VERSION_ERR;
	}

	if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_OCV_102, &ocv_version2 ) == ERRCODE_FAIL){
		if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_OCV_102, &ocv_version2 ) == ERRCODE_FAIL)
			return CHECK_VERSION_ERR;
	}
	//////////////////////if the two read results are different . this result is invalid ////////////////////////
	if( (afi_version1 != afi_version2)||(ocv_version1 != ocv_version2) )
		return CHECK_VERSION_ERR;
	FG_INFO("SH366102_CheckAFIVersion : Read IC AFI Version = 0x%x and OCV Version = 0x%x \r\n", afi_version1, ocv_version1);
	FG_INFO("SH366102_CheckAFIVersion : File AFI Version = 0x%x and OCV Version = 0x%x \r\n", AFI_Check_Version, OCV_Check_Version);
	if( (afi_version1 == AFI_Check_Version)&&(ocv_version1 == OCV_Check_Version) )
		return CHECK_VERSION_OK;
	else
		return CHECK_VERSION_AFI;
}

CHECK_VERSION_MODE SH366102_CheckNTCVersion(uint8_t address){
	uint16_t ntc_version1 = 0;
	uint16_t ntc_version2 = 0;

	//////////////////////Read 0x66 twice and then compare////////////////////////
	if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_NTC_102, &ntc_version1 ) == ERRCODE_FAIL){
		if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_NTC_102, &ntc_version1 ) == ERRCODE_FAIL)
			return CHECK_VERSION_ERR;
	}

	if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_NTC_102, &ntc_version2 ) == ERRCODE_FAIL){
		if(SH366102_Read_SBS_Conmmand_HalfWord(address, Check_NTC_102, &ntc_version2 ) == ERRCODE_FAIL)
			return CHECK_VERSION_ERR;
	}
	//////////////////////if the two read results are different . this result is invalid ////////////////////////
	if( (ntc_version1 != ntc_version2) )
		return CHECK_VERSION_ERR;
	FG_INFO("SH366102_CheckNTCVersion : Read IC NTC Version = 0x%x-----File NTC Version = 0x%x \r\n", ntc_version1, NTC_Check_Version);
	if( ntc_version1 == NTC_Check_Version)
		return CHECK_VERSION_OK;
	else
		return CHECK_VERSION_TS;
}

uint8_t *update_buffer = NULL;
unsigned int update_length = 0;

errcode_t SH366102_Updata(DTSI_FILE_TYPE binfile){
	uint32_t i = 0;
	uint16_t j = 0;
	errcode_t status = ERRCODE_FAIL;
	uint8_t operation_type = 0;
	uint8_t operation_length = 0;
	uint8_t device_address = 0x00;
	uint8_t reg_address = 0x00;
	uint8_t Compare[32] = {0x00};
	uint16_t delay_time = 0;
	uint8_t Check_Ok = 0;  //0代表 0x03操作 对比正确，1代表错误
	uint32_t structpos= 0;
	uint16_t line_number = 0;
//	for(i=0; i < length; i++ )
	switch (binfile)
	{
		case 0:
		update_buffer = (uint8_t*)&DTSI_BIN_102;
		update_length = BIN_LENGTH;
		break;
		case 1:
		update_buffer = (uint8_t*)&DTSI_NTC_102;
		update_length = NTC_LENGTH;
		break;
		case 2:
		update_buffer = (uint8_t*)DTSI_AFI_102;
		update_length = AFI_LENGTH;
		break;
		default:
		return ERRCODE_FAIL;
		break;
	}

	for(i=0; i< update_length; i=structpos)
	{
		operation_type = *(uint8_t *)( update_buffer + structpos +0);// 每一帧的第一个字节是操作类型：0x02--写入；0x03--对比；0x04--延时
		switch( operation_type)
		{
			case OPERATE_WRITE:
				device_address = *(uint8_t *)( update_buffer + structpos + 1);
				device_address = device_address >> 1;
				reg_address = *(uint8_t *)( update_buffer + structpos + 2);
				operation_length = *(uint8_t *)( update_buffer + structpos + 3);//后面要写的数据长度
				for(j = 0 ; j<FILE_WRITE_RETRY ; j++)
				{
					status = I2C_WriteBuff(device_address, reg_address , operation_length , (uint8_t *)( update_buffer + structpos + 4));//写数据
					if(status == ERRCODE_SUCC)
					{
						FG_INFO("%s Line %d Write success\r\n", "\x1b[32m",line_number);
						break;
					}
					if(j == FILE_LAST_RETRY)
					{
						FG_INFO("%s Line %d Write error\r\n","\x1b[31m",line_number);
						return ERRCODE_FAIL;
					}
				}
				structpos = structpos + operation_length + 4;
				break;

			case OPERATE_COMPARE:
				device_address = *(uint8_t *)( update_buffer + structpos + 1);
				device_address = device_address >> 1;
				reg_address = *(uint8_t *)( update_buffer + structpos + 2);
				operation_length = *(uint8_t *)( update_buffer + structpos +3);//后面要写的数据长度
				if(operation_length >32) //数据不会超过32个字节
					return ERRCODE_FAIL;
				for(j = 0 ; j < FILE_READ_RETRY ; j++)
				{
					status = I2C_Readbuff(device_address, reg_address , operation_length , Compare);//读数据
					FG_INFO("line %d read date is %s", line_number, Compare);
					if(status == ERRCODE_SUCC)
						break;
					if(j == FILE_LAST_RETRY)
						return ERRCODE_FAIL;
				}
				for(j=0 ; j<operation_length ; j++)
				{
					if(Compare[j] != *(uint8_t *)( update_buffer + structpos + 4 + j) )
					{
						Check_Ok = 1;
						FG_INFO("%s Line %d Read error\r\n","\x1b[31m",line_number);
						return ERRCODE_FAIL;
					}
					else{
						FG_INFO("%s Line %d  Read success\r\n","\x1b[31m",line_number);
						Check_Ok = 0;
					}
				}
				structpos = structpos + operation_length +4;
				break;

			case OPERATE_WAIT:
				operation_length = *(uint8_t *)( update_buffer + structpos +1);

				if(*(uint8_t *)( update_buffer+ structpos +1) == OPERATE_WAIT_LENGTH)
				{
					delay_time = ( (uint16_t)(*(uint8_t *)( update_buffer  + structpos +2) )<<8) + *(uint8_t *)( update_buffer + structpos +3);
					uapi_tcxo_delay_ms(delay_time);
				}
				else
					return ERRCODE_FAIL;
				structpos = structpos + operation_length + 2;
				break;

			default:
				return ERRCODE_FAIL;
				break;

		}
		line_number += 1;
	}
	return ERRCODE_SUCC;
}

bool SH366102_Update_Check(void){
	uint16_t DeviceType = 0x0000;
	CHECK_IAP_MODE IAP_Status = CHECK_IAP_MODE_ERR;
	DeviceType = SH366102_ReadDeviceType(SH366102_Address);

	if(0x6102 != DeviceType) //if device type is not 6102, retry read
		DeviceType = SH366102_ReadDeviceType(SH366102_Address);
	if(0x6102 != DeviceType){ //if device type is not 6102, check ic is't in isp mode
		IAP_Status = SH366102_CheckIAP(SH366102_Address);
		FG_INFO("IAP_Status = %d\r\n", IAP_Status);
		if(IAP_Status == CHECK_IAP_MODE_IN){//if in IAP Mode, update bin ,AFI, NTC file
        FG_INFO("BIN_LENGTH = %ld\r\n", BIN_LENGTH);
			SH366102_Updata(DTSI_BIN); //update firmware
			if(CHECK_VERSION_OK != SH366102_CheckFWVersion(SH366102_Address) ){ //check firmware version,
			//////*deal with update fail *//////
				return false;
			}
			SH366102_Updata(DTSI_NTC); //update ntc parameter
			if(CHECK_VERSION_OK != SH366102_CheckNTCVersion(SH366102_Address) ){
			//////*deal with update fail *//////
				return false;
			}

			SH366102_Updata(DTSI_AFI); //update afi cell parameter
			if(CHECK_VERSION_OK != SH366102_CheckAFIVersion(SH366102_Address) ){
			//////*deal with update fail *//////
				return false;
			}
		}else{
			//////* Not in IAP and DeviceType is not 0x6102*/////
			return false;
		}
	}
	else /////////if device type is 6102, check fwversion, afi version ,ntc version
	{
		set_device_state(DEVICE_FG);
		if(CHECK_VERSION_FW == SH366102_CheckFWVersion(SH366102_Address) ) //check Firmware version
			SH366102_Updata(DTSI_BIN); //update firmware
			uapi_tcxo_delay_ms(100);
		if(CHECK_VERSION_OK != SH366102_CheckFWVersion(SH366102_Address) ){
		//////*deal with update fail *//////
			FG_INFO("SH366102_Init: update bin is fail or check Fw version error\r\n");
			return false;
		}

		if(CHECK_VERSION_TS == SH366102_CheckNTCVersion(SH366102_Address) )
			SH366102_Updata(DTSI_NTC); //update ntc parameter
			uapi_tcxo_delay_ms(100);
		if(CHECK_VERSION_OK != SH366102_CheckNTCVersion(SH366102_Address) ){
		//////*deal with update fail *//////
			FG_INFO("SH366102_Init: update ntc is fail or check ntc version error\r\n");
			return false;
		}

		if(CHECK_VERSION_AFI == SH366102_CheckAFIVersion(SH366102_Address) )
			SH366102_Updata(DTSI_AFI); //update ntc parameter
			uapi_tcxo_delay_ms(100);
		if(CHECK_VERSION_OK != SH366102_CheckAFIVersion(SH366102_Address) ){
		//////*deal with update fail *//////
			FG_INFO("SH366102_Init: update afi is fail or check afi version error\r\n");
			return false;
		}

	}
	return true;
}
uint16_t last_battery_level = 0;
static uint8_t last_report_battery_level = 0;
static uint8_t first_battery_level_report = 1;
uint16_t last_battery_voltage = 0;
uint16_t get_battery_voltage(void)
{
    return last_battery_voltage;
}
void batt_check_timer_func(void)
{
    // static bool battery_level_smooth = false;

	int16_t battery_current = 0;
	float  battery_temp = 0.0;
	uint16_t battery_voltage = 0;
	static bool high_temp_alarm = false;
	static bool low_temp_alarm = false;
	static bool battery_is_full = false;
	static chg_st_type charge_status = CHARGE_STATUS_UNKNOWN;
	uint16_t battery_level = SH366102_ReadSOC(SH366102_Address);
	FG_INFO("batt_check_timer_func in\n");

	if ((battery_level!= last_battery_level) && (!first_battery_level_report))
	{
		FG_INFO("battery_level = %d\n", battery_level);
		if(!chg_state_get()){
			if(battery_level > last_battery_level){
				battery_level = last_battery_level;
			}else{
				last_battery_level = battery_level;
			}
		}
		else
		{
			last_battery_level = battery_level;
		}
	}

	battery_voltage = SH366102_ReadVoltage(SH366102_Address);
	last_battery_voltage = battery_voltage;

	battery_current = SH366102_ReadCurrent(SH366102_Address);
    // judge if stop charging
	if(battery_current <= 50 && battery_level == 100 && g_chargestatus == CHARGE_STATUS_CHARGING)
	{
		charge_status = CHARGE_STATUS_FULL;
		rgb_led_set_rgb(false,true,false);
		uapi_pin_set_mode(ULP_GPIO0, (pin_mode_t)HAL_PIO_FUNC_GPIO);
		uapi_gpio_set_dir(ULP_GPIO0, GPIO_DIRECTION_OUTPUT);
		uapi_gpio_set_val(ULP_GPIO0, GPIO_LEVEL_LOW);
		battery_is_full = true;
		// battery_level_smooth = false;

	}
	//judeg if recover charging
	if(battery_level <= 97 && g_chargestatus == CHARGE_STATUS_CHARGING )
	{

		uapi_pin_set_mode(ULP_GPIO0, (pin_mode_t)HAL_PIO_FUNC_GPIO);
		uapi_gpio_set_dir(ULP_GPIO0, GPIO_DIRECTION_OUTPUT);
		uapi_gpio_set_val(ULP_GPIO0, GPIO_LEVEL_HIGH);
		printf("pull gpio low then high\n");
	}
	//variable overturn
	if(g_chargestatus == CHARGE_STATUS_DISCHARGING)
	{
		battery_is_full = false;
	}
	 printf("g_chargestatus is %d,battery_is_full is %d,last_battery_level is %d\n",g_chargestatus,battery_is_full,last_battery_level);
     // if usb is present, level ui is always 100
	 if(g_chargestatus == CHARGE_STATUS_CHARGING && battery_is_full == true)
	{
		last_battery_level = 100;
	}


	battery_temp = SH366102_ReadTemperature(SH366102_Address);
	if(battery_temp >= 45.0 && high_temp_alarm == false)
	{
		high_temp_alarm = true;
		uapi_pin_set_mode(ULP_GPIO0, (pin_mode_t)HAL_PIO_FUNC_GPIO);
		uapi_gpio_set_dir(ULP_GPIO0, GPIO_DIRECTION_OUTPUT);
		uapi_gpio_set_val(ULP_GPIO0, GPIO_LEVEL_LOW);
	}
	else if(battery_temp < 40.0 && high_temp_alarm == true)
	{
		high_temp_alarm = false;
		uapi_pin_set_mode(ULP_GPIO0, (pin_mode_t)HAL_PIO_FUNC_GPIO);
		uapi_gpio_set_dir(ULP_GPIO0, GPIO_DIRECTION_OUTPUT);
		uapi_gpio_set_val(ULP_GPIO0, GPIO_LEVEL_HIGH);
	}

	if(battery_temp <= 0.0 && low_temp_alarm == false)
	{
		low_temp_alarm = true;
		uapi_pin_set_mode(ULP_GPIO0, (pin_mode_t)HAL_PIO_FUNC_GPIO);
		uapi_gpio_set_dir(ULP_GPIO0, GPIO_DIRECTION_OUTPUT);
		uapi_gpio_set_val(ULP_GPIO0, GPIO_LEVEL_LOW);
	}
	else if(battery_temp > 0.0 && low_temp_alarm == true)
	{
		low_temp_alarm = false;
		uapi_pin_set_mode(ULP_GPIO0, (pin_mode_t)HAL_PIO_FUNC_GPIO);
		uapi_gpio_set_dir(ULP_GPIO0, GPIO_DIRECTION_OUTPUT);
		uapi_gpio_set_val(ULP_GPIO0, GPIO_LEVEL_HIGH);
	}
if(g_chargestatus == CHARGE_STATUS_CHARGING)
	osal_timer_mod(&g_batt_check_timer,1000*30);
else
	osal_timer_mod(&g_batt_check_timer,SH366102_CHECK_BATT_TIME);
}


#if 0
errcode_t SH366102_UpdateBIN(void)
{
	uint16_t i = 0;
	uint16_t j = 0;
	SH366102_DownLoadProcess Process = {0,0};//烧录进度表

	errcode_t status = ERRCODE_FAIL;
	uint8_t  Frame_Cyecle =0;

	uint8_t operation_type = 0;
	uint8_t operation_length = 0;
	uint8_t device_address = 0x00;
	uint8_t reg_address = 0x00;
	uint8_t Compare[32] = {0x00};
	uint16_t delay_time = 0;
	uint8_t Check_Ok = 0;  //0代表 0x03操作 对比正确，1代表错误
	uint32_t structpos= 0;
	for(i=0 ; i<1722  ; i++)
	{
		I2C_DeInit(I2C1);
		I2C_Config();
		operation_type = *(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos +0);// 每一帧的第一个字节是操作类型：0x02--写入；0x03--对比；0x04--延时
		switch( operation_type)
		{
			case 0x02:
				device_address = *(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos + 1);
				reg_address = *(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos + 2);
				operation_length = *(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos + 3);//后面要写的数据长度
				for(j = 0 ; j<5 ; j++)
				{
					status = I2C_WriteBuff(device_address, reg_address , operation_length , (uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos + 4));//写数据
					if(status == ERRCODE_SUCC)
					{
						FG_INFO("%s Line %d Write success\r\n", "\x1b[32m",i);
						break;
					}
					if(j == 5)
					{
						FG_INFO("%s Line %d Write error\r\n","\x1b[31m",i);
						return ERRCODE_FAIL;
					}
				}
				structpos = structpos + operation_length + 4;
				break;

			case 0x03:
				device_address = *(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos + 1);
				reg_address = *(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos + 2);
				operation_length = *(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos +3);//后面要写的数据长度
				if(operation_length >32) //数据不会超过32个字节
					return ERRCODE_FAIL;
				for(j = 0 ; j < 5 ; j++)
				{
					status = I2C_Readbuff(device_address, reg_address , operation_length , Compare);//读数据
					FG_INFO("line %d read date is %s", i, Compare);
					if(status == ERRCODE_SUCC)
						break;
					if(j == 5)
						return ERRCODE_FAIL;
				}
				for(j=0 ; j<operation_length ; j++)
				{
					if(Compare[j] != *(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos + 4 + j) )
					{
						Check_Ok = 1;
						FG_INFO("%s Line %d ,Cycle %d Read error\r\n","\x1b[31m",i,Frame_Cyecle);
						return ERRCODE_FAIL;
					}
					else
						Check_Ok = 0;
				}
				structpos = structpos + operation_length +4;
				break;

			case 0x04:
				operation_length = *(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos +1);

				if(*(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos +1) == 0x02)
				{
					delay_time = ( (uint16_t)(*(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos +2) )<<8) + *(uint8_t *)( (uint32_t)&DTSI_BIN_102 + structpos +3);
					uapi_tcxo_delay_ms(delay_time);
				}
				else
					return ERRCODE_FAIL;
				structpos = structpos + operation_length + 2;
				break;

			default:
					break;

		}
	}

	return ERRCODE_SUCC;


}

errcode_t SH366102_UpdateAFI(void)
{
	uint16_t i = 0;
	uint16_t j = 0;
	SH366102_DownLoadProcess Process = {0,0};//烧录进度表

	errcode_t status = ERRCODE_FAIL;
	uint8_t  Frame_Cyecle =0;

	uint8_t operation_type = 0;
	uint8_t operation_length = 0;
	uint8_t device_address = 0x00;
	uint8_t reg_address = 0x00;
	uint8_t Compare[32] = {0x00};
	uint16_t delay_time = 0;
	uint8_t Check_Ok = 0;  //0代表 0x03操作 对比正确，1代表错误
	uint32_t structpos= 0;
//	uint16_t length = sizeof(DTSI_Addres)/4;
	for(i=0 ; i<76  ; i++)
	{
		I2C_DeInit(I2C1);
		I2C_Config();
		operation_type = *(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos + 0);// 每一帧的第一个字节是操作类型：0x02--写入；0x03--对比；0x04--延时
		switch( operation_type)
		{
			case 0x02:
				device_address = *(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos + 1);

				reg_address = *(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos + 2);
				operation_length = *(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos + 3);//后面要写的数据长度
				for(j = 0 ; j<5 ; j++)
				{
					status = I2C_WriteBuff(device_address, reg_address , operation_length , (uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos +4));//写数据
					if(status == ERRCODE_SUCC)
					{
						FG_INFO("%s Line %d Write success\r\n", "\x1b[32m",i);
						break;
					}
					if(j == 5)
					{
						FG_INFO("%s Line %d Write error\r\n","\x1b[31m",i);
						return ERRCODE_FAIL;
					}
				}
				structpos = structpos + operation_length +4;
				break;

			case 0x03:
				device_address = *(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos +1);
				reg_address = *(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos +2);
				operation_length = *(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos +3);//后面要写的数据长度
				if(operation_length >32) //数据不会超过32个字节
					return ERRCODE_FAIL;
				for(j = 0 ; j<5 ; j++)
				{
					status = I2C_Readbuff(device_address, reg_address , operation_length , Compare);//读数据
					FG_INFO("line %d read date is %s",i,Compare);
					if(status == ERRCODE_SUCC)
						break;
					if(j == 5)
						return ERRCODE_FAIL;
				}


				for(j=0 ; j<operation_length ; j++)
				{
					if(Compare[j] != *(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos +4+j) )
					{
						Check_Ok = 1;
						FG_INFO("%s Line %d ,Cycle  Read error\r\n","\x1b[31m",i);
						return ERRCODE_FAIL;
					}
					else
						Check_Ok = 0;
				}
				structpos = structpos + operation_length + 4;

				FG_INFO("%s Line %d Read success\r\n", "\x1b[32m",i);

				break;

			case 0x04:
				operation_length = *(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos +1);
				if(*(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos +1) == 0x02)
				{
					delay_time = ( (uint16_t)(*(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos +2) )<<8) + *(uint8_t *)( (uint32_t)&DTSI_AFI_102 + structpos +3);
					uapi_tcxo_delay_ms(delay_time);
				}
				else
					return ERRCODE_FAIL;
				structpos = structpos + operation_length + 2;
				break;

			default:
				break;

		}
	}

	return ERRCODE_SUCC;


}

errcode_t SH366102_UpdateNTC(void)
{
	uint16_t i = 0;
	uint16_t j = 0;
	SH366102_DownLoadProcess Process = {0,0};//烧录进度表

	errcode_t status = ERRCODE_FAIL;
	uint8_t  Frame_Cyecle =0;

	uint8_t operation_type = 0;
	uint8_t operation_length = 0;
	uint8_t device_address = 0x00;
	uint8_t reg_address = 0x00;
	uint8_t Compare[32] = {0x00};
	uint16_t delay_time = 0;
	uint8_t Check_Ok = 0;  //0代表 0x03操作 对比正确，1代表错误
	uint32_t structpos= 0;
	for(i=0 ; i<56  ; i++)
	{
		I2C_DeInit(I2C1);
		I2C_Config();
		operation_type = *(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +0);// 每一帧的第一个字节是操作类型：0x02--写入；0x03--对比；0x04--延时
		switch( operation_type)
		{
			case 0x02:
				device_address = *(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +1);

				reg_address = *(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +2);
				operation_length = *(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +3);//后面要写的数据长度
				for(j = 0 ; j<5 ; j++)
				{
					status = I2C_WriteBuff(device_address, reg_address , operation_length , (uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +4));//写数据
					if(status == ERRCODE_SUCC)
					{
						FG_INFO("%s Line %d Write success\r\n", "\x1b[32m",i);
						break;
					}
					if(j == 5)
					{
						FG_INFO("%s Line %d Write error\r\n","\x1b[31m",i);
						return ERRCODE_FAIL;
					}
				}
				structpos = structpos + operation_length +4;
				break;

			case 0x03:
				device_address = *(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +1);
				reg_address = *(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +2);
				operation_length = *(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +3);//后面要写的数据长度
				if(operation_length >32) //数据不会超过32个字节
					return ERRCODE_FAIL;
				for(j = 0 ; j<5 ; j++)
				{
					status = I2C_Readbuff(device_address, reg_address , operation_length , Compare);//读数据
					FG_INFO("line %d read date is %s",i,Compare);
					if(status == ERRCODE_SUCC)
						break;
					if(j == 5)
						return ERRCODE_FAIL;
				}


				for(j=0 ; j<operation_length ; j++)
				{
					if(Compare[j] != *(uint8_t *)( (uint32_t)&DTSI_NTC_102 +structpos +4+j) )
					{
						Check_Ok = 1;

						FG_INFO("%s Line %d ,Cycle Read error\r\n","\x1b[31m",i);

						return ERRCODE_FAIL;
					}
					else
						Check_Ok = 0;


				}

				structpos = structpos + operation_length + 4;
				FG_INFO("%s Line %d Read success\r\n", "\x1b[32m",i);

				break;

			case 0x04:
				operation_length = *(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +1);
				if(*(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +1) == 0x02)
				{
					delay_time = ( (uint16_t)(*(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +2) )<<8) + *(uint8_t *)( (uint32_t)&DTSI_NTC_102 + structpos +3);
					uapi_tcxo_delay_ms(delay_time);
				}
				else
					return ERRCODE_FAIL;
				structpos = structpos + operation_length + 2;
				break;

			default:
					break;

		}
	}

	return ERRCODE_SUCC;


}
#endif

int charge_isr_callback(void)
{
	int16_t battery_current = 0;
    PRINT("charge_isr_callback\n");
    battery_current = SH366102_ReadCurrent(SH366102_Address);
	if(battery_current > 0)
	rgb_led_set_rgb(true,false,false);
	else
	rgb_led_set_green(false,true,false);


    return 0;

}

int register_vbus_callback(void)
{
	uapi_pin_set_mode(S_AGPIO_L20, (pin_mode_t)HAL_PIO_FUNC_GPIO);
	uapi_gpio_set_dir(S_AGPIO_L20, GPIO_DIRECTION_INPUT);
	if (uapi_gpio_register_isr_func(S_AGPIO_L20, GPIO_INTERRUPT_RISING_EDGE ,charge_isr_callback) != ERRCODE_SUCC) {
        PRINT("PIN: %d int registet failed. \r\n", S_AGPIO_L20);
       uapi_gpio_unregister_isr_func(S_AGPIO_L20);
       PRINT("PIN: %d int unregistet success. \r\n", S_AGPIO_L20);
        return -1;
	}
	else
	PRINT("PIN: %d int registet success. \r\n", S_AGPIO_L20);
	return 0;

}


int fg_init(void)
{
    int ret = -1;
	bool bRet = false;
	CHECK_IAP_MODE IAP_Status;

	FG_INFO("fg_init in!\n");
	bRet =SH366102_Update_Check();
    if(bRet == false)
    {
        FG_INFO("SH366102_Update_Check failed!\n");
        return ret;
    }
	g_batt_check_timer.handler = batt_check_timer_func;
    g_batt_check_timer.interval = SH366102_CHECK_BATT_TIME;
	ret=osal_timer_init(&g_batt_check_timer);
    if(ret!=OSAL_SUCCESS)
    {
        FG_INFO("osal_timer_init failed\n");
    }
	else
	{
		FG_INFO("osal_timer_init success\n");
		osal_timer_start(&g_batt_check_timer);
	}
	ret = register_vbus_callback();
	set_device_state(DEVICE_FG);
	last_battery_level = SH366102_ReadSOC(SH366102_Address);
	last_battery_voltage = SH366102_ReadVoltage(SH366102_Address);
	return  ret;
}

void chg_detect_init(void)
{
	uapi_pin_set_mode(S_AGPIO_R5, (pin_mode_t)HAL_PIO_FUNC_GPIO);
	uapi_gpio_set_dir(S_AGPIO_R5, GPIO_DIRECTION_INPUT);
	if(uapi_gpio_get_val(S_AGPIO_R5) == GPIO_LEVEL_LOW )
	{
		/* 为1充电状态 */
		g_chargestatus = 1;
		rgb_led_set_rgb(true,false,false);
		osal_timer_mod(&g_batt_check_timer,1000*10);
	}
	else
	{
		/* 为1充电状态 */
		g_chargestatus = 0;
	}
}

int chg_state_get(void)
{
	return g_chargestatus;
}

uint8_t get_battery_level(void)
{
	uint8_t delta_level = 0;
	uint8_t report_battery_level = 0;
	if(first_battery_level_report == 1)
	{
		report_battery_level = last_battery_level;
		last_report_battery_level = report_battery_level;
		first_battery_level_report = 0;
		goto end;
	}
	if(!chg_state_get()){
		if(last_report_battery_level > last_battery_level){
			delta_level = last_report_battery_level - last_battery_level;
			if(delta_level > 1)
				report_battery_level = last_report_battery_level - 1;
			else
				report_battery_level = last_battery_level;
		}else{
			report_battery_level = last_report_battery_level;
		}
	}else{
		report_battery_level = last_battery_level;
	}
	last_report_battery_level = report_battery_level;
	end:
		return report_battery_level;
}
