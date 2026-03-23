/*
* Copyright (c) @CompanyNameMagicTag. 2022-2022. All rights reserved.
* Description: camera common type
* Author: Media Software Group
* Create: 2022-12-20
*/

#ifndef CAMERA_COMMON_TYPES_H
#define CAMERA_COMMON_TYPES_H

#include "securec.h"
#include "hw_common.h"
#include "hw_camera.h"

namespace OHOS {
namespace Camera {
using RetCode = int32_t;
enum {
    OK = 0,
    ERROR = 1,
    INVALID_PARAM = 3,
};

typedef enum {
    STREAM_SUPPORTED = 0,
    STREAM_NEED_RECONFIG,
    STERAM_NOT_SUPPORTED,
    STREAM_SUPPORTED_BUTT,
} HalStreamSupportType;

typedef struct {
    int32_t id;
    StreamAttr attr;
} HalStreamInfo;

class DataTableManager {
public:
    DataTableManager();
    ~DataTableManager();
};
} // namespace Camera
} // namespace OHOS
#endif /* CAMERA_COMMON_TYPES_H */
