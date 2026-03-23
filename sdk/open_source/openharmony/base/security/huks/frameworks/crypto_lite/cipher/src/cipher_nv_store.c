/*
 * Copyright (c) 2025-2030 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// ohos begin
// Add NV store
#include "cipher.h"
#include "cipher_log.h"
//#include "app_common.h"
#include <stdint.h>
#include <string.h>

// 位图大小计算（每个bit表示一个keyId）
#define BITMAP_SIZE ((KEY_ID_COUNT + 7) / 8)

// 使用一个特殊的keyId来存储位图（在密钥区间之前）
#define BITMAP_KEY_ID 0x26FF

// 全局位图变量
static uint8_t keyIdBitmap[BITMAP_SIZE] = {0};
static int bitmapInitialized = 0;

// 属性配置
static nv_key_attr_t nvAttr = {0, 1, 0, 0};

/**
 * @brief 初始化密钥ID位图
 * @return 0-成功，其他-失败
 */
static int32_t InitKeyIdBitmap(void)
{
    int32_t ret = 0;
    uint16_t readLen = 0;

    if (bitmapInitialized) {
        return 0;
    }

    // 尝试从NV读取位图
    ret = uapi_nv_read_with_attr(BITMAP_KEY_ID, BITMAP_SIZE, &readLen, keyIdBitmap, &nvAttr);
    if (ret != 0 || readLen != BITMAP_SIZE) {
        // 读取失败或长度不匹配，初始化位图为全0
        CIPHER_LOG_D("Initialize key_id bitmap for first use\n");
        memset_s(keyIdBitmap, BITMAP_SIZE, 0, BITMAP_SIZE);

        // 将初始化的位图保存到NV
        ret = uapi_nv_write_with_attr(BITMAP_KEY_ID, keyIdBitmap, BITMAP_SIZE, &nvAttr, NULL);
        if (ret != 0) {
            CIPHER_LOG_E("Failed to initialize key_id bitmap to NV\n");
            return ret;
        }
    }

    bitmapInitialized = 1;
    CIPHER_LOG_I("Key_id bitmap initialized successfully\n");
    return 0;
}

/**
 * @brief 保存位图到NV
 * @return 0-成功，其他-失败
 */
static int32_t SaveKeyIdBitmap(void)
{
    if (!bitmapInitialized) {
        CIPHER_LOG_E("Bitmap not initialized\n");
        return -1;
    }

    int32_t ret = uapi_nv_write_with_attr(BITMAP_KEY_ID, keyIdBitmap, BITMAP_SIZE, &nvAttr, NULL);
    if (ret != 0) {
        CIPHER_LOG_E("Failed to save key_id bitmap to NV\n");
    }
    return ret;
}

/**
 * @brief 检查keyId是否有效且在范围内
 * @param keyId 密钥ID
 * @return 1-有效，0-无效
 */
static int IsValidKeyId(uint16_t keyId)
{
    return (keyId >= KEY_ID_START && keyId < KEY_ID_END);
}

/**
 * @brief 设置keyId为已使用
 * @param keyId 密钥ID
 */
static void SetKeyIdUsed(uint16_t keyId)
{
    if (IsValidKeyId(keyId)) {
        uint16_t index = keyId - KEY_ID_START;
        keyIdBitmap[index / 8] |= (1 << (index % 8));
    }
}

/**
 * @brief 检查keyId是否已被使用
 * @param keyId 密钥ID
 * @return 1-已使用，0-未使用
 */
static int IsKeyIdUsed(uint16_t keyId)
{
    if (!IsValidKeyId(keyId)) {
        return 0;
    }
    uint16_t index = keyId - KEY_ID_START;
    return (keyIdBitmap[index / 8] >> (index % 8)) & 1;
}

/**
 * @brief 清除keyId的使用标记
 * @param keyId 密钥ID
 */
static void ClearKeyIdUsed(uint16_t keyId)
{
    if (IsValidKeyId(keyId)) {
        uint16_t index = keyId - KEY_ID_START;
        keyIdBitmap[index / 8] &= ~(1 << (index % 8));
    }
}

/**
 * @brief 保存密钥到NV存储
 * @param keyBuff 密钥数据缓冲区
 * @param keyBuffLen 密钥数据长度
 * @param keyId 密钥ID
 * @param overwrite 是否覆盖已存在的密钥
 * @return 0-成功，其他-失败
 */
