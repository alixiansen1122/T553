/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: audio decoder handle
 * Author: Media Software Group
 * Create: 2017-12-22
 */

#include "codec_config.h"
#include "codec_plugin_manager.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "CODEC_CONFIG"

int32_t SetDevice(CodecBufferChannel *channelCtx, CodecType type, DirectionType dirType,
    const Param *params)
{
    MEDIA_HAL_UNUSED(dirType);
    CHK_NULL_RETURN(params, MEDIA_HAL_INVALID_PARAM, "Param is null");
    CHK_NULL_RETURN(channelCtx, MEDIA_HAL_INVALID_PARAM, "channelCtx is null");
    CHK_NULL_RETURN(channelCtx->pluginHandle, MEDIA_HAL_INVALID_PARAM, "pluginHandle is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon, MEDIA_HAL_INVALID_PARAM, "pluginCommon is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon->pfnSetParameter, MEDIA_HAL_INVALID_PARAM, "pfnSetParameter is null");
    channelCtx->isBindedMode = true;

    if (type == AUDIO_DECODER || type == VIDEO_DECODER) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec/vdec not support setting device");
        return MEDIA_HAL_ERR;
    }

    int32_t ret = channelCtx->pluginCommon->pfnSetParameter(channelCtx->pluginHandle, params);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "videoEncoder.pfnSetParameter failed Ret:0x%x", ret);
        return ret;
    }
    channelCtx->channelStarted = true;
    return MEDIA_HAL_OK;
}

int32_t CodecVencSetParameter(const CodecBufferChannel *channelCtx, CodecType type,
    const Param *params)
{
    CHK_NULL_RETURN(params, MEDIA_HAL_INVALID_PARAM, "Param arg is null");
    CHK_NULL_RETURN(channelCtx, MEDIA_HAL_INVALID_PARAM, "channelCtx is null");
    CHK_NULL_RETURN(channelCtx->pluginHandle, MEDIA_HAL_INVALID_PARAM, "pluginHandle is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon, MEDIA_HAL_INVALID_PARAM, "pluginCommon is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon->pfnSetParameter, MEDIA_HAL_INVALID_PARAM, "pfnSetParameter is null");

    if (type != VIDEO_ENCODER) {
        MEDIA_HAL_LOGE(MODULE_NAME, "only venc support setting device");
        return MEDIA_HAL_ERR;
    }

    int32_t ret = channelCtx->pluginCommon->pfnSetParameter(channelCtx->pluginHandle, params);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "videoEncoder.pfnSetParameter failed Ret:Ox%x", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
