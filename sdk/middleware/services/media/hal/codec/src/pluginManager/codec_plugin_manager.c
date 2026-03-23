/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: video decoder comm
 * Author: Media Software Group
 * Create: 2017-12-22
 */

#include "codec_plugin_manager.h"
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#ifdef SUPPORT_DL
#include <dlfcn.h>
#endif
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"

#define ELEMENT_MAX_LEN 50
#define MODULE_NAME "CODEC_PLUGIN_MGR"

typedef struct SimpleCapbility {
    ResizableArray supportMimes;
    bool isHardware;
} SimpleCapbility;

typedef void *(*GetImplFunNew)(void);

typedef int (*GetImplFun)(void *);
typedef int (*UnImplFun)(void);
typedef void (*GetCapbilityFun)(void);

static MediaHalMutexHandle g_vdecPluginListLock = NULL;
static CodecListHead g_vdecPliginList;

static MediaHalMutexHandle g_adecPluginListLock = NULL;
static CodecListHead g_adecPliginList;

static MediaHalMutexHandle g_aencPluginListLock = NULL;
static CodecListHead g_aencPliginList;

static MediaHalMutexHandle g_vencPluginListLock = NULL;
static CodecListHead g_vencPliginList;

static const char *g_codeType[INVALID_TYPE + 1] = {
    "vdec", "venc", "adec", "aenc", ".so"
};

static uint32_t CheckPluginCommomImpl(const CodecPluginCommon *pluginCommon)
{
    CHK_NULL_RETURN(pluginCommon, MEDIA_HAL_INVALID_PARAM, "pluginCommon ptr is null");
    CHK_NULL_RETURN(pluginCommon->pfnCreate, MEDIA_HAL_INVALID_PARAM, "create fun ptr is null");
    CHK_NULL_RETURN(pluginCommon->pfnStart, MEDIA_HAL_INVALID_PARAM, "start fun ptr is null");
    CHK_NULL_RETURN(pluginCommon->pfnStop, MEDIA_HAL_INVALID_PARAM, "stop fun ptr is null");
    CHK_NULL_RETURN(pluginCommon->pfnReset, MEDIA_HAL_INVALID_PARAM, "reset fun ptr is null");
    CHK_NULL_RETURN(pluginCommon->pfnInvoke, MEDIA_HAL_INVALID_PARAM, "invoke fun ptr is null");
    return MEDIA_HAL_OK;
}

