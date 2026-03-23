/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef MSG_CENTER_UTILS_H
#define MSG_CENTER_UTILS_H
#include "stdint.h"
#include "errcode.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

errcode_t msg_center_start_js(char *bundleName);
errcode_t msg_center_send_response(uint8_t cmd_id, uint8_t type, uint8_t *usr_data, uint16_t usr_len);
bool msg_center_is_js_app_install(char *bundleName);
bool msg_center_is_forcibly_screen();
void msg_center_forcibly_screen(bool isForciblyScreen);
errcode_t msg_center_install_js_app(char *bundleName);
bool msg_center_is_js_app_install(char *bundleName);
errcode_t msg_center_install_js_app(char *bundleName);
bool msg_center_has_js_app_package(char *bundleName);
bool msg_center_is_js_app_front(char *bundleName);
int32_t  msg_center_get_js_app_version(char *bundleName);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* MSG_CENTER_UTILS_H */