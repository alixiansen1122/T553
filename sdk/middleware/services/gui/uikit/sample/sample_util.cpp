/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: sample utils
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */
#include "debug_print.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/rect.h"
#include "mbedtls/sha256.h"
#include "graphic_service.h"
#include "hals/display_dev.h"
#include "imgdecode/image_load.h"
#include "draw/draw_utils.h"
#include "gfx_utils/graphic_types.h"
#include "sample_util.h"

int CalcSampleUiHash(const char* viewId)
{
    GraphicService::GetInstance()->PostGraphicEvent([] {
        OHOS::RootView::GetInstance()->Invalidate();
    });

    GraphicService::GetInstance()->PostGraphicEvent([viewId] {
        OHOS::ImageInfo info = {0};
        OHOS::Screen::GetInstance().GetScreenBitmapByRect(info, nullptr, 1.0, true); // 1.0: scale factor
        
        // mbedtls_sha256 方法要求32位
        unsigned char* hash = new unsigned char[32];
        mbedtls_sha256(info.data, info.dataSize, hash, 0);
        ImageCacheFree(info);

        // 65: 32*2+1，32位hash数组每位按照16进制输出长度为2,再加上一个结束符
        char* output =  new char[65];
        for (int i = 0; i < 32; i++) { // 32: 长度
            sprintf_s(output + (i * 2), 3, "%02x", hash[i]); // 2:位; 3:多一个结束符
        }
        PRINT("screen hash value of %s=[%s] \n", viewId, output);
        
        delete[] hash;
        delete[] output;
    });
   
    return 0;
}