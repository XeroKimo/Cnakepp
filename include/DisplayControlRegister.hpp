#pragma once
#include "Types.hpp"
#include "MemoryRegion.hpp"
#include "Constants.hpp"

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

    enum class OBJCharacterVRAMMappingMode
    {
        TwoDimensional = 0,
        OneDimensional = 1
    };

    enum class DisplayControlRegister : u16
    {
        Background_Mode0 = 0,
        Background_Mode1 = 1,
        Background_Mode2 = 2,
        Background_Mode3 = 3,
        Background_Mode4 = 4,
        Background_Mode5 = 5,
        Background_Mode_Mask = (1 << 3) - 1,
        Display_Frame_Select = 1 << 4,
        H_Blank_Interval_Free_Flag = 1 << 5,
        OBJ_Character_VRAM_Mapping_Mode = 1 << 6,
        Forced_Blank_Flag = 1 << 7,
        Background0_Visibility_Flag = 1 << 8,
        Background1_Visibility_Flag = 1 << 9,
        Background2_Visibility_Flag = 1 << 10,
        Background3_Visibility_Flag = 1 << 11,
        Object_Visibility_Flag = 1 << 12,
        Display_Window0 = 1 << 13,
        Display_Window1 = 1 << 14,
        Display_OBJ_Window = 1 << 15,
    };

    constexpr DisplayControlRegister operator|(const DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        return static_cast<DisplayControlRegister>(static_cast<u16>(lh) | static_cast<u16>(rh));
    }

    constexpr DisplayControlRegister operator&(const DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        return static_cast<DisplayControlRegister>(static_cast<u16>(lh) & static_cast<u16>(rh));
    }

    constexpr DisplayControlRegister operator^(const DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        return static_cast<DisplayControlRegister>(static_cast<u16>(lh) ^ static_cast<u16>(rh));
    }
    
    constexpr DisplayControlRegister operator|=(DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        return lh = lh | rh;
    }

    constexpr DisplayControlRegister operator&=(DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        return lh = lh & rh;
    }

    constexpr DisplayControlRegister operator^=(DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        return lh = lh ^ rh;
    }

    constexpr DisplayControlRegister operator~(const DisplayControlRegister& lh)
    {
        return static_cast<DisplayControlRegister>(~static_cast<u16>(lh));
    }

    //Supress warning as checking this code with other compilers give no warning at all, so I shall naively assume this to be correct code
#pragma GCC diagnostic ignored "-Winvalid-constexpr"
    constexpr DisplayControlRegister operator|(const volatile DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        return static_cast<DisplayControlRegister>(static_cast<u16>(lh) | static_cast<u16>(rh));
    }

    constexpr DisplayControlRegister operator&(const volatile DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        return static_cast<DisplayControlRegister>(static_cast<u16>(lh) & static_cast<u16>(rh));
    }

    constexpr DisplayControlRegister operator^(const volatile DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        return static_cast<DisplayControlRegister>(static_cast<u16>(lh) ^ static_cast<u16>(rh));
    }
    
    constexpr DisplayControlRegister operator|=(volatile DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        lh = lh | rh;
        return lh;
    }

    constexpr DisplayControlRegister operator&=(volatile DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        lh = lh & rh;
        return lh;
    }

    constexpr DisplayControlRegister operator^=(volatile DisplayControlRegister& lh, const DisplayControlRegister& rh)
    {
        lh = lh ^ rh;
        return lh;
    }

    constexpr DisplayControlRegister operator~(const volatile DisplayControlRegister& lh)
    {
        return static_cast<DisplayControlRegister>(~static_cast<u16>(lh));
    }
    
#pragma GCC diagnostic warning "-Winvalid-constexpr"

    volatile DisplayControlRegister& GetDisplayControlRegister()
    {
        return VolatileMemory<DisplayControlRegister>(io_registers);
    } 

    struct BackgroundMode0
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode0;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background0_Visibility_Flag 
            | DisplayControlRegister::Background1_Visibility_Flag 
            | DisplayControlRegister::Background2_Visibility_Flag 
            | DisplayControlRegister::Background3_Visibility_Flag; 
        static constexpr bool rotationSupport = false;
        static constexpr bool scalingSupport = false;

        static void ShowBackground0() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background0_Visibility_Flag;
        }

        static void ShowBackground1() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background1_Visibility_Flag;
        }

        static void ShowBackground2() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background2_Visibility_Flag;
        }

        static void ShowBackground3() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background3_Visibility_Flag;
        }
    };

    struct BackgroundMode1
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode1;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background0_Visibility_Flag 
            | DisplayControlRegister::Background1_Visibility_Flag 
            | DisplayControlRegister::Background2_Visibility_Flag; 
            

        static void ShowBackground0() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background0_Visibility_Flag;
        }

        static void ShowBackground1() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background1_Visibility_Flag;
        }

        static void ShowBackground2() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background2_Visibility_Flag;
        }
    };

    struct BackgroundMode2
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode2;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag 
            | DisplayControlRegister::Background3_Visibility_Flag; 
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;

        static void ShowBackground1() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background1_Visibility_Flag;
        }

        static void ShowBackground2() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background2_Visibility_Flag;
        }
    };

    struct BackgroundMode3
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode3;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag;
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;
        
        static void ShowBackground2() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background2_Visibility_Flag;
        }
                
        static RGB15* ScreenBuffer()
        {
            return &VRAM<RGB15>(0);
        } 

        static VolatileRGB15* VolatileScreenBuffer()
        {
            return &VRAM<VolatileRGB15>(0);
        } 

        static void PlotPixel(Point position, RGB15 color) 
        {
            VolatileScreenBuffer()[Screen::PointToIndex(position)] = color;
        }
    };
    
    struct BackgroundMode4
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode4;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag;
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;

        static void ShowBackground2() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background2_Visibility_Flag;
        }
    };

    struct BackgroundMode5
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode5;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag;
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;

        static void ShowBackground2() 
        {
            GetDisplayControlRegister() |= DisplayControlRegister::Background2_Visibility_Flag;
        }
    };
}