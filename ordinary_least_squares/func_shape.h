#pragma once
#include "stdafx.h"

#include "i_paintable.h"

namespace ordinary_least_squares {

    class FuncShape final : public IPaintable {
    public:
        FuncShape(std::vector<double> as, Gdiplus::Color color, float width)
            : as(as), color(color), width(width) {}

        void Paint(Gdiplus::Graphics& graphics, std::function<PointF(PointF)>& toScreenCoord, std::function<PointF(PointF)>& toRealCoord) {
            
            float step = 0.8;


            float xMin = toRealCoord(PointF(0, 0)).X;
            float xMax = toRealCoord(PointF(500, 0)).X;
            float dx = abs(toRealCoord(PointF(500, 0)).X - toRealCoord(PointF(500 - step, 0)).X);

            while (xMin <= xMax + 2* dx) {

                float x = xMin;
                float y = 0;
                float xd = 1;

                for (auto a : as) {
                    y += a * xd;
                    xd *= x;
                }

                PointF point(toScreenCoord(PointF(x, y)));

                graphics.FillEllipse(&Gdiplus::SolidBrush(color), point.X, point.Y, width, width);



                xMin += dx;
            }


        }

    private:
        std::vector<double> as;
        Gdiplus::Color color;
        float width;
    };

}