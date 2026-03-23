#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Target Definitions File
# Copyright CompanyNameMagicTag 2022-2022. All rights reserved.
# ============================================================================

target = {
    #target1: liteos + lvgl + nandflash + mipi + no psram + mini_audio  典型单封LVGL版本
    'brandy-target1': {
        'board': 'evb',
        'base_target_name': 'target_standard_brandy_application_template',
        'std_libs': [],
        'defines': ['PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', '__LITEOS__', 'CONFIG_OTA_UPDATE_SUPPORT',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CONFIG_ZDIAG_NV_SUPPORT',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'CFG_DRIVERS_NANDFLASH',
                    'CONFIG_ZDIAG_AUDIO_PROC_SUPPORT', 'CONFIG_ZDIAG_AUDIO_DUMP_SUPPORT', 'CONFIG_ZDIAG_AUDIO_PROBE_SUPPORT',
                    'SUPPORT_BLE', 'SUPPORT_BREDR', 'MEMORY_MINI', 'ENABLE_LVGL', 'SW_UART_DEBUG', "SAVE_EXC_INFO",
                    "CONFIG_LOW_POWER_TEST", 'MIPI_ULPS_SUPPORT', 'ENABLE_ECC', 'CONFIG_DIAG_GPU_PROC_SUPPORT',
                    'CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE=0', 'SUPPORT_RC_CALIBRATION'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", 'dfx_nv', "osal", "arch_port", "-:testsuite",
                          "liteos_port", 'lcd', 'qspi_display', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch',
                          'ulp_aon', 'hal_l2ram',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc','fs_yaffs2', 'x_vfs_private',
                          'pm_service', 'pm_brandy', 'cmn_header', 'at_cmd', 'graphic_at_service', 'app_at_service',
                          'audio_at_service', 'media_at_service', 'bt_manager_at_service',
                          "audio_proc", "audio_dump", "audio_probe", "gpu_proc", 'bts_header'],
        'ram_component_set': ['bgh', 'media_mini', 'gpu', 'graphic_lvgl_mini', 'bgh_audio', 'dfx_set'],
        'image_analysis': True,
        'dsp_version': 'mini',
        'packet': True,
        'fs_image': False,
    },
    #target3: liteos + uikit + nandflash + mipi + psram + audio  典型合封UIKIT版本
    'brandy-target3': {
        'board': 'evb',
        'base_target_name': 'target_standard_brandy_application_template',
        'defines': ['PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', '__LITEOS__', 'CONFIG_OTA_UPDATE_SUPPORT',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CONFIG_PSRAM_SUPPORT',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'CFG_DRIVERS_NANDFLASH', 'CONFIG_ZDIAG_NV_SUPPORT',
                    'CONFIG_ZDIAG_AUDIO_PROC_SUPPORT', 'CONFIG_ZDIAG_AUDIO_DUMP_SUPPORT', 'CONFIG_ZDIAG_AUDIO_PROBE_SUPPORT', 'CONFIG_SEA_PHS_SUPPORT', 'CONFIG_DIAG_GPU_PROC_SUPPORT',
                    'SUPPORT_BLE', 'SUPPORT_BREDR', 'ENABLE_UIKIT', 'SUPPORT_GPU_JPEG', 'SUPPORT_GPU_GMMU', 'SUPPORT_GPU_OPENVG',
                    'SW_UART_DEBUG', 'SAVE_EXC_INFO', 'HASH_MEM_COPY','MIPI_ULPS_SUPPORT', 'CONFIG_LOW_POWER_TEST',
                    'ENABLE_ECC', 'SUPPORT_ALIPAY_SEC', 'SUPPORT_RC_CALIBRATION'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", 'dfx_nv', "osal", "arch_port", "-:testsuite",
                          "liteos_port", 'lcd', 'qspi_display', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch',
                          'ulp_aon', 'hal_l2ram', 'x_vfs_private',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'fs_yaffs2',
                          'pm_service', 'pm_brandy', 'cmn_header', 'at_cmd', 'graphic_at_service', 'app_at_service',
                          'audio_at_service', 'media_at_service', 'bt_manager_at_service',
                          "audio_proc", "audio_dump", "audio_probe", "gpu_proc", 'bts_header', 'lwip'],
        'ram_component_set': ['bgh', 'media', 'gpu', 'graphic_uikit', 'graphic_test', 'bgh_audio', 'dfx_set', 'alipay_set'],
        'dsp_version': 'max',
        'packet': True,
        'fs_image': False
    },
    #target4: liteos + nandflash + psram + mini_audio + no gui 外设类版本
    'brandy-target4': {
        'board': 'evb',
        'base_target_name': 'target_standard_brandy_application_template',
        'std_libs': ['c++', 'c++abi', 'unwind'],
        'defines': ['PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', 'CONFIG_OTA_UPDATE_SUPPORT',
                    '_ALL_SOURCE', 'I2C_SLAVE_REG_ADDR_4BYTE', 'SUPPORT_CXX',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'CFG_DRIVERS_NANDFLASH', 'HASH_MEM_COPY',
                    'CONFIG_ZDIAG_NV_SUPPORT', 'SUPPORT_BLE', 'SUPPORT_BREDR',
                    'SW_UART_DEBUG', 'SAVE_EXC_INFO', 'CONFIG_PSRAM_SUPPORT', 'CONFIG_SEA_PHS_SUPPORT',
                    'CONFIG_LOW_POWER_TEST', 'CONFIG_ADC_SUPPORT_AUTO_SCAN',
                    'CONFIG_ADC_SUPPORT_LONG_SAMPLE', 'ENABLE_ECC',  'CONFIG_DIAG_GPU_PROC_SUPPORT',
                    'SUPPORT_ALIPAY_SEC', 'SUPPORT_RC_CALIBRATION'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", 'dfx_nv', "osal", "arch_port", "-:testsuite",
                          "liteos_port", 'lcd', 'qspi_display', 'partition', 'partition_brandy', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch',
                          'ulp_aon', 'hal_l2ram', 'x_vfs_private',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc','fs_yaffs2',
                          'pm_service', 'pm_brandy', 'cmn_header', 'at_cmd', 'app_at_service',
                          'audio_at_service', 'bt_manager_at_service', "gpu_proc", 'bts_header'],
        'ram_component_set': ['bgh', 'gpu', 'media_target4', 'update_app', 'dfx_set', 'bgh_audio', 'alipay_set'],
        'dsp_version': 'max',
        'packet': True,
        'fs_image': False,
    },
    #target5: freertos + lvgl + nand + mipi + psram + audio + media  典型合封LVGL版本
    'brandy-target5': {
        'board': 'evb',
        'base_target_name': 'target_standard_brandy_application_freertos_template',
        'defines': ['PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', 'CONFIG_OTA_UPDATE_SUPPORT',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CONFIG_PSRAM_SUPPORT',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'CFG_DRIVERS_NANDFLASH', 'CONFIG_ZDIAG_NV_SUPPORT',
                    'CONFIG_ZDIAG_AUDIO_PROC_SUPPORT', 'CONFIG_ZDIAG_AUDIO_DUMP_SUPPORT', 'CONFIG_ZDIAG_AUDIO_PROBE_SUPPORT', 'CONFIG_SEA_PHS_SUPPORT', 'CONFIG_DIAG_GPU_PROC_SUPPORT',
                    'SUPPORT_BLE', "SUPPORT_BREDR", 'ENABLE_LVGL', "SUPPORT_GPU_JPEG", "SUPPORT_GPU_GMMU", "SUPPORT_GPU_OPENVG",
                    'SW_UART_DEBUG', 'MEMORY_NO_CACHE', "SAVE_EXC_INFO", "HASH_MEM_COPY", 'MIPI_ULPS_SUPPORT',
                    'CONFIG_LOW_POWER_TEST', 'ENABLE_ECC', 'SUPPORT_ALIPAY_SEC', 'SUPPORT_RC_CALIBRATION'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", 'dfx_nv', "osal", "arch_port", "-:testsuite",
                          'lcd', 'qspi_display', 'partition', 'partition_brandy', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch',
                          'ulp_aon', 'hal_l2ram', 'x_vfs_private',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'fs_yaffs2',
                          'pm_service', 'pm_brandy', 'cmn_header', 'at_cmd','graphic_at_service', 'app_at_service',
                          'audio_at_service', 'media_at_service', 'bt_manager_at_service',
                          "audio_proc", "audio_dump", "audio_probe", "gpu_proc", 'bts_header', 'lwip'],
        'ram_component_set': ['bgh', 'media', 'gpu', 'graphic_lvgl', 'bgh_audio', 'update_app', 'dfx_set', 'alipay_set'],
        'dsp_version': 'max',
        'packet': True,
        'fs_image': False
    },
    #native-js: liteos + uikit + nandflash + mipi + psram + audio  典型合封UIKIT版本
    #用户可参考user版本，删除reg/mem dump
    'brandy-native-js': {
        'board': 'evb',
        'base_target_name': 'target_standard_brandy_application_template',
        'defines': ['SUPPORT_LWIP', 'PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', '__LITEOS__', 'CONFIG_OTA_UPDATE_SUPPORT',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CONFIG_PSRAM_SUPPORT',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'CFG_DRIVERS_NANDFLASH', 'CONFIG_ZDIAG_NV_SUPPORT',
                    'CONFIG_ZDIAG_AUDIO_PROC_SUPPORT', 'CONFIG_ZDIAG_AUDIO_DUMP_SUPPORT', 'CONFIG_ZDIAG_AUDIO_PROBE_SUPPORT', 'CONFIG_SEA_PHS_SUPPORT', 'CONFIG_DIAG_GPU_PROC_SUPPORT',
                    'SUPPORT_BLE', 'SUPPORT_BREDR', 'ENABLE_UIKIT', 'SUPPORT_GPU_JPEG', 'SUPPORT_GPU_GMMU', 'SUPPORT_GPU_OPENVG',
                    'SW_UART_DEBUG', 'SAVE_EXC_INFO', "HASH_MEM_COPY", 'JS_ENABLE', 'CONFIG_LOW_POWER_TEST', 'SUPPORT_OHOSFWK',
                    'ENABLE_ECC', 'MIPI_ULPS_SUPPORT', 'SUPPORT_ALIPAY_SEC', 'SUPPORT_RC_CALIBRATION', 'SUPPORT_DIAL_DEBUG', 'MBEDTLS_NO_PLATFORM_ENTROPY', 'MBEDTLS_ENTROPY_HARDWARE_ALT',
                    'OH_MARKET_ENABLE', 'CONFIG_BRANDY_BLE_TRANS_OPTIMIZE', 'CONFIG_NATIVEAPP_TEST'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", 'dfx_nv', "osal", "arch_port", "-:testsuite",
                          "liteos_port", 'lcd', 'qspi_display', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch', 'ace_ble',
                          'ulp_aon', 'hal_l2ram', 'bt_manager_at_service',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'fs_yaffs2', 'x_vfs_private', "cipher_static",
                          'pm_service', 'pm_brandy', 'cmn_header', 'at_cmd', 'graphic_at_service', 'app_at_service', 'ohosfwk_at_service',
                          "audio_proc", "audio_dump", "audio_probe", "power_manager", "gpu_proc",'bts_header', 'lwip', 'mqtt',
                          'ace_kit_cipher_static'],
        'ram_component_set': ['bgh', 'media', 'gpu', 'graphic_uikit', 'bgh_audio', 'dfx_set', 'ohos_set', 'wearable_set', 'alipay_set', 'msg_center_service'],
        # 如果没有定义ssb_version，默认使用brandy-ssb
        'ssb_version': 'brandy-ssb-native-js',
        'dsp_version': 'max',
        'packet': True,
        'fs_image': True
    },
    #native-js-user: liteos + uikit + nandflash + mipi + psram + audio  典型合封UIKIT版本
    #离线日志版本
    #删除reg/mem dump，写法如下：'-:DUMP_MEM_SUPPORT', '-:DUMP_REG_SUPPORT', '-:SUPPORT_DFX_EXCEPTION'
    'brandy-native-js-user': {
        'base_target_name': 'brandy-native-js',
        'defines': ['-:SW_UART_DEBUG', '-:DUMP_MEM_SUPPORT', '-:DUMP_REG_SUPPORT', '-:SUPPORT_DFX_EXCEPTION'],
        # 如果没有定义ssb_version，默认使用brandy-ssb
        'ssb_version': 'brandy-ssb-native-js',
    },
    #auto-ota-server
    'brandy-auto-ota-ser': {
        'base_target_name': 'brandy-native-js',
        'defines': ['SUPPORT_AUTO_OTA_SERVER', 'CONFIG_DFX_SUPPORT_DIAG_UP_MACHINE=1'],
        # 如果没有定义ssb_version，默认使用brandy-ssb
        'ssb_version': 'brandy-ssb-native-js',
    },
    #auto-ota-client
    'brandy-auto-ota-cli': {
        'base_target_name': 'brandy-native-js',
        'defines': ['SUPPORT_AUTO_OTA_SERVER', 'SUPPORT_AUTO_OTA', 'CONFIG_DFX_SUPPORT_DIAG_UP_MACHINE=1'],
        'ram_component': ['auto_ota'],
        # 如果没有定义ssb_version，默认使用brandy-ssb
        'ssb_version': 'brandy-ssb-native-js',
    },
    #volte debug target
    'brandy-volte': {
        'base_target_name': 'brandy-native-js',
        'ram_component_set': ['volte_set'],
        'ram_component': ['volte_at_service'],
        'defines': ['FT_SINGLE_UART', 'SUPPORT_VOLTE', 'SUPPORT_LWIP',
                    'SUPPORT_IMS_CAT1_CHANNEL_TEST', 'SUPPORT_VOLTE_LOGFILE'],
        # 覆盖base target中的定义值
        'dsp_version': 'ultra',
        # 如果没有定义ssb_version，默认使用brandy-ssb
        'ssb_version': 'brandy-ssb-volte'
    },
    #gnss debug target
    'brandy-gnss': {
        'base_target_name': 'brandy-native-js',
        'ram_component_set': ['gnss_set', 'tiot_set'],
        'ram_component': ['gnss_at_service'],
        'defines': ['FT_SINGLE_UART', 'SUPPORT_GNSS_FEATURE'],
        # 如果没有定义ssb_version，默认使用brandy-ssb
        'ssb_version': 'brandy-ssb-gnss'
    },
    #nfc debug target
    'brandy-nfc': {
        'base_target_name': 'brandy-native-js',
        'ram_component_set': ['tiot_set', 'nfc_set'],
        'defines': ['SUPPORT_NFC'],
        'CONFIG_ENABLE_NFC_CONTROLLER_SAMPLE': 'y',
        'CONFIG_HAVE_NFC_POLL': 'y',
        'CONFIG_HAVE_NFC_LISTEN': 'y',
        # 如果没有定义ssb_version，默认使用brandy-ssb
        'ssb_version': 'brandy-ssb-nfc'
    },
    # diting target
    'brandy-diting':{
        'base_target_name': 'brandy-native-js',
        'ram_component_set': ['volte_set', 'gnss_set', 'tiot_set', 'sikey',
                              #delete for no lcd
                              '-:ohos_set', '-:wearable_set', '-:alipay_set', '-:msg_center_service', '-:graphic_uikit'],
        'ram_component': ['volte_at_service', 'gnss_at_service', 'audio_at_service', 'media_at_service', 'gsensor', 'esim',
                          #delete for no lcd
                          '-:power_manager', '-:ohosfwk_at_service', '-:graphic_at_service', '-:ace_kit_cipher_static', '-:ace_ble'],
        'defines': ['FT_SINGLE_UART', 'FT_BT_UART', 'SUPPORT_VOLTE', 'SUPPORT_LWIP',
                    'SUPPORT_IMS_CAT1_CHANNEL_TEST', 'SUPPORT_GNSS_FEATURE', 'SUPPORT_VOLTE_AUTOTEST', 'IMS_DEBUG_ENABLE', 'SUPPORT_ESIM', 'SIKEY', 'SUPPORT_FACTORY_TEST',
                    #delete for no lcd
                    '-:SUPPORT_ALIPAY_SEC',  '-:ENABLE_UIKIT', '-:SUPPORT_OHOSFWK', '-:JS_ENABLE', '-:SW_UART_DEBUG', '-:CONFIG_BRANDY_BLE_TRANS_OPTIMIZE',
                    '-:DUMP_MEM_SUPPORT', '-:DUMP_REG_SUPPORT', '-:SUPPORT_DFX_EXCEPTION'],
        # 覆盖base target中的定义值
        'dsp_version': 'ultra',
        # 如果没有定义ssb_version，默认使用brandy-ssb
        'ssb_version': 'brandy-ssb-diting'
    },
    #diting target
    'brandy-diting-nfc':{
        'base_target_name': 'brandy-native-js',
        'ram_component_set': ['volte_set', 'gnss_set', 'tiot_set', 'nfc_set'],
        'ram_component': ['volte_at_service', 'gnss_at_service', 'audio_at_service', 'media_at_service', 'gsensor', 'esim'],
        'defines': ['FT_SINGLE_UART', 'FT_BT_UART', 'SUPPORT_VOLTE', 'SUPPORT_LWIP', '-:SW_UART_DEBUG',
                    'SUPPORT_IMS_CAT1_CHANNEL_TEST', 'SUPPORT_GNSS_FEATURE', 'SUPPORT_VOLTE_AUTOTEST', 'IMS_DEBUG_ENABLE', 'SUPPORT_NFC', 'SUPPORT_ESIM'],
        'CONFIG_ENABLE_NFC_CONTROLLER_SAMPLE': 'y',
        'CONFIG_HAVE_NFC_POLL': 'y',
        'CONFIG_HAVE_NFC_LISTEN': 'y',
        # 覆盖base target中的定义值
        'dsp_version': 'ultra',
        # 如果没有定义ssb_version，默认使用brandy-ssb
        'ssb_version': 'brandy-ssb-diting'
    },
    #native-xts: liteos + uikit + nandflash + mipi + psram + audio  典型合封UIKIT版本
    'brandy-xts': {
        'board': 'evb',
        'base_target_name': 'target_standard_brandy_application_template',
        'defines': ['SUPPORT_LWIP', 'PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', '__LITEOS__', 'CONFIG_OTA_UPDATE_SUPPORT',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CONFIG_PSRAM_SUPPORT',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'CFG_DRIVERS_NANDFLASH', 'CONFIG_ZDIAG_NV_SUPPORT',
                    'CONFIG_ZDIAG_AUDIO_PROC_SUPPORT', 'CONFIG_ZDIAG_AUDIO_DUMP_SUPPORT', 'CONFIG_ZDIAG_AUDIO_PROBE_SUPPORT', 'CONFIG_SEA_PHS_SUPPORT',
                    'SUPPORT_BLE', 'SUPPORT_BREDR', 'ENABLE_UIKIT', 'SUPPORT_GPU_JPEG', 'SUPPORT_GPU_GMMU', 'SUPPORT_GPU_OPENVG', 'CONFIG_DIAG_GPU_PROC_SUPPORT',
                    'SW_UART_DEBUG', 'SAVE_EXC_INFO', "HASH_MEM_COPY", 'JS_ENABLE', 'CONFIG_LOW_POWER_TEST', 'SUPPORT_OHOSFWK',
                    'ENABLE_ECC', 'XTS_SUPPORT', 'SUPPORT_ALIPAY_SEC', 'SUPPORT_RC_CALIBRATION', 'MBEDTLS_NO_PLATFORM_ENTROPY', 'MBEDTLS_ENTROPY_HARDWARE_ALT',
                    'OH_MARKET_ENABLE'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", 'dfx_nv', "osal", "arch_port", "-:testsuite",
                          "liteos_port", 'lcd', 'qspi_display', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch', 'ace_ble',
                          'ulp_aon', 'hal_l2ram',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'fs_yaffs2', 'x_vfs_private',
                          'pm_service', 'pm_brandy', 'cmn_header', 'at_cmd', 'graphic_at_service', 'app_at_service', 'ohosfwk_at_service',
                          "audio_proc", "audio_dump", "audio_probe", "power_manager", "gpu_proc", 'bts_header', 'lwip'],
        'ram_component_set': ['bgh', 'media', 'gpu', 'graphic_uikit', 'bgh_audio', 'dfx_set', 'ohos_set', 'wearable_set', 'xts_set', 'xts_test_set', 'alipay_set', 'msg_center_service'],
        # 如果没有定义ssb_version，默认使用brandy-ssb
        'ssb_version': 'brandy-ssb-xts',
        'dsp_version': 'max',
        'packet': True,
        'fs_image': True
    },

    'brandy-target1-r': {
        'board': 'evb',
        'base_target_name': 'target_brandy_recovery_template',
        'defines': ['PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', '__LITEOS__',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CFG_DRIVERS_NANDFLASH',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'ENABLE_UIKIT', 'MEMORY_MINI', 'SW_UART_DEBUG',
                    'ENABLE_ECC', 'SUPPORT_RECOVERY_DISPLAY'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", "osal", "arch_port", "-:testsuite",
                          "liteos_port", 'lcd', 'qspi_display', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'x_vfs_private',
                          'ulp_aon', 'hal_l2ram', 'pm_service', 'pm_brandy', 'cmn_header'],
        'ram_component_set': ['pinctrl', 'dfx_set', 'gpu']
    },
    'brandy-target3-r': {
        'board': 'evb',
        'base_target_name': 'target_brandy_recovery_template',
        'defines': ['PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', '__LITEOS__',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CFG_DRIVERS_NANDFLASH',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'ENABLE_UIKIT', 'SW_UART_DEBUG', "HASH_MEM_COPY",
                    'ENABLE_ECC', 'SUPPORT_RECOVERY_DISPLAY'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", "osal", "arch_port", "-:testsuite",
                          "liteos_port", 'lcd', 'qspi_display', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'x_vfs_private',
                          'ulp_aon', 'hal_l2ram', 'pm_service', 'pm_brandy', 'cmn_header'],
        'ram_component_set': ['pinctrl', 'dfx_set', 'gpu']
    },
    'brandy-target4-r': {
        'board': 'evb',
        'base_target_name': 'target_brandy_recovery_template',
        'defines': ['PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', '__LITEOS__',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CFG_DRIVERS_NANDFLASH',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'ENABLE_UIKIT', 'SW_UART_DEBUG', 'HASH_MEM_COPY',
                    'ENABLE_ECC', 'SUPPORT_RECOVERY_DISPLAY'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", "osal", "arch_port", "-:testsuite",
                          "liteos_port", 'lcd', 'qspi_display', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch', 'x_vfs_private',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'pm_service', 'pm_brandy',
                          'ulp_aon', 'hal_l2ram', 'cmn_header'],
        'ram_component_set': ['pinctrl', 'dfx_set', 'gpu']
    },
    'brandy-target5-r': {
        'board': 'evb',
        'base_target_name': 'target_brandy_recovery_freertos_template',
        'defines': ['PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CFG_DRIVERS_NANDFLASH',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", "__FREERTOS__", 'ENABLE_UIKIT', 'SW_UART_DEBUG',
                    "HASH_MEM_COPY", 'ENABLE_ECC', 'SUPPORT_RECOVERY_DISPLAY'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", "osal", "arch_port", "-:testsuite",
                          'lcd', 'qspi_display', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'pm_service', 'pm_brandy', 'x_vfs_private',
                          'ulp_aon', 'hal_l2ram', 'cmn_header'],
        'ram_component_set': ['pinctrl', 'dfx_set', 'gpu']
    },
    'brandy-native-js-r': {
        'board': 'evb',
        'base_target_name': 'target_brandy_recovery_template',
        'defines': ['PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', '__LITEOS__',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CFG_DRIVERS_NANDFLASH',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'ENABLE_UIKIT', 'SW_UART_DEBUG', "HASH_MEM_COPY",
                    'ENABLE_ECC', 'SUPPORT_RECOVERY_DISPLAY', 'MBEDTLS_NO_PLATFORM_ENTROPY', 'MBEDTLS_ENTROPY_HARDWARE_ALT',
                    'OH_MARKET_ENABLE'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", "osal", "arch_port", "-:testsuite",
                          "liteos_port", 'lcd', 'qspi_display', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'x_vfs_private',
                          'ulp_aon', 'hal_l2ram', 'pm_service', 'pm_brandy', 'cmn_header'],
        'ram_component_set': ['pinctrl', 'dfx_set' ,'gpu']
    },
    #volte debug target
    'brandy-volte-r': {
        'base_target_name': 'brandy-native-js-r',
    },
    #gnss debug target
    'brandy-gnss-r': {
        'base_target_name': 'brandy-native-js-r',
    },
    #nfc debug target
    'brandy-nfc-r': {
        'base_target_name': 'brandy-native-js-r',
    },
    'brandy-diting-r': {
        'base_target_name': 'brandy-native-js-r',
    },
    'brandy-diting-nfc-r': {
        'base_target_name': 'brandy-native-js-r',
    },
    'brandy-xts-r': {
        'board': 'evb',
        'base_target_name': 'target_brandy_recovery_template',
        'defines': ['PRE_ASIC', 'BRANDY_PRODUCT_EVB4', 'VERSION_STANDARD', '__LITEOS__',
                    '_ALL_SOURCE', 'SUPPORT_CXX', 'I2C_SLAVE_REG_ADDR_4BYTE', 'CFG_DRIVERS_NANDFLASH',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'ENABLE_UIKIT', 'SW_UART_DEBUG', "HASH_MEM_COPY",
                    'ENABLE_ECC', 'MBEDTLS_NO_PLATFORM_ENTROPY', 'MBEDTLS_ENTROPY_HARDWARE_ALT',
                    'OH_MARKET_ENABLE'],
        'ram_component': ["algorithm", "dfx_port_brandy", "dfx_update", "osal", "arch_port", "-:testsuite",
                          "liteos_port", 'lcd', 'qspi_display', 'dfx_file_operation',
                          'psram', 'hal_mipi', 'mipi_tx', 'non_os', 'touch',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'x_vfs_private',
                          'ulp_aon', 'hal_l2ram', 'pm_service', 'pm_brandy', 'cmn_header'],
        'ram_component_set': ['pinctrl', 'dfx_set']
    },
    'testsuite-brandy-sdk': {
        'base_target_name': 'target_brandy_testsuite_application',
        'board': 'evb',
        'defines': ['SW_UART_DEBUG', 'USE_CMSIS_OS', 'CFG_DRIVERS_NANDFLASH',
                    'PRE_ASIC', 'BRANDY_PRODUCT_EVB4',
                    '_ALL_SOURCE', '__LITEOS__', 'SUPPORT_CXX', 'ENABLE_UIKIT',
                    "-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", 'MULTIPLEXING_UART',
                    'CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH', 'CONFIG_SPI_AUTO_SWITCH_DMA_THRESHOLD=0x8',
                    'CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH', 'CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD=0x8'],
        'ram_component': ["osal", "arch_port", "liteos_port", 'psram', 'pm_service', 'pm_brandy',
                          'hal_mips', 'lcd', 'qspi_display', 'hal_dma_v100', "dfx_port_brandy",
                          'dfx_log_brandy', 'hal_mipi', 'mipi_tx', 'non_os', 'touch', 'ulp_aon',
                          'hal_l2ram', "test_security", "test_spi", 'dfx_file_operation',
                          "test_timer", "test_int", "test_pm",
                          "test_lcd", "test_qspi_display", "test_systick", "test_tcxo",  "test_watchdog",
                          "test_flash", "test_pwm", "test_i2c", "test_cmsis",
                          "test_xip", "test_mipi", "test_touch", 'algorithm', 'test_sdio', 'x_vfs_private',
                          'x_dpal', 'x_vfs', 'x_disk', 'x_fat', 'drv_mmc', 'fs_yaffs2' , 'nand_flash' , 'mtd_com',
                          'cmn_header', 'test_calendar', 'test_gpio', 'test_pinctrl', 'test_dma', 'test_uart',
                          'test_reboot', 'test_rtc_unified', 'test_adc', 'test_pmp', 'efuse', 'hal_efuse_v100', 'efuse_porting'],
        'ram_component_set': ['dfx_set', '-:gpu_test', 'rtc_unified'],
        'os': 'liteos',
    },
    'brandy-ssb': {
        'base_target_name': 'target_ssb_template_brandy',
        'board': 'evb',
        'defines': ["-:TARGET_CHIP_BRANDY=1", "-:BRANDY_CHIP_V100=1", "PRE_ASIC", "BRANDY_PRODUCT_EVB4",
                    "SSB_USE_PLL", "VERSION_STANDARD", "BUILD_APPLICATION_SSB", "LIBBOOTLOADER", "LIBCODELOADER",
                    "LIBCPU_UTILS", "LIBAPP_VERSION", "LIBPANIC", "LIBCONNECTIVITY", "LIBERROR_CODE", "LIBSEC_RANDOM",
                    "LIBCODELOADER_SSB", "LIBJLINK_LOAD", "LIBLIB_UTILS", "LIBBUILD_VERSION", 'CFG_DRIVERS_NANDFLASH', 'ENABLE_ECC',
                    "NO_TIMEOUT", "CONFIG_UART_BAUD_RATE_DIV_8", 'SW_UART_DEBUG', 'LARGE_BUFFER'],
        'ram_component': ['arch_port', 'hal_l2ram', 'sec_boot', 'libboundscheck',
                          'dfx_exception', 'error_code', 'psram', 'osal', 'cmn_header', 'boot_ulp', 'boot_nandflash', 'boot_emmc'],
        'ram_component_set': ['pinctrl'],
        'target': 'ssb',
    },
    'brandy-ssb-native-js': {
        'base_target_name': 'brandy-ssb',
    },
    'brandy-ssb-xts': {
        'base_target_name': 'brandy-ssb',
    },
    #volte debug target
    'brandy-ssb-volte': {
        'base_target_name': 'brandy-ssb',
    },
    #gnss debug target
    'brandy-ssb-gnss': {
        'base_target_name': 'brandy-ssb',
    },
    #nfc debug target
    'brandy-ssb-nfc': {
        'base_target_name': 'brandy-ssb',
    },
    'brandy-ssb-diting': {
        'base_target_name': 'brandy-ssb',
        'defines': ["FT_SINGLE_UART"],
    },
}

