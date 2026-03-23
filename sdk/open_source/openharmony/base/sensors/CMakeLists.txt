#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_with_alias_if_exist(frameworks ${PROJECT_BINARY_DIR}/ohos/sensors/sensor_client)
add_subdirectory_with_alias_if_exist(services ${PROJECT_BINARY_DIR}/ohos/sensors/sensor_service)
add_subdirectory_with_alias_if_exist(sample ${PROJECT_BINARY_DIR}/ohos/sensors/sensor_sample)