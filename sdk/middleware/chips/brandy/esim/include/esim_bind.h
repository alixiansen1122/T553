/*
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Application core board init function for standard
 * Author:
 * Create:
 */

#ifndef ESIM_BIND_H
#define ESIM_BIND_H
#include "common.h"
#include "errcode.h"
#include "cardbind.h"
#include "esim_lpa.h"

typedef struct {
    char response[AT_BUFFER];
    char apdu[AT_CMD_LEN];
    char errtext[AT_ERROR_TXT_LEN];
    esim_bind_info_t bind_info;
} esim_bind_msg_t;

typedef enum {
    ESIM_BIND_NEED_PPK = 0,
    ESIM_BIND_NEED_BIND,
    ESIM_BIND_NEED_VERIFY,
    ESIM_BIND_VERIFY_SUCC,
    ESIM_BIND_LOCKED,

    ESIM_BIND_BUTT,
} esim_bind_status_t;

#define ERRCODE_ESIM_DUPLICATE_BINDING_INFORMATION_WRITING 0x8000A401
#define ERRCODE_ESIM_INVALID_PARAM 0x8000A402

void print_info(char *info, uint32_t length);
void uapi_esim_bind_init(void);
errcode_t uapi_esim_get_bind_info(esim_bind_info_t *bind_info);
void uapi_print_bind_info(esim_bind_info_t *bind_info);
errcode_t uapi_esim_write_bind_info(esim_bind_info_t *bind_info);
void uapi_esim_bind_info_test(void);
uint32_t uapi_esim_select_app(int logic, char *esim_apdu, char *errtext);
int32_t esim_get_card_status(esim_bind_status_t* status, uint8_t *bind_cnt, uint8_t *verify_cnt, char *ccho_response);
int32_t esim_write_ppk(char* response, char *apdu, char *errtext, esim_bind_info_t *bind_info);
int32_t esim_bind_card(char *response, char *apdu, char *errtext, esim_bind_info_t *bind_info);
int32_t esim_verify_card(char *response, char *apdu, char *errtext, esim_bind_info_t *bind_info);
int32_t esim_unlock_bind(void);
int32_t esim_get_verify_status(void);
int32_t esim_verify_waitready_process(void);
uint32_t uapi_get_esim_status(void);
uint32_t uapi_esim_write_ppk(void);
uint32_t uapi_esim_bind_card(void);
uint32_t uapi_esim_verify_card(void);
uint32_t uapi_esim_bind_main(void);
#endif