/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: wav file
 * Author: Media Software Group
 * Create: 2025-03-10
 */
#include "wav.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include "securec.h"
#include "media_hal_common.h"
#include "dfx_os_st.h"

#define WAV_RIFF_CHUNK_ID       (((int32_t)'F' << 24) | ((int32_t)'F' << 16) | ((int32_t)'I' << 8) | 'R')
#define WAV_FORMAT_CHUNK_ID     (((int32_t)' ' << 24) | ((int32_t)'t' << 16) | ((int32_t)'m' << 8) | 'f')
#define WAV_FACT_CHUNK_ID       (((int32_t)'t' << 24) | ((int32_t)'c' << 16) | ((int32_t)'a' << 8) | 'f')
#define WAV_DATA_CHUNK_ID       (((int32_t)'a' << 24) | ((int32_t)'t' << 16) | ((int32_t)'a' << 8) | 'd')
#define WAV_WAVE_ID             (((int32_t)'E' << 24) | ((int32_t)'V' << 16) | ((int32_t)'A' << 8) | 'W')
#define MODULE_NAME "WAV"
#define WAV_SUB_FORMAT_LEN 16
#define WAV_DEFAULTE_SAMPLERATE 48000
#define FILE_MODEL 0644

static void* WavDefaultMalloc(void *context, size_t size)
{
    (void)context;
    void *p = malloc(size);
    return p;
}

static void WavDefaultFree(void *context, void *p)
{
    (void)context;
    free(p);
}

static WavAllocFuncs g_defaultAllocFuncs = {
    &WavDefaultMalloc,
    &WavDefaultFree
};

static void *g_allocContext = NULL;
static const WavAllocFuncs* g_allocFuncs = &g_defaultAllocFuncs;

void WavSetAllocator(void *context, const WavAllocFuncs *funcs)
{
    g_allocContext = context;
    g_allocFuncs = funcs;
}

void* WavMalloc(size_t size)
{
    return g_allocFuncs->malloc(g_allocContext, size);
}

void WavFree(void *p)
{
    if (p != NULL) {
        g_allocFuncs->free(g_allocContext, p);
    }
}

char* WavStrdup(const char *str)
{
    size_t len = strlen(str) + 1;
    void *new = WavMalloc(len);
    if (new == NULL) {
        return NULL;
    }
    if (memcpy_s(new, len, str, len) != EOK) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "memcpy_s failed");
        WavFree(new);
        return NULL;
    }
    return new;
}

#pragma pack(push, 1)

typedef struct {
    uint32_t id;
    uint32_t size;
} WavChunkHeader;

typedef struct {
    WavChunkHeader header;

    uint64_t offset;

    struct {
        uint16_t formatTag;
        uint16_t channelCounts;
        uint32_t sampleRate;
        uint32_t avgBytesPerSec;
        uint16_t blockAlign;
        uint16_t bitsPerSample;

        uint16_t extSize;
        uint16_t validBitsPerSample;
        uint32_t channelMask;

        uint8_t subFormat[WAV_SUB_FORMAT_LEN];
    } body;
} WavFormatChunk;

typedef struct {
    WavChunkHeader header;

    uint64_t offset;

    struct {
        uint32_t sampleLength;
    } body;
} WavFactChunk;

typedef struct {
    WavChunkHeader header;
    uint64_t offset;
} WavDataChunk;

typedef struct {
    uint32_t id;
    uint32_t position;
    uint32_t chunkId;
    uint32_t chunkStart;
    uint32_t blockStart;
    uint32_t sampleOffset;
} CueMarker;

#define WAV_CUE_SIZE 4
#define WAV_DATA_HEAD_SIZE 8
#define WAV_CUE_HEAD_SIZE 12

#define WAV_MARKERS_ALL_SIZE (2 * 1024)
#define WAV_MARKERS_BUFFER_SIZE ((WAV_MARKERS_ALL_SIZE - WAV_CUE_HEAD_SIZE) / sizeof(CueMarker))
static char g_buffer[WAV_MARKERS_ALL_SIZE] = { 0 };

typedef struct {
    char cueFlag[WAV_CUE_SIZE];
    uint32_t cueSize;
    uint32_t numMarkers;
    CueMarker markers[WAV_MARKERS_BUFFER_SIZE];
} CueMarkList;

typedef struct {
    uint32_t id;
    uint32_t size;
    uint32_t waveId;
    uint64_t offset;
} WavMasterChunk;

