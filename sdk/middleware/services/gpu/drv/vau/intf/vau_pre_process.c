/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: vau pre process file
 */

#include "vau_pre_process.h"
#include "tcxo.h"
#include "vau_dfx.h"
#include "drv_gfx_port.h"
#include "gpu_mem_config.h"

#define VAU_PRE_SRC_SIZE  0x100
#define VAU_PRE_DST_SIZE  0x100
#define VAU_PRE_NODE_SIZE 0x200

#define VAU_PRE_PROC_SRC  GPU_SRAM_PRE_ADDR /* 0x6039FC00 ~ 0x6039FFFF */
#define VAU_PRE_PROC_DST  (VAU_PRE_PROC_SRC + VAU_PRE_SRC_SIZE)
#define VAU_PRE_NODE_ADDR (VAU_PRE_PROC_DST + VAU_PRE_DST_SIZE)

static td_u32 g_success = 0;
static td_u32 g_failure = 0;

static const td_u32 g_data_tmp[4] = {
    0xd2678153, 0x002021a0, 0x0f8003f8, 0x52492800
};

static const td_u32 g_node_tmp[92] = { // VAU IP节点寄存器个数92个
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x88003000, VAU_PRE_PROC_SRC, 0x00000010,
    0x00030005, 0x00000000, 0x00030005, 0x00000000,
    0x00000000, 0x00000000, 0x060803ff, 0x00255010,
    0x00000000, 0x00000000, 0x80000000, VAU_PRE_PROC_DST,
    0x00000040, 0x00030005, 0x00000000, 0x00030005,
    0x00000000, 0x80200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000001, 0x00000f8e, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x80000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static td_void vau_ip_reset(td_bool reset)
{
    td_u32 value;
    td_u32 retry = 0;

    if (reset) {
        uapi_reg_setbit(0x52000204, 2); // 2: VAU IP软复位
    } else {
        uapi_reg_clrbit(0x52000204, 2); // 2: VAU IP解软复位
    }

    while (retry++ < 100) { // 100 retry cnt
        uapi_reg_read(0x5601152c, value);
        if (value == reset) {
            break;
        }
    }

    if (retry >= 100) { // 100 retry cnt
        vau_err("vau_ip_reset fail!\n");
    }

    return;
}

td_void vau_ip_dcmp_tmp(td_void)
{
    osal_irq_disable(VAU_INT_NUM);

    uapi_reg_setbit(0x52000200, 7); // 7: VIDEO 门控打开
    uapi_reg_setbit(0x52000200, 3); // 3: VIDEO APB门控使能

    uapi_reg_setbit(0x56010000, 4); // 4: VAU 内部时钟打开
    uapi_reg_setbit(0x56010000, 3); // 3: VAU 内部时钟打开

    vau_ip_reset(TD_FALSE);

    td_s32 ret = memcpy_s((void *)VAU_PRE_PROC_SRC,  VAU_PRE_SRC_SIZE, (void *)g_data_tmp, sizeof(g_data_tmp));
    if (ret != EOK) {
        vau_err("memcpy_s to VAU_PRE_PROC_SRCfailure! ret = %d\n", ret);
        return;
    }
    ret = memcpy_s((void *)VAU_PRE_NODE_ADDR, VAU_PRE_NODE_SIZE, (void *)g_node_tmp, sizeof(g_node_tmp));
    if (ret != EOK) {
        vau_err("memcpy_s to VAU_PRE_NODE_ADDR failure! ret = %d\n", ret);
        return;
    }

    uapi_reg_write(0x560110fc, (td_u32)VAU_PRE_NODE_ADDR); // 配置链表首地址
    uapi_reg_write(0x56011500, 0x1);                       // 启动逻辑

    // 给临时解压100us的处理时间
    uapi_tcxo_delay_us(100);

    td_u32 interrupt;
    uapi_reg_read(0x56011504, interrupt);
    if ((interrupt & 0x8) != 0) {
        g_success++;
    } else {
        g_failure++;
    }

    // 临时解压执行完毕后做一次复位
    vau_ip_reset(TD_TRUE);
    vau_ip_reset(TD_FALSE);

    osal_irq_enable(VAU_INT_NUM);

    vau_dbg("vau_ip_dcmp_tmp sucess[%d] failure[%d]!\n", g_success, g_failure);

    return;
}
