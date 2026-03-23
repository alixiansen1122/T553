/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: PhoneService
 * Create: 2021-09-22
 */

#ifndef PHONE_SERVICE_H
#define PHONE_SERVICE_H

#include "bts_br_gap.h"
#include "bts_hfp_hf.h"
#include "ohos_types.h"
#include "phoneservice/PhoneAudio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MULTI_CALL_MAX_NUME 5 // 最大多路通话路数
#define CALL_LOG_MAX_NUM 30 // 最多记录30条通话记录，超出后删除最老的记录
#define NAME_SIZE 20
#define MAX_PHONE_NUM 20
#define CALL_VOLUME_MAX 15

typedef struct {
    bd_addr_t bd_addr;
    int id;
    int state;
    char number[MAX_PHONE_NUM];
    unsigned char number_len;
    bt_uuid_t uuid;
    bool multi_party;
    bool outgoing;
    bool in_band_ring;
    long creation_time;
} PhoneHfpCallInfo;

typedef struct {
    int operate;
    int lastStatus;
    char name[NAME_SIZE];
    unsigned char nameLen;
    unsigned char tel[MAX_PHONE_NUM];
    unsigned char telLen;
    uint64 activeTime; // 首次电话接通时间点
    uint64 creationTime; // 通话创建时间（不是接通时间）
    PhoneHfpCallInfo unitCalls;
} PhoneCallInfo;

enum OperateType {
    OPERATE_NO_OPE = 0, // 无操作
    OPERATE_DIALING, // 呼出，正在拨号中
    OPERATE_ALERTING, // 呼出，对方响铃
    OPERATE_INCOMING, // 来电呼入
    OPERATE_WAITING, // 来电呼入(等待)
    OPERATE_WATCH_ACTIVE, // 对方呼入，手表接听
    OPERATE_PHONE_ACTIVE, // 对方呼入，手机接听  (手表接收到此操作，调用DisconnectSco接口断开SCO连接)
    OPERATE_PEER_ACTIVE, // 呼出，对方接听
    OPERATE_PHONE_HOLD, // 手机挂起通话
    OPERATE_WATCH_REJECT, // 手表拒接来电
    OPERATE_WATCH_FINISH, // 手表发起通话结束（挂断通话）
    OPERATE_NON_WATCH_FINISH, // 非手表发起通话结束（手机挂断+拒接来电）
};

// 通话失败类型
typedef enum {
    ERROR_BT = 0, // 蓝牙
    ERROR_DIAL, // 拨号
} PhoneServiceErrorType;

// 通话结束类型
typedef enum {
    CALL_OUTGOING = 0, // 呼出电话
    CALL_INCOMING, // 呼入电话
    CALL_MISSED, // 未接来电
} CallFinishStatus;

typedef struct {
    char name[NAME_SIZE];
    unsigned char nameLen;
    unsigned char tel[MAX_PHONE_NUM];
    unsigned char telLen;
    CallFinishStatus status;
    uint64 creationTime; // 通话创建时间（不是接通时间）
    uint64 callDurTime; // 通话持续时间
} CallLogInfo;

// 通话状态改变回调函数
typedef void (*PhoneServiceCallChangedCb)(const PhoneCallInfo *phoneCall);
// 电话服务失败回调函数
typedef void (*PhoneServiceErrorCb)(PhoneServiceErrorType type, int volume);
// sco连接回调函数（状态只有断开和连接两种状态） 通话转移会导致sco连接和断连
typedef void (*PhoneServiceScoStatusCb)(hfp_sco_connect_state_t state);

typedef struct {
    PhoneServiceCallChangedCb callChanged;
    PhoneServiceErrorCb phoneServiceErrorCb;
    PhoneServiceScoStatusCb scoStatusCb;
} PhoneServiceCb;

/**
 * @brief 注册电话服务回调函数
 *
 * @param phoneServiceFun 注册函数
 * @return 返回值 成功返回0，失败返回错误码
 */
int RegisterCallChangedCb(PhoneServiceCb *phoneServiceFun);

/**
 * @brief 电话服务初始化
 */
void PhoneServiceInit(void);