#pragma pack(pop)

#define WAV_CHUNK_MASTER    ((uint32_t)1)
#define WAV_CHUNK_FORMAT    ((uint32_t)2)
#define WAV_CHUNK_FACT      ((uint32_t)4)
#define WAV_CHUNK_DATA      ((uint32_t)8)

struct WavFileInfo {
    int32_t             fp;
    char*               filename;
    uint32_t            mode;
    int32_t             isANewFile;

    WavMasterChunk      riffChunk;
    WavFormatChunk      formatChunk;
    WavFactChunk        factChunk;
    CueMarkList         cueChunk;
    WavDataChunk        dataChunk;
};

static const uint8_t DEFAULT_SUB_FORMAT[WAV_SUB_FORMAT_LEN] = {
    0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
};

void WavMarkAndDataInfo(WavFile *self)
{
    uint32_t size = self->dataChunk.offset - sizeof(WavChunkHeader);
    if (lseek(self->fp, size, SEEK_SET) < 0) {
        goto EXIT1;
    }
    if (self->cueChunk.numMarkers > 0) {
        self->cueChunk.cueSize = self->cueChunk.numMarkers * sizeof(CueMarker) +  WAV_CUE_SIZE;
        uint32_t markerSize = sizeof(CueMarker) * self->cueChunk.numMarkers + WAV_CUE_HEAD_SIZE;
        if (write(self->fp, &self->cueChunk, markerSize) <= 0) {
            goto EXIT2;
        }
        
        self->dataChunk.header.size += WAV_MARKERS_ALL_SIZE -
            (sizeof(CueMarker) * self->cueChunk.numMarkers) - WAV_CUE_HEAD_SIZE;
        if (self->dataChunk.header.id == WAV_DATA_CHUNK_ID) {
            if (write(self->fp, &self->dataChunk.header, sizeof(WavChunkHeader)) <= 0) {
                goto EXIT2;
            }
        }
    } else {
        memset_s(g_buffer, WAV_MARKERS_ALL_SIZE, 0xFF, WAV_MARKERS_ALL_SIZE);
        if (write(self->fp, g_buffer, WAV_MARKERS_ALL_SIZE) <= 0) {
            goto EXIT2;
        }
        if (self->dataChunk.header.size < WAV_MARKERS_ALL_SIZE) {
            self->dataChunk.header.size += WAV_MARKERS_ALL_SIZE - WAV_DATA_HEAD_SIZE;
        }
        if (self->dataChunk.header.id == WAV_DATA_CHUNK_ID) {
            if (lseek(self->fp, size, SEEK_SET) < 0) {
                goto EXIT1;
            }
            if (write(self->fp, &self->dataChunk.header, sizeof(WavChunkHeader)) <= 0) {
                goto EXIT2;
            }
        }
        if (lseek(self->fp, size + WAV_MARKERS_ALL_SIZE, SEEK_SET) < 0) {
            goto EXIT1;
        }
    }
    return;
EXIT1:
    MEDIA_HAL_LOGE(MODULE_NAME, "lseek() failed [errno %d: %s]", errno, strerror(errno));
    return;
EXIT2:
    MEDIA_HAL_LOGE(MODULE_NAME, "Error while writing to %s [errno %d: %s]",
        self->filename, errno, strerror(errno));
    return;
}

