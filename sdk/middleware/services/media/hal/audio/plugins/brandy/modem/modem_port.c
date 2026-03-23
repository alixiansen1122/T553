/*
* Copyright (c) @CompanyNameMagicTag. 2023-2023. All rights reserved.
* Description: modem port
* Author: Media Software Group
* Create: 2023-07-05
*/

#include "modem_port.h"

#include "modem_sub_port.h"

#include "plugin_common.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioModemPort"

typedef struct {
    /* port context */
    bool isOpened;
    /* sub port context */
    AudioHandle subPortHandle;
    /* sub port track context */
    AudioHandle subPortTrackHandle;
} ModemPortContext;

static int32_t ModemPortGetPortPluginCapability(const struct AudioPort *port,
    struct AudioPortCapability *capability)
{
    CHK_NULL_RETURN(port, MEDIA_HAL_INVALID_PARAM, "Input param port is null");
    CHK_NULL_RETURN(capability, MEDIA_HAL_INVALID_PARAM, "Input param capability is null");

    int32_t ret = AudioModemSubPortGetCapability(port, capability);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "get capability failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "%s get capability success", port->portName);
    return MEDIA_HAL_OK;
}

static int32_t ModemPortOpen(AudioHandle *portHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");

    int32_t ret;
    ModemPortContext *modemPortCtx = (ModemPortContext *)malloc(sizeof(ModemPortContext));
    if (modemPortCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    if (memset_s(modemPortCtx, sizeof(ModemPortContext), 0, sizeof(ModemPortContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE;
    }

    ret = AudioSDKInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKInit failed(0x%x)", ret);
        goto FREE;
    }

    ret = AudioModemSubPortCreate(&modemPortCtx->subPortHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioModemSubPortCreate failed(0x%x)", ret);
        goto FREE;
    }

    modemPortCtx->isOpened = true;
    *portHandle = modemPortCtx;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

FREE:
    free(modemPortCtx);
    return ret;
}

static int32_t ModemPortCreateTrack(AudioHandle portHandle, enum AudioPortPin audioPortPin,
    struct PortPluginAttr *portAttr, AudioHandle *trackHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    CHK_NULL_RETURN(trackHandle, MEDIA_HAL_INVALID_PARAM, "Input param trackHandle is null");
    CHK_NULL_RETURN(portAttr, MEDIA_HAL_INVALID_PARAM, "Input param portAttr is null");

    if (!AudioPluginIsValidPin(audioPortPin, "modem")) {
        MEDIA_HAL_LOGE(MODULE_NAME, "pin 0x%x is invalid", audioPortPin);
        return MEDIA_HAL_INVALID_PARAM;
    }

    ModemPortContext *modemPortCtx = (ModemPortContext *)portHandle;
    CHK_FAILED_RETURN(modemPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");

    int32_t ret = AudioModemSubPortCreateTrack(modemPortCtx->subPortHandle, portAttr, audioPortPin,
        &modemPortCtx->subPortTrackHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioModemSubPortCreateTrack failed");

    *trackHandle = modemPortCtx->subPortTrackHandle;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t ModemPortDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle)
{
    ModemPortContext *modemPortCtx = (ModemPortContext *)portHandle;
    CHK_FAILED_RETURN(modemPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");

    int32_t ret = AudioModemSubPortDestroyTrack(modemPortCtx->subPortHandle, trackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioModemSubPortDestroyTrack failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t ModemPortClose(AudioHandle portHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    ModemPortContext *modemPortCtx = (ModemPortContext *)portHandle;
    CHK_FAILED_RETURN(modemPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");

    int32_t ret = AudioSDKDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioSDKDeInit failed");

    ret = AudioModemSubPortDestroy(modemPortCtx->subPortHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioModemSubPortDestroy failed");

    modemPortCtx->isOpened = false;
    free(modemPortCtx);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

AudioPortPlugin g_modemPortPlugin = {
    .audioPortDir = PORT_OUT_IN,
    .audioPortPins = PIN_OUT_MODEM | PIN_IN_MODEM, // mask of PIN
    .GetPortPluginCapability = ModemPortGetPortPluginCapability,
    .Open = ModemPortOpen,
    .CreateTrack = ModemPortCreateTrack,
    .DestroyTrack = ModemPortDestroyTrack,
    .Close = ModemPortClose,
    .Invoke = NULL,
    .RegCallBack = NULL,
    .SetMute = NULL,
    .GetMute = NULL,
    .SetVolume = NULL,
    .GetVolume = NULL,
};

const AudioPortPlugin *GetModemPortPluginFuncs(AudioPluginType pluginType)
{
    MEDIA_HAL_UNUSED(pluginType);
    return &g_modemPortPlugin;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

