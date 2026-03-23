/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Graphic Lott Trim Path
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

#ifndef LOTT_TRIM_PATH_H
#define LOTT_TRIM_PATH_H
#include "common/graphic_hardware_types.h"
#include "gfx_utils/vector.h"

namespace OHOS {
using CmdVector = Graphic::Vector<uint8_t>;
using DataVector = Graphic::Vector<float>;
class LottTrimPath : public HeapBase {
public:
    LottTrimPath() = default;
    ~LottTrimPath();
    // if path is not changed, don't set it every frame to avoid unnecessary computation
    void SetPath(const Path& inPath);
    // Reset path and and clear its cached trim path
    void ClearPath();
    /**
     * @brief trim path using parameters startRatio, endRatio and offset
     * @param startRatio Indicates the start position ([0 ~ 1]) of trimed ratio range
     * @param endRatio Indicates the end position ([0 ~ 1]) of trimed ratio range
     * @param offset Indicates the offset position ([0 ~ 1]) based on range [startRatio, endRatio]
     * @param isParallel Indicates whether trim multiple path in parallel
     * @return Returns <b>Path</b> after trimming, it is created when the function was called firstly;
     *         reused or recreated in subsequent function calls; and released when the LottTrimPath obj is released
     */
    Path& TrimPath(float startRatio, float endRatio, float offset, bool isParallel = false);
    float Length();

    LottTrimPath(const LottTrimPath &) = delete;
    LottTrimPath &operator=(const LottTrimPath &) = delete;
    LottTrimPath(LottTrimPath &&) = delete;
    LottTrimPath &operator=(LottTrimPath &&) = delete;

private:
    struct PathScope {
        uint32_t startCmdIndex;
        uint32_t endCmdIndex;
        uint32_t startDataIndex;
        uint32_t endDataIndex;
    };
    void TrimPath();
    void TrimPath(const PathScope& scope, CmdVector& cmds, DataVector& data, float length);
    void TrimPathWithRange(const PathScope& scope, const Vector2<float> range, CmdVector& cmds, DataVector& data);
    void FillTrimPath(CmdVector& cmds, DataVector& data);

    float startRatio_{0.0f};
    float endRatio_{0.0f};
    bool isParallel_{false};
    float* segLen_{nullptr};
    float length_{0.0f};
    Path path_{0};
    Path trimPath_{0};
    uint32_t closeNum_{0};
    uint32_t moveNum_{0};
    bool needTrim_{false};
};
} // namespace OHOS
#endif // LOTT_TRIM_PATH_H