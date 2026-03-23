/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: format common interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef FORMAT_COMMON_H
#define FORMAT_COMMON_H

#include <stdint.h>
#include "plugin_common.h"
#include "format_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    STATE_IDLE,
    STATE_INIT,
    STATE_START,
    STATE_STOP,
} FormatState;

#define CHK_HANDLE_VALID(ctx) \
do { \
    if ((ctx) == NULL || (ctx)->innerCtx != (ctx)) { \
        MEDIA_HAL_LOGE(MODULE_NAME, "handle invalid"); \
        return MEDIA_HAL_ERR; \
    } \
} while (0)

CodecFormat CodecTypeToCodecFormat(CodecType type);
CodecType CodecFormatToCodecType(CodecFormat type);
SubtitleFormat SubTypeToSubFormat(SubtitleFormatType type);
void FormatPacketToFrame(const FormatPacket *packet, FormatFrame *frame);
void FormatFrameToPacket(const FormatFrame *frame, FormatPacket *packet);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // FORMAT_COMMON_H
/** @} */
