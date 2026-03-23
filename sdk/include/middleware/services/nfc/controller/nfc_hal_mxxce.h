/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: nfc MXX CE operations \n
 *
 * Date: 2025-03-05 \n
 */
/** @defgroup nfc MXX CE hal
 * @ingroup nfc MXX CE hal
 * @{
 */
#ifndef NFC_HAL_MXXCE_H
#define NFC_HAL_MXXCE_H

#include <stdbool.h>
#include "nfc_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Max supported MXX card number.
 * @else
 * @brief  最多支持的MXX卡片数量。
 * @endif
 */
#define MAX_MXX_CARD_NUM 20

/**
 * @if Eng
 * @brief  The memory size of a MXX card in byte.
 * @else
 * @brief  一张MXX卡占用的空间大小。
 * @endif
 */
#define MXX_MEM_SIZE 1024

/**
 * @if Eng
 * @brief  The length of UID in MXX card.
 * @else
 * @brief  MXX卡片的UID长度。
 * @endif
 */
#define MXX_UID_LEN 0x04

/**
 * @if Eng
 * @brief  Events reported by the chip in MXX CE mode.
 * @else
 * @brief  MXX卡模拟工作模式中芯片上报的事件。
 * @endif
 */
typedef enum {
    /** @if Eng  No event.
     *  @else    无事件。 @endif */
    NFC_MXXCE_EVENT_NONE,
    /** @if Eng  External field on event.
     *  @else    外部起场事件。 @endif */
    NFC_MXXCE_EVENT_FIELD_ON,
    /** @if Eng  External field off event.
     *  @else    外部断场事件。 @endif */
    NFC_MXXCE_EVENT_FIELD_OFF,
    /** @if Eng  Card information stored in the chip is updated event.
     *  @else    芯片中存储的卡片信息被更新事件。 @endif */
    NFC_MXXCE_EVENT_CARD_UPDATED,
    /** @if Eng  Chip reset event.
     *  @else    芯片复位事件。 @endif */
    NFC_MXXCE_EVENT_RESET,
} NfcMxxCeEvent;

/**
 * @if Eng
 * @brief  Callback for upper layer to handle event from NFCC.
 * @param  [in]     evtId  Event index.
 * @retval NFC_OK   Success.
 * @retval Other    Failure. For details, see @ref NfcErrorCode.
 * @else
 * @brief  上层注册的回调函数，用于处理NFCC上报的事件。
 * @param  [in]     evtId  事件编号。
 * @retval NFC_OK   成功。
 * @retval Other    失败。参考 @ref NfcErrorCode 。
 * @endif
 */
typedef NfcErrorCode (*MxxCeCallback)(NfcMxxCeEvent evtId);

/**
 * @if Eng
 * @brief  MXX card information structure.
 * @else
 * @brief  MXX卡片信息结构体。
 * @endif
 */
typedef struct {
    /** @if Eng  Pointers whith point to MXX card memory.
     *  @else    指示MXX卡片对应内存的指针。
     *  @endif */
    uint8_t *cardMem;
    /** @if Eng  Indicates whether the card is valid.
     *  @else    指示MXX卡片是否有效。
     *  @endif */
    uint8_t cardValid;
} MxxCardInfo;

/**
 * @if Eng
 * @brief  MXX CE feature context.
 * @else
 * @brief  MXX CE功能上下文。
 * @endif
 */
typedef struct {
    /** @if Eng  Callback to handle event from NFCC, see @ref MxxCeCallback.
     *  @else    上层注册的回调函数，用于处理NFCC上报的事件。参考 @ref MxxCeCallback 。
     *  @endif */
    MxxCeCallback eventCallback;
    /** @if Eng  Current MXX card index.
     *  @else    当前所选MXX卡片编号。
     *  @endif */
    uint8_t currentCardIndex;
    /** @if Eng  MXX cards information structure, see @ref MxxCardInfo, max card num see @ref MAX_MXX_CARD_NUM.
     *  @else    MXX卡片信息结构体。参考 @ref MxxCardInfo 。支持的最大卡片数量参考 @ref MAX_MXX_CARD_NUM 。
     *  @endif */
    MxxCardInfo mxxCardsInfo[MAX_MXX_CARD_NUM];
} MxxCeFeatureCtx;

/**
 * @if Eng
 * @brief  Registers the callback function to process events reported by the NFCC.
 * @param  [in]     handler  User-defined callback function interface, see @ref MxxCeCallback.
 * @else
 * @brief  注册处理NFCC上报事件的回调函数。
 * @param  [in]     handler  用户定义的回调函数接口，参考 @ref MxxCeCallback 。
 * @endif
 */
void NFC_HAL_MxxCeRegisterCallback(MxxCeCallback handler);

/**
 * @if Eng
 * @brief  Processes the data reported by the NFCC and converts the data into the corresponding events. \n
           For details, see @ref NfcMxxCeEvent.
 * @param  [in]     data  Received data.
 * @param  [in]     len  Data length.
 * @retval NFC_OK   Success.
 * @retval Other    Failure. For details, see @ref NfcErrorCode.
 * @else
 * @brief  对NFCC上报的数据进行处理，并转换成对应的事件，目前支持的事件参考 @ref NfcMxxCeEvent 。
 * @param  [in]     data  数据内容。
 * @param  [in]     len  数据长度。
 * @retval NFC_OK   成功。
 * @retval Other    失败。 参考 @ref NfcErrorCode 。
 * @endif
 */
