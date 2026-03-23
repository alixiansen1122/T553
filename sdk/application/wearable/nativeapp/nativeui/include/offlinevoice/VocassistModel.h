/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: voice assistant
 * Author: CompanyName
 * Create: 2022-01-18
 */

#ifndef VOCASSIST_MODEL_H
#define VOCASSIST_MODEL_H

#include "smart_voice_service.h"
#include "offlinevoice/VocassistView.h"

#ifdef __cplusplus
extern "C" {
#endif
void VoiceFunction(SmartVoiceEventType type, td_void *data, td_s32 size);
#ifdef __cplusplus
}
#endif

namespace OHOS {
typedef enum {
    ONE_ROAD_AND_INCOMING,
    ONE_OR_MORE_ROAD_AND_OTHER_STATUS,
    OTHER_STATUS
} PhoneStatus;

typedef enum {
    DISABLE,
    ENABLE
} EnableChoice;

class VocassistModel {
public:
    VocassistModel();
    virtual ~VocassistModel();
    static VocassistModel *GetInstance(void);
    void Init();                // 初始化语音识别服务
    void UnInit();              // 结束语音识别服务

    int GetPhoneDetailStatus();                         // 获取来电时的状态
    int GetToggleFlag();                                // 获取语音图标到波形变换的转换标志位
    void SetToggleFlag(int val);                        // 设置语音图标到波形变换的转换标志位
    int GetRegFlag();                                   // 获取识别出的命令操作标志位
    void SetRegFlag(int val);                           // 设置识别出的命令操作标志位
    int GetExitStatus();                                // 获取退出语音助手状态标志位
    void SetExitStatus(int val);                        // 设置退出语音助手状态标志位
    int GetConnectFlag();                               // 获取连接动画标志位
    void SetConnectFlag(int flag);                      // 设置连接动画标志位
    bool GetHasOneCmd();
    void SetHasOneCmd(bool fg);

private:
    int toggleFg{ALREADY_OPEN_VOCASSIST_IMAGE};     // 记录语音图标和波形标志位
    int regFlag{SWITCH_TO_UNKNOWN};         // 命令操作标志位
    int exitStatus{INTERRUPT_EXIT};         // 判断是否因太长时间未操作退出语音助手
    int imgConnectFlag{CONNECT_TO_VOCASSIST};  // 记录动画标志位
    bool hasOneCmd{false};                      // 每次只执行一条命令
};
}

#endif

