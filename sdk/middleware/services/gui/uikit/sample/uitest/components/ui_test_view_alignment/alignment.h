/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */
#ifndef ALIGNMENT_H
#define ALIGNMENT_H

namespace OHOS {
enum AlignmentType : uint16_t {
    ALIGN_DEFAULT      = 0xFF00,    // 默认值，不对齐
    ALIGN_TOP_LEFT     = 0x0000,    // 控件左上角对齐到父容器内容区左上角
    ALIGN_TOP_MID      = 0x0001,    // 控件顶部中点对齐到父容器内容区顶部中点
    ALIGN_TOP_RIGHT    = 0x0002,    // 控件右上角对齐到父容器内容区右上角
    ALIGN_MID_LEFT     = 0x0010,    // 控件右下角对齐到父容器内容区右下角
    ALIGN_MID_MID      = 0x0011,    // 控件右边缘中点对齐到父容器内容区右边缘中点
    ALIGN_MID_RIGHT    = 0x0012,    // 控件中心对齐到父容器内容区中心
    ALIGN_BOTTOM_LEFT  = 0x0020,    // 控件左下角对齐到父容器内容区左下角
    ALIGN_BOTTOM_MID   = 0x0021,    // 控件底部中点对齐到父容器内容区底部中点
    ALIGN_BOTTOM_RIGHT = 0x0022,    // 控件右下角对齐到父容器内容区右下角
    // 外部对齐方式
    ALIGN_OUT_TOP_LEFT     = 0x1000,  // 控件左下角对齐到参考对象左上角
    ALIGN_OUT_TOP_MID      = 0x1001,  // 控件底部中点对齐到参考对象顶部中点
    ALIGN_OUT_TOP_RIGHT    = 0x1002,  // 控件右下角对齐到参考对象右上角
    ALIGN_OUT_BOTTOM_LEFT  = 0x1010,  // 控件左上角对齐到参考对象左下角
    ALIGN_OUT_BOTTOM_MID   = 0x1011,  // 控件顶部中点对齐到参考对象底部中点
    ALIGN_OUT_BOTTOM_RIGHT = 0x1012,  // 控件右上角对齐到参考对象右下角
    ALIGN_OUT_LEFT_TOP     = 0x1020,  // 控件右上角对齐到参考对象左上角
    ALIGN_OUT_LEFT_MID     = 0x1021,  // 控件右侧中点对齐到参考对象左侧中点
    ALIGN_OUT_LEFT_BOTTOM  = 0x1022,  // 控件右下角对齐到参考对象左下角
    ALIGN_OUT_RIGHT_TOP    = 0x1030,  // 控件左上角对齐到参考对象右上角
    ALIGN_OUT_RIGHT_MID    = 0x1031,  // 控件左侧中点对齐到参考对象右侧中点
    ALIGN_OUT_RIGHT_BOTTOM = 0x1032,  // 控件左下角对齐到参考对象右下角
};

// 对齐方式编码定义
#define ALIGN_BASE_MASK   0x00FF  // 基础对齐掩码
#define ALIGN_OUTER_MASK  0x1000  // 外部对齐标志
#define ALIGN_X_MASK      0x00F0  // X轴对齐掩码
#define ALIGN_Y_MASK      0x000F  // Y轴对齐掩码
}
#endif // ALIGNMENT_H