void WavWriteHeader(WavFile* self)
{
    self->riffChunk.size =
        (uint32_t)sizeof(self->riffChunk.waveId) +
        (self->formatChunk.header.id == WAV_FORMAT_CHUNK_ID ?
            (sizeof(WavChunkHeader) + self->formatChunk.header.size) : 0) +
        (self->factChunk.header.id == WAV_FACT_CHUNK_ID ? (sizeof(WavChunkHeader) + self->factChunk.header.size) : 0) +
        (self->dataChunk.header.id == WAV_DATA_CHUNK_ID ? (sizeof(WavChunkHeader) + self->dataChunk.header.size) : 0);
    self->riffChunk.size += sizeof(CueMarker) * WAV_MARKERS_BUFFER_SIZE + 0xC;
    if (lseek(self->fp, 0, SEEK_SET) < 0) {
        goto EXIT1;
    }
    if ((write(self->fp, &self->riffChunk, sizeof(WavChunkHeader) + 0x4)) <= 0) {
        goto EXIT2;
    }
    if (self->formatChunk.header.id == WAV_FORMAT_CHUNK_ID) {
        if (lseek(self->fp, (long)(self->formatChunk.offset - sizeof(WavChunkHeader)), SEEK_SET) < 0) {
            goto EXIT1;
        }
        if (write(self->fp, &self->formatChunk.header, sizeof(WavChunkHeader)) <= 0) {
            goto EXIT2;
        }
        if (write(self->fp, &self->formatChunk.body, self->formatChunk.header.size) <= 0) {
            goto EXIT2;
        }
    }
    if (self->factChunk.header.id == WAV_FACT_CHUNK_ID) {
        if (lseek(self->fp, (long)(self->factChunk.offset - sizeof(WavChunkHeader)), SEEK_SET) < 0) {
            goto EXIT1;
        }
        if (write(self->fp, &self->factChunk.header, sizeof(WavChunkHeader)) <= 0) {
            goto EXIT2;
        }
        if (write(self->fp, &self->factChunk.body, self->factChunk.header.size) <= 0) {
            goto EXIT2;
        }
    }
    WavMarkAndDataInfo(self);
    return;
EXIT1:
    MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "lseek() failed [errno %d: %s]", errno, strerror(errno));
    return;
EXIT2:
    MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "Error while writing to %s [errno %d: %s]",
        self->filename, errno, strerror(errno));
    return;
}

void WavInitCueChunk(WavFile *self)
{
    self->cueChunk.cueFlag[0] = 'c';
    self->cueChunk.cueFlag[0x1] = 'u';
    self->cueChunk.cueFlag[0x2] = 'e';
    self->cueChunk.cueFlag[0x3] = ' ';
    self->cueChunk.cueSize = 0;
    self->cueChunk.numMarkers = 0;
}

uint32_t GetFileOpenMode(uint32_t mode)
{
    uint32_t openMode = O_RDWR | O_CREAT | O_TRUNC;
    if (mode & WAV_OPEN_READ) {
        if (!(mode & WAV_OPEN_WRITE)) {
            openMode = O_RDONLY;
        }
    } else {
        if (!(mode & WAV_OPEN_WRITE)) {
            MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "Invalid mode");
            return 0;
        }
    }
    return openMode;
}

int32_t WavInit(WavFile* self, const char* filename, uint32_t mode)
{
    if (memset_s(self, sizeof(WavFile), 0, sizeof(WavFile)) != EOK) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "memset_s failed");
        return MEDIA_HAL_ERR;
    }
    uint32_t openMode = GetFileOpenMode(mode);
    self->fp = open(filename, openMode, FILE_MODEL);
    if (self->fp < 0) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "Error when opening %s [errno %d: %s]",
            filename, errno, strerror(errno));
        return MEDIA_HAL_ERR;
    }
    self->filename = WavStrdup(filename);
    self->mode = mode;
    self->riffChunk.id = WAV_RIFF_CHUNK_ID;
    /* self->chunk.size = calculated by WavWriteHeader */
    self->riffChunk.waveId = WAV_WAVE_ID;
    self->riffChunk.offset = (uint64_t)sizeof(WavChunkHeader) + 0x4;

    self->formatChunk.header.id                = WAV_FORMAT_CHUNK_ID;
    self->formatChunk.header.size = (uint32_t)((unsigned long)&self->formatChunk.body.extSize -
        (unsigned long)&self->formatChunk.body);
    self->formatChunk.offset                   = self->riffChunk.offset + (uint64_t)sizeof(WavChunkHeader);
    self->formatChunk.body.formatTag          = WAV_FORMAT_PCM;
    self->formatChunk.body.channelCounts        = 1;
    self->formatChunk.body.sampleRate         = WAV_DEFAULTE_SAMPLERATE;
    self->formatChunk.body.avgBytesPerSec   = WAV_DEFAULTE_SAMPLERATE * 0x4;
    self->formatChunk.body.blockAlign         = 0x4;
    self->formatChunk.body.bitsPerSample     = 0x20;

    if (memcpy_s(self->formatChunk.body.subFormat, WAV_SUB_FORMAT_LEN, DEFAULT_SUB_FORMAT, WAV_SUB_FORMAT_LEN) != EOK) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "memcpy_s subFormat failed");
        return MEDIA_HAL_ERR;
    }
    self->dataChunk.header.id = WAV_DATA_CHUNK_ID;
    self->dataChunk.offset = self->formatChunk.offset + self->formatChunk.header.size +
        (uint64_t)sizeof(WavChunkHeader);
    WavInitCueChunk(self);
    return MEDIA_HAL_OK;
}

