#pragma once
#include "Types.hpp"
#include "MemoryRegion.hpp"
#include "Math.hpp"
#include "EnumFlags.hpp"
#include "VRAMFormats.hpp"
#include <limits>

namespace cgba
{   


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

    enum class DisplayStatusRegister : u16
    {
        verticalBlank = 1 << 0,
        horizontalBlank = 1 << 1,
        verticalCounter = 1 << 2,
        verticalBlankIRQEnable = 1 << 3,
        horizontalBlankIRQEnable = 1 << 4,
        verticalCounterIRQEnable = 1 << 5,
        
        verticalCounterSettingBitOffset = 8,
        verticalCounterSettingMask = std::numeric_limits<u8>::max() << verticalCounterSettingBitOffset
    };

    enum class DisplayStatusFlags : u16
    {
        verticalBlank = 1 << 0,
        horizontalBlank = 1 << 1,
        verticalCounter = 1 << 2,
        verticalBlankIRQEnable = 1 << 3,
        horizontalBlankIRQEnable = 1 << 4,
        verticalCounterIRQEnable = 1 << 5,
    };
        
    DECLARE_ENUM_FLAG_OPS(DisplayStatusRegister);
    DECLARE_ENUM_VOLATILE_FLAG_OPS(DisplayStatusRegister);

    DECLARE_ENUM_FLAG_OPS(DisplayStatusFlags);
    DECLARE_ENUM_VOLATILE_FLAG_OPS(DisplayStatusFlags);

    DECLARE_ENUM_FLAG_OPS2(DisplayStatusRegister, DisplayStatusFlags);
    DECLARE_ENUM_VOLATILE_FLAG_OPS2(DisplayStatusRegister, DisplayStatusFlags);
    
    
    struct Display
    {
        static constexpr Rectangle resolution{ .width = 240, .height = 160 };
        static constexpr i32 PointToIndex(Point<i32> position)
        {
            return position.x + position.y * resolution.width;
        }

        //Maybe make this private so that we can only set things 
        static volatile DisplayControlRegister& GetControlRegister()
        {
            return VolatileMemory<DisplayControlRegister>(display_control_register);
        }

        static volatile DisplayStatusRegister& GetStatusRegister()
        {
            return VolatileMemory<DisplayStatusRegister>(display_status_register);
        }

