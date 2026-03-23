/*
 * Copyright (c) @CompanyNameMagicTag. 2024-2024. All rights reserved.
 * Description: Data decryption implementation
 * Author: Media Software Group
 * Create: 2024-11-01
 */

#include "decrypt_data.h"
#include "audio_decrypt.h"
#include "media_hal_common.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define MODULE_NAME "DecryptData"

 static int32_t InitDecryptBuf(DecryptBufContext *bufContext)
{
    CHK_NULL_RETURN(bufContext, MEDIA_HAL_INVALID_PARAM, "context is NULL!");
    if (bufContext->readBuf == NULL) {
        bufContext->readBuf = (uint8_t *)MediaMallocz(bufContext->decryptSize);
        if (bufContext->readBuf == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed !");
            return MEDIA_HAL_NO_MEM;
        }
    }
    if (bufContext->decryptBuf == NULL) {
        bufContext->decryptBuf = (uint8_t *)MediaMallocz(bufContext->decryptSize);
        if (bufContext->decryptBuf == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed !");
            free(bufContext->readBuf);
            return MEDIA_HAL_NO_MEM;
        }
    }
    bufContext->pos = -1;
    return MEDIA_HAL_OK;
}

int32_t DeInitDecrypt(void *context)
{
    CHK_NULL_RETURN(context, MEDIA_HAL_INVALID_PARAM, "context is NULL!");
    DecryptBufContext *bufContext = (DecryptBufContext *)context;
    if (bufContext->decryptBuf != NULL) {
        free(bufContext->decryptBuf);
        bufContext->decryptBuf = NULL;
    }
    if (bufContext->readBuf != NULL) {
        free(bufContext->readBuf);
        bufContext->readBuf = NULL;
    }
    bufContext->decryptSize = 0;
    bufContext->pos = -1;
    bufContext->isNeedDecryptData = false;
    AUDIO_UnLoadDecryptLibrary();
    return MEDIA_HAL_OK;
}

bool IsDecrypt(void *context)
{
    if (context == NULL) {
        return false;
    }
    DecryptBufContext *bufContext = (DecryptBufContext *)context;
    return bufContext->isNeedDecryptData;
}

int32_t InitDecrypt(void *context, const char *url)
{
    CHK_NULL_RETURN(context, MEDIA_HAL_INVALID_PARAM, "context is NULL!");
    DecryptBufContext *bufContext = (DecryptBufContext *)context;
    bufContext->isNeedDecryptData = AUDIO_IsEncrypted(url);
    if (bufContext->isNeedDecryptData) {
        bufContext->decryptSize = AUDIO_GetDecryptUnitLength();
        if (InitDecryptBuf(bufContext) != MEDIA_HAL_OK) {
            return MEDIA_HAL_INVALID_PARAM;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t DecryptReadDataInner(int32_t fd, DecryptBufContext *bufContext, int32_t *readSize)
{
    int32_t ret = read(fd, bufContext->readBuf, (size_t)bufContext->decryptSize);
    if (ret < 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "read file failed! ret = %d", ret);
        *readSize = ret;
        return MEDIA_HAL_ERR;
    }
    *readSize = ret;
    if (ret == 0) {
        return ret;
    }
    ret = AUDIO_DecryptFunction(bufContext->readBuf, *readSize,
        bufContext->decryptBuf, *readSize);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ret = %d, read size = %d", ret, *readSize);
        return MEDIA_HAL_ERR;
    }
    bufContext->decryptSize = ret;
    return MEDIA_HAL_OK;
}

int32_t DecryptReadData(int32_t fd, void *context, uint8_t *buf, int32_t size)
{
    CHK_NULL_RETURN(context, MEDIA_HAL_INVALID_PARAM, "context is NULL!");
    DecryptBufContext *bufContext = (DecryptBufContext *)context;
    uint32_t readNum = (size / bufContext->decryptSize);
    if ((size % bufContext->decryptSize) > 0) {
        readNum += 1;
    }
    int32_t readSize = -1;
    // If there is no data in the buffer, the number of bytes required for decoding is read.
    if (bufContext->pos < 0) {
        int32_t ret = DecryptReadDataInner(fd, bufContext, &readSize);
        if (readSize == 0) {
            return readSize;
        }
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "decrypt read data failed!");
        bufContext->pos = 0; // Start position of the buffer pointer
        readNum -= 1;
    }
    if (bufContext->pos + size <= bufContext->decryptSize) {
        int32_t ret = memcpy_s(buf, size, bufContext->decryptBuf, bufContext->decryptSize);
        CHK_FAILED_RETURN(ret, EOK, ret, "memcpy_s failed!");
        bufContext->pos += size;
    } else {
        // Length of the data that can still be obtained
        int32_t availableSize = bufContext->decryptSize - bufContext->pos;
        // Send the remaining data to the buffer.
        int32_t ret = memcpy_s(buf, size, bufContext->decryptBuf + bufContext->pos, availableSize);
        CHK_FAILED_RETURN(ret, EOK, ret, "memcpy_s failed!");
        for (size_t i = 0; i < readNum; i++) {
            int32_t replenishSize = size - availableSize; // Required data length
            if (replenishSize == 0) {
                break;
            }
            ret = DecryptReadDataInner(fd, bufContext, &readSize);
            if (readSize == 0) {
                return readSize;
            }
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "decrypt read data failed!");
            if (replenishSize < readSize) {
                readSize = replenishSize;
            }
            // Copy the newly read data to the buffer.
            ret = memcpy_s(buf + availableSize, replenishSize, bufContext->decryptBuf, readSize);
            CHK_FAILED_RETURN(ret, EOK, ret, "memcpy_s failed!");
            availableSize += readSize;
            if (replenishSize > bufContext->decryptSize) {
                bufContext->pos = 0;
            } else {
                bufContext->pos = replenishSize;
            }
        }
    }
    return size;
}

