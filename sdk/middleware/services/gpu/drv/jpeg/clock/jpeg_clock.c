/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg clock
 */

#include "jpeg_clock.h"
#include "jpeg_hal.h"
#include "crg_reg_define.h"

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

/*-------------------------------- par define ---------------------------------------------*/

/*-------------------------------- func release -------------------------------------------*/

td_void jpeg_clock_open(td_void)
{
    volatile u_video_clken_cfg video_clken_cfg;
    uapi_reg_read(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
    video_clken_cfg.bits.jpgd_clken = 1;
    uapi_reg_write(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
}

td_void jpeg_clock_close(td_void)
{
    volatile u_video_clken_cfg video_clken_cfg;
    uapi_reg_read(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
    video_clken_cfg.bits.jpgd_clken = 0;
    uapi_reg_write(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
}

td_void jpeg_clock_reset(td_void)
{
    volatile u_video_srst_reg video_srst_reg;
    uapi_reg_read(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
    video_srst_reg.bits.jpg_srst_req = 1;
    uapi_reg_write(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
    return;
}

td_void jpeg_clock_dereset(td_void)
{
    volatile u_video_srst_reg video_srst_reg;
    uapi_reg_read(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
    video_srst_reg.bits.jpg_srst_req = 0;
    uapi_reg_write(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
    return;
}
