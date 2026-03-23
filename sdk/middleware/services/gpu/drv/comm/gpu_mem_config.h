/*
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description : gpu sram config file
 */

#ifndef COMM_GPU_MEM_CONFIG_H
#define COMM_GPU_MEM_CONFIG_H

#include "td_base.h"
#include "psram.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define GPU_SRAM_START      0x60240000
#define GPU_SRAM_END        0x6039FFFF
#define GPU_SRAM_SIZE       (GPU_SRAM_END + 1 - GPU_SRAM_START)

#define GPU_SRAM_VIR_START  0x61240000
#define GPU_SRAM_VIR_END    0x61FFFFFF

#define GPU_SRAM_VIR_FB0    0x61240000
#define GPU_SRAM_VIR_FB1    0x61A40000

#define GPU_REG_LUT_ADDR    0x603E0000
#define GPU_REG_LUT_SIZE    0x3FFF

#ifdef SUPPORT_GPU_GMMU
#define GPU_SRAM_PRE_SIZE   (1 * 1024) /* 1K for vau ip tmp dcmp */
#define GPU_SRAM_PRE_ADDR   (GPU_SRAM_END + 1 - GPU_SRAM_PRE_SIZE) /* 0x6039FC00 ~ 0x6039FFFF */

#define GPU_SRAM_DRAW_SIZE  (127 * 1024) /* 127K for cmd/data */
#define GPU_SRAM_DRAW_ADDR  (GPU_SRAM_PRE_ADDR - GPU_SRAM_DRAW_SIZE) /* 0x60380000 ~ 0x6039FC00 */

#define GPU_SRAM_NODE_SIZE  (80 * 1024) /* 80K for node/handle */
#define GPU_SRAM_NODE_ADDR  (GPU_SRAM_DRAW_ADDR - GPU_SRAM_NODE_SIZE) /* 0x6036C000 ~ 0x60380000 */

#define GPU_SRAM_CMD_SIZE   (GPU_SRAM_PRE_SIZE + GPU_SRAM_DRAW_SIZE + GPU_SRAM_NODE_SIZE)

#define GPU_PSRAM_SIZE      (PSRAM_DISP_VAU_NODE_LENGTH)
#define GPU_PSRAM_ADDR      (PSRAM_DISP_VAU_NODE_ADDR)
#else
#define GPU_SRAM_PRE_SIZE   (1 * 1024) /* 1K for vau ip tmp dcmp */
#define GPU_SRAM_PRE_ADDR   (GPU_SRAM_END + 1 - GPU_SRAM_PRE_SIZE)   /* 0x6039FC00 ~ 0x6039FFFF */

#define GPU_SRAM_NODE_SIZE  (57 * 1024) /* 57K for node/handle */
#define GPU_SRAM_NODE_ADDR  (GPU_SRAM_PRE_ADDR - GPU_SRAM_NODE_SIZE) /* 0x60391800 ~ 0x6039FC00 */

#define GPU_SRAM_CMD_SIZE   (GPU_SRAM_PRE_SIZE + GPU_SRAM_NODE_SIZE)

#define GPU_SRAM_DRAW_SIZE  (256 * 1024) /* 256K for cmd/data */
#define GPU_SRAM_DRAW_ADDR  (PSRAM_DISP_VAU_NODE_ADDR) /* PSRAM */

#define GPU_PSRAM_SIZE      (PSRAM_DISP_VAU_NODE_LENGTH - GPU_SRAM_DRAW_SIZE)
#define GPU_PSRAM_ADDR      (GPU_SRAM_DRAW_ADDR + GPU_SRAM_DRAW_SIZE)
#endif

td_u32 gpu_mem_get_cmd_size(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* COMM_GPU_MEM_CONFIG_H */
