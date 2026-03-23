/**
 * @defgroup    bsl_uio bsl_uio.h
 * @ingroup     bsl
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       统一IO抽象
 */

#ifndef BSL_UIO_H
#define BSL_UIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup bsl_uio
 *
 * @brief   UIO模块控制结构
 */
typedef struct UIO_ControlBlock BSL_UIO;

/**
 * @ingroup bsl_uio
 *
 * @brief   传输协议枚举
 */
typedef enum {
    BSL_UIO_TCP,    /**< TCP协议 */
    BSL_UIO_UDP,    /**< UDP协议 */
    BSL_UIO_SCTP,   /**< SCTP协议 */
    BSL_UIO_UNKNOWN, /**< 未知协议，不应出现 */
} BSL_UIO_TransportType;

/**
 * @ingroup bsl_uio
 *
 * @brief   Sctp auth key, hitls通过BSL_UIO_Method.ctrl方法传入 BSL_UIO_SCTP_ADD_AUTH_SHARED_KEY 指令
 *          以通知用户需要设置auth key
 */
typedef struct {
    uint16_t shareKeyId;    /**< share key id */
    uint16_t authKeySize;   /**< auth key size */
    const uint8_t *authKey;       /**< auth key */
} BSL_UIO_SctpAuthKey;

/**
 * @ingroup bsl_uio
 *
 * @brief   BSL_UIO_CtrlParameter 控制IO的回调,hitls通知用户需要实现的功能
 *          可用作ctrl回调的cmd参数
 */
typedef enum {
    /* 公共的使用0x0xx */
    BSL_UIO_SET_PEER_IP_ADDR = 0x000,
    BSL_UIO_GET_PEER_IP_ADDR,
    BSL_UIO_SET_FD,
    BSL_UIO_GET_FD,

    /* SCTP 使用0x3XX */
    BSL_UIO_SCTP_CHECK_PEER_AUTH = 0x300,     /**< SCTP检查对端是否支持认证 */
    BSL_UIO_SCTP_ADD_AUTH_SHARED_KEY,         /**< 添加新的sctp auth key，sctp可以用新的key接收数据 */
    BSL_UIO_SCTP_ACTIVE_AUTH_SHARED_KEY,      /**< 激活新的sctp auth key，sctp需要用新的key发送数据 */
    BSL_UIO_SCTP_DEL_PRE_AUTH_SHARED_KEY,     /**< SCTP删除上个shared key */
    BSL_UIO_SCTP_SND_BUFF_IS_EMPTY,           /**< SCTP发送队列是否已为空, ctrl.param为(uint8_t *)的指针
                                                    当用户返回ctrl.param大于0,表明队列是为空. 反之等于0,表明队列不为空 */
    BSL_UIO_SCTP_RECV_BUFF_IS_EMPTY,          /**< SCTP接收队列是否已为空, ctrl.param为(uint8_t *)的指针
                                                    当用户返回ctrl.param大于0,表明队列是为空. 反之等于0,表明队列不为空 */
    BSL_UIO_SCTP_GET_SEND_STREAM_ID,
    BSL_UIO_SCTP_SET_APP_STREAM_ID,
} BSL_UIO_CtrlParameter;

typedef struct {
    uint8_t *addr;
    uint32_t size;
} BSL_UIO_CtrlGetPeerIpAddrParam;

/**
 * @ingroup bsl_uio
 *
 * @brief   BSL_UIO_Method是控制IO的回调，需要产品注册给HITLS以供HITLS底层调用
 */
typedef struct BSL_UIO_MethodStruct {
    BSL_UIO_TransportType type;
    int32_t (*write)(BSL_UIO *uio, const void *buf, uint32_t len, uint32_t *writeLen);    /**< UIO写函数 */
    int32_t (*read)(BSL_UIO *uio, void *buf, uint32_t len, uint32_t *readLen);            /**< UIO读函数 */
    int32_t (*ctrl)(BSL_UIO *uio, int32_t cmd, int32_t larg, void *parg);              /**< UIO控制函数 */
} BSL_UIO_Method;

/**
 * @ingroup bsl_uio
 *
 * @brief   创建一个新的UIO对象。
 * @param   method  [IN] UIO方法结构体
 * @retval  UIO对象 创建成功
 * @retval  NULL UIO创建失败
 */
BSL_UIO *BSL_UIO_New(const BSL_UIO_Method *method);

/**
 * @ingroup bsl_uio
 *
 * @brief   释放UIO对象。
 * @param   uio  [IN] UIO对象。
 */
void BSL_UIO_Free(BSL_UIO *uio);

