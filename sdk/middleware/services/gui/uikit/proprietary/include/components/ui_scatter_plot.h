/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UIScatterPlot
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#ifndef UI_SCATTER_PLOT_H
#define UI_SCATTER_PLOT_H

#include "components/ui_chart.h"

namespace OHOS {
class UIScatterPlotDataSerial : public UIChartDataSerial {
public:
    UIScatterPlotDataSerial()
    {
        PointStyle style;
        style.radius = 5; // 5 : default radius
        style.strokeWidth = 0;
        style.fillColor = Color::White();
        style.strokeColor = Color::Black();

        pointStyle_ = style;
    }

    ~UIScatterPlotDataSerial() override {}

    void SetPointStyle(const PointStyle& style)
    {
        pointStyle_ = style;
    }

    const PointStyle& GetPointStyle() const
    {
        return pointStyle_;
    }

private:
    PointStyle pointStyle_;
};

class UIScatterPlot : public UIChart {
public:
    /**
     * @brief A constructor used to create a <b>UIScatterPlot</b> instance.
              UIScatter plot is a chart that displays data points corresponding their x and y values.
     */
    UIScatterPlot() {}

    /**
     * @brief A destructor used to delete the <b>UIScatterPlot</b> instance.
     */
    ~UIScatterPlot() override {}

    /**
     * @brief Refreshes a scatter plot and redraws the dirty region.
     *        Only the parts that need to be redrawn are refreshed, for example, new data points.
     *        This function provides better performance than {@link Invalidate}.
     */
    void RefreshChart() override;

protected:
    void DrawDataSerials(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
    void DrawPoints(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, UIScatterPlotDataSerial* data);
    void DrawSinglePoint(BufferInfo& gfxDstBuffer, Point center,
        const UIChartDataSerial::PointStyle& style, const Rect& mask);
};
} // namespace OHOS
#endif // UI_SCATTER_PLOT_H