static int32_t AddPluginToList(CodecType type, void *plugin, CodecPluginNode *node)
{
    switch (type) {
        case VIDEO_DECODER: {
            AvVideoDecoder *vdecPlugin = (AvVideoDecoder *)plugin;
            node->videoDecoder = *vdecPlugin;
            MediaHalInitStaticMutexLock(&g_vdecPluginListLock);
            MediaHalMutexLock(g_vdecPluginListLock);
            CODEC_LIST_ADD(&node->listPtr, &g_vdecPliginList);
            MediaHalMutexUnLock(g_vdecPluginListLock);
            break;
        }
        case VIDEO_ENCODER: {
            AvVideoEncoder *vencPlugin = (AvVideoEncoder *)plugin;
            node->videoEncoder = *vencPlugin;
            MediaHalInitStaticMutexLock(&g_vencPluginListLock);
            MediaHalMutexLock(g_vencPluginListLock);
            CODEC_LIST_ADD(&node->listPtr, &g_vencPliginList);
            MediaHalMutexUnLock(g_vencPluginListLock);
            break;
        }
        case AUDIO_DECODER: {
            AvAudioDecoder *adecPlugin = (AvAudioDecoder *)plugin;
            node->audioDecoder = *adecPlugin;
            MediaHalInitStaticMutexLock(&g_adecPluginListLock);
            MediaHalMutexLock(g_adecPluginListLock);
            CODEC_LIST_ADD(&node->listPtr, &g_adecPliginList);
            MediaHalMutexUnLock(g_adecPluginListLock);
            break;
        }
        case AUDIO_ENCODER: {
            AvAudioEncoder *aencPlugin = (AvAudioEncoder *)plugin;
            node->audioEncoder = *aencPlugin;
            MediaHalInitStaticMutexLock(&g_aencPluginListLock);
            MediaHalMutexLock(g_aencPluginListLock);
            CODEC_LIST_ADD(&node->listPtr, &g_aencPliginList);
            MediaHalMutexUnLock(g_aencPluginListLock);
            break;
        }
        default: {
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid type:%d", type);
            return MEDIA_HAL_ERR;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t RegisterCodecPlugin(void *plugin, void *libModule, const char *compeltePath, CodecType type)
{
    CHK_NULL_RETURN(plugin, MEDIA_HAL_ERR, "plugin null");

    CodecPluginCommon *pluginCommon = (CodecPluginCommon *)plugin;
    if (CheckPluginCommomImpl(pluginCommon) != MEDIA_HAL_OK) {
        return MEDIA_HAL_ERR;
    }

    pluginCommon->module = libModule;
    if (strcpy_s((char *)pluginCommon->pluginName,
        sizeof(pluginCommon->pluginName), compeltePath) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strcpy_s failed");
        return MEDIA_HAL_ERR;
    }

    CodecPluginNode *node = (CodecPluginNode *)malloc(sizeof(CodecPluginNode));
    if (node == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_ERR;
    }
    if (memset_s(node, sizeof(CodecPluginNode), 0x00, sizeof(CodecPluginNode)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }

    if (AddPluginToList(type, plugin, node) != MEDIA_HAL_OK) {
        free(node);
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int InitList(CodecType type)
{
    switch (type) {
        case VIDEO_DECODER: {
            INIT_LIST_HEAD(&g_vdecPliginList);
            break;
        }
        case VIDEO_ENCODER: {
            INIT_LIST_HEAD(&g_vencPliginList);
            break;
        }
        case AUDIO_DECODER: {
            INIT_LIST_HEAD(&g_adecPliginList);
            break;
        }
        case AUDIO_ENCODER: {
            INIT_LIST_HEAD(&g_aencPliginList);
            break;
        }
        default: {
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid type:%d", type);
            return MEDIA_HAL_ERR;
        }
    }
    return MEDIA_HAL_OK;
}
#ifdef SUPPORT_DL
static int GetAbsPath(const char *dirPath, char *absPath)
{
    if (realpath(dirPath, absPath) == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "realpath error:%s", dirPath);
        return MEDIA_HAL_ERR;
    }
    if (access(absPath, F_OK | R_OK) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "file is not exist or read:%s", absPath);
        return MEDIA_HAL_ERR;
    }

    return MEDIA_HAL_OK;
}

static int GetCompeltePath(const char *absPath, const struct dirent* pdirent, char *compeltePath, int length)
{
    if (strcpy_s(compeltePath, length, absPath) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strcpy_s failed");
        return MEDIA_HAL_ERR;
    }
    if (strcat_s(compeltePath, length, "/") != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strcat_s failed");
        return MEDIA_HAL_ERR;
    }
    if (strcat_s(compeltePath, length, pdirent->d_name) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strcat_s failed");
        return MEDIA_HAL_ERR;
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "complete path is %s", compeltePath);
    return MEDIA_HAL_OK;
}

static void CheckAddPlugin(const char *compeltePath, const char *getImplSymbol, CodecType type)
{
    void *soModule = MediaHalDLOpen(compeltePath);
    if (soModule == NULL) {
        return;
    }

    GetImplFunNew getImplFun = MediaHalDLSym(soModule, getImplSymbol);
    if (getImplFun == NULL) {
        MediaHalDLClose(soModule);
        return;
    }
    void *plugin = getImplFun();
    if (plugin == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "getImplFun fail");
        MediaHalDLClose(soModule);
        return;
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "find valid impl %s, plugin name is %s", getImplSymbol, compeltePath);
    if (RegisterCodecPlugin(plugin, soModule, compeltePath, type) != MEDIA_HAL_OK) {
        MediaHalDLClose(soModule);
        MEDIA_HAL_LOGE(MODULE_NAME, "register plugin fail, type:%d path is:%s", type, compeltePath);
    }
    free(plugin);
}
#endif

int SearchLoadValidPlugin(const char *dirPath, const char *getImplSymbol, CodecType type)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "type[%d] in", type);
    MEDIA_HAL_UNUSED(dirPath);
    MEDIA_HAL_UNUSED(getImplSymbol);
#ifdef SUPPORT_DL
    char absPath[PATH_MAX] = { 0 };
    CHK_FAILED_RETURN_NO_LOG(GetAbsPath(dirPath, absPath), MEDIA_HAL_OK, MEDIA_HAL_ERR);

    DIR *pdir = opendir(absPath);
    if (pdir == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "opendir:%s fail", absPath);
        return MEDIA_HAL_ERR;
    }

    InitList(type);
    struct dirent* pdirent = NULL;
    const char *keyStr = g_codeType[type];

    while ((pdirent = readdir(pdir)) != NULL) {
        if (strstr(pdirent->d_name, keyStr) == NULL || pdirent->d_name[0] == '.') {
            continue;
        }
        char compeltePath[PATH_MAX] = { 0 };
        if (GetCompeltePath(absPath, pdirent, compeltePath, PATH_MAX) != MEDIA_HAL_OK) {
            closedir(pdir);
            return MEDIA_HAL_ERR;
        }
        CheckAddPlugin(compeltePath, getImplSymbol, type);
    }
    closedir(pdir);
#else
    InitList(type);
    // now only support vdec in static load way
    if (type != VIDEO_DECODER) {
        MEDIA_HAL_LOGI(MODULE_NAME, "only support VIDEO_DECODER, now is [%d]", type);
        return MEDIA_HAL_OK;
    }
    // Directly create the related plug-in, and then call the registration function.
    void *plugin = VDecoderGetImpl();
    if (plugin == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "VDecoderGetImpl fail");
        return MEDIA_HAL_ERR;
    }

    // Pay attention to libModule when unloading.
    const char *compeltePath = "/usr/lib/fake_vdec.so";
    if (RegisterCodecPlugin(plugin, NULL, compeltePath, type) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "register plugin fail, type:%d path is:%s", type, compeltePath);
    }
    free(plugin);

#endif
    MEDIA_HAL_LOGI(MODULE_NAME, "type[%d] out", type);
    return MEDIA_HAL_OK;
}

static void* FindSpecificPluginInternal(AvCodecMime mime, bool hardwareFirst,
    const CodecListHead *pluginList, CodecType type)
{
    MEDIA_HAL_UNUSED(hardwareFirst);
    CodecListHead *pos = NULL;
    CodecPluginNode *node = NULL;
    bool isMatch = false;
    CODEC_LIST_FOR_EACH(pos, pluginList)
    {
        node = CODEC_LIST_ENTRY(pos, CodecPluginNode, listPtr);
        CodecPluginCommon *pluginCommon =
            (type == VIDEO_DECODER) ? &(node->videoDecoder.pluginCommon) :
            ((type == VIDEO_ENCODER) ? &(node->videoEncoder.pluginCommon) :
            ((type == AUDIO_DECODER) ? &(node->audioDecoder.pluginCommon) :
            ((type == AUDIO_ENCODER) ? &(node->audioEncoder.pluginCommon) : NULL)));
        if (pluginCommon == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid type");
            return NULL;
        }
        for (int i = 0; i < pluginCommon->validCapbilityNum; i++) {
            isMatch = pluginCommon->capbilites[i].mime == mime &&
                      pluginCommon->isHardwarePlugin == true;
            if (isMatch) {
                break;
            }
        }
    }
    if (isMatch) {
        return (type == VIDEO_DECODER) ? (void *)(&node->videoDecoder) :
               ((type == AUDIO_DECODER) ? (void *)(&node->audioDecoder) :
               ((type == VIDEO_ENCODER) ? (void *)(&node->videoEncoder) :
               ((type == AUDIO_ENCODER) ? (void *)(&node->audioEncoder) : NULL)));
    }
    return NULL;
}

