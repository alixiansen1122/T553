/*
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Application core board init function for standard
 * Author:
 * Create:
 */
#ifndef ESIM_BRANDY_H
#define ESIM_BRANDY_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "base.h"
#include "lpa.h"

#define AT_BUFFER 1024
#define AT_CMD_LEN 524
#define AT_CSIM_CMD_LEN 550
#define AT_ERROR_TXT_LEN 128
#define AT_CMD_TIMEOUT 4800
#define ACTIVE_CODE_BUF_SIZE 128
#define ICCID_BUF_SIZE 64

void esim_apdu_transmit_deinit(void);
int32_t esim_apdu_transmit_init(void);
int32_t esim_get_imei(char* imei, size_t len);
void esim_get_iccid(char* iccid_res);
void esim_get_activecode(char* code_res);
int32_t esim_http_hostreq(char *url, int32_t port, struct Header_list *hlist, char *txbuf, int32_t txLen, char **rxbuf);
int32_t esim_apdu_transmit_recv(unsigned char *txbuf, unsigned long txlen, unsigned char **rxbuf, unsigned long *rxlen);
int32_t esim_waitfor_ready(void);
int32_t esim_metadata_process(struct API_Metadata metadata);
char esim_get_net_flag(void);
void esim_set_net_flag(bool flag);
char *esim_itoa(char *p, uint32_t x);
int esim_atoi(const char *str);
int esim_atoi_hex(const char *str);
uint32_t esim_set_iccid(char* iccid_res, uint32_t para_len);
uint32_t esim_set_activecode(char* code_res, uint32_t para_len);
int32_t esim_buf_malloc(char **buff1, int32_t buff1_len, char **buff2, int32_t buff2_len);

void esim_strHex(char *dest, unsigned char *src, unsigned short srcLen);
#endif