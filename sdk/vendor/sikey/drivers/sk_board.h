#ifndef _SK_BOARD_H_
#define _SK_BOARD_H_
#include "stdlib.h"
#include "stdio.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif



#define GSENSOR_ERR_STATE  0x01
#define FG_ERR_STATE       0x02
#define LCD_ERR_STATE      0x08
#define TP_ERR_STATE       0x10

typedef enum
{
    DEVICE_GSENSOR = 0,
    DEVICE_FG,
    DEVICE_BAROMETER,
    DEVICE_WIFI,
    DEVICE_GPS,
    DEVICE_CHG,
    DEVICE_MAX
}device_type;

typedef enum
{
    STATION_BC = 0,
    STATION_PT,
    STATION_MAX
}station_type;

typedef enum {
    GPS_MANUFACTURER_NONE=0,
    GPS_MANUFACTURER_TD,
    GPS_MANUFACTURER_ZKW,
    GPS_MANUFACTURER_MAX
}WATCH_GPS_MANUFACTURER;

extern int sk_board_init(void);

extern uint32_t get_devive_state(void);
extern void set_device_state(device_type sensor_type);

uint8_t sk_nv_write(uint8_t sectionId, uint32_t len, uint8_t *data);
uint8_t sk_nv_read(uint8_t sectionId, uint32_t len, uint8_t *data);
//void  get_misc_params_from_nv(MISC_PARAMS_TYPE type,void *data);
//void set_misc_params_to_nv(MISC_PARAMS_TYPE type,void *data);
#ifdef __cplusplus
}
#endif
#endif
