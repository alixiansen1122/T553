#include "gsensor/gsensor.h"
#include "i2c.h"
#include "i2c_porting.h"
#include "lsm6dso_reg.h"
#include "tcxo.h"
#include "sk_board.h"
#include "gpio.h"
#include "timer.h"
#include "paqichong_data_process.h"

uint8_t lsm6dso_i2c_bus = I2C_BUS_1;
stmdev_ctx_t dev_ctx={0};

static int16_t data_raw_acceleration[3];
static int16_t data_raw_angular_rate[3];
static int16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
bool g_sleep_flag = false;
#define GPIO_INT_1 S_AGPIO_R7
#define GPIO_INT_2 S_AGPIO_R8

static void platform_delay(uint32_t ms)
{
    uapi_tcxo_delay_ms(ms);
}

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
    int32_t ret = 0;
    uint8_t retry_count = 0;
    const uint8_t max_retries = 3;

    i2c_data_t data = {0};
    uint8_t *send_data = NULL;

    if (len == 0 || bufp == NULL) {
        GSENSOR_PRINT("platform_write: invalid parameters\n");
        return -1;
    }

    send_data = malloc(len + 1);
    if (send_data == NULL) {
        GSENSOR_PRINT("platform_write: malloc failed\n");
        return -1;
    }

    memset(send_data, 0, len + 1);
    memcpy(send_data, &reg, 1);
    memcpy(send_data + 1, bufp, len);

    data.send_buf = send_data;
    data.send_len = len + 1;
    data.receive_buf = NULL;
    data.receive_len = 0;

    for (retry_count = 0; retry_count < max_retries; retry_count++) {
        ret = uapi_i2c_master_write(*((i2c_bus_t *)handle), LSM6DSO_I2C_ADD_H, &data);
        if (ret == 0) {
            break;
        }

        GSENSOR_PRINT("lsm6dso write reg 0x%x failed! retry %d, ret = %d\n",
                     reg, retry_count + 1, ret);

        if (retry_count < max_retries - 1) {
            platform_delay(1);
        }
    }

    free(send_data);
    return ret;
}

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
    int32_t ret = 0;
    uint8_t retry_count = 0;
    const uint8_t max_retries = 3;

    i2c_data_t data = {0};

    if (bufp == NULL || len == 0) {
        GSENSOR_PRINT("platform_read: invalid parameters\n");
        return -1;
    }

    data.send_buf = &reg;
    data.send_len = 1;
    data.receive_buf = bufp;
    data.receive_len = len;

    for (retry_count = 0; retry_count < max_retries; retry_count++) {
        ret = uapi_i2c_master_writeread(*((i2c_bus_t *)handle), LSM6DSO_I2C_ADD_H, &data);
        if (ret == 0) {
            break;
        }

        GSENSOR_PRINT("lsm6dso read reg 0x%x failed! retry %d, ret = %d\n",
                     reg, retry_count + 1, ret);

        if (retry_count < max_retries - 1) {
            platform_delay(1);
        }
    }

    return ret;
}