static int64_t DecryptSeekDataInner(int32_t fd, DecryptBufContext *bufContext,
    uint64_t seekPos, const int32_t whence)
{
    uint64_t restPos = 0;
    if (seekPos > AUDIO_GetDecryptUnitLength()) {
        // Number of bytes that exceed the length required for decryption.
        restPos = seekPos % AUDIO_GetDecryptUnitLength();
    }
    // seek to an integer multiple of the decrypted byte
    int32_t ret = lseek(fd, (off_t)seekPos - restPos, SEEK_SET);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "errno = %d, seekPos = %llu", errno, seekPos - restPos);
        return MEDIA_HAL_ERR;
    }
    int32_t readSize = -1;
    // Read the number of bytes to be decrypted from the current position and decrypt the bytes.
    ret = DecryptReadDataInner(fd, bufContext, &readSize);
    if (readSize == 0) {
        return readSize;
    }
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "decrypt read data failed!");
    bufContext->pos = (int32_t)restPos;
    MEDIA_HAL_LOGI(MODULE_NAME, "restPos = %d", bufContext->pos);
    return seekPos;
}

int64_t DecryptSeekData(int32_t fd, void *context, uint64_t seekPos, const int32_t whence)
{
    CHK_NULL_RETURN(context, MEDIA_HAL_INVALID_PARAM, "context is NULL!");
    if (whence == SEEK_END) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support SEEK_END type!");
        return MEDIA_HAL_INVALID_PARAM;
    }
    DecryptBufContext *bufContext = (DecryptBufContext *)context;
    MEDIA_HAL_LOGI(MODULE_NAME, "seekPos = %llu, whence = %d", seekPos, whence);
    int64_t ret = 0;
    int64_t currentPosition = lseek(fd, 0, SEEK_CUR);
    if (currentPosition == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "errno = %d, seekPos = %llu", errno, seekPos);
        return MEDIA_HAL_ERR;
    }
    if (whence == SEEK_CUR) {
        seekPos = currentPosition + seekPos;
    }
    // Obtains the real position of the file pointer.
    int64_t realPos = currentPosition - (bufContext->decryptSize - bufContext->pos);
    if (seekPos >= realPos) { // Seek backwards
        int64_t seekSize = (seekPos - currentPosition); // Length of the seek request.
        int64_t availableSize = bufContext->decryptSize - (bufContext->decryptSize - bufContext->pos);
        if (seekSize <= availableSize) { // If the buffer length is sufficient for seeking
            bufContext->pos += (int32_t)seekSize; // Seeking in the buffer
        } else {
            // If not, clear the buffer and seek to the corresponding position.
            ret = DecryptSeekDataInner(fd, bufContext, seekPos, whence);
        }
    } else { // Seek forward
        int64_t seekSize = (currentPosition - seekPos);
        int64_t availableSize = bufContext->pos; // Length that can be sought
        if (seekSize <= availableSize) {  // Seeking in the buffer
            bufContext->pos -= seekSize;
        } else {
            // Seeking outside the buffer
            ret = DecryptSeekDataInner(fd, bufContext, seekPos, whence);
        }
    }
    return (ret < 0) ? (-errno) : ret;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif