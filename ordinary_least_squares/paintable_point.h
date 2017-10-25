#pragma once
#include "stdafx.h"

#include "i_paintable.h"

namespace ordinary_least_squares {

    class PaintablePoint final : public IPaintable {
    public:
        PaintablePoint(PointF point, Gdiplus::Color color, float width)
            : point(point), color(color), width(width) {}

        void Paint(Gdiplus::Graphics& graphics, std::function<PointF(PointF)>& toScreenCoord, std::function<PointF(PointF)>& toRealCoord) {

            PointF coord = toScreenCoord(PointF(point.X, point.Y));
            graphics.FillEllipse(&Gdiplus::SolidBrush(color), coord.X - width / 2, coord.Y - width / 2, width, width);

        }

    private:
        PointF point;
        Gdiplus::Color color;
        float width;
    };

}