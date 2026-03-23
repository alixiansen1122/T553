/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: primary port
* Author: Media Software Group
* Create: 2021-09-28
*/

#include "primary_port.h"

#include "primary_sub_port_in.h"
#include "primary_sub_port_out.h"

#include "plugin_common.h"
#include "media_hal_common.h"
#ifdef AUDIO_HAID_SUPPORT
#include "soc_uapi_haid.h"
#include "soc_uapi_anc.h"
#endif
#ifdef AUDIO_SDK_SUPPORT
#include "soc_uapi_audio_sys.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioPrimaryPort"
#define PRIMARY_ATTR_BUF_LEN 25
#define PRIMARY_GET_CONFIG_ATTR_BUF_LEN 520
#define ANC_CONFIG_BUF_LEN 512
#define ANC_ATTR_BUF_LEN 530
typedef struct {
    /* port context */
    bool isOpened;
    /* sub port context */
    AudioHandle subPortInHandle;
    AudioHandle subPortOutHandle;
    AudioHandle subPortTrackHandle;
#ifdef AUDIO_HAID_SUPPORT
    td_handle haidHandle;
#endif
} PrimaryPortContext;

typedef int32_t (*InvokeFunc)(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr);

typedef struct {
    enum PluginInvokeId invokeId;
    InvokeFunc invoke;
} PrimaryInvoke;

#ifdef AUDIO_HAID_SUPPORT
static PluginHaidEventCallBack g_haidPrimaryEventCallBack = NULL;
#endif

