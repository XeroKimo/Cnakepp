#pragma once
#include "Types.hpp"
#include "MemoryRegion.hpp"
#include "Math.hpp"
#include "EnumFlags.hpp"
#include "VRAMFormats.hpp"
#include <limits>
#include <bit>
#include "bn_assert.h"

namespace cgba
{   
    template<std::integral MaskType, class IOType, MaskType MaskSize, MaskType MaskShift>
        requires (MaskShift < sizeof(MaskType) * CHAR_BIT)
    struct RegisterBit
    {
        using type = IOType;
        static constexpr MaskType bitMask = (MaskSize == 1) ? 1 << MaskShift : ((1 << MaskSize) - 1) << MaskShift;
        static constexpr MaskType negatedBitMask = static_cast<MaskType>(~bitMask);
        static constexpr MaskType bitShift = MaskShift;

        template<std::integral RegisterTy>
            requires (sizeof(RegisterTy) == sizeof(MaskType))
        static constexpr void Set(RegisterTy& _register) requires (MaskSize == 1)
        {
            _register |= bitMask;
        }
        
        template<std::integral RegisterTy>
            requires (sizeof(RegisterTy) == sizeof(MaskType))
        static constexpr void Set(RegisterTy& _register, IOType value)
        {
            Reset(_register);
            if constexpr(std::is_enum_v<IOType>)
                _register |= static_cast<std::underlying_type_t<IOType>>(value) << MaskShift;
            else
                _register |= value << MaskShift;
        }

        template<std::integral RegisterTy>
            requires (sizeof(RegisterTy) == sizeof(MaskType))
        static constexpr void Reset(RegisterTy& _register)
        {
            _register &= negatedBitMask;
        }
    
        template<std::integral RegisterTy>
            requires (sizeof(RegisterTy) == sizeof(MaskType))
        static constexpr void Flip(RegisterTy& _register) requires (MaskSize == 1)
        {
            _register ^= bitMask;
        }
        
        template<std::integral RegisterTy>
            requires (sizeof(RegisterTy) == sizeof(MaskType))
        static constexpr IOType Get(const RegisterTy& _register)
        {
            return { (_register & bitMask) >> bitShift };
        }
    };

    //Min and Max are inclusive
    template<class Ty, Ty Min, Ty Max>
    struct Range
    {
        Ty value {};

        Range(Ty _value) :
            value{_value}
        {
            BN_ASSERT(_value >= Min && _value <= Max);
        }

        operator Ty() const { return value; }
    };

    
    template<class IOType, auto MaskSize, auto MaskShift>
    using u16RegisterBit = RegisterBit<u16, IOType, MaskSize, MaskShift>;

    template<class Ty, bool Volatile>
    struct RegisterData
    {
        Ty data;
    };

    template<class Ty>
    struct RegisterData<Ty, true>
    {
        volatile Ty data;
    };

    enum class OBJCharacterVRAMMappingMode : u32
    {
        TwoDimensional = 0,
        OneDimensional = 1//static_cast<u16>(DisplayControlFlags::OBJ_Character_VRAM_Mapping_Mode)
    };

    enum WordBool : u32
    {
        False,
        True
    };

    template<bool Volatile>
    struct DisplayControlRegisterTemplate : RegisterData<u16, Volatile>
    {        
        using RegisterData<u16, Volatile>::data;
        using BackgroundMode = u16RegisterBit<Range<u32, 0, 3>, 3, 0>;
        using Display_Frame_Select = u16RegisterBit<Range<u32, 0, 1>, 1, 4>;
        using H_Blank_Interval_Free_Flag = u16RegisterBit<WordBool, 1, 5>;
        using OBJ_Character_VRAM_Mapping_Mode = u16RegisterBit<OBJCharacterVRAMMappingMode, 1, 6>;
        using Forced_Blank_Flag = u16RegisterBit<WordBool, 1, 7>;
        using Background0_Visibility_Flag = u16RegisterBit<WordBool, 1, 8>;
        using Background1_Visibility_Flag = u16RegisterBit<WordBool, 1, 9>;
        using Background2_Visibility_Flag = u16RegisterBit<WordBool, 1, 10>;
        using Background3_Visibility_Flag = u16RegisterBit<WordBool, 1, 11>;
        using Object_Visibility_Flag = u16RegisterBit<WordBool, 1, 12>;
        using Display_Window0 = u16RegisterBit<WordBool, 1, 13>;
        using Display_Window1 = u16RegisterBit<WordBool, 1, 14>;
        using Display_OBJ_Window = u16RegisterBit<WordBool, 1, 15>;