int32_t InnerSaveKeyToNv(unsigned char *keyBuff, uint32_t keyBuffLen, uint16_t keyId, bool overwrite)
{
    int32_t ret = 0;

    // 初始化位图（如果未初始化）
    if (!bitmapInitialized) {
        ret = InitKeyIdBitmap();
        if (ret != 0) {
            CIPHER_LOG_E("Failed to initialize key_id bitmap\n");
            return ret;
        }
    }

    // 检查keyId是否有效
    if (!IsValidKeyId(keyId)) {
        CIPHER_LOG_E("Invalid keyId: 0x%04X, valid range: [0x%04X, 0x%04X)\n", 
                    keyId, KEY_ID_START, KEY_ID_END);
        return -1;
    }

    // 检查参数有效性
    if (keyBuff == NULL || keyBuffLen == 0) {
        CIPHER_LOG_E("Invalid key buffer or length\n");
        return -1;
    }

    // 检查keyId是否已被使用
    bool keyUsed = IsKeyIdUsed(keyId);
    if (keyUsed && !overwrite) {
        CIPHER_LOG_E("KeyId 0x%04X already used and overwrite not allowed\n", keyId);
        return -1;
    }

    // 保存密钥到NV
    ret = uapi_nv_write_with_attr(keyId, keyBuff, keyBuffLen, &nvAttr, NULL);
    if (ret != 0) {
        CIPHER_LOG_E("Failed to write key 0x%04X to nv, ret=%d\n", keyId, ret);
        return ret;
    }

    // 如果之前未使用，标记为已使用并保存位图
    if (!keyUsed) {
        SetKeyIdUsed(keyId);
        ret = SaveKeyIdBitmap();
        if (ret != 0) {
            CIPHER_LOG_E("Failed to save key_id bitmap after writing key 0x%04X\n", keyId);
        } else {
            CIPHER_LOG_D("Key 0x%04X saved successfully (new)\n", keyId);
        }
    } else {
        CIPHER_LOG_I("Key 0x%04X saved successfully (overwritten)\n", keyId);
    }

    return ret;
}

/**
 * @brief 从NV读取密钥
 * @param keyId 密钥ID
 * @param data 输出缓冲区
 * @return 0-成功，其他-失败
 */
int32_t InnerGetKeyFromNv(uint16_t keyId, uint8_t *data, uint16_t *actualKeyLen)
{
    int32_t ret = 0;
    uint16_t keyLen = 0;

    // 初始化位图（如果未初始化）
    if (!bitmapInitialized) {
        ret = InitKeyIdBitmap();
        if (ret != 0) {
            CIPHER_LOG_E("Failed to initialize key_id bitmap\n");
            return ret;
        }
    }

    // 检查keyId是否有效
    if (!IsValidKeyId(keyId)) {
        CIPHER_LOG_E("Invalid keyId: 0x%04X\n", keyId);
        return -1;
    }

    // 检查keyId是否已被使用
    if (!IsKeyIdUsed(keyId)) {
        CIPHER_LOG_E("KeyId 0x%04X not exists\n", keyId);
        return -1;
    }

    // 检查输出缓冲区
    if (data == NULL || actualKeyLen == NULL) {
        CIPHER_LOG_E("Invalid output buffer\n");
        return -1;
    }

    // 从NV读取密钥
    ret = uapi_nv_read_with_attr(keyId, UT_NV_MAX_LENGTH, &keyLen, data, &nvAttr);
    if (ret != 0) {
        CIPHER_LOG_E("Failed to read key 0x%04X from nv, ret=%d\n", keyId, ret);

        // 如果读取失败，可能密钥数据已损坏，清除使用标记
        ClearKeyIdUsed(keyId);
        SaveKeyIdBitmap(); // 尝试保存更新后的位图
    } else {
        CIPHER_LOG_I("Key 0x%04X read successfully, length=%d\n", keyId, keyLen);
        *actualKeyLen = keyLen; // 设置实际长度
    }

    return ret;
}

/**
 * @brief 从NV删除密钥
 * @param keyId 密钥ID
 * @return 0-成功，其他-失败
 */
