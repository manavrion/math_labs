#pragma once
#include "stdafx.h"

#include "i_paintable.h"

namespace ordinary_least_squares {

    class Line final : public IPaintable {
    public:
        Line(PointF a, PointF b, Gdiplus::Color color, float width)
            : a(a), b(b), color(color), width(width) {}

        void Paint(Gdiplus::Graphics& graphics, std::function<PointF(PointF)>& toScreenCoord) {
            graphics.DrawLine(&Gdiplus::Pen(color, width), toScreenCoord(a), toScreenCoord(b));
        }

    private:
        PointF a;
        PointF b;
        Gdiplus::Color color;
        float width;
    };

}