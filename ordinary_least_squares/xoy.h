#pragma once
#include "stdafx.h"

#include "i_paintable.h"
#include "line.h"

namespace ordinary_least_squares {

    class XOY final : public IPaintable {
    public:
        XOY(Gdiplus::Color color, float width)
            : color(color), width(width) {
            
        }

        void Paint(Gdiplus::Graphics& graphics, std::function<PointF(PointF)>& toScreenCoord, std::function<PointF(PointF)>& toRealCoord) {
            
            Gdiplus::Pen pen(color, width);

            Gdiplus::Pen tpen(Gdiplus::Color(120, 255, 255, 255), width);



            PointF tmpx = toRealCoord(PointF(450, 0));
            PointF tmpx2 = toRealCoord(PointF(50, 0));
            float dx = abs(toRealCoord(PointF(440, 0)).X - tmpx.X);

            // ox
            graphics.DrawLine(&pen, toScreenCoord(PointF(tmpx2.X, 0)), toScreenCoord(PointF(tmpx.X, 0)));
            graphics.DrawLine(&pen, toScreenCoord(PointF(tmpx.X - dx, dx)), toScreenCoord(PointF(tmpx.X, 0)));
            graphics.DrawLine(&pen, toScreenCoord(PointF(tmpx.X - dx, -dx)), toScreenCoord(PointF(tmpx.X, 0)));


            PointF tmpxm = toRealCoord(PointF(500, 0));
            PointF tmpx2m = toRealCoord(PointF(0, 0));
            graphics.DrawLine(&tpen, toScreenCoord(PointF(tmpx2m.X, 0)), toScreenCoord(PointF(tmpxm.X, 0)));


            PointF tmpy = toRealCoord(PointF(0, 450));
            PointF tmpy2 = toRealCoord(PointF(0, 50));
            // oy
            graphics.DrawLine(&pen, toScreenCoord(PointF(0, tmpy.Y)), toScreenCoord(PointF(0, tmpy2.Y)));
            graphics.DrawLine(&pen, toScreenCoord(PointF(dx, tmpy2.Y - dx)), toScreenCoord(PointF(0, tmpy2.Y)));
            graphics.DrawLine(&pen, toScreenCoord(PointF(-dx, tmpy2.Y - dx)), toScreenCoord(PointF(0, tmpy2.Y)));


            PointF tmpym = toRealCoord(PointF(0, 500));
            PointF tmpy2m = toRealCoord(PointF(0, 0));
            // oy
            graphics.DrawLine(&tpen, toScreenCoord(PointF(0, tmpym.Y)), toScreenCoord(PointF(0, tmpy2m.Y)));



        }

    private:
        Gdiplus::Color color;
        float width;
    };

}