        DisplayControlRegisterTemplate() = default;
        DisplayControlRegisterTemplate(const DisplayControlRegisterTemplate<!Volatile>& other) :
            RegisterData<u16, Volatile>{ other.data }
        {
            
        }
                
        DisplayControlRegisterTemplate& operator=(const DisplayControlRegisterTemplate& other) = default;
        DisplayControlRegisterTemplate& operator=(const DisplayControlRegisterTemplate<!Volatile>& other)
        {
            data = other.data;
            return *this;
        }
        
        void SetBackgroundMode(BackgroundMode::type value)
        {
            BackgroundMode::Set(data, value);
        }

        BackgroundMode::type GetBackgroundMode() const
        {
            return BackgroundMode::Get(data);
        }

        void SetDisplayFrame(Display_Frame_Select::type value)
        {
            Display_Frame_Select::Set(data, value);
        }

        void FlipDisplayFrame()
        {
            Display_Frame_Select::Flip(data);
        }

        Display_Frame_Select::type GetDisplayFrame() const
        {
            return Display_Frame_Select::Get(data);
        }
        
        void SetHBlankIntervalFreeFlag()
        {
            H_Blank_Interval_Free_Flag::Set(data);
        }

        H_Blank_Interval_Free_Flag::type GetHBlankIntervalFreeFlag() const
        {
            return H_Blank_Interval_Free_Flag::Get(data);
        }
        
        void SetOBJCharacterVRAMMappingMode(OBJ_Character_VRAM_Mapping_Mode::type mode)
        {
            OBJ_Character_VRAM_Mapping_Mode::Set(data, mode);
        }

        OBJ_Character_VRAM_Mapping_Mode::type GetOBJCharacterVRAMMappingMode() const
        {
            return OBJ_Character_VRAM_Mapping_Mode::Get(data);
        }
        
        void SetForcedBlankFlag()
        {
            OBJ_Character_VRAM_Mapping_Mode::Set(data);
        }
        
        void ResetForcedBlankFlag()
        {
            OBJ_Character_VRAM_Mapping_Mode::Reset(data);
        }

        Forced_Blank_Flag::type GetForcedBlankFlag() const
        {
            return Forced_Blank_Flag::Get(data);
        }

        void ShowBackground(Range<u32, 0, 3> layer)
        {
            data |= Background0_Visibility_Flag::bitMask << layer;
        }

        void HideBackground(Range<u32, 0, 3> layer)
        {
            data &= Background0_Visibility_Flag::bitMask << layer;
        }
        // using Background0_Visibility_Flag = u16RegisterBit<u32, 1, 8>;
        // using Background1_Visibility_Flag = u16RegisterBit<u32, 1, 9>;
        // using Background2_Visibility_Flag = u16RegisterBit<u32, 1, 10>;
        // using Background3_Visibility_Flag = u16RegisterBit<u32, 1, 11>;

        void ShowObjects()
        {
            Object_Visibility_Flag::Set(data);
        }

        void HideObjects()
        {
            Object_Visibility_Flag::Reset(data);
        }
        
        void ToggleObjectVisibility()
        {
            Object_Visibility_Flag::Flip(data);
        }

        Object_Visibility_Flag::type AreObjectsVisible() const
        {
            return Object_Visibility_Flag::Get(data);
        }
        