int lsm6dso_get_chip_id(uint8_t *whoamI)
{
    int8_t ret = 0;
    int8_t retry = 0;

    while (retry++ < 5)
    {
        ret = lsm6dso_device_id_get(&dev_ctx, whoamI);
        if (ret != 0)
        {
            platform_delay(20);
            continue;
        }
        break;
    }
    return ret;
}
static void smd_isr_handler(pin_t pin, uintptr_t param)
{
    unused(param);
    unused(pin);

    GSENSOR_PRINT("gpio isr value is %d\n", uapi_gpio_get_val(GPIO_INT_2));
    if(g_sleep_flag == true)
    {
        g_sleep_flag = false;
        paqichong_event_write(PAQICHONG_START_MEASURE_TIMER, 0);
    }
}
int lsm6dso_init(void)
{
    int ret = 0, retry_count = 0;
    uint8_t whoamI = 0, rst = 1;
    uint8_t count = 0;
    lsm6dso_pp_od_t pp_od;
    lsm6dso_h_lactive_t h_lactive;
    lsm6dso_pin_int1_route_t int1_route;
    lsm6dso_pin_int2_route_t int2_route;
    lsm6dso_int_mode_t int_mode = {0,1,1};
    lsm6dso_emb_sens_t emb_sens;
    /* Initialize mems driver interface */
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.mdelay = platform_delay;
    dev_ctx.handle = &lsm6dso_i2c_bus;

    /* Check device ID */
    ret = lsm6dso_get_chip_id(&whoamI);
    if (ret != 0)
    {
        GSENSOR_PRINT("read LSM6DSO_ID failed\n");
        return ret;
    }
    if (whoamI != LSM6DSO_ID)
    {
        GSENSOR_PRINT("LSM6DSO_ID=0x%x,whoamI=0x%x\n", LSM6DSO_ID, whoamI);
        return -1;
    }
    set_device_state(DEVICE_GSENSOR);
    GSENSOR_PRINT("LSM6DSO_ID=0x%x,whoamI=0x%x\n", LSM6DSO_ID, whoamI);
    /* Restore default configuration */
    lsm6dso_reset_set(&dev_ctx, PROPERTY_ENABLE);
    while (rst && (retry_count < 5))
    {
        lsm6dso_reset_get(&dev_ctx, &rst);
        retry_count++;
        platform_delay(2);
    }
    if (rst)
    {
        GSENSOR_PRINT("LSM6DSO reset failed\n");
        return -1;
    }

    /* Disable I3C interface */
    ret = lsm6dso_i3c_disable_set(&dev_ctx, LSM6DSO_I3C_DISABLE);
    if (ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_i3c_disable_set failed\n");
        return -1;
    }
    /* Enable Block Data Update */
    ret = lsm6dso_block_data_update_set(&dev_ctx, PROPERTY_DISABLE);
    if (ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_block_data_update_set failed\n");
        return -1;
    }
    /* Set Output Data Rate */
    ret = lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_26Hz);
    if (ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_xl_data_rate_set failed\n");
        return -1;
    }
    ret = lsm6dso_gy_data_rate_set(&dev_ctx, LSM6DSO_GY_ODR_26Hz);
    if (ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_gy_data_rate_set failed\n");
        return -1;
    }
    /* Set full scale */
    ret = lsm6dso_xl_full_scale_set(&dev_ctx, LSM6DSO_16g);
    if (ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_xl_full_scale_set failed\n");
        return -1;
    }
    ret = lsm6dso_gy_full_scale_set(&dev_ctx, LSM6DSO_2000dps);
    if (ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_gy_full_scale_set failed\n");
        return -1;
    }
    /* Configure filtering chain(No aux interface)
     * Accelerometer - LPF1 + LPF2 path
     */
    ret = lsm6dso_xl_hp_path_on_out_set(&dev_ctx, LSM6DSO_LP_ODR_DIV_100);
    if (ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_xl_hp_path_on_out_set failed\n");
        return -1;
    }
    ret = lsm6dso_xl_filter_lp2_set(&dev_ctx, PROPERTY_ENABLE);
    if (ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_xl_filter_lp2_set failed\n");
        return -1;
    }
    ret = lsm6dso_xl_hp_path_internal_set(&dev_ctx, LSM6DSO_USE_HPF);
    if (ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_xl_hp_path_internal_set failed\n");
        return -1;
    }

    ret = lsm6dso_pin_mode_get(&dev_ctx, &pp_od);
    if(ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_pin_mode_get failed\n");
        return -1;
    }
    GSENSOR_PRINT("pp_od:%d\n", pp_od);
    ret = lsm6dso_pin_polarity_get(&dev_ctx, &h_lactive);
    if(ret != 0)
    {
        GSENSOR_PRINT("lsm6dso_pin_polarity_get failed\n");
        return -1;
    }
    GSENSOR_PRINT("h_lactive:%d\n", h_lactive);


     ret = lsm6dso_wkup_dur_set(&dev_ctx, 0x01);
     if(ret != 0)
     {
         GSENSOR_PRINT("lsm6dso_wkup_dur_set failed\n");
         return -1;
     }

     ret = lsm6dso_wkup_threshold_set(&dev_ctx, 0x1);
     if(ret != 0)
     {
         GSENSOR_PRINT("lsm6dso_wkup_threshold_set failed\n");
         return -1;
     }

     /* Enable interrupt generation on Inactivity INT2 pin */
     ret = lsm6dso_pin_int2_route_get(&dev_ctx,NULL, &int2_route);
     if(ret != 0)
     {
         GSENSOR_PRINT("lsm6dso_pin_int1_route_get failed\n");
         return -1;
     }
     int2_route.wake_up = PROPERTY_ENABLE;
     ret = lsm6dso_pin_int2_route_set(&dev_ctx,NULL, int2_route);
     if(ret != 0)
     {
         GSENSOR_PRINT("lsm6dso_pin_int1_route_set failed\n");
         return -1;
     }

     uapi_pin_set_mode(GPIO_INT_2, (pin_mode_t)HAL_PIO_FUNC_GPIO);
     uapi_gpio_set_dir(GPIO_INT_2, HAL_GPIO_DIRECTION_INPUT);
     printf("gpio isr value is %d\n", uapi_gpio_get_val(GPIO_INT_2));
     if(uapi_gpio_register_isr_func(GPIO_INT_2, GPIO_INTERRUPT_RISING_EDGE, smd_isr_handler)==ERRCODE_SUCC)
     {
         GSENSOR_PRINT("register gpio isr success\n");
     }

    return 0;
}
bool get_sleep_flag(void)
{
    return g_sleep_flag;
}

