#ifndef __SS_API_H_
#define __SS_API_H_

#define SECPUF_CSI_VERSION  "1.1.6"

#include "iotsec.h"

/**
 * @brief 初始化
 * 
 * @param oempwd OEM专属访问密码
 * @param rootKey32bytes  安全存储系统根密钥
 * @param mcuid 主控MCU ID
 * @param mcuidLen  主控MCU ID的长度
 * @return int  0表示成功，⾮0表示失败
 */
csi_error_t csi_init(const uint32_t oempwd[4], const uint8_t *rootKey32bytes, const uint8_t *mcuid, uint32_t mcuidLen);

/**
 * @brief  去初始化
 */
csi_error_t csi_deinit(void);

#endif
