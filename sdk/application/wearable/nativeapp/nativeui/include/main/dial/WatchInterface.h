/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: WatchInterface
 * Author:
 * Create: 2025-03-17
 */

#ifndef WATCH_INTERFACE_H
#define WATCH_INTERFACE_H
#include <cstdint>
#include "format_type.h"
#include "dial_market.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get all watch face information
 * @param [out] watch_dial_info **info
 *         A pointer to a pointer where the retrieved watch face information will be stored.
 * @param [out] uint8_t *num
 *         A pointer to the number of watch face information entries retrieved.
 * @return bool
 *         - true: Successfully retrieved watch face information.
 *           The pointer pointed to by `info` needs to be released
 * externally.
 *         - false: Failed to retrieve watch face information.
 * @remarks
 *          - If the function returns true,
 *            the memory pointed to by `info` must be freed by the caller to prevent
 * memory leaks.
 *          - If the function returns false, the values of `info` and `num` remain unchanged.
 */
bool GetAllWatchFaceInfo(watch_dial_info **info, uint8_t *num);

/**
 * @brief Get information of a specific watch face by UUID
 * @param[in] uuid Unique identifier of the watch face
 * @param[out] watch Pointer to store the watch face information
 * @return bool Returns true if successful, false otherwise
 */
bool GetWatchFaceInfo(uint32_t uuid, watch_dial_info *watch);

/**
 * @brief Get the capability set of the watch
 * @param[out] capability Pointer to store the watch capability information
 * @return bool Returns true if successful, false otherwise
 */
bool GetWatchCapability(watch_dev_cap_info *capability);

/**
 * @brief Get system information
 * @param[out] sysInfo Pointer to store the system information
 * @return bool Returns true if successful, false otherwise
 */
bool GetWatchSystemInfo(watch_sys_info *sysInfo);

/**
 * @brief Get information of the currently active watch face
 * @param[out] watch Pointer to store the watch face information
 * @return bool Returns true if successful, false otherwise
 */
bool GetCurrentWatchFaceInfo(watch_dial_info *watch);

/**
 * @brief Set the currently active watch face by UUID
 * @param[in] uuid Unique identifier of the watch face to set as active
 * @return bool Returns true if successful, false otherwise
 */
bool SetCurrentWatchFace(uint32_t uuid);

/**
 * @brief Uninstall a watch face by UUID
 * @param[in] uuid Unique identifier of the watch face to uninstall
 * @return bool Returns true if successful, false otherwise
 * @note If the UUID corresponds to the currently active watch face, it will switch to the default watch face
 */
bool UninstallWatch(uint32_t uuid);

/**
 * @brief Retrieves the unique identifier (UUID) associated with a specified path.
 *
 * @param[in] path The path to look up.
 * @param[out] uuid A pointer to a variable where the UUID will be stored if found.
 *
 * @return bool
 *         - true if the UUID is successfully retrieved.
 *         - false if the path is invalid or the UUID cannot be obtained.
 *
 * @remarks The caller is responsible for ensuring that the 'uuid' pointer is valid and properly aligned.
 */
bool GetUuidByPath(const char *path, uint32_t *uuid);

#ifdef __cplusplus
}
#endif
#endif  // WATCH_INTERFACE_H