void WavFinalize(WavFile* self)
{
    WavFree(self->filename);
    if (self->fp < 0) {
        return;
    }

    int32_t ret = close(self->fp);
    self->fp = -1; /* cannot open when File Descriptor equals -1 */
    if (ret != 0) {
        return;
    }
}

WavFile* WavOpen(const char* filename, uint32_t mode)
{
    WavFile* self = WavMalloc(sizeof(WavFile));
    if (self == NULL) {
        return NULL;
    }

    int32_t ret = WavInit(self, filename, mode);
    if (ret != MEDIA_HAL_OK) {
        WavFree(self);
        return NULL;
    }
    return self;
}

void WavClose(WavFile* self)
{
    WavFinalize(self);
    WavFree(self);
}

static size_t WavGetSampleSize(const WavFile *self)
{
    return self->formatChunk.body.blockAlign / self->formatChunk.body.channelCounts;
}
 
size_t WavWrite(WavFile* self, const void *buffer, size_t count)
{
    size_t writeCount = 0;
    uint32_t channels = WavGetNumChannels(self);
    size_t sampleSize = WavGetSampleSize(self);
    if (!(self->mode & WAV_OPEN_WRITE)) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "This WavFile is not writable");
        return 0;
    }
    if (self->formatChunk.body.formatTag == WAV_FORMAT_EXTENSIBLE) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "Extensible format is not supported");
        return 0;
    }
    if (count == 0) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "the wite count is 0!");
        return 0;
    }
    writeCount = write(self->fp, buffer, channels * count);
    if (writeCount <= 0) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "Error when writing to %s [errno %d: %s]",
            self->filename, errno, strerror(errno));
        return 0;
    }
    self->riffChunk.size += writeCount;
    if (self->factChunk.header.id == WAV_FACT_CHUNK_ID) {
        self->factChunk.body.sampleLength += writeCount / channels;
    }
    self->dataChunk.header.size += writeCount;
    return writeCount;
}

void WavSetSampleSize(WavFile *self, size_t sampleSize)
{
    if (!(self->mode & WAV_OPEN_WRITE) && !((self->mode & WAV_OPEN_APPEND) &&
        self->isANewFile && self->dataChunk.header.size == 0)) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "This WavFile is not writable");
        return;
    }

    if (sampleSize < 1) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "Invalid sample size: %zu", sampleSize);
        return;
    }

    self->formatChunk.body.blockAlign = (uint16_t)(sampleSize * self->formatChunk.body.channelCounts);
    self->formatChunk.body.avgBytesPerSec = self->formatChunk.body.blockAlign * self->formatChunk.body.sampleRate;
    self->formatChunk.body.bitsPerSample = (uint16_t)(sampleSize * 0x8);
    if (self->formatChunk.body.formatTag == WAV_FORMAT_EXTENSIBLE) {
        self->formatChunk.body.validBitsPerSample = (uint16_t)(sampleSize * 0x8);
    }
}

void WavSetFormat(WavFile* self, uint16_t format)
{
    if (!(self->mode & WAV_OPEN_WRITE) && !((self->mode & WAV_OPEN_APPEND) &&
        self->isANewFile && self->dataChunk.header.size == 0)) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "This WavFile is not writable");
        return;
    }

    if (format == self->formatChunk.body.formatTag) {
        return;
    }

    self->formatChunk.body.formatTag = format;
    if (format != WAV_FORMAT_PCM && format != WAV_FORMAT_EXTENSIBLE) {
        self->formatChunk.body.extSize = 0;
        self->formatChunk.header.size =
        (uint32_t)((unsigned long)&self->formatChunk.body.extSize - (unsigned long)&self->formatChunk.body);
    } else if (format == WAV_FORMAT_EXTENSIBLE) {
        self->formatChunk.body.extSize = 0x16;
        self->formatChunk.header.size = (uint32_t)sizeof(WavFormatChunk) - (uint32_t)sizeof(WavChunkHeader);
    }

    if (format == WAV_FORMAT_ALAW || format == WAV_FORMAT_MULAW || format == WAV_FORMAT_726) {
        uint16_t sampleSize = WavGetSampleSize(self);
        if (sampleSize != 1) {
            WavSetSampleSize(self, 1);
        }
    } else if (format == WAV_FORMAT_IEEE_FLOAT) {
        uint16_t sampleSize = WavGetSampleSize(self);
        if (sampleSize != 0x4 && sampleSize != 0x8) {
            WavSetSampleSize(self, 0x4);
        }
    }
}

