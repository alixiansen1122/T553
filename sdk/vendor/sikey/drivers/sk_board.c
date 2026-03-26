#include "sk_board.h"
#include "thread_init.h"
#include "tcxo.h"
#include "log_common.h"
#include "log_def.h"
#include "log_uart.h"
#ifdef __LITEOS__
#include "los_status.h"
#endif
#include "pinctrl_porting.h"
#include "gpio.h"
#include "pinctrl.h"
#include "hal_gpio.h"
#include <time.h>
#include <sys/time.h>
#include "common.h"
#include "key_id.h"
#include "fg/sh366102/sh366102.h"
#include "gsensor/gsensor.h"
#include "barometer/barometer.h"
#include "led/rgb_led.h"
#include "sk_audio.h"
#include "modem_cmd.h"
#include "mqtt_service.h"

uint32_t device_state = 0;




uint32_t get_device_state(void)
{
    return device_state;
}

void set_device_state(device_type sensor_type)
{
    device_state =  device_state |(1<<sensor_type);
}

uint8_t sk_nv_write(uint8_t sectionId, uint32_t len, uint8_t * data) {
    switch (sectionId) {
    case NV_SECTION_SK_DEVICE_INFO:
        uapi_nv_write(NV_ID_SK_DEVICE_INFO, data, len);
        break;
    case NV_SECTION_SENSOR_CALI_INFO:
        uapi_nv_write(NV_ID_SK_SENSOR_CALI, data, len);
        break;
    case NV_SECTION_GYRO_CALI_INFO:
        uapi_nv_write(NV_ID_MODEL_VERSION_INFO, data, len);
        break;
    default:
        break;
    }
}

uint8_t sk_nv_read(uint8_t sectionId, uint32_t len, uint8_t *data) {
    uint16_t real_len = 0;
    switch (sectionId){
    case NV_SECTION_SK_DEVICE_INFO:
        uapi_nv_read(NV_ID_SK_DEVICE_INFO, len, &real_len, data);
        break;
    case NV_SECTION_SENSOR_CALI_INFO:
        uapi_nv_read(NV_ID_SK_SENSOR_CALI, len, &real_len, data);
        break;
    case NV_SECTION_GYRO_CALI_INFO:
        uapi_nv_read(NV_ID_MODEL_VERSION_INFO, len, &real_len, data);
        break;

    default:
        break;
    }
}

int i2c_bus_init(void)
{
    uapi_pin_set_mode(I2C_1_SCL_PIN, PIN_MODE_1);
    uapi_pin_set_mode(I2C_1_SDA_PIN, PIN_MODE_1);

    if (ERRCODE_SUCC != uapi_i2c_master_init(I2C_BUS_1, 400000, I2C_SPEED_MODE_FS))
    {
        printf("i2c init bus failed!\n");
        return -1;
    }
    uapi_tcxo_delay_ms(10);
    return 0;
}

int sk_board_init(void)
{
    int ret = 0;

    /*i2c bus init*/
    ret = i2c_bus_init();
    if (0 != ret) {
       printf("i2c init bus failed!\n");
    }
    ret = gsensor_init();
    if (0 != ret) {
        printf("gsensor init failed!\n");
    }
#if 1
    ret = barometer_init();
    if (0 != ret) {
        printf("barometer init failed!\n");
    }
#endif
    fg_init();
    rgb_led_init();
    //paqichong_tack_init();
    // sk_audio_task_init();
    modem_callback_init();
    mqtt_service_init();
    return ret;
}
