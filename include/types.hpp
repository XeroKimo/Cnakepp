#pragma once
#include "bn_cstring.h"
#include  <cstring>

namespace cgba
{
    using uintptr = unsigned int;

    static_assert(sizeof(uintptr) == sizeof(void*));

    using i32 = int;
    using u32 = unsigned int;
    using i16 = short;
    using u16 = unsigned short;
    using i8 = signed char;
    using u8 = unsigned char;

    template<class Ty, class Ty2>
    struct is_same
    {
        static constexpr bool value = false; 
    };

    template<class Ty>
    struct is_same<Ty, Ty>
    {
        static constexpr bool value = true;
    };

    template<class Ty, class Ty2>
    concept same_as = is_same<Ty, Ty2>::value;

    
    static_assert(sizeof(i32) == 4);
    static_assert(sizeof(u32) == 4);
    static_assert(sizeof(i16) == 2);
    static_assert(sizeof(u16) == 2);
    static_assert(sizeof(i8) == 1);
    static_assert(sizeof(u8) == 1);

    template<class Ty>
    concept integral = same_as<Ty, u8> 
    || same_as<Ty, i8>
    || same_as<Ty, i16>
    || same_as<Ty, u16>
    || same_as<Ty, i32>
    || same_as<Ty, u32>;

    template<class Ty>
    concept integral_like = (sizeof(Ty) == sizeof(u8) && alignof(Ty) == alignof(u8)) 
    || (sizeof(Ty) == sizeof(i8) && alignof(Ty) == alignof(i8)) 
    || (sizeof(Ty) == sizeof(u16) && alignof(Ty) == alignof(u16)) 
    || (sizeof(Ty) == sizeof(i16) && alignof(Ty) == alignof(u16)) 
    || (sizeof(Ty) == sizeof(u32) && alignof(Ty) == alignof(i32)) 
    || (sizeof(Ty) == sizeof(i32) && alignof(Ty) == alignof(i32));

    constexpr uintptr mem_io = 0x0400'0000;
    constexpr uintptr vram = 0x0600'0000;

    template<integral_like Ty>
    volatile Ty& Memory(uintptr location)
    {
        return *reinterpret_cast<volatile Ty*>(location);
    }

    template<integral_like Ty>
    Ty& MemoryV(uintptr location)
    {
        return *reinterpret_cast<Ty*>(location);
    }

    enum class BackgroundMode
    {
        Zero,
        One,
        Two,
        Three,
        Four,
        Five
    };

    class RGB15
    {
        static constexpr u16 subColorMask = 0b0000'0000'0001'1111;
        static constexpr u16 u16RedBitShift = 0;
        static constexpr u16 u16GreenBitShift = 5;
        static constexpr u16 u16BlueBitShift = 10;

        static constexpr u16 u16RedMask = subColorMask << u16RedBitShift;
        static constexpr u16 u16GreenMask = subColorMask << u16GreenBitShift;
        static constexpr u16 u16BlueMask = subColorMask << u16BlueBitShift;

    private:
        u16 data = 0;

    public:
        constexpr RGB15() = default;
        constexpr explicit RGB15(u16 color) : data{ color }
        {

        } 

        constexpr RGB15(u32 r, u32 g, u32 b) :
            data{ static_cast<u16>(static_cast<u16>(r) | (static_cast<u16>(g) << u16GreenBitShift) | (static_cast<u16>(b) << u16BlueBitShift)) }
        {
        }

        constexpr void operator=(const RGB15& value) volatile
        {
            data = value.data;
        }

        constexpr void SetRed(u32 value) { SetValue(value, u16RedMask); }
        constexpr void SetGreen(u32 value) { SetValue(value, u16GreenMask); }
        constexpr void SetBlue(u32 value) { SetValue(value, u16BlueMask); }

        constexpr u16 GetRed() const noexcept { return GetValue(u16RedMask, u16RedBitShift); }
        constexpr u16 GetGreen() const noexcept { return GetValue(u16GreenMask, u16GreenBitShift); }
        constexpr u16 GetBlue() const noexcept { return GetValue(u16BlueMask, u16BlueBitShift); }

