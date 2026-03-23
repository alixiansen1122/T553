/**
 * Copyright (c) @CompanyNameMagicTag 2025-2025. All rights reserved. \n
 *
 * Description: Provides gsensor driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-05-20, Create file. \n
 */
#ifndef GSENSOR_H
#define GSENSOR_H

#include "gsensor_queue.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum gensor_lp_odr {
    POWER_DOWN,
    ODR_1_6_HZ,
    ODR_12_5_HZ,
    ODR_25_HZ,
    ODR_50_HZ,
    ODR_100_HZ,
    ODR_200_HZ,
    ODR_200_HZ_2,
    ODR_200_HZ_3,
    ODR_200_HZ_4,
    LP_ODR_MAX
} gensor_lp_odr_t;

typedef enum gensor_bandwidth {
    ODR_DIVIDED_BY_2,
    ODR_DIVIDED_BY_4,
    ODR_DIVIDED_BY_10,
    ODR_DIVIDED_BY_20,
    BAND_WIDTH_MAX
} gensor_bandwidth_t;

typedef enum gensor_fullscale {
    FULLSCALE_2G,
    FULLSCALE_4G,
    FULLSCALE_8G,
    FULLSCALE_16G
} gensor_fullscale_t;

/**
 * @brief  gsensor driver init.
 * @param  [in]  sample_freq 采样频率 参考 @ref gensor_lp_odr_t.
 * @param  [in]  bandwidth 带宽 参考 @ref gensor_bandwidth_t.
 * @return ERRCODE_SUCC 成功, Other 失败，参考 @ref errcode_t.
 */
errcode_t gsensor_driver_init(gensor_lp_odr_t sample_freq, gensor_bandwidth_t bandwidth);

/**
 * @brief  gsensor driver deinit.
 * @return ERRCODE_SUCC 成功, Other 失败，参考 @ref errcode_t.
 */
errcode_t gsensor_driver_deinit(void);

/**
 * @brief  gsensor driver get acceleration data.
 * @param  [in]  data 加速度数据 参考 @ref gsensor_data_t.
 * @return ERRCODE_SUCC 成功, Other 失败，参考 @ref errcode_t.
 */
errcode_t gsensor_driver_acc_data_get(gsensor_data_t *data);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif