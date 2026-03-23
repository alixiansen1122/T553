/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Protocol interface
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef PROTOCOL_INTERFACE_H
#define PROTOCOL_INTERFACE_H

#include "plugin_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    PluginDesc desc;

    int (*protocol_find)(const char *url);
    int (*protocol_open)(const char *url, int flags, void **handle);
    int (*protocol_seek)(void *handle, int64_t pos, int whence);
    int (*protocol_seek_stream)(void *handle, int32_t streamId, int64_t pts, uint32_t flags);
    int (*protocol_read)(void *handle, unsigned char *buf, int size);
    int (*protocol_write)(void *handle, const unsigned char *buf, int size);
    int (*protocol_invoke)(void *handle, uint32_t invokeId, void *arg);
    int (*protocol_close)(void *handle);
} ProtocolPluginEntry;

typedef const ProtocolPluginEntry *(*GetProtocolFun)(void);
const ProtocolPluginEntry *GetProtocol(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // PROTOCOL_INTERFACE_H