void* FindSpecificPlugin(AvCodecMime mime, bool hardwareFirst, CodecType type)
{
    void *pluginEntry = NULL;

    switch (type) {
        case VIDEO_DECODER: {
            MediaHalInitStaticMutexLock(&g_vdecPluginListLock);
            MediaHalMutexLock(g_vdecPluginListLock);
            pluginEntry = FindSpecificPluginInternal(mime, hardwareFirst, &g_vdecPliginList, type);
            MediaHalMutexUnLock(g_vdecPluginListLock);
            break;
        }
        case VIDEO_ENCODER: {
            MediaHalInitStaticMutexLock(&g_vencPluginListLock);
            MediaHalMutexLock(g_vencPluginListLock);
            pluginEntry = FindSpecificPluginInternal(mime, hardwareFirst, &g_vencPliginList, type);
            MediaHalMutexUnLock(g_vencPluginListLock);
            break;
        }
        case AUDIO_DECODER: {
            MediaHalInitStaticMutexLock(&g_adecPluginListLock);
            MediaHalMutexLock(g_adecPluginListLock);
            pluginEntry = FindSpecificPluginInternal(mime, hardwareFirst, &g_adecPliginList, type);
            MediaHalMutexUnLock(g_adecPluginListLock);
            break;
        }
        case AUDIO_ENCODER: {
            MediaHalInitStaticMutexLock(&g_aencPluginListLock);
            MediaHalMutexLock(g_aencPluginListLock);
            pluginEntry = FindSpecificPluginInternal(mime, hardwareFirst, &g_aencPliginList, type);
            MediaHalMutexUnLock(g_aencPluginListLock);
            break;
        }
        default: {
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid type");
            break;
        }
    }
    return pluginEntry;
}

static int UnloadVdecPlugin(void)
{
    CodecListHead *pos = NULL;
    CodecPluginNode *node = NULL;
    CodecListHead *tmp = NULL;

    MediaHalInitStaticMutexLock(&g_vdecPluginListLock);
    MediaHalMutexLock(g_vdecPluginListLock);
    CODEC_LIST_FOR_EACH_SAFE(pos, tmp, &g_vdecPliginList)
    {
        node = CODEC_LIST_ENTRY(pos, CodecPluginNode, listPtr);
#ifdef SUPPORT_DL
        dlclose(node->videoDecoder.pluginCommon.module);
#endif
        CODEC_LIST_DEL(&node->listPtr);
        free(node);
    }
    MediaHalMutexUnLock(g_vdecPluginListLock);
    return MEDIA_HAL_OK;
}

static int UnloadAdecPlugin(void)
{
    CodecListHead *pos = NULL;
    CodecPluginNode *node = NULL;
    CodecListHead *tmp = NULL;
    MediaHalInitStaticMutexLock(&g_adecPluginListLock);
    MediaHalMutexLock(g_adecPluginListLock);
    CODEC_LIST_FOR_EACH_SAFE(pos, tmp, &g_adecPliginList)
    {
        node = CODEC_LIST_ENTRY(pos, CodecPluginNode, listPtr);
#ifdef SUPPORT_DL
        UnImplFun unImplFun = dlsym(node->audioDecoder.pluginCommon.module, "ADecoderImplUnRegister");
        if (unImplFun == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "unImplFun null, plugin[%s]", node->audioDecoder.pluginCommon.pluginName);
            MediaHalMutexUnLock(g_adecPluginListLock);
            return MEDIA_HAL_ERR;
        }
        int ret = unImplFun();
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "unimpl fun fail, plugin[%s]", node->audioDecoder.pluginCommon.pluginName);
            MediaHalMutexUnLock(g_adecPluginListLock);
            return MEDIA_HAL_ERR;
        }
        dlclose(node->audioDecoder.pluginCommon.module);