static int32_t PrimarySubPortCreateTrack(PrimaryPortContext *portCtx, struct PortPluginAttr *portAttr, bool isMic)
{
    int32_t ret;
    if (isMic) {
        ret = AudioPrimarySubPortInCreateTrack(portCtx->subPortInHandle, portAttr, &portCtx->subPortTrackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortInCreateTrack failed");
    } else {
        ret = AudioPrimarySubPortOutCreateTrack(portCtx->subPortOutHandle, portAttr, &portCtx->subPortTrackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortOutCreateTrack failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t PrimarySubPortDestroyTrack(const PrimaryPortContext *portCtx, AudioHandle trackHandle)
{
    int32_t ret;
    AudioPortTrack *portTrack = (AudioPortTrack *)trackHandle;
    if (portTrack->audioPin == PIN_IN_MIC) {
        ret = AudioPrimarySubPortInDestroyTrack(portCtx->subPortInHandle, trackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortInDestroyTrack failed");
    } else {
        ret = AudioPrimarySubPortOutDestroyTrack(portCtx->subPortOutHandle, trackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortOutDestroyTrack failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t PrimaryPortGetPortPluginCapability(const struct AudioPort *port,
    struct AudioPortCapability *capability)
{
    CHK_NULL_RETURN(port, MEDIA_HAL_INVALID_PARAM, "Input param port is null");
    CHK_NULL_RETURN(capability, MEDIA_HAL_INVALID_PARAM, "Input param capability is null");

    int32_t ret;
    if (port->dir == PORT_IN) {
        ret = AudioPrimarySubPortInGetCapability(port, capability);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortInGetCapability failed");
    } else if (port->dir == PORT_OUT) {
        ret = AudioPrimarySubPortOutGetCapability(port, capability);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortOutGetCapability failed");
    } else {
        MEDIA_HAL_LOGE(MODULE_NAME, "port direction: 0x%x is invalid", port->dir);
        return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "%s get capability success", port->portName);
    return MEDIA_HAL_OK;
}

static int32_t PrimaryPortOpen(AudioHandle *portHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");

    int32_t ret;
    PrimaryPortContext *priPortCtx = (PrimaryPortContext *)malloc(sizeof(PrimaryPortContext));
    if (priPortCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    if (memset_s(priPortCtx, sizeof(PrimaryPortContext), 0, sizeof(PrimaryPortContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE;
    }

#ifdef AUDIO_SDK_SUPPORT

    ret = AudioSDKInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary audio sdk init failed(0x%x)", ret);
        goto FREE;
    }

#endif

    ret = AudioPrimarySubPortInCreate(&priPortCtx->subPortInHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPrimarySubPortInCreate failed(0x%x)", ret);
        goto FREE;
    }

    ret = AudioPrimarySubPortOutCreate(&priPortCtx->subPortOutHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPrimarySubPortOutCreate failed(0x%x)", ret);
        goto FREE;
    }

    priPortCtx->isOpened = true;
    *portHandle = priPortCtx;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

FREE:
    if (priPortCtx->subPortInHandle != NULL) {
        free(priPortCtx->subPortInHandle);
    }
    free(priPortCtx);
    return ret;
}

static int32_t PrimaryPortCreateTrack(AudioHandle portHandle, enum AudioPortPin audioPortPin,
    struct PortPluginAttr *portAttr, AudioHandle *trackHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    CHK_NULL_RETURN(trackHandle, MEDIA_HAL_INVALID_PARAM, "Input param trackHandle is null");
    CHK_NULL_RETURN(portAttr, MEDIA_HAL_INVALID_PARAM, "Input param portAttr is null");

    PrimaryPortContext *priPortCtx = (PrimaryPortContext *)portHandle;
    CHK_FAILED_RETURN(priPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");

    if (!AudioPluginIsValidPin(audioPortPin, "primary")) {
        MEDIA_HAL_LOGE(MODULE_NAME, "pin 0x%x is invalid", audioPortPin);
        return MEDIA_HAL_INVALID_PARAM;
    }

    bool isMic = (audioPortPin == PIN_IN_MIC) ? true : false;
    int32_t ret = PrimarySubPortCreateTrack(priPortCtx, portAttr, isMic);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "PrimarySubPortCreateTrack failed");

    *trackHandle = priPortCtx->subPortTrackHandle;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t PrimaryPortDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    CHK_NULL_RETURN(trackHandle, MEDIA_HAL_INVALID_PARAM, "Input param trackHandle is null");

    PrimaryPortContext *priPortCtx = (PrimaryPortContext *)portHandle;
    CHK_FAILED_RETURN(priPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");

    int32_t ret = PrimarySubPortDestroyTrack(priPortCtx, trackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "PrimarySubPortDestroyTrack failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t PrimaryPortClose(AudioHandle portHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    PrimaryPortContext *priPortCtx = (PrimaryPortContext *)portHandle;
    CHK_FAILED_RETURN(priPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");

    int32_t ret;

#ifdef AUDIO_SDK_SUPPORT
    ret = AudioSDKDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioSDKDeInit failed");
#endif

    ret = AudioPrimarySubPortInDestroy(priPortCtx->subPortInHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioPrimarySubPortInDestroy failed");

    ret = AudioPrimarySubPortOutDestroy(priPortCtx->subPortOutHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioPrimarySubPortOutDestroy failed");

    priPortCtx->isOpened = false;
    free(priPortCtx);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_HAID_SUPPORT
#ifdef AUDIO_SDK_SUPPORT
static int32_t PrimaryTwsSetMode(const PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary tws invokeAttr->value == NULL");
    uapi_audio_tws_mode twsMode = UAPI_AUDIO_TWS_MODE_MAX;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%d", &twsMode);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws mode sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_audio_set_tws_mode(twsMode);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws mode failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryTwsSetRole(const PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary tws invokeAttr->value == NULL");
    uapi_audio_tws_role twsRole = UAPI_AUDIO_TWS_ROLE_MAX;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%d", &twsRole);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws Role sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_audio_set_tws_role(twsRole);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws Role failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}
#endif

static td_s32 PrimaryPortHaidEventCallBack(td_handle haid, uapi_haid_event_type eventType,
    td_void *param, td_void *context)
{
    MEDIA_HAL_UNUSED(haid);
    CHK_NULL_RETURN(g_haidPrimaryEventCallBack, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    return g_haidPrimaryEventCallBack(eventType, param, context);
}

static int32_t PrimaryHaidCreate(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    td_s32 ret = uapi_haid_init();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary haid init failed!");
        return ret;
    }
    uapi_haid_attr haid_attr = {};
    ret = uapi_haid_get_default_attr(&haid_attr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary haid get default attr failed!");
        return ret;
    }
    ret = uapi_haid_create(&priPortCtx->haidHandle, &haid_attr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary haid create failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidDestroy(PrimaryPortContext* priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    td_s32 ret = uapi_haid_destroy(priPortCtx->haidHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary haid destroy failed!");
        return ret;
    }
    ret = uapi_haid_deinit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary haid init failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetEnable(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "PluginInvokeAttr is null");
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    bool enable = false;
    int32_t retValue;
    int32_t ret = sscanf_s((const char*)invokeAttr->value, "%d", &retValue);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid enable sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (retValue == 1) {
        enable = true;
    }
    ret = uapi_haid_set_enable(priPortCtx->haidHandle, enable);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid enable failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetEnable(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "PluginInvokeAttr is null");
    bool enable = false;
    td_s32 ret = uapi_haid_get_enable(priPortCtx->haidHandle, &enable);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid enable failed!");
        return ret;
    }
    ret = sprintf_s(invokeAttr->value, PRIMARY_ATTR_BUF_LEN, "%d", enable);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid enable sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetVolume(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    uapi_haid_volume volume = {};
    int32_t ret = sscanf_s((const char*)invokeAttr->value, "%d#%d", &volume.integer, &volume.decimal);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid volume sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_haid_set_volume(priPortCtx->haidHandle, &volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid volume failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetVolume(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    uapi_haid_volume volume = {};
    volume.integer = 0;
    volume.decimal = 0;
    td_s32 ret = uapi_haid_get_volume(priPortCtx->haidHandle, &volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid volume failed!");
        return ret;
    }
    ret = sprintf_s(invokeAttr->value, PRIMARY_ATTR_BUF_LEN, "%d#%d", volume.integer, volume.decimal);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid volume sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetHardwareConfig(const PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "input param invokeAttr is null");
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "input param invokeAttr value is null");
    int32_t ret = uapi_haid_set_hardware_config(priPortCtx->haidHandle, (const td_s8 *)invokeAttr->value,
        *invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    td_s32 ret = uapi_haid_set_config(priPortCtx->haidHandle, (const td_s8*)invokeAttr->value, *invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    td_s32 ret = uapi_haid_get_config(priPortCtx->haidHandle, (td_s8 *)invokeAttr->value, invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetAllConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    td_s32 ret = uapi_haid_set_all_config(priPortCtx->haidHandle, (const td_s8*)invokeAttr->value, *invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid all config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetAllConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    td_s32 ret = uapi_haid_get_all_config(priPortCtx->haidHandle, (td_s8*)invokeAttr->value, invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid all config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryTonePlayForExamine(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    td_u32 frequence;
    uapi_haid_volume volume = {};
    int32_t ret = sscanf_s((const char*)invokeAttr->value, "%u#%d:%d", &frequence, &volume.integer, &volume.decimal);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary play tone sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_haid_sinus_tone_start(priPortCtx->haidHandle, frequence, &volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary play tone failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryToneStopForExamine(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    td_s32 ret = uapi_haid_sinus_tone_stop(priPortCtx->haidHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary stop tone failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetScene(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    uapi_haid_scene scene;
    int32_t ret = sscanf_s((const char*)invokeAttr->value, "%d", &scene);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid scene sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_haid_set_scene(priPortCtx->haidHandle, scene);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid scene failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetScene(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    uapi_haid_scene scene;
    td_s32 ret = uapi_haid_get_scene(priPortCtx->haidHandle, &scene);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid scene failed!");
        return ret;
    }
    ret = sprintf_s(invokeAttr->value, PRIMARY_ATTR_BUF_LEN, "%d", scene);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid scene sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}


static int32_t PrimaryAncInit(const PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    int32_t ret = uapi_anc_init();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary anc init failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryAncDeinit(const PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    int32_t ret = uapi_anc_deinit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary anc deinit failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryAncSetConfig(const PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary anc invokeAttr->value == NULL");
    td_s8 config[ANC_CONFIG_BUF_LEN] = {0};
    uapi_aha_mode ahaMode;
    uint32_t len = 0;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%d#%u:%s", &ahaMode, &len, config, ANC_CONFIG_BUF_LEN);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary anc set config sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_anc_set_config(ahaMode, config, len);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary anc set config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryAncGetConfig(const PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    uapi_aha_mode ahaMode;
    uint32_t configLen;
    td_s8 config[ANC_CONFIG_BUF_LEN] = { 0 };
    int32_t ret = uapi_anc_get_config(&ahaMode, config, &configLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary anc get config failed!");
        return ret;
    }

    ret = sprintf_s(invokeAttr->value, ANC_ATTR_BUF_LEN, "%d#%u:%s", ahaMode, configLen, config);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get anc config sprintf_s failed, ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static PrimaryInvoke g_primaryInvokeFuncList[] = {
    {PLUGIN_INVOKE_HAID_CREATE, PrimaryHaidCreate},
    {PLUGIN_INVOKE_HAID_DESTROY, PrimaryHaidDestroy},
    {PLUGIN_INVOKE_HAID_SET_ENABLE, PrimaryHaidSetEnable},
    {PLUGIN_INVOKE_HAID_GET_ENABLE, PrimaryHaidGetEnable},
    {PLUGIN_INVOKE_HAID_SET_VOLUME, PrimaryHaidSetVolume},
    {PLUGIN_INVOKE_HAID_GET_VOLUME, PrimaryHaidGetVolume},
    {PLUGIN_INVOKE_HAID_SET_HARDWARE_CONFIG, PrimaryHaidSetHardwareConfig},
    {PLUGIN_INVOKE_HAID_SET_CONFIG, PrimaryHaidSetConfig},
    {PLUGIN_INVOKE_HAID_GET_CONFIG, PrimaryHaidGetConfig},
    {PLUGIN_INVOKE_HAID_SET_ALL_CONFIG, PrimaryHaidSetAllConfig},
    {PLUGIN_INVOKE_HAID_GET_ALL_CONFIG, PrimaryHaidGetAllConfig},
    {PLUGIN_INVOKE_HAID_TONE_PLAY, PrimaryTonePlayForExamine},
    {PLUGIN_INVOKE_HAID_TONE_STOP, PrimaryToneStopForExamine},
    {PLUGIN_INVOKE_HAID_SET_SCENE, PrimaryHaidSetScene},
    {PLUGIN_INVOKE_HAID_GET_SCENE, PrimaryHaidGetScene},
    {PLUGIN_INVOKE_TWS_SET_MODE, PrimaryTwsSetMode},
    {PLUGIN_INVOKE_TWS_SET_ROLE, PrimaryTwsSetRole},
    {PLUGIN_INVOKE_ANC_INIT, PrimaryAncInit},
    {PLUGIN_INVOKE_ANC_DEINIT, PrimaryAncDeinit},
    {PLUGIN_INVOKE_ANC_SET_CONFIG, PrimaryAncSetConfig},
    {PLUGIN_INVOKE_ANC_GET_CONFIG, PrimaryAncGetConfig},
};
#endif

static int32_t PrimaryPortInvoke(const AudioHandle portHandle, enum PluginInvokeId invokeId,
    struct PluginInvokeAttr *invokeAttr)
{
#ifdef AUDIO_HAID_SUPPORT
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    PrimaryPortContext *priPortCtx = (PrimaryPortContext *)portHandle;
    size_t size = sizeof(g_primaryInvokeFuncList) / sizeof(PrimaryInvoke);
    InvokeFunc invokeFunc = NULL;
    for (size_t i = 0; i < size; i++) {
        if (g_primaryInvokeFuncList[i].invokeId == invokeId) {
            invokeFunc = g_primaryInvokeFuncList[i].invoke;
            break;
        }
    }
    if (invokeFunc == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found invoke function!, invokeId = %d", invokeId);
        return MEDIA_HAL_INVALID_PARAM;
    }
    return invokeFunc(priPortCtx, invokeAttr);
#else
    MEDIA_HAL_UNUSED(portHandle);
    MEDIA_HAL_UNUSED(invokeId);
    MEDIA_HAL_UNUSED(invokeAttr);
    return MEDIA_HAL_OK;
#endif
}

static int32_t PrimaryPortRegCallBack(const AudioHandle portHandle, PluginHaidEventCallBack callBack, void *cookie)
{
#ifdef AUDIO_HAID_SUPPORT
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    PrimaryPortContext* priPortCtx = (PrimaryPortContext*)portHandle;
    td_s32 ret = uapi_haid_register_event_proc(priPortCtx->haidHandle, PrimaryPortHaidEventCallBack, cookie);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "haid register event failed! ret = %x", ret);
        return ret;
    }
    g_haidPrimaryEventCallBack = callBack;
#else
    MEDIA_HAL_UNUSED(portHandle);
    MEDIA_HAL_UNUSED(callBack);
    MEDIA_HAL_UNUSED(cookie);
#endif
    return MEDIA_HAL_OK;
}


AudioPortPlugin g_primaryPortPlugin = {
    .audioPortDir = PORT_OUT_IN,
    .audioPortPins = PIN_IN_MIC | PIN_OUT_SPEAKER,
    .GetPortPluginCapability = PrimaryPortGetPortPluginCapability,
    .Open = PrimaryPortOpen,
    .CreateTrack = PrimaryPortCreateTrack,
    .DestroyTrack = PrimaryPortDestroyTrack,
    .Close = PrimaryPortClose,
    .Invoke = PrimaryPortInvoke,
    .RegCallBack = PrimaryPortRegCallBack,
    .SetMute = NULL,
    .GetMute = NULL,
    .SetVolume = NULL,
    .GetVolume = NULL,
};

const AudioPortPlugin *GetPrimaryPortPluginFuncs(AudioPluginType pluginType)
{
    MEDIA_HAL_UNUSED(pluginType);
    return &g_primaryPortPlugin;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
