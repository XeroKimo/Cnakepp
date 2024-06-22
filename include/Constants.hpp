#pragma once
#include "Types.hpp"
#include "Math.hpp"

namespace cgba
{
    struct Screen
    {
        static constexpr Rectangle resolution{ .width = 240, .height = 160 };
        static constexpr i32 PointToIndex(Point position)
        {
            return position.x + position.y * resolution.width;
        }
    };
}