        constexpr u16 Data() const noexcept { return data;}
        constexpr explicit operator u16() const noexcept { return data; }

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

    static_assert(sizeof(RGB15) == sizeof(u16));
    static_assert(alignof(RGB15) == alignof(u16));
    
    struct Point
    {
        i32 x;
        i32 y;
    };

    struct Rectangle
    {
        i32 width;
        i32 height;
    };

    constexpr Rectangle screenResolution{ .width = 240, .height = 160 };

    enum class DisplayControlStatus : u16
    {
        Background_Mode0 = 0,
        Background_Mode1 = 1,
        Background_Mode2 = 2,
        Background_Mode3 = 3,
        Background_Mode4 = 4,
        Background_Mode5 = 5,
        Background_Mode_Mask = 0b111,
        Display_Frame_Select = 1 << 4,
        H_Blank_Interval_Free = 1 << 5,
        OBJ_Character_VRAM_Mapping = 1 << 6,
        Forced_Blank = 1 << 7,
        Background0_Visibility_Flag = 1 << 8,
        Background1_Visibility_Flag = 1 << 9,
        Background2_Visibility_Flag = 1 << 10,
        Background3_Visibility_Flag = 1 << 11,
        Object_Visibility_Flag = 1 << 12,
        Display_Window0 = 1 << 13,
        Display_Window1 = 1 << 14,
        Display_OBJ_Window = 1 << 15,
    };

    constexpr DisplayControlStatus operator|(const DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        return static_cast<DisplayControlStatus>(static_cast<u16>(lh) | static_cast<u16>(rh));
    }

    constexpr DisplayControlStatus operator&(const DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        return static_cast<DisplayControlStatus>(static_cast<u16>(lh) & static_cast<u16>(rh));
    }

    constexpr DisplayControlStatus operator^(const DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        return static_cast<DisplayControlStatus>(static_cast<u16>(lh) ^ static_cast<u16>(rh));
    }
    
    constexpr DisplayControlStatus operator|=(DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        return lh = lh | rh;
    }

    constexpr DisplayControlStatus operator&=(DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        return lh = lh & rh;
    }

    constexpr DisplayControlStatus operator^=(DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        return lh = lh ^ rh;
    }

    constexpr DisplayControlStatus operator~(const DisplayControlStatus& lh)
    {
        return static_cast<DisplayControlStatus>(~static_cast<u16>(lh));
    }

    //Supress warning as checking this code with other compilers give no warning at all, so I shall naively assume this to be correct code
#pragma GCC diagnostic ignored "-Winvalid-constexpr"
    constexpr DisplayControlStatus operator|(const volatile DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        return static_cast<DisplayControlStatus>(static_cast<u16>(lh) | static_cast<u16>(rh));
    }

    constexpr DisplayControlStatus operator&(const volatile DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        return static_cast<DisplayControlStatus>(static_cast<u16>(lh) & static_cast<u16>(rh));
    }

    constexpr DisplayControlStatus operator^(const volatile DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        return static_cast<DisplayControlStatus>(static_cast<u16>(lh) ^ static_cast<u16>(rh));
    }
    
    constexpr DisplayControlStatus operator|=(volatile DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        lh = lh | rh;
        return lh;
    }

    constexpr DisplayControlStatus operator&=(volatile DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        lh = lh & rh;
        return lh;
    }

    constexpr DisplayControlStatus operator^=(volatile DisplayControlStatus& lh, const DisplayControlStatus& rh)
    {
        lh = lh ^ rh;
        return lh;
    }

    constexpr DisplayControlStatus operator~(const volatile DisplayControlStatus& lh)
    {
        return static_cast<DisplayControlStatus>(~static_cast<u16>(lh));
    }
    
#pragma GCC diagnostic warning "-Winvalid-constexpr"

    volatile DisplayControlStatus& GetDisplayControlStatus()
    {
        return reinterpret_cast<volatile DisplayControlStatus&>(Memory<u16>(mem_io));
    } 

