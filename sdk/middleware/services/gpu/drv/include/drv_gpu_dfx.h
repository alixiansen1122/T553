/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description : drv gpu dfx
 */

#ifndef DRV_INCLUDE_DRV_GPU_DFX_H
#define DRV_INCLUDE_DRV_GPU_DFX_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 vau_dfx_proc_read(int argc, const char *argv[]);
td_s32 dfx_get_proc_info(int argc, const char *argv[]);
td_s32 dfx_dpu_print(int argc, const char *argv[]);
td_s32 jpeg_dfx_proc_read(int argc, const char *argv[]);
td_s32 gmmu_dfx_print_proc_info(int argc, const char *argv[]);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_INCLUDE_DRV_GPU_DFX_H */