void WavSetNumChannels(WavFile* self, uint16_t channelCounts)
{
    if (!(self->mode & WAV_OPEN_WRITE) && !((self->mode & WAV_OPEN_APPEND) &&
        self->isANewFile && self->dataChunk.header.size == 0)) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "This WavFile is not writable");
        return;
    }

    if (channelCounts < 1) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "Invalid number of channels: %u", channelCounts);
        return;
    }

    uint16_t oldChannelCounts = self->formatChunk.body.channelCounts;
    if (channelCounts == oldChannelCounts) {
        return;
    }

    self->formatChunk.body.channelCounts = channelCounts;
    self->formatChunk.body.blockAlign = self->formatChunk.body.blockAlign / oldChannelCounts * channelCounts;
    self->formatChunk.body.avgBytesPerSec = self->formatChunk.body.blockAlign * self->formatChunk.body.sampleRate;
}

void WavSetSampleRate(WavFile* self, uint32_t sampleRate)
{
    if (!(self->mode & WAV_OPEN_WRITE) && !((self->mode & WAV_OPEN_APPEND) &&
        self->isANewFile && self->dataChunk.header.size == 0)) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "This WavFile is not writable");
        return;
    }

    if (sampleRate == self->formatChunk.body.sampleRate) {
        return;
    }

    self->formatChunk.body.sampleRate = sampleRate;
    self->formatChunk.body.avgBytesPerSec = self->formatChunk.body.blockAlign * self->formatChunk.body.sampleRate;
}

uint32_t WavGetSampleRate(WavFile *self)
{
    return self->formatChunk.body.sampleRate;
}

uint32_t WavGetNumChannels(WavFile *self)
{
    return self->formatChunk.body.channelCounts;
}

void WavSetValidBitsPerSample(WavFile* self, uint16_t bits)
{
    if (!(self->mode & WAV_OPEN_WRITE) && !((self->mode & WAV_OPEN_APPEND) &&
        self->isANewFile && self->dataChunk.header.size == 0)) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "This WavFile is not writable");
        return;
    }

    if (bits < 1 || bits > 0x8 * self->formatChunk.body.blockAlign / self->formatChunk.body.channelCounts) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "Invalid ValidBitsPerSample: %u", bits);
        return;
    }

    if ((self->formatChunk.body.formatTag == WAV_FORMAT_ALAW || self->formatChunk.body.formatTag == WAV_FORMAT_MULAW) &&
        bits != 0x8) {
        MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "Invalid ValidBitsPerSample: %u", bits);
        return;
    }

    if (self->formatChunk.body.formatTag != WAV_FORMAT_EXTENSIBLE) {
        self->formatChunk.body.bitsPerSample = bits;
        self->formatChunk.body.blockAlign = bits / 0x8 * self->formatChunk.body.channelCounts;
        self->formatChunk.body.avgBytesPerSec = self->formatChunk.body.blockAlign * self->formatChunk.body.sampleRate;
    } else {
        self->formatChunk.body.bitsPerSample =
            0x8 * self->formatChunk.body.blockAlign / self->formatChunk.body.channelCounts;
        self->formatChunk.body.validBitsPerSample = bits;
    }
}

int32_t WavRecordMarks(WavFile *self, uint32_t marks)
{
    self->cueChunk.numMarkers++;
    if (self->cueChunk.numMarkers <= WAV_MARKERS_BUFFER_SIZE) {
        int16_t index = self->cueChunk.numMarkers - 1;
        self->cueChunk.markers[index].id = self->cueChunk.numMarkers;
        self->cueChunk.markers[index].position = marks;
        self->cueChunk.markers[index].chunkId = 0x61746164;
        self->cueChunk.markers[index].chunkStart = 0;
        self->cueChunk.markers[index].blockStart = 0;
        self->cueChunk.markers[index].sampleOffset = marks;
        return MEDIA_HAL_OK;
    }
    self->cueChunk.numMarkers--;
    MediaHalLogPrintf(MODULE_NAME, MEDIA_HAL_LOG_LEVEL_ERR, "The number of markers exceeds the limit[%d]",
        WAV_MARKERS_BUFFER_SIZE);
    return MEDIA_HAL_ERR;
}