    struct BackgroundMode0
    {
        static constexpr DisplayControlStatus modeValue = DisplayControlStatus::Background_Mode0;
        static constexpr DisplayControlStatus backgroundLayerSupport = DisplayControlStatus::Background0_Visibility_Flag 
            | DisplayControlStatus::Background1_Visibility_Flag 
            | DisplayControlStatus::Background2_Visibility_Flag 
            | DisplayControlStatus::Background3_Visibility_Flag; 
        static constexpr bool rotationSupport = false;
        static constexpr bool scalingSupport = false;

        static void ShowBackground0() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background0_Visibility_Flag;
        }

        static void ShowBackground1() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background1_Visibility_Flag;
        }

        static void ShowBackground2() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background2_Visibility_Flag;
        }

        static void ShowBackground3() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background3_Visibility_Flag;
        }
    };

    struct BackgroundMode1
    {
        static constexpr DisplayControlStatus modeValue = DisplayControlStatus::Background_Mode1;
        static constexpr DisplayControlStatus backgroundLayerSupport = DisplayControlStatus::Background0_Visibility_Flag 
            | DisplayControlStatus::Background1_Visibility_Flag 
            | DisplayControlStatus::Background2_Visibility_Flag; 
            

        static void ShowBackground0() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background0_Visibility_Flag;
        }

        static void ShowBackground1() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background1_Visibility_Flag;
        }

        static void ShowBackground2() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background2_Visibility_Flag;
        }
    };

    struct BackgroundMode2
    {
        static constexpr DisplayControlStatus modeValue = DisplayControlStatus::Background_Mode2;
        static constexpr DisplayControlStatus backgroundLayerSupport = DisplayControlStatus::Background2_Visibility_Flag 
            | DisplayControlStatus::Background3_Visibility_Flag; 
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;

        static void ShowBackground1() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background1_Visibility_Flag;
        }

        static void ShowBackground2() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background2_Visibility_Flag;
        }
    };

    struct BackgroundMode3
    {
        static constexpr DisplayControlStatus modeValue = DisplayControlStatus::Background_Mode3;
        static constexpr DisplayControlStatus backgroundLayerSupport = DisplayControlStatus::Background2_Visibility_Flag;
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;
        
        static void ShowBackground2() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background2_Visibility_Flag;
        }

        static volatile RGB15* ScreenBuffer()
        {
            return &Memory<RGB15>(vram);
        } 

        static void VolatileMemcpy(volatile void* destination, void* source, std::size_t size)
        {
            char* begin = static_cast<char*>(source);
            char* end = static_cast<char*>(source) + size;
            for(;begin != end; ++begin)
            {
                *static_cast<volatile char*>(destination) = *begin;
            }
        }

        static void PlotPixel(Point position, RGB15 color) 
        {
            //std::memcpy(&(&MemoryV<u16>(vram))[position.x + position.y * screenResolution.width], &color, sizeof(color));
            //bn::memcpy(&(&MemoryV<u16>(vram))[position.x + position.y * screenResolution.width], &color, sizeof(color));
            //VolatileMemcpy(&(&Memory<u16>(vram))[position.x + position.y * screenResolution.width], &color, sizeof(color));

            //(&Memory<u16>(vram))[position.x + position.y * screenResolution.width] = color.Data();
            ScreenBuffer()[position.x + position.y * screenResolution.width] = color;
        }
    };
    
    struct BackgroundMode4
    {
        static constexpr DisplayControlStatus modeValue = DisplayControlStatus::Background_Mode4;
        static constexpr DisplayControlStatus backgroundLayerSupport = DisplayControlStatus::Background2_Visibility_Flag;
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;

        static void ShowBackground2() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background2_Visibility_Flag;
        }
    };

    struct BackgroundMode5
    {
        static constexpr DisplayControlStatus modeValue = DisplayControlStatus::Background_Mode5;
        static constexpr DisplayControlStatus backgroundLayerSupport = DisplayControlStatus::Background2_Visibility_Flag;
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;

        static void ShowBackground2() 
        {
            GetDisplayControlStatus() |= DisplayControlStatus::Background2_Visibility_Flag;
        }
    };
};