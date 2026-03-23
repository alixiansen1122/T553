/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: ogg manager file
 * Author: Media Software Group
 * Create: 2025-09-28
 */
#include "ogg_manager.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include "securec.h"
#include "media_hal_common.h"
#include "ogg.h"

#define MODULE_NAME "ogg_manager"

#define HEADER_STRLEN 9
#define OPUS_DELAY_TIME_DIVISOR 0
#define OPUS_DELAY_TIME_DIVIDED 100
#define OPUS_ID_HEADER_LEN 19
#define OPUS_COMMENT_HEADER_LEN 25
#define OPUS_COMMENT_TAIL_LEN 4
#define OPUS_OPUSTAGS_LEN 12
#define OPUS_SAMPLERATE 48000

typedef struct {
    ogg_stream_state oggStream;
    int32_t sampleRate;
    int32_t channels;
    int64_t granulepos;
    int64_t packetCount;
    int32_t serialno;
    bool isInited;
    ogg_page *page;
    ogg_packet *packet;
    unsigned char *idHeader;
    unsigned char *commentHeader;
} OggContext;

static OggContext g_oggCtx = {};

static int32_t OggHeaderInit(int32_t channels)
{
    const char *idHeader = "OpusHead";
    const char *commentHeader = "OpusTags";
    const char *vendor = "libogg-1.3.5";
    uint32_t vendorLen = strlen(vendor);
    uint32_t preSkip = g_oggCtx.sampleRate * OPUS_DELAY_TIME_DIVISOR / OPUS_DELAY_TIME_DIVIDED;
    g_oggCtx.idHeader = (uint8_t *)(MediaHalCalloc(OPUS_ID_HEADER_LEN));
    uint32_t commentHeaderResLen = OPUS_COMMENT_HEADER_LEN - OPUS_OPUSTAGS_LEN;
    CHK_NULL_RETURN(g_oggCtx.idHeader, MEDIA_HAL_ERR, "ogg idHeader malloc failed!");
    if (memcpy_s(g_oggCtx.idHeader, OPUS_ID_HEADER_LEN, idHeader, strlen(idHeader)) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ogg idHeader memcpy failed!");
        goto FREE_ID_HEADER;
    }
    g_oggCtx.idHeader[0x8] = 0x01;
    g_oggCtx.idHeader[0x9] = channels;
    g_oggCtx.idHeader[0xA] = preSkip & 0xFF;
    g_oggCtx.idHeader[0xB] = (preSkip >> 0x8);
    g_oggCtx.idHeader[0xC] = g_oggCtx.sampleRate & 0xFF;
    g_oggCtx.idHeader[0xD] = (g_oggCtx.sampleRate >> 0x8) & 0xFF;
    g_oggCtx.idHeader[0xE] = (g_oggCtx.sampleRate >> 0x10) & 0xFF;
    g_oggCtx.idHeader[0xF] = (g_oggCtx.sampleRate >> 0x18) & 0xFF;
    g_oggCtx.commentHeader = (uint8_t *)(MediaHalCalloc(OPUS_COMMENT_HEADER_LEN));
    if (g_oggCtx.commentHeader == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ogg commentHeader malloc failed!");
        goto FREE_ID_HEADER;
    }
    if (memcpy_s(g_oggCtx.commentHeader, OPUS_COMMENT_HEADER_LEN, commentHeader, strlen(commentHeader)) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ogg commentHeader memcpy failed!");
        goto FREE_COMMENT_HEADER;
    }
    g_oggCtx.commentHeader[0x8] = vendorLen & 0xFF;
    g_oggCtx.commentHeader[0x9] = (vendorLen >> 0x8) & 0xFF;
    g_oggCtx.commentHeader[0xA] = (vendorLen >> 0x10) & 0xFF;
    g_oggCtx.commentHeader[0xB] = (vendorLen >> 0x18) & 0xFF;
    if (memcpy_s(g_oggCtx.commentHeader + OPUS_OPUSTAGS_LEN, commentHeaderResLen, vendor, strlen(vendor)) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ogg vendor memcpy failed!");
        goto FREE_COMMENT_HEADER;
    }
    return MEDIA_HAL_OK;
FREE_COMMENT_HEADER:
    MediaHalFree((void **)&g_oggCtx.commentHeader);
    g_oggCtx.commentHeader = NULL;
FREE_ID_HEADER:
    MediaHalFree((void **)&g_oggCtx.idHeader);
    g_oggCtx.idHeader = NULL;
    return MEDIA_HAL_ERR;
}

static int32_t OggStreamFlush(int32_t fd)
{
    while (ogg_stream_flush(&g_oggCtx.oggStream, g_oggCtx.page)) {
        int32_t ret = write(fd, g_oggCtx.page->header, g_oggCtx.page->header_len);
        if (ret != g_oggCtx.page->header_len) {
            MEDIA_HAL_LOGE(MODULE_NAME, "errno:%d, errmsg:%s", errno, strerror(errno));
            return MEDIA_HAL_ERR;
        }
        ret = write(fd, g_oggCtx.page->body,  g_oggCtx.page->body_len);
        if (ret != g_oggCtx.page->body_len) {
            MEDIA_HAL_LOGE(MODULE_NAME, "errno:%d, errmsg:%s", errno, strerror(errno));
            return MEDIA_HAL_ERR;
        }
    }
    return MEDIA_HAL_OK;
}

static void OggParamInit(int32_t sampleRate, int32_t channels)
{
    g_oggCtx.sampleRate = sampleRate;
    g_oggCtx.channels = channels;
    g_oggCtx.granulepos = 0;
    g_oggCtx.packetCount = 0;
    g_oggCtx.serialno = 0x1;
}

