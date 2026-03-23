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
* @file se_reg.h
*
* @brief This file provides se system and crypto engine relevant register definition.
*
*/
#ifndef _SE_REG_H_
#define _SE_REG_H_

/* ===========================================  Includes  =========================================== */
#include "se_config.h"
#include "typedef.h"
#define NDEBUG
#include <assert.h>

/* ====================================== OTP and MTP Size, Defined By OEM  ==========================*/
#define MTP_SIZE             (800) ///define MTP Size, Unit is Kbits
#define OTP_USER_SIZE        (32)  ///define OTP User Size, Unit is Kbits
#define MTP_USER_SIZE        (600) ///define MTP User Size, Unit is Kbits

/* ============================================  Define  ============================================ */
#define LSCON_DAT								(0xD739F6EA)
#define LFSPAN_EN               (0x534890CF)
#define WPKEY_EN                (0) 
#define WPKEY_DIS               (0x8AD4F396)
#define KBUF_LEN                (10)
#define KBUF_ADDR_OFFSET        (36)

#define NUM_OEM_MODE            (0x26CA2848)
#define NUM_USER_MODE           (0x6E7FA98B)
#define NUM_FAIL_MODE           (0x94C32A7D)

#define PWD_TEST_MODE_0         (0x5C6DFA18)
#define PWD_TEST_MODE_1         (0x7B1AD48C)
#define PWD_TEST_MODE_2         (0x27C64236)
#define PWD_TEST_MODE_3         (0x69DF12AD)

#define PWD_OEM_MODE_0          (0x688A4B82)
#define PWD_OEM_MODE_1          (0x3611275A)
#define PWD_OEM_MODE_2          (0x4CFFC187)
#define PWD_OEM_MODE_3          (0x4EFE9D3C)

/* ===========================================  Typedef  ============================================ */
/*!
* @brief Authentic Key Type Enumeration.
*/
typedef enum
{
    CE_AUTH_KEY_OTP_ECC = 0,
    CE_AUTH_KEY_OTP_HMAC,
    CE_AUTH_KEY_MTP_ECC,
		CE_AUTH_KEY_MTP_HMAC,
		CE_AUTH_KEY_PUF_PUB,
		CE_AUTH_KEY_MAX
} CE_AUTH_KEY_TYPE;

/*!
* @brief Key Encryption Type Enumeration.
*/
typedef enum
{
    CE_KEK_CKEK = 0,
    CE_KEK_HKEK,
    CE_KEK_MAX,
} CE_KEK_TYPE;

/*!
* @brief Key Encryption Type Enumeration.
*/
typedef enum
{
    CE_KEY_PUF = 0,
    CE_KEY_OTP,
    CE_KEY_MTP,
} CE_KEY_TYPE;

/*!
* @brief Hash Algorithm Type Enumeration.
*/
typedef enum
{
    CE_HASH_SHA256 = 0,
    CE_HASH_SM3,
    CE_HASH_MAX,
} CE_HASH_ALGO_TYPE;

/*!
* @brief Hash Last Flag Type Enumeration.
*/
typedef enum
{
    CE_HASH_N_LAST = 0,
    CE_HASH_LAST,
		CE_HASH_LAST_MAX,
} CE_HASH_LAST_TYPE;

/*!
* @brief ECC Algorithm Type Enumeration.
*/
typedef enum
{
    CE_ECCB163 = 0,
    CE_ECCB233,
    CE_ECCB283,
    CE_ECC_MAX,
} CE_ECC_ALGO_TYPE;

/*!
* @brief Crypto Engine Mode Type Enumeration.
*/
typedef enum
{
    CE_MODE_HASH = 0,
    CE_MODE_HMAC,
    CE_MODE_HMAC_AUTH,
    CE_MODE_ECC_SCALAR_MUL,
    CE_MODE_ECC_AUTH,
    CE_MODE_GET_RAND,
    CE_MODE_MAX,
} CE_MODE_TYPE;

/*!
* @brief Crypto Engine Mode Type Enumeration.
*/
typedef enum
{
    CE_STATUS_SUCCESS = 0,
    CE_STATUS_POINT_NOT_ON_ECC,
    CE_STATUS_KEY_CRC_ERROR,
    CE_STATUS_DFA_ERROR,
    CE_STATUS_KEY_ID_INVALID,
    CE_STATUS_AUTH_FAIL,
    CE_STATUS_MAX,
} CE_SATAUS_TYPE;


/*!
* @brief  SE Moduel Select Type Enumeration.
*/
typedef enum
{
    SE_POWER_STATE_DSLEEP = 0,
		SE_POWER_STATE_SLEEP,
		SE_POWER_STATE_MAX
} SE_POWER_STATE_TYPE;

/*!
* @brief  SE Moduel Select Type Enumeration.
*/
typedef enum
{
    SE_GPIO_WAKUP_EDGE_RISING = 0,
		SE_GPIO_WAKUP_EDGE_FALLING,
		SE_GPIO_WAKUP_EDGE_MAX
} SE_GPIO_WAKUP_EDGE_TYPE;

/*!
* @brief  SE Moduel Select Type Enumeration.
*/
typedef enum
{
    SE_MODULE_CE = 2,
		SE_MODULE_ECC,
		SE_MODULE_HASH,
		SE_MODULE_LFSPAN,
		SE_MODULE_RRAM,
		SE_MODULE_MAX
} SE_MODULE_TYPE;

/*!
* @brief  SE Operation Mode Type Enumeration.
*/
typedef enum
{
    SE_OPMODE_TEST = 0,
		SE_OPMODE_OEM,
		SE_OPMODE_USER,
		SE_OPMODE_FAIL,
		SE_OPMODE_DEBUG,
		SE_OPMODE_MAX
} SE_OPMODE_TYPE;

/*!
* @brief  SE GPIO Type Enumeration.
*/
typedef enum
{
    SE_GPIO0 = 0,
		SE_GPIO1,
		SE_GPIO2,
		SE_GPIO3,
		SE_GPIO4,
		SE_GPIO_SWI,
		SE_GPIO_MAX
} SE_GPIO_NUM_TYPE;

/*!
* @brief  SE GPIO Mode Type Enumeration.
*/
typedef enum
{
    SE_GPIO_DIO = 0,
    SE_GPIO_AIO
} SE_GPTO_MODE_TYPE;

/*!
* @brief  SE GPIO Direction Type Enumeration.
*/
typedef enum
{
    SE_GPIO_OUTPUT = 0,
    SE_GPIO_INPUT
} SE_GPIO_DIRECTION_TYPE;

/*!
* @brief  SE GPIO Direction Type Enumeration.
*/
typedef enum
{
    SE_GPIO_PULL_UP = 0,
    SE_GPIO_PULL_DOWN
} SE_GPIO_PULLUPDOWN_TYPE;

/*!
* @brief  SE GPIO Multi-Function Type Enumeration.
*/
typedef enum
{
    SE_GPIO_FUNCTION_SPI = 0,
		SE_GPIO_FUNCTION_GPIO,
		SE_GPIO_FUNCTION_TESTBUS_0,
		SE_GPIO_FUNCTION_TESTBUS_1,
		SE_GPIO_FUNCTION_TESTBUS_2,
		SE_GPIO_FUNCTION_TESTBUS_3,
		SE_GPIO_FUNCTION_TESTBUS_4,
		SE_GPIO_FUNCTION_TESTBUS_5,	
		SE_GPIO_FUNCTION_MAX		
} SE_GPIO_FUNCTION_TYPE;

/*!
* @brief  SE Status Type Enumeration.
*/
typedef enum
{
    SE_STATUS_SUCCESS = 0,
		SE_STATUS_FAIL,
		SE_STATUS_MAX,
} SE_STATUS_TYPE;

/*!
* @brief  SE Status Type Enumeration.
*/
typedef enum
{
    SE_LFSPAN_NOT_FULL = 0,
		SE_LFSPAN_FULL,
		SE_LFSPAN_MAX,
} SE_LFSPAN_STATUS_TYPE;

/*!
* @brief  SE Lifespan Mode Type Enumeration.
*/
typedef enum
{
    LFSPAN_MODE_MTP = 0,
		LFSPAN_MODE_OTP,
		LFSPAN_MODE_MAX,
} LFSPAN_MODE_TYPE;

/*!
* @brief crypto engine configuration structure.
*/
typedef struct
{
    uint64 devId;                            ///<device ID
    uint8 cryptoKeyID;	                     ///<crypto Engine Key ID
    CE_KEY_TYPE cryptoKeySrc;                ///<crypto Engine key Source(PUF/OTP/MTP)
    CE_HASH_LAST_TYPE cryptoHashLastFlag;    ///<crypto Engine Hash Last Flag
    CE_ECC_ALGO_TYPE cryptoEccAlgo;          ///<crypto Engine ECC Algorithm Selection(ECC163/233/283)
    CE_HASH_ALGO_TYPE cryptoHashAlgo;        ///<crypto Engine Hash Algorithm Selection(SHA256 or SM3)
    CE_MODE_TYPE cryptoMode;	               ///<crypto Engine Mode Selection
    void* regOpContext;  ///<used as  the context parameter of op functions
    void *regOp; //!< table of functions to access registers in se1 
} SE_DevConfigType;

/* ===========================================  BASE Address Definition  ============================================ */
#define RERAM_ADDR_BASE       0x0000
#define RERAM_ADDR_LEN        0x7FFF

#define SYSTEM_ADDR_BASE      0x8000
#define SYSTEM_ADDR_LEN       0xFF

#define IBUF_ADDR_BASE        0x8100
#define IBUF_ADDR_LEN         0xFF

#define OBUF_ADDR_BASE        0x8200
#define OBUF_ADDR_LEN         0xFF

#define CRYPTO_ADDR_BASE      0x8300
#define CRYPTO_ADDR_LEN       0xFF

#define RERAM_CFG_ADDR_BASE   0x8400
#define RERAM_CFG_ADDR_LEN    0xFF

#define BOOT_ADDR_BASE        0x8500
#define BOOT_ADDR_LEN         0xFF

#define PUF_LEN								(0x10)
#define PUF_SADDR             (RERAM_ADDR_BASE + 0x7FC0)
#define PUF0_SADDR						(PUF_SADDR)
#define PUF1_SADDR            (PUF0_SADDR + PUF_LEN)
#define PUF2_SADDR            (PUF1_SADDR + PUF_LEN)
#define PUF3_SADDR            (PUF2_SADDR + PUF_LEN)

#define OTP_CHIP_ADDR_LEN     (0x40)
#define OTP_OEM_ADDR_LEN      (0xE0)
#define MTP_CHIP_ADDR_LEN     (0xA0)
#define MTP_OEM_ADDR_LEN      (0xA40)

#define OTP_CHIP_SADDR        (RERAM_ADDR_BASE + 0x00)  
#define OTP_OEM_SADDR         (OTP_CHIP_SADDR + OTP_CHIP_ADDR_LEN)   
#define OTP_USER_SADDR        (OTP_OEM_SADDR + OTP_OEM_ADDR_LEN)
#define OTP_SUSER_SADDR       (OTP_USER_SADDR + (OTP_USER_SIZE + 1) * 32)

#define MTP_CHIP_SADDR        (PUF_SADDR - MTP_CHIP_ADDR_LEN) 
#define MTP_OEM_SADDR         (MTP_CHIP_SADDR - MTP_OEM_ADDR_LEN)  
#define MTP_USER_SADDR        (MTP_OEM_SADDR - (MTP_USER_SIZE - 1) * 32)
#define MTP_SUSER_SADDR       (PUF_SADDR - MTP_SIZE * 32)

#define WP_SADDR              MTP_OEM_SADDR

#define MTP_MAX_ADDR          (0x7FC0)
#define MTP_USER_MAX_ADDR     (0x74E0)
#define OTP_USER_MIN_ADDR     (0x0120)


#define OTP_CKEK_S_SADDR      (OTP_CHIP_SADDR + 0x38) 
#define OTP_CKEK_N_SADDR      (OTP_CHIP_SADDR + 0x3B) 

///OTP OEM Area
#define OTP_HKEK_S_SADDR      (OTP_OEM_SADDR + 0x0A) 
#define OTP_HKEK_N_SADDR      (OTP_OEM_SADDR + 0x0D) 
#define OTP_ECC_KEY_SADDR     (OTP_OEM_SADDR + 0x20) 
#define OTP_HMAC_KEY_SADDR    (OTP_OEM_SADDR + 0x80) 

///MTP OEM AREA
#define MTP_HKEK_S_SADDR      (MTP_OEM_SADDR + 0x24)    
#define MTP_HKEK_N_SADDR      (MTP_OEM_SADDR + 0x27)  
#define MTP_ECC_KEY_SADDR     (MTP_OEM_SADDR + 0x40)   
#define MTP_HMAC_KEY_SADDR    (MTP_OEM_SADDR + 0x540)   

///PUF PUBLIC KEY AREA
#define PUF_PUBKEY_SADDR      (OTP_USER_SADDR + 0x4) 
#define PUF_CERT_SADDR				(PUF_PUBKEY_SADDR + 0x12)
#define OEM_SN_SADDR          (PUF_CERT_SADDR + 0x44)

/* ===========================================  System Address Definition  ============================================ */
#define SYSCON                 SYSTEM_ADDR_BASE+0x00
#define DMACON                 SYSTEM_ADDR_BASE+0x01
#define ANASTAUS               SYSTEM_ADDR_BASE+0x02
#define P0DAT                  SYSTEM_ADDR_BASE+0x03
#define P0CON                  SYSTEM_ADDR_BASE+0x04
#define RRAMHBIT               SYSTEM_ADDR_BASE+0x05
#define PWDREG0                SYSTEM_ADDR_BASE+0x06
#define PWDREG1                SYSTEM_ADDR_BASE+0x07
#define PWDREG2                SYSTEM_ADDR_BASE+0x08
#define PWDREG3                SYSTEM_ADDR_BASE+0x09
#define LSCON                  SYSTEM_ADDR_BASE+0x0A
#define UIDH0                  SYSTEM_ADDR_BASE+0x0B
#define UIDH1                  SYSTEM_ADDR_BASE+0x0C
#define UIDH2                  SYSTEM_ADDR_BASE+0x0D
#define UIDH3                  SYSTEM_ADDR_BASE+0x0E
#define AOCON                  SYSTEM_ADDR_BASE+0x10
#define PDCON                  SYSTEM_ADDR_BASE+0x11
#define ANACON0                SYSTEM_ADDR_BASE+0x12
#define ANACON1                SYSTEM_ADDR_BASE+0x13
#define ANACON2                SYSTEM_ADDR_BASE+0x14
#define ANACON3                SYSTEM_ADDR_BASE+0x15
#define OUSPLIT                SYSTEM_ADDR_BASE+0x16
#define OSUSPLIT               SYSTEM_ADDR_BASE+0x17
#define MSUSPLIT               SYSTEM_ADDR_BASE+0x18
#define RRAMCON                SYSTEM_ADDR_BASE+0x19
#define RRAMCON1               SYSTEM_ADDR_BASE+0x1A
#define RRAMCON2               SYSTEM_ADDR_BASE+0x1B
#define RRAMCON3               SYSTEM_ADDR_BASE+0x1C
#define LSID2CON               SYSTEM_ADDR_BASE+0x3A

/* ===========================================  Boot Address Definition  ============================================ */
#define BLSID0                 BOOT_ADDR_BASE+0x00
#define BLSID1                 BOOT_ADDR_BASE+0x40
#define BLSID2                 BOOT_ADDR_BASE+0x41
#define BCECON                 BOOT_ADDR_BASE+0x01
#define BOTPCLKEN              BOOT_ADDR_BASE+0x02
#define BSYSCON                BOOT_ADDR_BASE+0x03
#define B1WITIM0               BOOT_ADDR_BASE+0x04
#define B1WITIM1               BOOT_ADDR_BASE+0x05
#define B1WITIM2               BOOT_ADDR_BASE+0x06
#define BLFSPANEN              BOOT_ADDR_BASE+0x31
#define BOTPIFSELEN            BOOT_ADDR_BASE+0x32
#define BDBMKEY0               BOOT_ADDR_BASE+0x33
#define BDBMKEY1               BOOT_ADDR_BASE+0x34
#define BDBMKEY2               BOOT_ADDR_BASE+0x35
#define BDBMKEY3               BOOT_ADDR_BASE+0x36
#define BWPKEY                 BOOT_ADDR_BASE+0x37
#define BLSNUM                 BOOT_ADDR_BASE+0x42
#define BOUSPLIT               BOOT_ADDR_BASE+0x43
#define BOSUSPLIT              BOOT_ADDR_BASE+0x44
#define BMSUSPLIT              BOOT_ADDR_BASE+0x45
#define BUSERPSW0              BOOT_ADDR_BASE+0x27
#define BUSERPSW1              BOOT_ADDR_BASE+0x28
#define BUSERPSW2              BOOT_ADDR_BASE+0x29
#define BUSERPSW3              BOOT_ADDR_BASE+0x2A
#define BMTPSPLIT              BOOT_ADDR_BASE+0x30
#define BUID0                  BOOT_ADDR_BASE+0x80
#define BUID1                  BOOT_ADDR_BASE+0x81
#define BUID2                  BOOT_ADDR_BASE+0x82
#define BUID3                  BOOT_ADDR_BASE+0x83
#define BMTPLSCNTH             BOOT_ADDR_BASE+0xC0
#define BMTPLSCNTN             BOOT_ADDR_BASE+0x07

/* ===========================================  Crypto Address Definition  ============================================ */
#define CRYPTO_REG_CECTRL       CRYPTO_ADDR_BASE+0x00
#define CRYPTO_REG_CESTAT       CRYPTO_ADDR_BASE+0x01
#define SM3EN_MASK							(0x4000)
#define SM3EN_Pos								(14)
#define SHAEN_MASK							(0x2000)
#define SHAEN_Pos								(13)
#define ECCEN_MASK							(0x1000)
#define ECCEN_Pos								(12)
#define KIDINV_MASK							(0x80)
#define KIDINV_Pos							(7)
#define DFAERR_MASK							(0x40)
#define DFAERR_Pos							(6)
#define CRCERR_MASK							(0x20)
#define CRCERR_Pos							(5)
#define PVERERR_MASK						(0x10)
#define PVERERR_Pos							(4)
#define RDY_MASK								(0x01)
#define RDY_Pos									(0)

/* ===========================================  Interface Command Definition  ============================================ */
#define RDROMID               0x33
#define MTCHROM               0x55
#define SCHROM                0xF0
#define SKPROM                0xCC
#define FUNCEN                0xAA
#define SGLWR                 0x11
#define SGLRD                 0x22
#define MULWR                 0x06
#define MULRD                 0x03

#endif /*_SE_REG_H_*/
