/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: user api for sample dpm fir module
 * Author: @CompanyNameTag
 */

#ifndef __DPM_FIR_API_H__
#define __DPM_FIR_API_H__

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    EXT_ERR_DPM_FIR_INVALID_PARAM = 0x80005400,
    EXT_ERR_DPM_FIR_INVALID_ID = 0x80005401,
    EXT_ERR_DPM_FIR_INVALID_PORT = 0x80005402,
    EXT_ERR_DPM_FIR_INVALID_HANDLE = 0x80005403,
    EXT_ERR_DPM_FIR_MEM_ALLOC = 0x80005404,
    EXT_ERR_DPM_FIR_MEM_COPY = 0x80005405,
    EXT_ERR_DPM_FIR_MEM_OVERLOAD = 0x80005406,
    EXT_ERR_DPM_FIR_MEM_EXCEPTION = 0x80005407,
    EXT_ERR_DPM_FIR_BUF_EMPTY = 0x80005408,
    EXT_ERR_DPM_FIR_BUF_FULL = 0x80005409,
    EXT_ERR_DPM_FIR_BUF_UNDERFLOW = 0x8000540a,
    EXT_ERR_DPM_FIR_BUF_OVERFLOW = 0x8000540b,
    EXT_ERR_DPM_FIR_INST_FULL = 0x8000540c,
    EXT_ERR_DPM_FIR_INST_BUSY = 0x8000540d,
    EXT_ERR_DPM_FIR_INST_STOP = 0x8000540e,
    EXT_ERR_DPM_FIR_OUTPUT_FULL = 0x8000540f,
    EXT_ERR_DPM_FIR_INIT_FAILED = 0x80005410,
    EXT_ERR_DPM_FIR_DEINIT_FAILED = 0x80005411,
    EXT_ERR_DPM_FIR_OPEN_FAILED = 0x80005412,
    EXT_ERR_DPM_FIR_CLOSE_FAILED = 0x80005413,
    EXT_ERR_DPM_FIR_IOCTL_FAILED = 0x80005414,
    EXT_ERR_DPM_FIR_NOT_INIT = 0x80005415,
    EXT_ERR_DPM_FIR_NOT_SUPPORTED = 0x80005416,
    EXT_ERR_DPM_FIR_NOT_AVAILABLE = 0x80005417,
    EXT_ERR_DPM_FIR_FRAME_CORRUPT = 0x80005418,
    EXT_ERR_DPM_FIR_NULL_PTR = 0x80005419,
    EXT_ERR_DPM_FIR_NOT_ENOUGH_DATA = 0x8000541a,
} ext_dpm_fir_errno;

/**
 * @if Eng
 * @brief  Defines the information about fir data frame.
 * @else
 * @brief  定义FIR数据帧信息
 * @endif
 */
typedef struct {
    double *data_buffer;       /*!< @if Eng Pointer to the buffer for storing the data
                                    @else   data数据缓冲指针 @endif */
    unsigned int data_samples; /*!< @if Eng Number of sampling points of the data.
                                    @else   数据采样点数量 @endif */
} dpm_fir_frame;

/**
 * @if Eng
 * @brief  Defines the handle of dpm fir module.
 * @else
 * @brief  定义FIR模块句柄
 * @endif
 */
typedef struct dpm_fir_inst *dpm_fir_handle;

/**
 * @if Eng
 * @brief  fir module init.
 * @param  [in]  fir fir handle
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  fir模块初始化
 * @param  [in]  fir fir实例句柄
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int dpm_fir_init(dpm_fir_handle *fir);

/**
 * @if Eng
 * @brief  fir module deinit.
 * @param  [in]  fir fir handle
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  fir模块去初始化
 * @param  [in]  fir fir实例句柄
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int dpm_fir_deinit(const dpm_fir_handle fir);

/**
 * @if Eng
 * @brief  Set fir filter coefficient
 * @param  [in]  fir fir handle
 * @param  [in]  coef_a input const config data of fir
 * @param  [in]  coef_b input const config data of fir
 * @param  [in]  input_data_samples input data samples of fir
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  设置fir实例配置
 * @param  [in]  fir fir实例句柄
 * @param  [in]  coef_a 输入参量数组
 * @param  [in]  coef_b 输入参量数组
 * @param  [in]  input_data_samples 输入数据采样点数
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int dpm_fir_set_coef(const dpm_fir_handle fir,
                     const dpm_fir_frame *coef_a,
                     const dpm_fir_frame *coef_b,
                     unsigned int input_data_samples);

/**
 * @if Eng
 * @brief  Get fir calculation output of input data
 * @param  [in]  fir fir handle
 * @param  [in]  in_frame input raw data of fir proc
 * @param  [out] out_frame output data of fir process
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  fir滤波运算
 * @param  [in]  fir fir实例句柄
 * @param  [in]  in_frame 原始采样数据
 * @param  [out] out_frame 输出滤波后数据
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int dpm_fir_proc(const dpm_fir_handle fir,
                 const dpm_fir_frame *in_frame,
                 dpm_fir_frame *out_frame);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DPM_FIR_API_H__ */