        void ShowWindow0()
        {
            Display_Window0::Set(data);
        }

        void HideWindow0()
        {
            Display_Window0::Reset(data);
        }
        
        void ToggleWindow0Visibility()
        {
            Display_Window0::Flip(data);
        }

        Display_Window0::type IsWindow0Visible() const
        {
            return Display_Window0::Get(data);
        }
        
        void ShowWindow1()
        {
            Display_Window1::Set(data);
        }

        void HideWindow1()
        {
            Display_Window1::Reset(data);
        }
        
        void ToggleWindow1Visibility()
        {
            Display_Window1::Flip(data);
        }

        Display_Window1::type IsWindow1Visible() const
        {
            return Display_Window1::Get(data);
        }
        
        void ShowObjectWindow()
        {
            Display_OBJ_Window::Set(data);
        }

        void HideObjectWindow()
        {
            Display_OBJ_Window::Reset(data);
        }
        
        void ToggleObjectWindowVisibility()
        {
            Display_OBJ_Window::Flip(data);
        }

        Display_OBJ_Window::type IsObjectWindowVisible() const
        {
            return Display_OBJ_Window::Get(data);
        }
    };

    using DisplayControlRegister = DisplayControlRegisterTemplate<false>;
    using VolatileDisplayControlRegister = DisplayControlRegisterTemplate<false>;

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
        
    DECLARE_BIT_FLAG_OPS(DisplayStatusRegister);
    DECLARE_VOLATILE_BIT_FLAG_OPS(DisplayStatusRegister);

    DECLARE_BIT_FLAG_OPS(DisplayStatusFlags);
    DECLARE_VOLATILE_BIT_FLAG_OPS(DisplayStatusFlags);

    DECLARE_BIT_FLAG_OPS2(DisplayStatusRegister, DisplayStatusFlags);
    DECLARE_VOLATILE_BIT_FLAG_OPS2(DisplayStatusRegister, DisplayStatusFlags);
    
    
    struct Display
    {
        static VolatileDisplayControlRegister& GetControlRegister()
        {
            return Memory<VolatileDisplayControlRegister>(display_control_register);
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
            currentStatus.SetBackgroundMode(Ty::modeValue);
            GetControlRegister() = currentStatus;
            return Ty::_dummy;
        }

        template<class Ty>
        static Ty& GetBackgroundMode()
        {
            ////TODO: Maybe get an assert that doesn't rely on BN?
            BN_ASSERT(GetControlRegister().GetBackgroundMode() == Ty::modeValue);
            return Ty::_dummy;
        }
    };
    
    static constexpr uintptr background_control_register_base_address = 0x0400'0008;
    static constexpr uintptr background_scroll_offset_register_base_address = 0x0400'0010;
    static constexpr uintptr background_rotation_scale_register_base_address = 0x0400'0020;

    enum class BackgroundControlRegisterBitshift : u16
    {
        Priority = 0,
        Character_Base_Block = 2,
        Mosaic = 6,
        Pallette = 7,
        Screen_Base_Block = 8,
        Display_Overflow = 13,
        Size = 14,
    };

    enum class BackgroundControlRegister : u16
    {
        Priority = (1 << 2) - 1,
        Character_Base_Block = ((1 << 2) - 1) << 2,
        Mosaic = 1 << 6,
        Pallette = 1 << 7,
        Screen_Base_Block = ((1 << 4) - 1) << 8,
        Display_Overflow = 1 << 13,
        Size = ((1 << 2) - 1) << 14,
    };

    DECLARE_BIT_FLAG_OPS(BackgroundControlRegister);
    DECLARE_BIT_FLAG_OPS2(BackgroundControlRegister, i32);
    DECLARE_VOLATILE_BIT_FLAG_OPS(BackgroundControlRegister);

    DECLARE_BIT_FLAG_OPS2(i32, BackgroundControlRegister);
    DECLARE_BIT_SHIFT_OPS2(i32, BackgroundControlRegisterBitshift);
    DECLARE_BIT_SHIFT_OPS2(BackgroundControlRegister, BackgroundControlRegisterBitshift);

    static_assert(sizeof(BackgroundControlRegister) == 2, "The docs says background control register is 2 bytes");
    
    struct BackgroundScrollRegister
    {
        volatile Point<i32> offset;
    };

    struct AffineTransform
    {
        Fixed<i16, 8> x; 
        Fixed<i16, 8> dx; 
        Fixed<i16, 8> y; 
        Fixed<i16, 8> dy; 
    };
    
    struct BackgroundTransformRegister
    {
        volatile AffineTransform transform; 
        volatile Point<Fixed<i32, 8>> pivot; 
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

        //TODO: If this does unoptimal code gen, make this be some view class instead that is a wider type underneath and do
        //bit operations manually
        using PixelFormat = VolatilePallette8;
        
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

    class CommonBackgroundView
    {
    private:
        i32 layer;

    public:        
        constexpr CommonBackgroundView(i32 inLayer) :
            layer{inLayer}
        {
            
        }

    public:
        volatile BackgroundControlRegister& GetControlRegister()
        {
            return VolatileMemory<BackgroundControlRegister>(background_control_register_base_address + layer * sizeof(BackgroundControlRegister));
        }

        void SetPriority(i32 priority)
        {
            BackgroundControlRegister oldValues = GetControlRegister();
            oldValues &= BackgroundControlRegister::Priority;
            oldValues |= (priority << BackgroundControlRegisterBitshift::Priority) & BackgroundControlRegister::Priority;
            GetControlRegister() = oldValues;
        }

        i32 GetPriority()
        {
            return static_cast<i32>(((GetControlRegister() & BackgroundControlRegister::Priority) >> BackgroundControlRegisterBitshift::Priority));
        }
        
        void EnableMosaic()
        {
            GetControlRegister() |= BackgroundControlRegister::Mosaic;
        }
        void DisableMosaic()
        {
            GetControlRegister() &= BackgroundControlRegister::Mosaic;
        }
        bool IsMosaicEnabled()
        {
            return (GetControlRegister() & BackgroundControlRegister::Mosaic) == BackgroundControlRegister::Mosaic;
        }

        void Show()
        {
            Display::GetControlRegister().ShowBackground(layer);
        }

        void Hide()
        {
            Display::GetControlRegister().ShowBackground(layer);
        }

        i32 GetLayer() const { return layer; }
    };

    struct CommonTileBackgroundView : public CommonBackgroundView
    {
        static constexpr Rectangle tileSize{ 8, 8 };
        static constexpr Rectangle mapMinSize{ 32, 32 };
        static constexpr Rectangle mapMaxSize{ 64, 64 };

        void SetCharacterBaseBlock(Range<u32, 0, 3> value)
        {
            BackgroundControlRegister oldValues = GetControlRegister();
            oldValues &= BackgroundControlRegister::Character_Base_Block;
            oldValues |= (value << BackgroundControlRegisterBitshift::Character_Base_Block) & BackgroundControlRegister::Character_Base_Block;
            GetControlRegister() = oldValues;
        }
        
        u32 GetCharacterBaseBlock()
        {
            return static_cast<u32>((GetControlRegister() & BackgroundControlRegister::Character_Base_Block) >> BackgroundControlRegisterBitshift::Character_Base_Block);
        }
        
        void SetPalletteMode(u32 mode)
        {
            BackgroundControlRegister oldValues = GetControlRegister();
            oldValues &= BackgroundControlRegister::Pallette;
            oldValues |= (mode << BackgroundControlRegisterBitshift::Pallette) & BackgroundControlRegister::Pallette;
            GetControlRegister() = oldValues;
        }
        
        u32 GetPalletteMode()
        {
            return static_cast<u32>((GetControlRegister() & BackgroundControlRegister::Pallette) >> BackgroundControlRegisterBitshift::Pallette);
        }
        
