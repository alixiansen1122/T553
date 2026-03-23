#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

#add_subdirectory_with_alias_if_exist(agnss/hdi_service ${PROJECT_BINARY_DIR}/ohos/location/location_agnss_hdi)
add_subdirectory_with_alias_if_exist(gnss/hdi_service ${PROJECT_BINARY_DIR}/ohos/location/location_gnss_hdi)
#add_subdirectory_with_alias_if_exist(gnss/gnss_gs21 ${PROJECT_BINARY_DIR}/ohos/location/location_gnss_gs21)
add_subdirectory_with_alias_if_exist(gnss/gnss_emulator ${PROJECT_BINARY_DIR}/ohos/location/location_gnss_emulator)
add_subdirectory_with_alias_if_exist(gnss/utils ${PROJECT_BINARY_DIR}/ohos/location/location_gnss_utils)
add_subdirectory_with_alias_if_exist(utils ${PROJECT_BINARY_DIR}/ohos/location/location_utils)