NfcErrorCode NFC_HAL_MxxCeMessageProc(uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  Initialize the NFCC chip to the MXX CE mode.
 * @retval NFC_OK   Success.
 * @retval Other    Failure. For details, see @ref NfcErrorCode.
 * @else
 * @brief  初始化NFCC芯片为MXX卡模拟模式。
 * @retval NFC_OK   成功。
 * @retval Other    失败。 参考 @ref NfcErrorCode 。
 * @endif
 */
NfcErrorCode NFC_HAL_MxxCeModeInit(void);

/**
 * @if Eng
 * @brief  Start MXX CE discovery.
 * @retval NFC_OK   Success.
 * @retval Other    Failure. For details, see @ref NfcErrorCode.
 * @else
 * @brief  开始MXX卡模拟侦听业务。
 * @retval NFC_OK   成功。
 * @retval Other    失败。 参考 @ref NfcErrorCode 。
 * @endif
 */
NfcErrorCode NFC_HAL_MxxCeStart(void);

/**
 * @if Eng
 * @brief  Stop MXX CE discovery.
 * @retval NFC_OK   Success.
 * @retval Other    Failure. For details, see @ref NfcErrorCode.
 * @else
 * @brief  停止MXX卡模拟侦听业务。
 * @retval NFC_OK   成功。
 * @retval Other    失败。 参考 @ref NfcErrorCode 。
 * @endif
 */
NfcErrorCode NFC_HAL_MxxCeStop(void);

/**
 * @if Eng
 * @brief  Initialize a MXX card.
 * @param  [in]     index  Card index, should not be greater than @ref MAX_MXX_CARD_NUM.
 * @param  [in]     mem  Memory allocated for the card.
 * @param  [in]     memSize  The size of memory allocated for the card, should be @ref MXX_MEM_SIZE.
 * @param  [in]     isBlank  Indicates whether to initialize the card as a blank card.
 * @retval NFC_OK   Success.
 * @retval Other    Failure. For details, see @ref NfcErrorCode.
 * @else
 * @brief  初始化一张MXX卡片。
 * @param  [in]     index  卡片编号，不应大于 @ref MAX_MXX_CARD_NUM 。
 * @param  [in]     mem  为该卡片分配的内存。
 * @param  [in]     memSize  为该卡片分配的内存大小，应为 @ref MXX_MEM_SIZE 。
 * @param  [in]     isBlank  是否初始化为空白卡。
 * @retval NFC_OK   成功。
 * @retval Other    失败。 参考 @ref NfcErrorCode 。
 * @endif
 */
NfcErrorCode NFC_HAL_MxxCeInitCard(uint8_t index, uint8_t *mem, uint16_t memSize, bool isBlank);

/**
 * @if Eng
 * @brief  Switch the MXX card and download the card information to the NFCC.
 * @param  [in]     index  Card index, should not be greater than @ref MAX_MXX_CARD_NUM.
 * @retval NFC_OK   Success.
 * @retval Other    Failure. For details, see @ref NfcErrorCode.
 * @else
 * @brief  切换MXX卡片，并下发卡片信息给NFCC。
 * @param  [in]     index  卡片编号，不应大于 @ref MAX_MXX_CARD_NUM 。
 * @retval NFC_OK   成功。
 * @retval Other    失败。 参考 @ref NfcErrorCode 。
 * @endif
 */
NfcErrorCode NFC_HAL_MxxCeSwitchCardByIndex(uint8_t index);

/**
 * @if Eng
 * @brief  Command the NFCC to report the card information of the MXX card in use.
 * @retval NFC_OK   Success.
 * @retval Other    Failure. For details, see @ref NfcErrorCode.
 * @else
 * @brief  命令NFCC上报当前使用的MXX卡的卡片信息。
 * @retval NFC_OK   成功。
 * @retval Other    失败。 参考 @ref NfcErrorCode 。
 * @endif
 */
NfcErrorCode NFC_HAL_MxxCeUploadCurrentCard(void);

/**
 * @if Eng
 * @brief  Get the current card index, should not be greater than @ref MAX_MXX_CARD_NUM.
 * @return An uint8_t value equals to current card index.
 * @else
 * @brief  获取当前卡片编号。
 * @return uint8_t类型当前卡片编号。
 * @endif
 */
uint8_t NFC_HAL_MxxCeGetCurrentCardIndex(void);

/**
 * @if Eng
 * @brief  Delete a specified MXX card.
 * @param  [in]     index  Card index, should not be greater than @ref MAX_MXX_CARD_NUM.
 * @retval NFC_OK   Success.
 * @retval Other    Failure. For details, see @ref NfcErrorCode.
 * @else
 * @brief  删除指定MXX卡片。
 * @param  [in]     index  卡片编号，不应大于 @ref MAX_MXX_CARD_NUM 。
 * @retval NFC_OK   成功。
 * @retval Other    失败。 参考 @ref NfcErrorCode 。
 * @endif
 */
NfcErrorCode NFC_HAL_MxxCeRemoveCardByIndex(uint8_t index);

/**
 * @if Eng
 * @brief  Delete all MXX cards.
 * @retval NFC_OK   Success.
 * @retval Other    Failure. For details, see @ref NfcErrorCode.
 * @else
 * @brief  删除所有MXX卡片。
 * @retval NFC_OK   成功。
 * @retval Other    失败。 参考 @ref NfcErrorCode 。
 * @endif
 */
NfcErrorCode NFC_HAL_MxxCeRemoveAllCards(void);

/**
 * @if Eng
 * @brief  Check whether the specified MXX card is valid.
 * @param  [in]     index  Card index, should not be greater than @ref MAX_MXX_CARD_NUM.
 * @retval true     Valid.
 * @retval false    Invalid.
 * @else
 * @brief  判断指定MXX卡片是否有效。
 * @param  [in]     index  卡片编号，不应大于 @ref MAX_MXX_CARD_NUM 。
 * @retval true     有效。
 * @retval false    无效。
 * @endif
 */
bool NFC_HAL_MxxCeIsValidCard(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif
/**
 * @}
 */
