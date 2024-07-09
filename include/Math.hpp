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

        friend constexpr bool operator==(const Point& lh, const Point& rh) = default;

        friend constexpr Point& operator+=(Point& lh, const Point& rh)
        {
            lh.x += rh.x;
            lh.y += rh.y;
            return lh;
        }
        
        friend constexpr Point operator+(Point lh, const Point& rh)
        {
            return lh += rh;
        }
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
    
    //Min and Max are inclusive
    template<class Ty, Ty Min, Ty Max>
    struct Clamped
    {
        Ty value {};

        constexpr Clamped(Ty _value) :
            value{_value}
        {
            Clamp();
        }

        friend constexpr Clamped operator+(Clamped lh, Ty rh)
        {
            return lh += rh;
        }
        

        friend constexpr Clamped& operator+=(Clamped& lh, Ty rh)
        {
            lh.value += rh;
            return lh;
        }

        constexpr operator Ty() const { return value; }

    private:
        void Clamp()
        {
            if(value < Min)
                value = Min;
            else if(value > Max)
                value = Max;
        }
    };
}