target_group = {
    # ci protect target list
    'pack_brandy_target1': ['brandy-ssb', 'brandy-target1-r', 'brandy-target1'],
    'pack_brandy_target3': ['brandy-ssb', 'brandy-target3-r', 'brandy-target3'],
    'pack_brandy_target4': ['brandy-ssb', 'brandy-target4-r', 'brandy-target4'],
    'pack_brandy_target5': ['brandy-ssb', 'brandy-target5-r', 'brandy-target5'],
    'pack_brandy_native_js': ['brandy-ssb-native-js', 'brandy-native-js-r', 'brandy-native-js'],
    'pack_brandy_native_js_user': ['brandy-ssb-native-js', 'brandy-native-js-r', 'brandy-native-js-user'],
    'pack_brandy_xts': ['brandy-ssb-xts', 'brandy-xts-r', 'brandy-xts'],
    'pack_brandy_auto_ota_ser': ['brandy-ssb-native-js', 'brandy-auto-ota-ser'],
    'pack_brandy_auto_ota_cli': ['brandy-ssb-native-js', 'brandy-auto-ota-cli'],
    'pack_brandy_gnss': ['brandy-ssb-gnss', 'brandy-gnss-r', 'brandy-gnss'],
    'pack_brandy_volte': ['brandy-ssb-volte', 'brandy-volte-r', 'brandy-volte'],
    'pack_brandy_nfc': ['brandy-ssb-nfc', 'brandy-nfc-r', 'brandy-nfc'],
    'pack_brandy_diting': ['brandy-ssb-diting', 'brandy-diting-r', 'brandy-diting'],
    'pack_brandy_diting_nfc': ['brandy-ssb-diting', 'brandy-diting-nfc-r', 'brandy-diting-nfc'],

    'brandy_all': ['brandy-ssb',
                   'brandy-target1', 'brandy-target3', 'brandy-target4', 'brandy-target5', 'brandy-native-js', 'brandy-native-js-user', 'brandy-xts',
                   'brandy-target1-r', 'brandy-target3-r', 'brandy-target4-r', 'brandy-target5-r', 'brandy-native-js-r', 'brandy-xts-r',
                   'testsuite-brandy-sdk']
}
