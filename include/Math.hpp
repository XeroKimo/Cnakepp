#pragma once
#include "types.hpp"
#include <concepts>
#include <climits>
#include <limits>
#include "bn_assert.h"

namespace cgba
{
    template<class Ty>
    struct Point
    {
        Ty x;
        Ty y;
    };

    struct Rectangle
    {
        i32 width;
        i32 height;
    };
    
    static_assert(std::numeric_limits<float>::is_iec559);

    //TODO: Actually implement fixed point maths
    template<std::integral Ty, i32 DecimalPoint>
        requires (DecimalPoint < sizeof(Ty) * CHAR_BIT)
    struct Fixed
    {
        Ty data;
    };
    

    //Min and Max are inclusive
    template<class Ty, Ty Min, Ty Max>
    struct Range
    {
        Ty value {};

        constexpr Range(Ty _value) :
            value{_value}
        {
            BN_ASSERT(_value >= Min && _value <= Max);
        }

        constexpr operator Ty() const { return value; }
    };
}