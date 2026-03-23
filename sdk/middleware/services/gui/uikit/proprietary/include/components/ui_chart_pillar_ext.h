/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UIChartPillarExt
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#ifndef GRAPHIC_LITE_UI_CHART_PILLAR_EXT_H
#define GRAPHIC_LITE_UI_CHART_PILLAR_EXT_H

#include "gfx_utils/vector.h"
#include "components/ui_chart.h"

namespace OHOS {
/**
 * @brief Provides special functions for implementing a bar chart.
 */
class UIChartPillarExt : public UIChartPillar {
public:
    /* *
     * @brief Enumerates pillar chart types.
     *
     */
    enum class PillarType {
        PILLAR_NORMAL,
        PILLAR_STACKED,
        PILLAR_BUTT
    };

    /**
     * @brief A constructor used to create a <b>UIChartPillarExt</b> instance.
     */
    UIChartPillarExt() {}

    /**
     * @brief A destructor used to delete the <b>UIChartPillarExt</b> instance.
     */
    ~UIChartPillarExt() override {}

    /**
     * @brief Refreshes a bar chart and redraws the dirty region.
     *
     * Only the parts that need to be redrawn are refreshed, for example, new data points.
     * This function provides better performance than {@link Invalidate}.
     */
    void RefreshChart() override;

    /**
     * @brief Set pillar type.
     * @param type pillar type.
     */
    void SetPillarType(PillarType type)
    {
        pillarType_ = type;
    }

    /**
     * @brief Get pillar type.
     * @return pillar type.
     */
    PillarType GetPillarType()
    {
        return pillarType_;
    }

    /**
     * @brief Set pillar percentage.
     * @param percentage pillar percentage, range is 0 to 1.0.
     */
    void SetPillarPercentage(float percentage)
    {
        if (FloatLess(percentage, 0)) {
            percentage = 0;
            GRAPHIC_LOGW("Min percentage is 0, less than 0 should be set 0.");
        }
        if (FloatMore(percentage, 1)) {
            percentage = 1;
            GRAPHIC_LOGW("Max percentage is 1.0, more than 1.0 should be set 1.0.");
        }
        pillarPercentage_ = percentage;
    }

    /**
     * @brief Get pillar percentage.
     * @return pillar percentage.
     */
    float GetPillarPercentage()
    {
        return pillarPercentage_;
    }

protected:
    void DrawDataSerials(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

private:
    /**
     * @brief Defines the font style.
     */
    struct StackedLineData {
        /** Start Point. */
        Point start;
        /** End Point. */
        Point end;
        /** Width of each bar. */
        uint16_t barWidth;
        /** Index of the bar. */
        uint16_t index;
        /** Accumulate height of the bar. */
        int16_t aclHeight;
    };
    float pillarPercentage_ = 0.4;
    PillarType pillarType_ = PillarType::PILLAR_NORMAL;
    void DrawNormalPillarData(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void DrawStackedPillarData(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void DrawStackedLine(BufferInfo& gfxDstBuffer, Rect mask,
        StackedLineData lineData, ListNode<UIChartDataSerial*>* iter);
    void DrawLine(BufferInfo& gfxDstBuffer, const Rect& mask, Point start, Point end, int16_t width,
        ColorType color);
    void DrawEllipse(BufferInfo& gfxDstBuffer, const Rect& mask, Point start, Point end, int16_t width,
        ColorType color);
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    void HWDrawEllipse(BufferInfo& gfxDstBuffer, const Rect& mask, Point start, Point end, int16_t width,
        uint32_t color);
#endif
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_CHART_PILLAR_EXT_H
/**
 * @}
 */
