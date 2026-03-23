
#include "latm_simple_muxer.h"
#include "media_hal_common.h"

#define MODULE_NAME "LatmSimpleMuxer"

#define LATM_DEBUG 1
#define MAX_BYTES_IN_HEADER 30

#define ONE_BIT 1
#define TWO_BIT 2
#define THREE_BIT 3
#define FOUR_BIT 4
#define FIVE_BIT 5
#define SIX_BIT 6
#define SEVEN_BIT 7
#define BIT_NUM_OF_THREE_BYTE 24

#define THREE_BYTE 3
#define FOUR_BYTE 4

#define AV_WB32(p, val) do {                 \
        uint32_t d = (val);                  \
        ((uint8_t*)(p))[3] = (uint8_t)(d);               \
        ((uint8_t*)(p))[2] = (uint8_t)((d)>>8);          \
        ((uint8_t*)(p))[1] = (uint8_t)((d)>>16);         \
        ((uint8_t*)(p))[0] = (uint8_t)((d)>>24);         \
    } while (0)

typedef struct PutMultiBitContext {
    uint32_t bitBuf; // for cache
    uint32_t bitLeft; // for cache
    uint8_t *buf;
    uint8_t *bufPtr;
    uint8_t *bufEnd;
    int32_t sizeInBits;
    uint32_t totalCountInBits;
} PutMultiBitContext;

/**
 * Initialize the PutMultiBitContext s.
 *
 * @param buffer the buffer where to put bits
 * @param buffer_size the size in bytes of buffer
 */
static void InitPutBits(PutMultiBitContext *s, uint8_t *buffer, int32_t bufferSize)
{
    s->sizeInBits = BIT_NUM_ONE_BYTE * bufferSize;
    s->buf          = buffer;
    s->bufEnd      = s->buf + bufferSize;
    s->bufPtr      = s->buf;
    s->bitLeft     = BIT_NUM_OF_INT;
    s->bitBuf      = 0;
    s->totalCountInBits = 0;
}

/**
 * Write up to 31 bits into a bitstream.
 * Use put_bits32 to write 32 bits.
 */
static bool PutBits(PutMultiBitContext *s, uint32_t n, uint32_t value)
{
    uint32_t bitBuf;
    uint32_t bitLeft;
    if (n > BIT_NUM_OF_INT || value >= (1U << n)) {
        return false;
    }

    bitBuf  = s->bitBuf;
    bitLeft = s->bitLeft;

    /* XXX: optimize */
#ifdef BITSTREAM_WRITER_LE
    bitBuf |= value << (BIT_NUM_OF_INT - bitLeft);
    if (n >= bitLeft) {
        if (THREE_BYTE < s->bufEnd - s->bufPtr) {
            AV_WL32(s->bufPtr, bitBuf);
            s->bufPtr += FOUR_BYTE;
        } else {
            return false;
        }
        bitBuf     = value >> bitLeft;
        bitLeft   += BIT_NUM_OF_INT;
    }
    bitLeft -= n;
#else // now
    if (n < bitLeft) {
        bitBuf     = (bitBuf << n) | value;
        bitLeft   -= n;
        s->totalCountInBits += n;
    } else {
        bitBuf   <<= bitLeft;
        bitBuf    |= value >> (n - bitLeft);
        if (s->bufEnd - s->bufPtr > THREE_BYTE) {
            AV_WB32(s->bufPtr, bitBuf);
            s->bufPtr += FOUR_BYTE;
        } else {
            return false;
        }
        s->totalCountInBits += n;
        bitLeft   += BIT_NUM_OF_INT - n;
        bitBuf     = value;
    }
#endif

    s->bitBuf  = bitBuf;
    s->bitLeft = bitLeft;
    return true;
}

/**
 * Pad the end of the output stream with zeros.
 */
// flush 后，会凑整，即如果最后不是整字节，会凑成整字节
// 因为，s->bitBuf 搬移到 目的地的时候，以字节为单位搬移的，以0凑整的
// 例如当前真实设置的 bits 是 10B+6b，6b会升成1B，因此bits就是11B=88b
static void FlushPutBits(PutMultiBitContext *s)
{
#ifndef BITSTREAM_WRITER_LE
    if (s->bitLeft < BIT_NUM_OF_INT) { // now
        s->bitBuf <<= s->bitLeft;
    }
#endif
    while (s->bitLeft < BIT_NUM_OF_INT) {
        if (s->bufPtr >= s->bufEnd) {
        }
#ifdef BITSTREAM_WRITER_LE
        *s->bufPtr++ = s->bitBuf;
        s->bitBuf  >>= BIT_NUM_ONE_BYTE;
#else
        *s->bufPtr++ = (uint8_t)(s->bitBuf >> BIT_NUM_OF_THREE_BYTE); // now
        s->bitBuf  <<= BIT_NUM_ONE_BYTE;
#endif
        s->bitLeft  += BIT_NUM_ONE_BYTE;
    }
    s->bitLeft = BIT_NUM_OF_INT;
    s->bitBuf  = 0;
}

