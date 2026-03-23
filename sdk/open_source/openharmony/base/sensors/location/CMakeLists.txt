#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_with_alias_if_exist(frameworks/native/locator_sdk ${PROJECT_BINARY_DIR}/ohos/location/locator_sdk)
add_subdirectory_with_alias_if_exist(services/location_locator ${PROJECT_BINARY_DIR}/ohos/location/location_locator)
add_subdirectory_with_alias_if_exist(services/location_gnss ${PROJECT_BINARY_DIR}/ohos/location/location_gnss)
add_subdirectory_with_alias_if_exist(services/utils ${PROJECT_BINARY_DIR}/ohos/location/location_common)
add_subdirectory_with_alias_if_exist(sample ${PROJECT_BINARY_DIR}/ohos/location/location_sample)