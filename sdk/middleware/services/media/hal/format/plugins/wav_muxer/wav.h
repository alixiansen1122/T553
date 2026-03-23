/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: wav file
 * Author: Media Software Group
 * Create: 2025-03-10
 */
#ifndef WAV_H
#define WAV_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
enum {
    WAV_FALSE,
    WAV_TRUE
};

/* wave file format codes */
#define WAV_FORMAT_PCM          ((uint16_t)0x0001)
#define WAV_FORMAT_IEEE_FLOAT   ((uint16_t)0x0003)
#define WAV_FORMAT_ALAW         ((uint16_t)0x0006)
#define WAV_FORMAT_MULAW        ((uint16_t)0x0007)
#define WAV_FORMAT_726          ((uint16_t)0x0008)
#define WAV_FORMAT_EXTENSIBLE   ((uint16_t)0xfffe)

typedef enum {
    WAV_OK,         /** no error */
    WAV_ERR_OS,     /** error when {wave} called a stdio function */
    WAV_ERR_FORMAT, /** not a wave file or unsupported wave format */
    WAV_ERR_MODE,   /** incorrect mode when opening the wave file or calling mode-specific API */
    WAV_ERR_PARAM,  /** incorrect parameter passed to the API function */
} WavErrCode;

typedef struct {
    WavErrCode  code;
    char*       message;
    int         isLiteral;
} WavErrInfo;

typedef struct {
    void*   (*malloc)(void *context, size_t size);
    void    (*free)(void *context, void *p);
} WavAllocFuncs;

void WavSetAllocator(void *context, const WavAllocFuncs *funcs);

void* WavMalloc(size_t size);
void WavFree(void *p);

char* WavStrdup(const char *str);

#define WAV_OPEN_READ       1
#define WAV_OPEN_WRITE      2
#define WAV_OPEN_APPEND     4

typedef struct WavFileInfo WavFile;

WavFile* WavOpen(const char* filename, uint32_t mode);
void     WavClose(WavFile* self);
void WavWriteHeader(WavFile *self);
size_t WavWrite(WavFile* self, const void *buffer, size_t count);
void WavSetSampleSize(WavFile *self, size_t sampleSize);
void WavSetFormat(WavFile* self, uint16_t format);
void WavSetSampleRate(WavFile *self, uint32_t sampleRate);
uint32_t WavGetSampleRate(WavFile *self);
void WavSetNumChannels(WavFile* self, uint16_t channelCounts);
uint32_t WavGetNumChannels(WavFile *self);
void WavSetValidBitsPerSample(WavFile* self, uint16_t bits);
int32_t WavRecordMarks(WavFile *self, uint32_t marks);

#ifdef __cplusplus
}
#endif
#endif
