#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include "http_api.h"
#include "cJSON.h"

extern unsigned int uploading_log;
void paqichong_save_log(const char *fmt, ...);
void handle_debug_data(cJSON *data);

extern int32_t gsensor_calibrate(void);
extern void gsensor_read_data(int16_t *acc_data, int16_t *gyro_data);