/**
 * @brief 按cmd处理特定的UIO实现
 * @details 如果HiTLS无某个cmd的默认实现，则会调uio的ctrl回调
 *
 * @param uio [IN] UIO 对象
 * @param cmd [IN] 不同的cmd对UIO对象执行不同的操作，必须有HiTLS实现或自定义实现
 * @param larg [IN] parg的大小，数组为元素个数，指针为指向类型的大小，由cmd决定，详见下
 * @param parg [IN/OUT] 由cmd决定，详见下
 * @retval BSL_SUCCESS 成功
 * @retval 非BSL_SUCCESS 具体参见bsl_errno.h
 *
 * @brief 在UIO对象中设置对端ip地址。
 * @details 设置的地址格式为网络字节序二进制地址，长度为4或者16。
 *           将为dtls的HelloVerifyRequest所用而提供生成cookie。
 * @param uio [IN] UIO 对象
 * @param cmd [IN] BSL_UIO_SET_PEER_IP_ADDR
 * @param larg [IN] 对端地址的大小：长度应为4或者16。
 * @param parg [IN] 对端地址。
 *
 * @brief 在UIO对象中获取对端ip地址。
 * @details 获取的地址格式为网络字节序二进制地址，输入长度应大于设置的大小。
 *          目的将为dtls的HelloVerifyRequest所用而提供生成cookie
 * @param uio [IN] UIO 对象
 * @param cmd [IN] BSL_UIO_GET_PEER_IP_ADDR
 * @param larg [IN] sizeof(BSL_UIO_CtrlGetPeerIpAddrParam)
 * @param parg [IN] BSL_UIO_CtrlGetPeerIpAddrParam *，其中：
 *             addr [IN/OUT] 对端地址，
 *             size [IN/OUT] IN: 输入缓存的大小 OUT: 输出对端地址的大小。
 *
 * @brief 在UIO对象中获取sctp发送的stream id
 * @details 在需要用户在 BSL_UIO_Method.write中调用，并根据获取的stream id去发送sctp消息
 * @param uio [IN] UIO 对象
 * @param cmd [IN] BSL_UIO_SCTP_GET_SEND_STREAM_ID
 * @param larg [IN] sizeof(uint16_t)
 * @param parg [IN/OUT] 发送的流id，uint16_t *类型
 *
 * @brief 在UIO对象中设置sctp发送的app消息stream id
 * @details 若用户需要对业务消息走特定的stream id, 则可以调用此接口
 * @param uio [IN] UIO 对象
 * @param cmd [IN] BSL_UIO_SCTP_SET_APP_STREAM_ID
 * @param larg [IN] sizeof(uint16_t)
 * @param parg [IN] uint16_t指针，指向app流id
 *
 * @brief 获取fd
 * @details 如果UIO对象的底层有fd，则可以获取fd
 * @param uio [IN] UIO 对象
 * @param cmd [IN] BSL_UIO_GET_FD
 * @param larg [IN] sizeof(int32_t)
 * @param parg [IN/OUT] int32_t类型的指针，用于存放获取的fd
 *
 * @brief 设置fd
 * @details 如果UIO对象的底层有fd，则可以设置fd
 * @param uio [IN] UIO 对象
 * @param cmd [IN] BSL_UIO_SET_FD
 * @param larg [IN] sizeof(int32_t)
 * @param parg [IN] int32_t类型的指针，用于指向被设置的fd
 */
int32_t BSL_UIO_Ctrl(BSL_UIO *uio, int32_t cmd, int32_t larg, void *parg);

/**
 * @ingroup bsl_uio
 *
 * @brief   获取UIO传输协议类型。
 * @param   uio  [IN] UIO 对象。
 * @return  协议类型
 */
BSL_UIO_TransportType BSL_UIO_GetTransportType(const BSL_UIO *uio);

/**
 * @ingroup bsl_uio
 *
 * @brief   在UIO对象中设置用户数据。
 * @details UIO will not modify the user data, user can add some information
 *      for the UIO, and get the information by use BSL_UIO_GetUserData function; After you set user data by calling
 *      BSL_UIO_SetUserData, you need to call BSL_UIO_SetUserData again before calling BSL_UIO_Free to set
 *      user data to null to ensure that all memory is released.
 * @param   uio   [IN] UIO 对象。
 * @param   data  [IN] 用户数据指针
 * @retval  BSL_SUCCESS 成功
 * @retval  BSL_NULL_INPUT 无效空指针
 */
int32_t BSL_UIO_SetUserData(BSL_UIO *uio, void *data);

