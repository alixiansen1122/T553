#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_with_alias_if_exist(hal_lite/sensor_mgr ${PROJECT_BINARY_DIR}/ohos/sensor_hdi_lite/sensor_mgr)
add_subdirectory_with_alias_if_exist(hal_lite/sensors/hsensor/acc_simulator ${PROJECT_BINARY_DIR}/ohos/sensor_hdi_lite/hsensors/acc_simulator)
add_subdirectory_with_alias_if_exist(hal_lite/sensors/hsensor/barometer ${PROJECT_BINARY_DIR}/ohos/sensor_hdi_lite/hsensors/barometer)
add_subdirectory_with_alias_if_exist(hal_lite/sensors/hsensor/gyroscope ${PROJECT_BINARY_DIR}/ohos/sensor_hdi_lite/hsensors/gyroscope)
add_subdirectory_with_alias_if_exist(hal_lite/sensors/hsensor/heart_rate_counter ${PROJECT_BINARY_DIR}/ohos/sensor_hdi_lite/hsensors/heart_rate_counter)
add_subdirectory_with_alias_if_exist(hal_lite/sensors/hsensor/pedometer ${PROJECT_BINARY_DIR}/ohos/sensor_hdi_lite/hsensors/pedometer)
add_subdirectory_with_alias_if_exist(hal_lite/sensors/hsensor/wear_detection ${PROJECT_BINARY_DIR}/ohos/sensor_hdi_lite/hsensors/wear_detection)
add_subdirectory_with_alias_if_exist(hal_lite/sensors/vsensor/acc_alg_simulator ${PROJECT_BINARY_DIR}/ohos/sensor_hdi_lite/vsensors)