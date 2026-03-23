#ifndef _LPA_H__
#define _LPA_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include "base.h"

#pragma GCC diagnostic ignored "-Wunused-variable"
static char UICC_CAPABILE[][20] = {
    "contactlessSupport",
    "usimSupport",
    "isimSupport",
    "csimSupport",
    "akaMilenage",
    "akaCave",
    "akaTuak128",
    "akaTuak256",
    "RFU",
    "RFU",
    "gbaAuthenUsim",
    "gbaAuthenISim",
    "mbmsAuthenUsim",
    "eapClient",
    "javacard",
    "multos",
    "multipleUsimSupport",
    "multipleIsimSupport",
    "multipleCsimSupport",
    "berTlvFileSupport",
    "dfLinkSupport",
    "catTp",
    "getIdentity",
    "profile-a-x25519",
    "profile-b-p256",
    "suciCalculatorApi"
};

#pragma GCC diagnostic ignored "-Wunused-variable"
static  char RSP_CAPABILE[][32] = {
    "additionalProfile",
    "crlSupport",
    "rpmSupport",
    "testProfileSupport",
    "deviceInfoExtensibilitySupport",
    "serviceSpecificDataSupport"
};

#pragma GCC diagnostic ignored "-Wunused-variable"
static  char FORBIDDEN_PPR[][32] = {
    "pprUpdateControl",
    "ppr1_DisablingNotAllowed",
    "ppr2_DeletionNotAllowed"
};

struct API_EuiccInfo{
    char EID[65];
    char GSM_RSP_Version[16];
    char Profile_Version[16];
    char Uicc_Capability[512];
    char RSP_Capability[128];
    char Forbidden_PPR[128];
    char CI_PKID_LIST_Verification[9][256];
    char CI_PKID_LIST_Signing[9][256];
    char SAS_Accreditation_Number[65];          //UTF8String (SIZE(0..64))
    char Platform_Lable[65];
    char Discovery_Base_URL[256];
};

struct EventEntry{
    char eventId[64];
    char rspServerAddress[256];
};

struct API_Metadata{
    char Provider[33];              //UTF8String (SIZE(0..32))
    char ProfileName[65];           //UTF8String (SIZE(0..64))
    char ICCID[21];                 //固定10字节
    char ProfileClass[5];           //INTEGER {test(0), provisioning(1), operational(2)}
    char ppr1_DisablingNotAllowed;  //Disabling of this Profile is not allowed
    char ppr2_DeletionNotAllowed;   //Deletion of this Profile is not allowed
    struct API_OperatorId ProfileOwner;  //mccMnc + EF GID1 + EF GID2
};


/**
 * 发转收函数
 * struct API_Metadata  txBuf   发送数据
 * **/
typedef int (*MetadataCallback)(struct API_Metadata);
int api_matedataRegisterCallback(MetadataCallback metadataCallback);

// == profile 管理 ==
//获取全部配置
int api_getProfileList(struct API_Profile pList[],int pageSize,int pageNum,int *count,char *msg);
//获取全部配置,可配制profile类型
int api_getProfileClassList(struct API_Profile pList[],int pageSize,int pageNum,int *count,int profileClass,char *msg);
//获取单个配置信息
int api_getProfile(struct API_Profile *profile,char *ICCID,char *msg);
//修改别名
int api_editProfile(char *ICCID,char* NickName,char *msg);
//使能配置信息,refresh=1
int api_enableProfile(char *ICCID,char *msg);
//使能配置信息,可配refresh标志
int api_enableProfileFlag(char *ICCID,int refreshflag,char *msg);
//失能配置信息,refresh=1
int api_disableProfile(char *ICCID,char *msg);
//失能配置信息,可配refresh标志
int api_disableProfileFlag(char *ICCID,int refreshflag,char *msg);
//删除配置信息
int api_deleteProfile(char *ICCID,char *msg);

// == 查询 Euicc ==
//获取euicc信息
int api_getEuicc(struct API_EuiccInfo *euiccInfo,char *msg);

// == 下载配置 ==
//从dp+服务器下载配置
int api_profileDownload(char *dpAddress,char *matchId,char *confirmCode,char *msg);
//从dp+服务器下载配置
int api_profileDownloadAC(char *AC,char *confirmCode,char *msg);
//从ds服务器下载配置
int api_profileDownloadDs(struct EventEntry eventList[],int pageSize,int pageNum,int *count,char *msg);

//== 设置 == 
//获取默认dp+地址
int api_getConfigAddr(char *dpAddress,char *msg);
//设置默认dp+地址
int api_setConfigAddr(char *dpAddress,char *msg);
//重发全部通知
int api_rerunNotification(char *msg);

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif