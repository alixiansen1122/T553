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
* @file se_version.h
*
* @brief This file provides se version header function .
*
*/
#ifndef _SE_VERSION_H_
#define _SE_VERSION_H_
#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */

/* ============================================  Define  ============================================ */
#define SE_VERSION_MAIN      0x00
#define SE_VERSION_SUB       0x06

/*
	SE_0006_20211015:
	1.Modify g_hkekSalt and g_hkekNum

	SE_0005_20211015:
	1.Modify MTP User Space Calculate formula
	2.Delete Debug log in menu display to avoid auto-test exception
	
	SE_0004_20210923:
	1.Add SE-1 Reset function
	2.Add Log
	3.modify command to support lifespan 2^20 counter

	SE_0003_20210916:
	1.Add Software Reset Command in Menu
	2.Add Hex Input Number Support in Menu 
	3.Modify SE_Crypto_KeyWrapping()parameters
	
	SE_0002_20210909:
	1.Add define to Shrink Size
	2.Optimize Code
	
	SE_0001_20210827:
	1:first Release Verison






*/
#ifdef __cplusplus
}
#endif
#endif /* _SE_VERSION_H_ */
