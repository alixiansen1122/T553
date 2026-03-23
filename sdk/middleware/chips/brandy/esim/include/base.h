#ifndef _EPA_BASE_H__
#define _EPA_BASE_H__

#ifdef  __cplusplus
extern "C" {
#endif

enum API_ERR{
    ERR_CONFIG_FORMAT       = -302,     //配置文件格式错误
    ERR_CONFIG_FILE         = -301,     //配置文件读写错误
    ERR_SE_PROFILE_STATE    = -204,     //se返回Profile状态异常
    ERR_SE_COMMUNICA        = -203,     //se通讯异常
    ERR_SE_STATE_WORD       = -202,     //se返回错误状态字
    ERR_SE_TAG              = -201,     //se返回数据异常
    ERR_HTTP_CONNECT        = -103,     //网络连接错误
    ERR_HTTP_INTERFACE      = -102,     //网络接口返回错误
    ERR_HTTP_DATA           = -101,     //网络返回数据错误
    ERR_API_METADATA        = -6,       //profile下载元数据确认失败
    ERR_API_CC_REQUIRED     = -5,       //profile下载要求输入确认码
    ERR_API_BUSY            = -4,       //IPA接口忙
    ERR_API_STATE           = -3,       //接口调用返回错误状态
    ERR_API_PARAMETER       = -2,       //接口调用错误
    ERR_UNDEFINE            = -1,       //未定义错误
    OK                      = 0         //成功
};

struct Header_list {
  char data[512];
  struct Header_list *next;
};

struct API_OperatorId{
    char mccMnc[7];                 //MCC&MNC coded as 3GPP TS 24.008
    char gid1[31];                  //EF GID1对应6F3E
    char gid2[31];                  //EF GID2对应6F3F
};


struct API_Profile{
    char Provider[33];      //UTF8String (SIZE(0..32))
    char ProfileName[65];   //UTF8String (SIZE(0..64))
    char ICCID[21];         //固定10字节
    char IsdpAid[33];       //最大16字节 《7816-4》
    char State[5];          //INTEGER {disabled(0), enabled(1)}
    char NickName[65];      //UTF8String (SIZE(0..64))
    char ProfileClass[5];   //INTEGER {test(0), provisioning(1), operational(2)}
    struct API_OperatorId ProfileOwner;  //mccMnc + EF GID1 + EF GID2
};


//获取返回版本号
char* api_version(void);


//获取esim产品编号
int api_esimProductEdition(char *productEdition, char *msg);


//设置设备IMEI，用于下载时上报
int api_setupIMEI(char *imei);

/**
 * 发转收函数
 * unsigned char*  txBuf   发送数据
 * unsigned long   txLen   发送数据长度
 * unsigned char** rxBuf   接收缓存区指针
 * unsigned long*  rxLen   接收数据长度
 * **/
typedef int (*ApduTransmitRecv)(unsigned char*, uint64_t, unsigned char**, uint64_t*);
typedef int (*WaitForReady)(void);
int api_seInterfaceInit(ApduTransmitRecv apduTransmitRecv,WaitForReady waitForReady);

/**
 * httpPost接口
 * unsigned char*       url     网络请求地址
 * unsigned int         port    网络端口
 * struct Header_list*  hlist   网络请求头部数据
 * unsigned char*  txBuf   发送数据
 * unsigned long   txLen   发送数据长度
 * unsigned char** rxBuf   接收缓存区指针
 * **/
typedef int (*HttpPostReq)(char*,int,struct Header_list*,char*,int,char**);

//注册网络接口
int api_httpInterfaceInit(HttpPostReq httpPostReq);

/**
 * storeData 接口
 * char*       data     存储数据
 * 
 * ret: 返回数据(1个字节) 0-成功 / 其他-错误
 * **/
typedef int (*StoreData)(char*);

/**
 * getData 接口
 * char**       data    读出的数据。 
 * 
 * ret:   函数返回数据(1个字节) 0-成功 / 其他-错误
 * **/
typedef int (*GetData)(char**);

//注册 数据存取接口c
int api_configInterfaceInit(StoreData storeData,GetData getData);

/**
 * OnIccidChanged 接口
 * const char*       iccid     当前激活的iccid
 * 
 * ret: 返回数据(1个字节) 0-成功 / 其他-错误
 * **/
typedef void (*OnIccidChanged)(const char*);

//注册 iccid改变监听接口
int api_onIccidChangeRegister(OnIccidChanged onIccidChanged);

/**
 * OnProfileDownload 接口
 * const char*       dp+Address     下载dp+地址
 * 
 * ret: 返回数据(1个字节) 0-成功 / 其他-错误
 * **/
typedef void (*OnProfileDownload)(const char*);
//注册 profile下载回调接口
int api_onProfileDownloadRegister(OnProfileDownload onProfileDownload);

//获取Eid信息
int api_getEid(char *eid,char *msg);

//重置profile和测试profile相关信息
int api_memoryReset(char *msg);

//获取当前激活的prifile
int api_getCurrentProfile(struct API_Profile *profile, char *msg);

//获取提供profile的下一个profile。如未提供，则返回当前profile的下一个profile。
int api_getNextProfile(struct API_Profile *profile, char* mccMnc ,char *msg);

#ifdef  __cplusplus
}
#endif

#endif