static void OggPacketIn(unsigned char *packet, int32_t bytes)
{
    g_oggCtx.packet->packet = packet;
    g_oggCtx.packet->bytes = bytes;
    g_oggCtx.packet->b_o_s = g_oggCtx.packetCount != 0 ? 0 : 1; // 1 means first page
    g_oggCtx.packet->e_o_s = 0;
    g_oggCtx.packet->granulepos = g_oggCtx.packetCount != 0 ? g_oggCtx.granulepos : -1; // -1 means first page
    g_oggCtx.packet->packetno = g_oggCtx.packetCount++;
    ogg_stream_packetin(&g_oggCtx.oggStream, g_oggCtx.packet);
}

int32_t OggCtxInit(int32_t fd, int32_t sampleRate, int32_t channels)
{
    if (g_oggCtx.isInited) {
        MEDIA_HAL_LOGE(MODULE_NAME, "Ogg context has inited!");
        goto EXIT;
    }
    g_oggCtx.packet = (ogg_packet *)MediaHalCalloc(sizeof(ogg_packet));
    if (g_oggCtx.packet == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ogg context packet malloc failed!");
        goto EXIT;
    }
    g_oggCtx.page = (ogg_page *)MediaHalCalloc(sizeof(ogg_page));
    if (g_oggCtx.page == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ogg context page malloc failed!");
        goto FREE_PACKET;
    }
    OggParamInit(sampleRate, channels);
    if (ogg_stream_init(&g_oggCtx.oggStream, g_oggCtx.serialno) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ogg stream init failed!");
        goto FREE_PAGE;
    }
    if (OggHeaderInit(channels) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "Header init failed!");
        goto STREAM_CLEAR;
    }
    OggPacketIn(g_oggCtx.idHeader, OPUS_ID_HEADER_LEN);
    if (OggStreamFlush(fd) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ogg packet flush failed!");
        goto HEADER_FREE;
    }
    OggPacketIn(g_oggCtx.commentHeader, OPUS_COMMENT_HEADER_LEN);
    if (OggStreamFlush(fd) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ogg header flush failed!");
        goto HEADER_FREE;
    }
    g_oggCtx.isInited = true;
    return MEDIA_HAL_OK;
HEADER_FREE:
    MediaHalFree((void **)&g_oggCtx.idHeader);
    MediaHalFree((void **)&g_oggCtx.commentHeader);
STREAM_CLEAR:
    ogg_stream_clear(&g_oggCtx.oggStream);
FREE_PAGE:
    MediaHalFree((void **)&g_oggCtx.page);
FREE_PACKET:
    MediaHalFree((void **)&g_oggCtx.packet);
EXIT:
    return MEDIA_HAL_ERR;
}

void OggCtxDeInit()
{
    g_oggCtx.sampleRate = 0;
    g_oggCtx.channels = 0;
    g_oggCtx.granulepos = 0;
    g_oggCtx.packetCount = 0;
    if (g_oggCtx.packet != NULL) {
        MediaHalFree(g_oggCtx.packet);
        g_oggCtx.packet = NULL;
    }
    if (g_oggCtx.page != NULL) {
        MediaHalFree(g_oggCtx.page);
        g_oggCtx.page = NULL;
    }
    if (g_oggCtx.idHeader != NULL) {
        MediaHalFree(g_oggCtx.idHeader);
        g_oggCtx.idHeader = NULL;
    }
    if (g_oggCtx.commentHeader != NULL) {
        MediaHalFree(g_oggCtx.commentHeader);
        g_oggCtx.commentHeader = NULL;
    }
    ogg_stream_clear(&g_oggCtx.oggStream);
    g_oggCtx.isInited = false;
}

static int32_t OggWritePage(int32_t fd)
{
    while (ogg_stream_pageout(&g_oggCtx.oggStream, g_oggCtx.page)) {
        int32_t ret = write(fd, g_oggCtx.page->header, g_oggCtx.page->header_len);
        if (ret != g_oggCtx.page->header_len) {
            MEDIA_HAL_LOGE(MODULE_NAME, "errno:%d, errmsg:%s", errno, strerror(errno));
            return MEDIA_HAL_ERR;
        }
        ret = write(fd, g_oggCtx.page->body, g_oggCtx.page->body_len);
        if (ret != g_oggCtx.page->body_len) {
            MEDIA_HAL_LOGE(MODULE_NAME, "errno:%d, errmsg:%s", errno, strerror(errno));
            return MEDIA_HAL_ERR;
        }
    }
    return MEDIA_HAL_OK;
}

int32_t OggWritePacket(int32_t fd, uint8_t *buffer, int64_t bufferLen, int64_t samplePoint)
{
    if (bufferLen < 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input buffer invalid");
        return MEDIA_HAL_ERR;
    }
    OggPacketIn((unsigned char *)(buffer), bufferLen);
    g_oggCtx.granulepos += samplePoint;
    return OggWritePage(fd);
}

int32_t OggWriteEosPacket(int32_t fd)
{
    unsigned char emptyData = 0;
    g_oggCtx.packet->packet = &emptyData;
    g_oggCtx.packet->bytes = 1;
    g_oggCtx.packet->b_o_s = 0;
    g_oggCtx.packet->e_o_s = 1;
    g_oggCtx.packet->granulepos = g_oggCtx.granulepos;
    g_oggCtx.packet->packetno = g_oggCtx.packetCount;
    ogg_stream_packetin(&g_oggCtx.oggStream, g_oggCtx.packet);
    return OggStreamFlush(fd);
}