#include "barometer.h"
#include "lqbp002ta.h"


float barometer_read_data(void)
{
    float pressure = 0.0;
    pressure = lqbp002ta_get_data();
    BAROMETER_PRINT("pressure = %f\n", pressure);
    return pressure;
}

int barometer_init(void)
{
    int ret = 0;
    lqbp002ta_init();
    if (lqbp002ta_i2c_err < 0) {
        BAROMETER_PRINT("lqbp002ta_init failed\n");
        return -1;
    }
    BAROMETER_PRINT("lqbp002ta_init success\n");
    return 0;
}

