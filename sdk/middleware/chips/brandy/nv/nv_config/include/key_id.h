/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: NV - key_id.
 */

#ifndef __KEY_ID_H__
#define __KEY_ID_H__

#define REGION_KEY_NUMS 0x1000 /* 每个区域容纳key_id的个数 */


/* Please use the format of NV_ID_XXX */

/* system factory key_id [0x0001,0x1000) */


/* user factory key_id [0x1000,0x2000) */


/* system normal key_id [0x2000,0x3000) */
#define NV_ID_SECURE_CONN               0x2001 /* secure connect */
#define NV_ID_OFFLINELOG_ENBALE_FLAG    0x2002 /* Offline log enable flag */

#define NV_ID_BTSRV_LOCAL_DEV_INFO      0x2101 /* btsrv_local_device_info_t */
#define NV_ID_BTSRV_PAIRED_DEV_INFO     0x2102 /* btsrv_paired_device_info_t */

#define NV_ID_BTSRV_CALIBRATION_CONFIG  0x2103 /* btsrv_local_tx_power_info_t */
#define BTH_NV_SEC_ID                   0x2104 /* bth_product_data_config_t */

#define NV_ID_SYS_LANGUAGE_ID           0x2105  /* sys_language_id */
#define NV_ID_SYS_PRODUCT_INFO          0x2106  /* sys_product_info */
#define NV_ID_SYS_ESIM_BIND_INFO        0x2107  /* esim bind info */
#define NV_ID_MODEL_VERSION_INFO        0x2108  /* paqichong model version info */

/* dial market key_id [0x2500,0x2600) */
#define NV_ID_SECURE_DIAL               0x2500 /* secure dial */

/* ace cipher key_id [0x2700,0x2900) */
#define NV_ID_ACE_CIPHER                0x2700 /* ace cipher key id */
// Use a special keyId to store the bitmap (before the key range)
#define BITMAP_KEY_ID                   0x26FF

/* user normal key_id [0x3000,0x4000) */
#define NV_ID_AUDIO_VQE_PARAM           0x320C /* 3A params for VOIP scene, audio_vqe_param_t */
#define NV_ID_AUDIO_VQE_KWS_PARAM       0x320D /* 3A params for KWS scene, audio_vqe_param_t */
#define NV_ID_AUDIO_AEF_SWS_PARAM       0x320E /* 3A params for SWS scene, audio_aef_param_t */

/* reserved key_id [0x4000,0xFFFF) */
#define NV_ID_SAMPLE_0 0x4000 /* Eg. */
#define NV_ID_SAMPLE_1 0x4001 /* Eg. */
#define NV_ID_SAMPLE_2 0x4002 /* Eg. */
#define NV_ID_SAMPLE_3 0x4003 /* Eg. */
#define NV_ID_SAMPLE_4 0x4004 /* Eg. */
#define NV_ID_FTM_CONFIG                0x4109
#define NV_ID_RESTORE_ENABLE            0xA000 /* NV restore factory region flag */

#define NV_ID_SK_DEVICE_INFO              0xA001
#define NV_ID_SK_SENSOR_CALI              0xA002

#endif /* __KEY_ID_H__ */