        void SetScreenBaseBlock(Range<u32,0, 31> value)
        {
            BackgroundControlRegister oldValues = GetControlRegister();
            oldValues &= BackgroundControlRegister::Screen_Base_Block;
            oldValues |= (value << BackgroundControlRegisterBitshift::Screen_Base_Block) & BackgroundControlRegister::Screen_Base_Block;
            GetControlRegister() = oldValues;
        }
        
        u32 GetScreenBaseBlock()
        {
            return static_cast<u32>((GetControlRegister() & BackgroundControlRegister::Screen_Base_Block) >> BackgroundControlRegisterBitshift::Screen_Base_Block);
        }
        
        void SetSize(Range<u32,0, 3> value)
        {
            BackgroundControlRegister oldValues = GetControlRegister();
            oldValues &= BackgroundControlRegister::Size;
            oldValues |= (value << BackgroundControlRegisterBitshift::Size) & BackgroundControlRegister::Size;
            GetControlRegister() = oldValues;
        }
        
        u32 GetSize()
        {
            return static_cast<u32>((GetControlRegister() & BackgroundControlRegister::Size) >> BackgroundControlRegisterBitshift::Size);
        }
    };

    template<class Derived>
    struct TileBackgroundMixin
    {

    };

    template<class Derived>
    struct AffineBackgroundMixin
    {

    };

    class TileBackgroundView : public CommonTileBackgroundView, 
        public TileBackgroundMixin<TileBackgroundView>
    {
    };

    class AffineTileBackgroundView : public CommonTileBackgroundView, 
        public AffineBackgroundMixin<TileBackgroundView>
    {
        void SetDisplayOverflow(Range<u32,0, 1> value)
        {
            BackgroundControlRegister oldValues = GetControlRegister();
            oldValues &= BackgroundControlRegister::Display_Overflow;
            oldValues |= (value << BackgroundControlRegisterBitshift::Display_Overflow) & BackgroundControlRegister::Display_Overflow;
            GetControlRegister() = oldValues;
        }
        
        u32 GetDisplayOverflow()
        {
            return static_cast<u32>((GetControlRegister() & BackgroundControlRegister::Display_Overflow) >> BackgroundControlRegisterBitshift::Display_Overflow);
        }
    };

    template<class Format>
    class BitmapBackgroundView : public CommonBackgroundView, 
        public AffineBackgroundMixin<BitmapBackgroundView<Format>>
    {
    public:

        void PlotPixel(Point<i32> position, Format::ColorFormat color)
        {
            Format::PixelAt(position) = color;
        }
    };

    struct BackgroundMode0
    {
        // static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode0;
        static constexpr u32 modeValue = 0;
        // static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background0_Visibility_Flag 
        //     | DisplayControlRegister::Background1_Visibility_Flag 
        //     | DisplayControlRegister::Background2_Visibility_Flag 
        //     | DisplayControlRegister::Background3_Visibility_Flag; 
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
        // static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode1;
        static constexpr u32 modeValue = 1;
        // static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background0_Visibility_Flag 
        //     | DisplayControlRegister::Background1_Visibility_Flag 
        //     | DisplayControlRegister::Background2_Visibility_Flag; 

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
        // static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode2;
        static constexpr u32 modeValue = 2;
        // static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag 
        //     | DisplayControlRegister::Background3_Visibility_Flag; 
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
        // static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode3;
        static constexpr u32 modeValue = 3;
        // static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag;
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
        // static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode4;
        static constexpr u32 modeValue = 4;
        // static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag;
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
        // static constexpr DisplayControlRegister modeValue = DisplayControlRegister::Background_Mode5;
        static constexpr u32 modeValue = 5;
        // static constexpr DisplayControlRegister backgroundLayerSupport = DisplayControlRegister::Background2_Visibility_Flag;
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