void lsm6dso_read_sensor_data(int16_t *acc_data, int16_t *gyro_data)
{
    uint8_t reg;
    lsm6dso_all_sources_t all_sources = {0};
    static uint8_t sleep_count = 0;

    lsm6dso_all_sources_get(&dev_ctx,&all_sources);
	GSENSOR_PRINT("sleep status:%d\n", all_sources.sleep_state);

    if(all_sources.sleep_state == 1)
    {
      sleep_count++;

    }
    else
    {
      sleep_count = 0;
      g_sleep_flag = false;
    }

    if(sleep_count > 100)
    {
        g_sleep_flag = true;
        sleep_count = 0;
        paqichong_event_write(PAQICHONG_STOP_MEASURE_TIMER, 0);
    }
    /* Read output only if new xl value is available */
    lsm6dso_xl_flag_data_ready_get(&dev_ctx, &reg);
    GSENSOR_PRINT("status accel reg = 0x%x\r\n",reg);
    if (reg)
    {
        /* Read acceleration field data */
        memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
        lsm6dso_acceleration_raw_get(&dev_ctx, data_raw_acceleration);

        GSENSOR_PRINT("Acceleration [mg]:%d\t%d\t%d\r\n",
                      data_raw_acceleration[0], data_raw_acceleration[1], data_raw_acceleration[2]);
       acc_data[0] = data_raw_acceleration[0];
       acc_data[1] = -data_raw_acceleration[1];
       acc_data[2] = -data_raw_acceleration[2];
       GSENSOR_PRINT("Acceleration [mg]:%d\t%d\t%d\r\n",
                      acc_data[0], acc_data[1], acc_data[2]);

    }
    /* Fill in acc data */


    lsm6dso_gy_flag_data_ready_get(&dev_ctx, &reg);
    GSENSOR_PRINT("status gyro reg = 0x%x\r\n",reg);
    if (reg)
    {
        /* Read angular rate field data */
        memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
        lsm6dso_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);

        GSENSOR_PRINT("Angular rate [mdps]:%d\t%d\t%d\r\n",
                      data_raw_angular_rate[0], data_raw_angular_rate[1], data_raw_angular_rate[2]);
       gyro_data[0] = data_raw_angular_rate[0];
       gyro_data[1] = -data_raw_angular_rate[1];
       gyro_data[2] = -data_raw_angular_rate[2];
       GSENSOR_PRINT("Angular rate [mdps]:%d\t%d\t%d\r\n",
                      gyro_data[0], gyro_data[1], gyro_data[2]);
    }

    /* Fill in gyro data */

}