/**
 * @brief 手表呼出电话
 *
 * @param number 呼出号码
 * @param len 号码长度，只包括号码数字部分，不包括结尾'\0'
 * @return 返回值 成功返回0，失败返回错误码
 */
int DialCall(unsigned char *number, unsigned char len);

/**
 * @brief 手表接听呼入来电
 *
 * @return 返回值 成功返回0，失败返回错误码
 */
int AcceptIncomingCall(void);

/**
 * @brief 手表拒接呼入来电
 *
 * @return 返回值 成功返回0，失败返回错误码
 */
int RejectIncomingCall(void);

/**
 * @brief 手表结束通话
 *
 * @return 返回值 成功返回0，失败返回错误码
 */
int FinishCall(void);

/**
 * @brief 获取当前通话的最新变化状态
 *
 * @return 返回值 通话变化状态
 */
int GetCurrCallState(void);

/**
 * @brief 判断此次通话是否是完整通话流程
 *
 * @return 返回值 true:是；false:不是
 */
bool IsCompleteCallFlow(void);

/**
 * @brief 获取当前正在通话路数
 *
 * @return 返回值 返回当前正在通话路数
 */
int GetCurrCallNum(void);

/**
 * @brief 获取当前通话路数和对应的通话信息
 *
 * @param callList 用来接收通话信息，需调用者分配空间，大小为MULTI_CALL_MAX_NUME * sizeof(PhoneCallInfo)
 * @param listNum callList中PhoneCallInfo个数，大小为MULTI_CALL_MAX_NUME
 * @param retNum 接收当前正在进行的通话路数
 * @return 返回值 成功返回0，失败返回错误码
 */
int GetCallInfoInCallList(PhoneCallInfo callList[], int listNum, int *retNum);

/**
 * @brief 获取当前通话音量大小（0 -15）
 *
 * @param volume
 *
 * @return 返回值 当前通话音量大小
 */
unsigned char GetCallCurrVolume(void);

/**
 * @brief 设置手表通话Speaker音量
 *
 * @param volume
 *
 * @return 返回值 void
 */
void SetSpeakerVolume(unsigned char volume);

/**
 * @brief 设置通话Speaker静音/取消静音
 *
 * @param isMute true:设置通话Speaker静音；false：设置通话Speaker取消静音
 * @return 返回值 成功返回ture，失败返回false
 */
bool SetSpeakerMute(bool isMute);

/**
 * @brief 设置通话mic静音/取消静音
 *
 * @param isMute true:设置通话mic静音；false：设置通话mic取消静音
 * @return 返回值 成功返回ture，失败返回false
 */
bool SetMicMute(bool isMute);

/**
 * @brief 获取SCO的连接状态
 *
 * @return 返回值 返回SCO的连接状态
 */
int GetScoConnectState(void);

/**
 * @brief 通话转移到手表
 *
 * @return 返回值 成功返回0，失败返回错误码
 */
int TransCallToWatch(void);

/**
 * @brief 通话转移到手机
 *
 * @return 返回值 成功返回0，失败返回错误码
 */
int TransCallToMobilePhone(void);

/**
 * @brief 获取通话记录
 *
 * @param info 接收获取的通话记录，由调用者释放free
 * @param num 接收通话记录条数
 * @return 返回值 成功返回0，失败返回错误码
 */
int GetCallLog(CallLogInfo **info, int *num);

/**
 * @brief 删除第一条通话记录
 *
 * @return 返回值 成功返回0，失败返回错误码
 */
int RemoveFirstCallLog(void);

/**
 * @brief 清空通话记录
 *
 * @return 返回值 成功返回0，失败返回错误码
 */
int ClearCallLogFile(void);

/**
 * @brief 获取蓝牙状态
 *
 * @return 返回值 蓝牙连接状态
 */
uint32 GetBtConnectStatus(void);

/**
 * @brief 释放蓝牙通话媒体资源
 *
 * @return 返回值 无
 */
void AudioRelease(void);

uint32 GetCallCodec(void);

#ifdef __cplusplus
}
#endif

#endif // PHONE_SERVICE_H