#endif
        CODEC_LIST_DEL(&node->listPtr);
        free(node);
    }
    MediaHalMutexUnLock(g_adecPluginListLock);
    return MEDIA_HAL_OK;
}

static int UnloadAencPlugin(void)
{
    CodecListHead *pos = NULL;
    CodecPluginNode *node = NULL;
    CodecListHead *tmp = NULL;
    MediaHalInitStaticMutexLock(&g_aencPluginListLock);
    MediaHalMutexLock(g_aencPluginListLock);
    CODEC_LIST_FOR_EACH_SAFE(pos, tmp, &g_aencPliginList)
    {
        node = CODEC_LIST_ENTRY(pos, CodecPluginNode, listPtr);
#ifdef SUPPORT_DL
        UnImplFun unImplFun = dlsym(node->audioEncoder.pluginCommon.module, "AEncoderImplUnRegister");
        if (unImplFun == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "unImplFun null, plugin[%s]", node->audioEncoder.pluginCommon.pluginName);
            MediaHalMutexUnLock(g_aencPluginListLock);
            return MEDIA_HAL_ERR;
        }
        int ret = unImplFun();
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "unimpl fun fail, plugin[%s]", node->audioEncoder.pluginCommon.pluginName);
            MediaHalMutexUnLock(g_aencPluginListLock);
            return MEDIA_HAL_ERR;
        }
        dlclose(node->audioEncoder.pluginCommon.module);
#endif
        CODEC_LIST_DEL(&node->listPtr);
        free(node);
    }
    MediaHalMutexUnLock(g_aencPluginListLock);
    return MEDIA_HAL_OK;
}

static int UnloadVencPlugin(void)
{
    CodecListHead *pos = NULL;
    CodecPluginNode *node = NULL;
    CodecListHead *tmp = NULL;
    MediaHalInitStaticMutexLock(&g_vencPluginListLock);
    MediaHalMutexLock(g_vencPluginListLock);
    CODEC_LIST_FOR_EACH_SAFE(pos, tmp, &g_vencPliginList)
    {
        node = CODEC_LIST_ENTRY(pos, CodecPluginNode, listPtr);
#ifdef SUPPORT_DL
        UnImplFun unImplFun = dlsym(node->videoEncoder.pluginCommon.module, "VEncoderImplUnRegister");
        if (unImplFun == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "unImplFun null, plugin[%s]", node->videoEncoder.pluginCommon.pluginName);
            MediaHalMutexUnLock(g_vencPluginListLock);
            return MEDIA_HAL_ERR;
        }
        int ret = unImplFun();
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "unimpl fun fail, plugin[%s]", node->videoEncoder.pluginCommon.pluginName);
            MediaHalMutexUnLock(g_vencPluginListLock);
            return MEDIA_HAL_ERR;
        }
        dlclose(node->videoEncoder.pluginCommon.module);
#endif
        CODEC_LIST_DEL(&node->listPtr);
        free(node);
    }
    MediaHalMutexUnLock(g_vencPluginListLock);
    return MEDIA_HAL_OK;
}

int UnLoadPlugin(CodecType type)
{
    switch (type) {
        case VIDEO_DECODER: {
            return UnloadVdecPlugin();
        }
        case VIDEO_ENCODER: {
            return UnloadVencPlugin();
        }
        case AUDIO_DECODER: {
            return UnloadAdecPlugin();
        }
        case AUDIO_ENCODER: {
            return UnloadAencPlugin();
        }
        default: {
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid type:%d", type);
            return MEDIA_HAL_ERR;
        }
    }
}