        static u16 GetVerticalCounter()
        {
            return VolatileMemory<u16>(vertical_counter_register) & std::numeric_limits<u8>::max();
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

        static void ShowBackground(i32 layer)
        {
            Display::SetDisplayControlFlags(static_cast<DisplayControlFlags>(static_cast<u16>(cgba::DisplayControlFlags::Background0_Visibility_Flag) << layer));
        }

        static void HideBackground(i32 layer)
        {
            Display::ResetDisplayControlFlags(static_cast<DisplayControlFlags>(static_cast<u16>(cgba::DisplayControlFlags::Background0_Visibility_Flag) << layer));
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
    
    static constexpr uintptr background_control_register_base_address = 0x0400'0008;
    static constexpr uintptr background_scroll_offset_register_base_address = 0x0400'0010;
    static constexpr uintptr background_rotation_scale_register_base_address = 0x0400'0020;
    
    struct BackgroundControlRegister
    {
        volatile u16 data;
    };    
    
    struct BackgroundScrollRegister
    {
        volatile Point<i32> offset;
    };

    struct AffineTransform
    {
        Fixed<i16, 7> x; 
        Fixed<i16, 7> dx; 
        Fixed<i16, 7> y; 
        Fixed<i16, 7> dy; 
    };
    
    struct BackgroundTransformRegister
    {
        volatile AffineTransform transform; 
        volatile Point<Fixed<i32, 7>> pivot; 
    };

    static_assert(sizeof(BackgroundTransformRegister) == 16);

    struct Background3BitmapFormat
    {
        static constexpr uintptr frame_buffer_base_address = vram;
        static constexpr Rectangle frame_buffer_size{ 240, 160 };
        using ColorFormat = RGB15;
        using PixelFormat = VolatileRGB15;

        static constexpr i32 PositionToIndex(Point<i32> position) { return position.x + position.y * frame_buffer_size.width; }
        static PixelFormat& PixelAt(Point<i32> position) { return (&Memory<PixelFormat>(frame_buffer_base_address))[PositionToIndex(position)]; }
    };

    struct Background4BitmapFormat
    {
        static constexpr uintptr frame_buffer_base_address = vram;
        static constexpr Rectangle frame_buffer_size{ 240, 160 };
        using ColorFormat = Pallette8;

        //TODO: Make this a volatile Pallette8
        //TODO: If this does unoptimal code gen, make this be some view class instead that is a wider type underneath and do
        //bit operations manually
        using PixelFormat = VolatileRGB15;
        
        static constexpr i32 PositionToIndex(Point<i32> position) { return position.x + position.y * frame_buffer_size.width; }
        static PixelFormat& PixelAt(Point<i32> position) { return (&Memory<PixelFormat>(frame_buffer_base_address))[PositionToIndex(position)]; }
    };
    
    struct Background5BitmapFormat
    {
        static constexpr uintptr frame_buffer_base_address = vram;
        static constexpr Rectangle frame_buffer_size{ 160, 128 };
        using ColorFormat = RGB15;
        using PixelFormat = VolatileRGB15;
        
        static constexpr i32 PositionToIndex(Point<i32> position) { return position.x + position.y * frame_buffer_size.width; }
        static PixelFormat& PixelAt(Point<i32> position) { return (&Memory<PixelFormat>(frame_buffer_base_address))[PositionToIndex(position)]; }
    };

    class TileBackgroundView
    {
    private:
        i32 layer;

    public:
        constexpr TileBackgroundView(i32 inLayer) :
            layer{inLayer}
        {
            
        }

    public:
        void Show()
        {
            Display::ShowBackground(layer);
        }

        void Hide()
        {
            Display::HideBackground(layer);
        }
    };

    class AffineTileBackgroundView
    {
    private:
        i32 layer;

    public:
        constexpr AffineTileBackgroundView(i32 inLayer) :
            layer{inLayer}
        {
            
        }

    public:
        void Show()
        {
            Display::ShowBackground(layer);
        }

        void Hide()
        {
            Display::HideBackground(layer);
        }

    };

    template<class Format>
    class BitmapBackgroundView
    {
    private:
        i32 layer;

    public:
        constexpr BitmapBackgroundView(i32 inLayer) :
            layer{inLayer}
        {
            
        }

    public:
        void Show()
        {
            Display::ShowBackground(layer);
        }

        void Hide()
        {
            Display::HideBackground(layer);
        }

        void PlotPixel(Point<i32> position, Format::ColorFormat color)
        {
            Format::PixelAt(position) = color;
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

        static TileBackgroundView GetBackground0() { return TileBackgroundView{0}; }
        static TileBackgroundView GetBackground1() { return TileBackgroundView{1}; }
        static TileBackgroundView GetBackground2() { return TileBackgroundView{2}; }
        static TileBackgroundView GetBackground3() { return TileBackgroundView{3}; }
        
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

        static TileBackgroundView GetBackground0() { return TileBackgroundView{0}; }
        static TileBackgroundView GetBackground1() { return TileBackgroundView{1}; }
        static AffineTileBackgroundView GetBackground2() { return AffineTileBackgroundView{2}; }
        
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

        static AffineTileBackgroundView GetBackground2() { return AffineTileBackgroundView{2}; }
        static AffineTileBackgroundView GetBackground3() { return AffineTileBackgroundView{3}; }
        
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
        
        static BitmapBackgroundView<Background3BitmapFormat> GetBackground2() { return {2}; }
        
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

        static BitmapBackgroundView<Background4BitmapFormat> GetBackground2() { return {2}; }

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

        static BitmapBackgroundView<Background5BitmapFormat> GetBackground2() { return {2}; }

    private:
        BackgroundMode5() = default;
        BackgroundMode5(const BackgroundMode5&) = delete;
        BackgroundMode5(BackgroundMode5&&) = delete;
        BackgroundMode5& operator=(const BackgroundMode5&) = delete;
        BackgroundMode5& operator=(BackgroundMode5&&) = delete;
    };
}