/**
 * @return the total number of bits written to the bitstream.
 */
// flush前调用 和 flush后调用 是不同的，flush前是准确的
// 原因在于 flush 搬移基本单元是字节
static inline uint32_t PutBitSCount(PutMultiBitContext *s)
{
    return (s->bufPtr - s->buf) * BIT_NUM_ONE_BYTE + BIT_NUM_OF_INT - s->bitLeft; // flush 后，会凑整
}

static void LatmWriteFrameHeader12Byte(PutMultiBitContext *bs, const AudioBasicParam *audioparam)
{
    /* AudioMuxElement */
    const unsigned int useSameStreamMux = 0;
    PutBits(bs, ONE_BIT, useSameStreamMux); /* useSameStreamMux */
    if (useSameStreamMux == 0) {
        /* StreamMuxConfig */
        PutBits(bs, ONE_BIT, 1); /* audioMuxVersion */
        PutBits(bs, ONE_BIT, 0); /* audioMuxVersionA */
        /* LatmGetValue */
        PutBits(bs, TWO_BIT, 0); /* bytesForValue */
        PutBits(bs, BIT_NUM_ONE_BYTE, 0xff); /* valueTmp */
        PutBits(bs, ONE_BIT, 1); /* allStreamsSameTimeFraming */
        PutBits(bs, SIX_BIT, 0); /* numSubFrames */
        PutBits(bs, FOUR_BIT, 0); /* numProgram */
        PutBits(bs, THREE_BIT, 0); /* numLayer */
        /* LatmGetValue */
        uint32_t ascLen = 0x16;
        PutBits(bs, TWO_BIT, 0); /* bytesForValue */
        PutBits(bs, BIT_NUM_ONE_BYTE, ascLen); /* valueTmp */
        /* AudioSpecificConfig */
        PutBits(bs, FIVE_BIT, audioparam->audioObjType);
        PutBits(bs, FOUR_BIT, audioparam->sampleFreqIndex);
        PutBits(bs, FOUR_BIT, audioparam->channelConfig);
        // GASpecificConfig
        PutBits(bs, ONE_BIT, 0); // frame length - 1024 samples
        PutBits(bs, ONE_BIT, 0); // does not depend on core coder
        PutBits(bs, ONE_BIT, 0); // is not extension
        /* fillbits */
        const uint32_t bitNumOfAudioSpecificConfig = 16;
        ascLen -= bitNumOfAudioSpecificConfig;
        PutBits(bs, ascLen, 0);
        PutBits(bs, THREE_BIT, 0); /* frameLengthType */
        PutBits(bs, BIT_NUM_ONE_BYTE, 0xC0); /* latmBufferFullness */
        PutBits(bs, ONE_BIT, 0); /* otherDataPresent */
        PutBits(bs, ONE_BIT, 0); /* crcCheckPresent */
    }
    return;
}

// 根据参数形成 LATM 头
void ConstructLatmHeader12Byte(const AudioBasicParam *audioparam, uint32_t size,
    uint32_t *bitNumOfHeader, uint8_t *headerBuf, uint32_t headBufSize)
{
    PutMultiBitContext bs;
    InitPutBits(&bs, headerBuf, MAX_BYTES_IN_HEADER);
    LatmWriteFrameHeader12Byte(&bs, audioparam);

    /* PayloadLengthInfo() */
    int32_t i;
    for (i = 0; i <= (int32_t)size - MAX_OF_BYTE; i += MAX_OF_BYTE) {
        PutBits(&bs, BIT_NUM_ONE_BYTE, MAX_OF_BYTE);
    }
    PutBits(&bs, BIT_NUM_ONE_BYTE, size - i);

    uint32_t putBitBeforeFlush = PutBitSCount(&bs);
    FlushPutBits(&bs); // align
    uint32_t putBitAfterFlush = PutBitSCount(&bs);
    if (putBitBeforeFlush != putBitAfterFlush || putBitBeforeFlush % BIT_NUM_ONE_BYTE != 0) {
        MEDIA_HAL_LOGW(MODULE_NAME, "flush bef vs after not same: [%d]vs[%d] or not mliti of 8",
            putBitBeforeFlush, putBitAfterFlush);
    }
    MEDIA_HAL_UNUSED(headBufSize);

    *bitNumOfHeader = putBitBeforeFlush;
    return;
}