int32_t InnerDeleteKeyFromNv(uint16_t keyId)
{
    int32_t ret = 0;
    uint16_t keyLen = 0;
    // 初始化位图（如果未初始化）
    if (!bitmapInitialized) {
        ret = InitKeyIdBitmap();
        if (ret != 0) {
            CIPHER_LOG_E("Failed to initialize key_id bitmap\n");
            return ret;
        }
    }

    // 检查keyId是否有效
    if (!IsValidKeyId(keyId)) {
        CIPHER_LOG_E("Invalid keyId: 0x%04X\n", keyId);
        return -1;
    }

    // 检查keyId是否已被使用
    if (!IsKeyIdUsed(keyId)) {
        CIPHER_LOG_E("KeyId 0x%04X not exists\n", keyId);
        return -1;
    }
    uint8_t *tempBuffer = (uint8_t*)APPV_MALLOC(UT_NV_MAX_LENGTH);
    if (tempBuffer == NULL) {
        CIPHER_LOG_E("Failed to allocate memory for temp deletion data\n");
        return -1;
    }
    ret = uapi_nv_read_with_attr(keyId, UT_NV_MAX_LENGTH, &keyLen, tempBuffer, &nvAttr);
    if (ret != 0) {
        APPV_FREE(tempBuffer);
        tempBuffer = NULL;
        CIPHER_LOG_E("Failed to read key length for deletion 0x%04X, ret=%X\n", keyId, ret);
        return -1;
    }
    APPV_FREE(tempBuffer);
    tempBuffer = NULL;
    uint8_t *zeroData = (uint8_t*)APPV_MALLOC(keyLen);
    if (zeroData == NULL) {
        CIPHER_LOG_E("Failed to allocate memory for deletion data\n");
        return -1;
    }

    // 用0xf填充数据
    memset_s(zeroData, keyLen, 0xFF, keyLen);
    // 删除密钥：覆盖原始数据
    ret = uapi_nv_write_with_attr(keyId, zeroData, keyLen, &nvAttr, NULL);
    if (ret != 0) {
        APPV_FREE(zeroData);
        zeroData = NULL;
        CIPHER_LOG_E("Failed to delete key 0x%04X from nv, ret=%d\n", keyId, ret);
        return ret;
    }
    APPV_FREE(zeroData);
    zeroData = NULL;
    // 清除使用标记并保存位图
    ClearKeyIdUsed(keyId);
    ret = SaveKeyIdBitmap();
    if (ret != 0) {
        CIPHER_LOG_E("Failed to save key_id bitmap after deleting key 0x%04X\n", keyId);
    } else {
        CIPHER_LOG_I("Key 0x%04X deleted successfully\n", keyId);
    }

    return ret;
}

/**
 * @brief 获取可用的密钥ID列表
 * @param availableIds 输出可用ID数组
 * @param maxCount 数组最大容量
 * @return 可用ID数量
 */
uint16_t InnerGetAvailableKeyIds(uint16_t *availableIds, uint16_t maxCount)
{
    // 初始化密钥ID位图
    uint16_t count = 0;

    // 检查位图是否已初始化，若未初始化则进行初始化
    if (!bitmapInitialized) {
        if (InitKeyIdBitmap() != 0) {
            return 0;
        }
    }

    // 检查输入参数是否有效
    if (availableIds == NULL || maxCount == 0) {
        return 0;
    }

    // 遍历所有密钥ID，查找可用的密钥ID
    for (uint16_t i = 0; i < KEY_ID_COUNT && count < maxCount; i++) {
        // 检查当前密钥ID是否可用，8：一个字节8位
        if (!(keyIdBitmap[i / 8] & (1 << (i % 8)))) {
            // 将可用的密钥ID存入数组，并增加计数
            availableIds[count++] = KEY_ID_START + i;
        }
    }

    // 返回实际获取的可用密钥ID的数量
    return count;
}

/**
 * @brief 获取已使用的密钥ID列表
 * @param usedIds 输出已使用ID数组
 * @param maxCount 数组最大容量
 * @return 已使用ID数量
 */
uint16_t InnerGetUsedKeyIds(uint16_t *usedIds, uint16_t maxCount)
{
    // 初始化计数器
    uint16_t count = 0;

    // 如果位图未初始化，则尝试初始化
    if (!bitmapInitialized) {
        if (InitKeyIdBitmap() != 0) {
            return 0;
        }
    }

    // 检查参数有效性
    if (usedIds == NULL || maxCount == 0) {
        return 0;
    }

    // 遍历所有密钥ID，查找已使用的密钥ID
    for (uint16_t i = 0; i < KEY_ID_COUNT && count < maxCount; i++) {
        // 检查当前密钥ID是否已使用，8：一个字节8位
        if (keyIdBitmap[i / 8] & (1 << (i % 8))) {
            // 将已使用的密钥ID存入数组，并递增计数器
            usedIds[count++] = KEY_ID_START + i;
        }
    }

    // 返回实际获取的已使用密钥ID数量
    return count;
}
// ohos end