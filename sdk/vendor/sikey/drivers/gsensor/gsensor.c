#include "gsensor.h"
#include "lsm6dsow_api.h"
#include "dev_storage.h"
#include "lsm6dso_reg.h"

extern nv_sensor_cali_t                    sensor_cali_data;
extern model_version_t                     gyro_cali_data;

void gsensor_read_data(int16_t *acc_data, int16_t *gyro_data)
{


    lsm6dso_read_sensor_data(acc_data, gyro_data);
	for(int i=0;i<3;i++)
	{
		acc_data[i] += sensor_cali_data.acc_sensor_cali[i];
	}
	gyro_data[0] += gyro_cali_data.major_version;
	gyro_data[1] += gyro_cali_data.minor_version;
	gyro_data[2] += gyro_cali_data.patch_version;


}
#define LSM6DSOW_CALI_NUM 5
#define LSM6DSOW_LSB_1G			2049			// mg
int32_t gsensor_calibrate(void)
{
	int16_t acc_data[3]={0}, acc_data_avg[3]={0};
    int16_t gyro_data[3]={0}, gyro_data_avg[3]={0};
    int16_t lsm6dsow_cali[3]={0};
	int16_t lsm6dsow_cali_gyro[3]={0};
	int16_t icount;
	int32_t ret = 0;

		for(icount = 0; icount < LSM6DSOW_CALI_NUM; icount++)
		{
			lsm6dso_read_sensor_data(acc_data,gyro_data);
			acc_data_avg[0] += acc_data[0];	//data[0];
			acc_data_avg[1] += acc_data[1];	//data[1];
			acc_data_avg[2] += acc_data[2];	//data[2];
			gyro_data_avg[0] += gyro_data[0];	//data[3];
			gyro_data_avg[1] += gyro_data[1];	//data[4];
			gyro_data_avg[2] += gyro_data[2];	//data[5];
			osal_mdelay(50);
		}

		acc_data_avg[0] = acc_data_avg[0]/LSM6DSOW_CALI_NUM;
		acc_data_avg[1] = acc_data_avg[1]/LSM6DSOW_CALI_NUM;
		acc_data_avg[2] = acc_data_avg[2]/LSM6DSOW_CALI_NUM;

		gyro_data_avg[0] = gyro_data_avg[0]/LSM6DSOW_CALI_NUM;
		gyro_data_avg[1] = gyro_data_avg[1]/LSM6DSOW_CALI_NUM;
		gyro_data_avg[2] = gyro_data_avg[2]/LSM6DSOW_CALI_NUM;

		lsm6dsow_cali[0]  = 0-acc_data_avg[0];
		lsm6dsow_cali[1]  = 0-acc_data_avg[1];
		lsm6dsow_cali[2]  = -LSM6DSOW_LSB_1G-acc_data_avg[2];

		lsm6dsow_cali_gyro[0] = 0-gyro_data_avg[0];
		lsm6dsow_cali_gyro[1] = 0-gyro_data_avg[1];
		lsm6dsow_cali_gyro[2] = 0-gyro_data_avg[2];


        ret = watch_storage_set(STORAGE_SENSOR_DATA,lsm6dsow_cali,sizeof(lsm6dsow_cali));
		if(ret != 0)
        {
            printf("lsm6dsow_cali_store failed\n");
		}
		ret = watch_storage_set(STORAGE_GYRO_DATA,lsm6dsow_cali_gyro,sizeof(lsm6dsow_cali_gyro));
		if(ret != 0)
        {
            printf("lsm6dsow_cali_gyro_store failed\n");
		}
        return ret;

}


int gsensor_init(void)
{
    int ret = 0;
    ret = lsm6dso_init();
    return ret;
}
