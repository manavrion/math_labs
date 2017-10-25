#pragma once
#include "stdafx.h"

#include <functional>

namespace ordinary_least_squares {

    struct IPaintable {
        virtual void Paint(Gdiplus::Graphics& graphics, std::function<PointF(PointF)>& toScreenCoord) = 0;
    };

}