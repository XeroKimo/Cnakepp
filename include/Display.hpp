#pragma once
#include "Types.hpp"
#include "MemoryRegion.hpp"
#include "Math.hpp"
#include "EnumFlags.hpp"

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
        OBJ_Character_VRAM_Mapping_Mode =  1 << 6,
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

    enum class DisplayControlFlags : u16
    {
        Display_Frame_Select = static_cast<u16>(DisplayControlRegister::Display_Frame_Select),
        H_Blank_Interval_Free_Flag = static_cast<u16>(DisplayControlRegister::H_Blank_Interval_Free_Flag),
        OBJ_Character_VRAM_Mapping_Mode = static_cast<u16>(DisplayControlRegister::OBJ_Character_VRAM_Mapping_Mode),
        Forced_Blank_Flag = static_cast<u16>(DisplayControlRegister::Forced_Blank_Flag),
        Background0_Visibility_Flag = static_cast<u16>(DisplayControlRegister::Background0_Visibility_Flag),
        Background1_Visibility_Flag = static_cast<u16>(DisplayControlRegister::Background1_Visibility_Flag),
        Background2_Visibility_Flag = static_cast<u16>(DisplayControlRegister::Background2_Visibility_Flag),
        Background3_Visibility_Flag = static_cast<u16>(DisplayControlRegister::Background3_Visibility_Flag),
        Object_Visibility_Flag = static_cast<u16>(DisplayControlRegister::Object_Visibility_Flag),
        Display_Window0 = static_cast<u16>(DisplayControlRegister::Display_Window0),
        Display_Window1 = static_cast<u16>(DisplayControlRegister::Display_Window1),
        Display_OBJ_Window = static_cast<u16>(DisplayControlRegister::Display_OBJ_Window),
    };

    enum class OBJCharacterVRAMMappingMode
    {
        TwoDimensional = 0,
        OneDimensional = static_cast<u16>(DisplayControlFlags::OBJ_Character_VRAM_Mapping_Mode)
    };


    DECLARE_ENUM_FLAG_OPS(DisplayControlRegister);
    DECLARE_ENUM_VOLATILE_FLAG_OPS(DisplayControlRegister);

    DECLARE_ENUM_FLAG_OPS(DisplayControlFlags);
    DECLARE_ENUM_VOLATILE_FLAG_OPS(DisplayControlFlags);
    
    DECLARE_ENUM_FLAG_OPS2(DisplayControlRegister, DisplayControlFlags);
    DECLARE_ENUM_FLAG_OPS2(DisplayControlRegister, OBJCharacterVRAMMappingMode);
    DECLARE_ENUM_VOLATILE_FLAG_OPS2(DisplayControlRegister, DisplayControlFlags);

    
    struct Display
    {
        static constexpr Rectangle resolution{ .width = 240, .height = 160 };
        static constexpr i32 PointToIndex(Point position)
        {
            return position.x + position.y * resolution.width;
        }

        //Maybe make this private so that we can only set things 
        static volatile DisplayControlRegister& GetControlRegister()
        {
            return VolatileMemory<DisplayControlRegister>(display_control_register);
        }

        template<class Ty>
        static Ty& SetBackgroundMode()
        {
            cgba::DisplayControlRegister currentStatus = GetControlRegister();
            currentStatus &= ~cgba::DisplayControlRegister::Background_Mode_Mask;
            currentStatus |= Ty::modeValue;
            GetControlRegister() = currentStatus;
            return Ty::_dummy;
        }

        template<class Ty>
        static Ty& GetBackgroundMode()
        {
            ////TODO: Figure out a working assert that ensures access to this get backgorund mode matches the expected type
            //assert(GetControlRegister() & Ty::modeValue)
            return Ty::_dummy;
        }

        static void SetDisplayControlFlags(DisplayControlFlags flags)
        {
            GetControlRegister() |= flags;
        }

        static void ResetDisplayControlFlags(DisplayControlFlags flags)
        {
            GetControlRegister() &= ~flags;
        }
        
        static void EnableOAMHBlank()
        {
            SetDisplayControlFlags(DisplayControlFlags::H_Blank_Interval_Free_Flag);
        }

        static void DisableOAMHBlank()
        {
            ResetDisplayControlFlags(DisplayControlFlags::H_Blank_Interval_Free_Flag);
        }

        static void FlipFrameSelect()
        {
            GetControlRegister() ^= DisplayControlFlags::Display_Frame_Select;
        }
        
        static void SetOBJCharacterVRAMMappingMode(OBJCharacterVRAMMappingMode mode)
        {
            DisplayControlRegister oldState = GetControlRegister();
            oldState &= DisplayControlRegister::OBJ_Character_VRAM_Mapping_Mode;
            oldState |= mode;
            GetControlRegister() |= oldState;
        }

        static void EnableForcedBlank()
        {
            SetDisplayControlFlags(DisplayControlFlags::Forced_Blank_Flag);
        }

        static void DisableForcedBlank()
        {
            ResetDisplayControlFlags(DisplayControlFlags::Forced_Blank_Flag);
        }

        static void ShowBackground0()
        {
            SetDisplayControlFlags(DisplayControlFlags::Background0_Visibility_Flag);
        }

        static void ShowBackground1()
        {
            SetDisplayControlFlags(DisplayControlFlags::Background1_Visibility_Flag);
        }

        static void ShowBackground2()
        {
            SetDisplayControlFlags(DisplayControlFlags::Background2_Visibility_Flag);
        }

        static void ShowBackground3()
        {
            SetDisplayControlFlags(DisplayControlFlags::Background3_Visibility_Flag);
        }

        static void ShowObjectLayer()
        {
            SetDisplayControlFlags(DisplayControlFlags::Object_Visibility_Flag);
        }

        static void ShowWindow0()
        {
            SetDisplayControlFlags(DisplayControlFlags::Display_Window0);
        }

        static void ShowWindow1()
        {
            SetDisplayControlFlags(DisplayControlFlags::Display_Window1);
        }

        static void ShowObjectWindow()
        {
            SetDisplayControlFlags(DisplayControlFlags::Display_OBJ_Window);
        }
        
        static void HideBackground0()
        {
            ResetDisplayControlFlags(DisplayControlFlags::Background0_Visibility_Flag);
        }

        static void HideBackground1()
        {
            ResetDisplayControlFlags(DisplayControlFlags::Background1_Visibility_Flag);
        }

        static void HideBackground2()
        {
            ResetDisplayControlFlags(DisplayControlFlags::Background2_Visibility_Flag);
        }

        static void HideBackground3()
        {
            ResetDisplayControlFlags(DisplayControlFlags::Background3_Visibility_Flag);
        }

        static void HideObjectLayer()
        {
            ResetDisplayControlFlags(DisplayControlFlags::Object_Visibility_Flag);
        }
        
        static void HideWindow0()
        {
            ResetDisplayControlFlags(DisplayControlFlags::Display_Window0);
        }

        static void HideWindow1()
        {
            ResetDisplayControlFlags(DisplayControlFlags::Display_Window1);
        }

        static void HideObjectWindow()
        {
            ResetDisplayControlFlags(DisplayControlFlags::Display_OBJ_Window);
        }
    };

    struct BackgroundMode0
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode0;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background0_Visibility_Flag 
            | DisplayControlRegister::Background1_Visibility_Flag 
            | DisplayControlRegister::Background2_Visibility_Flag 
            | DisplayControlRegister::Background3_Visibility_Flag; 
        static constexpr bool rotationSupport = false;
        static constexpr bool scalingSupport = false;

        static BackgroundMode0 _dummy;

        static void ShowBackground0() 
        {
            Display::ShowBackground0();
        }

        static void ShowBackground1() 
        {
            Display::ShowBackground1();
        }

        static void ShowBackground2() 
        {
            Display::ShowBackground2();
        }

        static void ShowBackground3() 
        {
            Display::ShowBackground3();
        }
        
        static void HideBackground0() 
        {
            Display::HideBackground0();
        }

        static void HideBackground1() 
        {
            Display::HideBackground1();
        }

        static void HideBackground2() 
        {
            Display::HideBackground2();
        }

        static void HideBackground3() 
        {
            Display::HideBackground3();
        }
        
    private:
        BackgroundMode0() = default;
        BackgroundMode0(const BackgroundMode0&) = delete;
        BackgroundMode0(BackgroundMode0&&) = delete;
        BackgroundMode0& operator=(const BackgroundMode0&) = delete;
        BackgroundMode0& operator=(BackgroundMode0&&) = delete;
    };

    struct BackgroundMode1
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode1;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background0_Visibility_Flag 
            | DisplayControlRegister::Background1_Visibility_Flag 
            | DisplayControlRegister::Background2_Visibility_Flag; 

        static BackgroundMode1 _dummy;
            
        static void ShowBackground0() 
        {
            Display::ShowBackground0();
        }

        static void ShowBackground1() 
        {
            Display::ShowBackground1();
        }

        static void ShowBackground2() 
        {
            Display::ShowBackground2();
        }
            
        static void HideBackground0() 
        {
            Display::HideBackground0();
        }

        static void HideBackground1() 
        {
            Display::HideBackground1();
        }

        static void HideBackground2() 
        {
            Display::HideBackground2();
        }
        
    private:
        BackgroundMode1() = default;
        BackgroundMode1(const BackgroundMode1&) = delete;
        BackgroundMode1(BackgroundMode1&&) = delete;
        BackgroundMode1& operator=(const BackgroundMode1&) = delete;
        BackgroundMode1& operator=(BackgroundMode1&&) = delete;
    };

    struct BackgroundMode2
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode2;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag 
            | DisplayControlRegister::Background3_Visibility_Flag; 
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;
        
        static BackgroundMode2 _dummy;

        static void ShowBackground1() 
        {
            Display::ShowBackground1();
        }

        static void ShowBackground2() 
        {
            Display::ShowBackground2();
        }

        static void HideBackground1() 
        {
            Display::HideBackground1();
        }

        static void HideBackground2() 
        {
            Display::HideBackground2();
        }
        
    private:
        BackgroundMode2() = default;
        BackgroundMode2(const BackgroundMode2&) = delete;
        BackgroundMode2(BackgroundMode2&&) = delete;
        BackgroundMode2& operator=(const BackgroundMode2&) = delete;
        BackgroundMode2& operator=(BackgroundMode2&&) = delete;
    };

    struct BackgroundMode3
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode3;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag;
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;
        static BackgroundMode3 _dummy;
        
        static void ShowBackground2() 
        {
            Display::ShowBackground2();
        }
        
        static void HideBackground2() 
        {
            Display::HideBackground2();
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
            VolatileScreenBuffer()[Display::PointToIndex(position)] = color;
        }
        
    private:
        BackgroundMode3() = default;
        BackgroundMode3(const BackgroundMode3&) = delete;
        BackgroundMode3(BackgroundMode3&&) = delete;
        BackgroundMode3& operator=(const BackgroundMode3&) = delete;
        BackgroundMode3& operator=(BackgroundMode3&&) = delete;
    };
    
    struct BackgroundMode4
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode4;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag;
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;
        static BackgroundMode4 _dummy;

        static void ShowBackground2() 
        {
            Display::ShowBackground2();
        }
        
        static void HideBackground2() 
        {
            Display::HideBackground2();
        }

    private:
        BackgroundMode4() = default;
        BackgroundMode4(const BackgroundMode4&) = delete;
        BackgroundMode4(BackgroundMode4&&) = delete;
        BackgroundMode4& operator=(const BackgroundMode4&) = delete;
        BackgroundMode4& operator=(BackgroundMode4&&) = delete;
    };

    struct BackgroundMode5
    {
        static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode5;
        static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag;
        static constexpr bool rotationSupport = true;
        static constexpr bool scalingSupport = true;
        static BackgroundMode5 _dummy;

        static void ShowBackground2() 
        {
            Display::ShowBackground2();
        }
        
        static void HideBackground2() 
        {
            Display::HideBackground2();
        }

    private:
        BackgroundMode5() = default;
        BackgroundMode5(const BackgroundMode5&) = delete;
        BackgroundMode5(BackgroundMode5&&) = delete;
        BackgroundMode5& operator=(const BackgroundMode5&) = delete;
        BackgroundMode5& operator=(BackgroundMode5&&) = delete;
    };
}