/* Copyright Statement:
*
* This software/firmware and related documentation ("Innostar Software") are
* protected under relevant copyright laws. The information contained herein is
* confidential and proprietary to Innostar Inc. and/or its licensors. Without
* the prior written permission of Innostar inc. and/or its licensors, any
* reproduction, modification, use or disclosure of Innostar Software, and
* information contained herein, in whole or in part, shall be strictly
* prohibited.
*
* Innostar Inc. (C) 2020. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("INNOSTAR SOFTWARE")
* RECEIVED FROM INNOSTAR AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
* ON AN "AS-IS" BASIS ONLY. INNOSTAR EXPRESSLY DISCLAIMS ANY AND ALL
* WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
* NONINFRINGEMENT. NEITHER DOES INNOSTAR PROVIDE ANY WARRANTY WHATSOEVER WITH
* RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
* INCORPORATED IN, OR SUPPLIED WITH THE INNOSTAR SOFTWARE, AND RECEIVER AGREES
* TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
* RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
* OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN INNOSTAR
* SOFTWARE. INNOSTAR SHALL ALSO NOT BE RESPONSIBLE FOR ANY INNOSTAR SOFTWARE
* RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND INNOSTAR'S
* ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE INNOSTAR SOFTWARE
* RELEASED HEREUNDER WILL BE, AT INNOSTAR'S OPTION, TO REVISE OR REPLACE THE
* INNOSTAR SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
* CHARGE PAID BY RECEIVER TO INNOSTAR FOR SUCH INNOSTAR SOFTWARE AT ISSUE.
*/

/*!
* @file se_config.h
*
* @brief This file provides se version header function .
*
*/
#ifndef _SE_CONFIG_H_
#define _SE_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */

/* ============================================  Define  ============================================ */

/* Operation Mode Definition */
#define TEST_MODE
#define OEM_MODE
#define USER_MODE

/*Communication Interface Definition */
//#define ENABLE_SWI_INTERFACE /* Use SWI for Communication Interface */

#define ENABLE_HW_ECC  /* Enable Hardware ECC Interface */
#define ENABLE_HW_HMAC /* Enable Hardware HMAC Interface */

#define ENABLE_SOFTWARE_KEY_WRAPPING /* Enable Software Key Wrapping Algorithm */

#ifdef ENABLE_SOFTWARE_KEY_WRAPPING
#define ENABLE_SOFTEARE_CRC
#define ENABLE_SOFTWARE_PRESENT
#endif

/*Software Crypto Library Interface Definition */

#define ENABLE_SOFTWARE_LIBRARY /* Enable Software Crypto Algorithm Library */

#ifdef ENABLE_SOFTWARE_LIBRARY

#define ENABLE_SOFTWARE_ECC     /* Enable Software ECC Algorithm */
#define ENABLE_SOFTWARE_HMAC    /* Enable Software HASH/HMAC Algorithm */

#ifdef ENABLE_SOFTWARE_ECC
#define ENABLE_SOFTWARE_ECC_163
//#define ENABLE_SOFTWARE_ECC_233
//#define ENABLE_SOFTWARE_ECC_283
#define ENABLE_SOFTWARE_ECC_ENC
#define ENABLE_SOFTWARE_ECDSA
#endif

#ifdef ENABLE_SOFTWARE_ECDSA
#define ENABLE_SOFTWARE_HMAC
#define ENABLE_SOFTWARE_RNG
#endif

#ifdef ENABLE_SOFTWARE_HMAC
//#define ENABLE_SOFTWARE_HMAC_SM3
#define ENABLE_SOFTWARE_HMAC_SHA256
#endif

#endif

/*Debug Interface Definition */
#define ENABLE_DEBUG_MENU

#ifdef ENABLE_DEBUG_MENU
//#define ENABLE_DEBUG_MENU_CRYPTO
//#define ENABLE_DEBUG_MENU_ECC
//#define ENABLE_DEBUG_MENU_HMAC
//#define ENABLE_DEBUG_MENU_DEV
//#define ENABLE_DEBUG_MENU_HAL
//#define ENABLE_DEBUG_MENU_CP
//#define ENABLE_DEBUG_MENU_ANTI
#endif


#ifdef SE_CUSTOM_CONFIG   //Allow APP undef some MACROs to disable some sub menu
#include "se_custom_config.h"
#endif

#ifdef __cplusplus
}
#endif
#endif /* _SE_CONFIG_H_ */