/**
 * @ingroup bsl_uio
 *
 * @brief   获取UIO对象中的用户数据。
 * @details The user data comes from users, and tls will not change any thing
 *          for user data, user can add some customize information.
 * @param   uio   [IN] UIO 对象。
 * @retval  用户存储的数据结构指针 获取成功
 * @retval  NULL 获取的数据不存在
 */
void *BSL_UIO_GetUserData(const BSL_UIO *uio);

/**
 * @ingroup bsl_uio
 *
 * @brief   在UIO对象中设置对端ip地址。
 * @details 设置的地址格式为网络字节序二进制地址，长度为4或者16。 \n
 *          如果为DTLS 服务器并且使能cookie交换, 则用该对端ip地址用来生成cookie \n
 *          以让服务器通过发送HelloVerifyRequest给客户端.
 * @param   uio   [IN] UIO 对象。
 * @param   addr  [IN] addr 对端IP地址。
 * @param   size   [IN] size 对端地址的大小：长度应为4或者16。
 * @retval  BSL_SUCCESS 成功
 * @retval  BSL_NULL_INPUT 无效空指针
 * @retval  BSL_UIO_FAIL 设置失败,长度不正确
 */
int32_t BSL_UIO_SetPeerIpAddr(BSL_UIO *uio, uint8_t *addr, uint32_t size);

/**
 * @ingroup bsl_uio
 *
 * @brief   在UIO对象中获取对端ip地址。
 * @details 获取的地址格式为网络字节序二进制地址，输入长度应大于设置的大小。 \n
 *          目的:为dtls服务器的HelloVerifyRequest所用(生成cookie的输入因子)
 * @param   uio   [IN] UIO 对象。
 * @param   addr  [IN] addr 对端地址。
 * @param   size   [IN/OUT] size IN: 输入缓存的大小 OUT: 输出对端地址的大小。
 * @retval  BSL_SUCCESS 成功
 * @retval  BSL_NULL_INPUT 无效空指针
 * @retval  BSL_UIO_FAIL 获取失败,未设置地址或长度不正确
 */
int32_t BSL_UIO_GetPeerIpAddr(BSL_UIO *uio, uint8_t *addr, uint32_t *size);

/**
 * @ingroup bsl_uio
 *
 * @brief   在UIO对象中获取sctp发送的stream id
 * @details 如果为DTLS OVER SCTP, 发送数据时SCTP应该从HITLS获取发送的流id,按该流id发送数据。 \n
 *          非业务数据在DTLS协议规定使用流id为0发送数据。 \n
 *          至于业务数据(app data)用户可以通过BSL_UIO_SctpSetAppStreamId 设置业务数据发送的流id。
 * @attention 如果为DTLS OVER SCTP, 用户需要在 BSL_UIO_Method.write中调用，并根据获取的stream id去发送sctp消息。
 * @param   uio   [IN] UIO 对象。
 * @param   sendStreamId  [OUT] sendStreamId 发送的流id。
 * @retval  BSL_SUCCESS 成功
 * @retval  BSL_NULL_INPUT 无效空指针
 */
int32_t BSL_UIO_SctpGetSendStreamId(BSL_UIO *uio, uint16_t *sendStreamId);

/**
 * @ingroup bsl_uio
 *
 * @brief   在UIO对象中设置sctp发送的app消息stream id
 * @details 若用户需要对业务消息走特定的stream id, 则可以调用此接口
 *
 * @param   uio   [IN] UIO 对象。
 * @param   sendAppStreamId  [OUT] sendAppStreamId 发送的app流id。
 * @retval  BSL_SUCCESS 成功
 * @retval  BSL_NULL_INPUT 无效空指针
 */
int32_t BSL_UIO_SctpSetAppStreamId(BSL_UIO *uio, uint16_t sendAppStreamId);

/**
 * @ingroup bsl_uio
 *
 * @brief   为UIO对象设置文件描述符fd。
 * @param   uio [IN] UIO 对象。
 * @param   fd  [IN] 文件描述符, 对于TCP/UDP是socket fd, 对于SCTP是偶联id。
 * @retval  BSL_SUCCESS 设置成功
 * @retval  BSL_NULL_INPUT 入参空指针
 */
int32_t BSL_UIO_SetFd(BSL_UIO *uio, int32_t fd);

/**
 * @ingroup bsl_uio
 *
 * @brief   获取UIO对象中的文件描述符fd。
 * @param   uio  [IN] UIO 对象
 * @retval  -1 获取失败
 * @retval  其他值 文件描述符fd
 */
int32_t BSL_UIO_GetFd(BSL_UIO *uio);

#ifdef __cplusplus
}
#endif

#endif
