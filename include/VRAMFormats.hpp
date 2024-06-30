#pragma once
#include "Types.hpp"

namespace cgba
{
    template<bool isVolatile>
    struct RGB15TemplateData
    {
        u16 data;
    };
    
    template<>
    struct RGB15TemplateData<true>
    {
        volatile u16 data;
    };

    template<bool isVolatile>
    class RGB15Template : private RGB15TemplateData<isVolatile>
    {
        static constexpr u16 subColorMask = 0b0000'0000'0001'1111;
        static constexpr u16 u16RedBitShift = 0;
        static constexpr u16 u16GreenBitShift = 5;
        static constexpr u16 u16BlueBitShift = 10;

        static constexpr u16 u16RedMask = subColorMask << u16RedBitShift;
        static constexpr u16 u16GreenMask = subColorMask << u16GreenBitShift;
        static constexpr u16 u16BlueMask = subColorMask << u16BlueBitShift;

    private:
        using RGB15TemplateData<isVolatile>::data;

    public:
        constexpr RGB15Template() = default;
        
        constexpr explicit RGB15Template(u16 color) : RGB15TemplateData<isVolatile>{ color }
        {

        } 

        constexpr RGB15Template(u32 r, u32 g, u32 b) :
            RGB15TemplateData<isVolatile>{ static_cast<u16>(static_cast<u16>(r) | (static_cast<u16>(g) << u16GreenBitShift) | (static_cast<u16>(b) << u16BlueBitShift)) }
        {
        }

        constexpr void SetRed(u32 value) { SetValue(value, u16RedMask); }
        constexpr void SetGreen(u32 value) { SetValue(value, u16GreenMask); }
        constexpr void SetBlue(u32 value) { SetValue(value, u16BlueMask); }

        constexpr u16 GetRed() const noexcept { return GetValue(u16RedMask, u16RedBitShift); }
        constexpr u16 GetGreen() const noexcept { return GetValue(u16GreenMask, u16GreenBitShift); }
        constexpr u16 GetBlue() const noexcept { return GetValue(u16BlueMask, u16BlueBitShift); }

        constexpr u16 Data() const noexcept { return data;}
        constexpr explicit operator u16() const noexcept { return data; }
        constexpr operator RGB15Template<!isVolatile>() const noexcept { return RGB15Template<!isVolatile>{ data }; }

    private:
        constexpr void SetValue(u32 value, u16 colorMask)
        {
            data = (data & ~colorMask) | (value & colorMask);
        }

        constexpr u16 GetValue(u16 colorMask, u16 colorBitShift) const noexcept
        {
            return (data & colorMask) >> colorBitShift;
        }
    };

    using RGB15 = RGB15Template<false>;
    using VolatileRGB15 = RGB15Template<true>;

    static_assert(std::is_trivially_copyable_v<RGB15>);
    static_assert(sizeof(RGB15) == sizeof(u16) && alignof(RGB15) == alignof(u16));
        
    struct Pallette4
    {
        u8 index;
    };
       
    struct Pallette8
    {
        u8 index;
    };
    
    struct VolatilePallette8
    {
        volatile u8 index;

        VolatilePallette8() = default;
        VolatilePallette8(const Pallette8& v) :
            index{v.index}
        {   

        }

        operator Pallette8() const { return {index}; }
    };

    struct Tile4
    {
        u8 data[8 * 8 / 2];

        constexpr Tile4() = default;
        constexpr Tile4(u8 (&data)[8 * 8]);
        constexpr Tile4(u8 (&data)[8 * 8 / 2]);
    };

    struct Tile8
    {
        u8 data[8 * 8];
    };

    struct BackgroundMapTile
    {
        u16 data;
    };
}