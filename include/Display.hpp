#pragma once
#include "Types.hpp"
#include "MemoryRegion.hpp"
#include "Math.hpp"
#include "EnumFlags.hpp"
#include "VRAMFormats.hpp"
#include <limits>
#include <bit>
#include "bn_assert.h"
#include "PackedRegister.hpp"

namespace cgba
{   

    enum class OBJCharacterVRAMMappingMode : u32
    {
        TwoDimensional = 0,
        OneDimensional = 1//static_cast<u16>(DisplayControlFlags::OBJ_Character_VRAM_Mapping_Mode)
    };

    template<bool Volatile>
    struct DisplayControlRegisterTemplate
    {        
        using BackgroundMode = u16PackedRegisterData<Range<u32, 0, 3>, 3, 0>;
        using Display_Frame_Select = u16PackedRegisterData<Range<u32, 0, 1>, 1, 4>;
        using H_Blank_Interval_Free_Flag = u16PackedRegisterData<WordBool, 1, 5>;
        using OBJ_Character_VRAM_Mapping_Mode = u16PackedRegisterData<OBJCharacterVRAMMappingMode, 1, 6>;
        using Forced_Blank_Flag = u16PackedRegisterData<WordBool, 1, 7>;
        using Background0_Visibility_Flag = u16PackedRegisterData<WordBool, 1, 8>;
        using Background1_Visibility_Flag = u16PackedRegisterData<WordBool, 1, 9>;
        using Background2_Visibility_Flag = u16PackedRegisterData<WordBool, 1, 10>;
        using Background3_Visibility_Flag = u16PackedRegisterData<WordBool, 1, 11>;
        using Object_Visibility_Flag = u16PackedRegisterData<WordBool, 1, 12>;
        using Display_Window0 = u16PackedRegisterData<WordBool, 1, 13>;
        using Display_Window1 = u16PackedRegisterData<WordBool, 1, 14>;
        using Display_OBJ_Window = u16PackedRegisterData<WordBool, 1, 15>;

        ConditionallyVolatile_T<u16, Volatile> data;

        DisplayControlRegisterTemplate() = default;
        DisplayControlRegisterTemplate(const DisplayControlRegisterTemplate<!Volatile>& other) :
            data{ other.data }
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
        // using Background0_Visibility_Flag = u16PackedRegisterData<u32, 1, 8>;
        // using Background1_Visibility_Flag = u16PackedRegisterData<u32, 1, 9>;
        // using Background2_Visibility_Flag = u16PackedRegisterData<u32, 1, 10>;
        // using Background3_Visibility_Flag = u16PackedRegisterData<u32, 1, 11>;

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
    using VolatileDisplayControlRegister = DisplayControlRegisterTemplate<true>;

    template<bool Volatile>
    struct DisplayStatusRegisterTemplate
    { 
        using Vertical_Blank_Flag = u16PackedRegisterData<WordBool, 1, 0>;
        using Horizontal_Blank_Flag = u16PackedRegisterData<WordBool, 1, 1>;
        using Vertical_Counter_Flag = u16PackedRegisterData<WordBool, 1, 2>;
        using Enable_Vertical_Blank_IRQ = u16PackedRegisterData<WordBool, 1, 3>;
        using Enable_Horizontal_Blank_IRQ = u16PackedRegisterData<WordBool, 1, 4>;
        using Enable_Vertical_Counter_IRQ = u16PackedRegisterData<WordBool, 1, 5>;
        using Vertical_Count_Setting = u16PackedRegisterData<Range<u32, 0, 227>, 8, 8>;
                
        ConditionallyVolatile_T<u16, Volatile> data;

        Vertical_Blank_Flag::type IsVBlankFlagSet() const
        {
            return Vertical_Blank_Flag::Get(data);
        }

        Horizontal_Blank_Flag::type IsHBlankFlagSet() const
        {
            return Horizontal_Blank_Flag::Get(data);
        }

        Vertical_Counter_Flag::type IsVCounterFlagSet() const
        {
            return Vertical_Counter_Flag::Get(data);
        }

        void EnableVBlankIRQ()
        {
            Enable_Vertical_Blank_IRQ::Set(data);
        }

        void DisableVBlankIRQ()
        {
            Enable_Vertical_Blank_IRQ::Reset(data);
        }

        Enable_Vertical_Blank_IRQ::type IsVBlankIRQSet() const
        {
            return Enable_Vertical_Blank_IRQ::Get(data);
        }
        
        void EnableHBlankIRQ()
        {
            Enable_Horizontal_Blank_IRQ::Set(data);
        }

        void DisableHBlankIRQ()
        {
            Enable_Horizontal_Blank_IRQ::Reset(data);
        }

        Enable_Horizontal_Blank_IRQ::type IsHBlankIRQSet() const
        {
            return Enable_Horizontal_Blank_IRQ::Get(data);
        }
        
        void EnableVCounterIRQ()
        {
            Enable_Vertical_Counter_IRQ::Set(data);
        }

        void DisableVCounterIRQ()
        {
            Enable_Vertical_Counter_IRQ::Reset(data);
        }

        Enable_Vertical_Counter_IRQ::type IsVCounterIRQSet() const
        {
            return Enable_Vertical_Counter_IRQ::Get(data);
        }

        void SetVCounterSetting(Vertical_Count_Setting::type value)
        {
            Vertical_Count_Setting::Set(data, value);
        }

        Vertical_Count_Setting::type GetVCounterSetting() const
        {
            return Vertical_Count_Setting::Get(data);
        }
    };

    using DisplayStatusRegister = DisplayStatusRegisterTemplate<false>;
    using VolatileDisplayStatusRegister = DisplayStatusRegisterTemplate<true>;
    
    struct Display
    {
        static constexpr Rectangle hardwareScreenSizePixels{ 240, 160 };

        static VolatileDisplayControlRegister& GetControlRegister()
        {
            return Memory<VolatileDisplayControlRegister>(display_control_register);
        }

        static volatile VolatileDisplayStatusRegister& GetStatusRegister()
        {
            return Memory<VolatileDisplayStatusRegister>(display_status_register);
        }

        static u16 GetVerticalCounter()
        {
            return Memory<volatile u16>(vertical_counter_register) & std::numeric_limits<u8>::max();
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
    
    enum class DisplayAreaOverflowMode : u32
    {
        Transparent = 0,
        Wrap_Around = 1
    };
    
    enum class TextScreenSizeMode : u32
    {
        //256x256 pixel, supporting up to 32x32 tiles
        W256_H256 = 0,
        
        //512x256 pixels, supporting up to 64x32 tiles
        W512_H256 = 1,

        //256x512 pixels, supporting up to 32x64 tiles
        W256_H512 = 2,

        //512x512 pixels, supporting up to 64x64 tiles
        W512_H512 = 3
    };
    
    enum class AffineScreenSizeMode : u32
    {
        //128x128 pixel, supporting up to 16x16 tiles
        W128_H128 = 0,
        
        //256x256 pixels, supporting up to 32x32 tiles
        W256_H256 = 1,

        //512x512 pixels, supporting up to 64x64 tiles
        W512_H512 = 2,

        //1024x1024 pixels, supporting up to 128x128 tiles
        W1024_H1024 = 3
    };

    template<auto Mode>
    struct ScreenSizeConstants;



    struct AffineBackgroundTileDescription
    {
        u8 tileNumber;
                
        constexpr void SetTileNumber(Range<u32, 0, std::numeric_limits<u8>::max()> value)
        {
            tileNumber = static_cast<u8>(value);
        }

        constexpr Range<u32, 0, std::numeric_limits<u8>::max()> GetTileNumber() const
        {
            return tileNumber;
        }
    };

    template<>
    struct ScreenSizeConstants<TextScreenSizeMode::W256_H256>
    {
        static constexpr Rectangle screenSizePixels{ 256, 256 };
        static constexpr Rectangle screenSizeTiles{ 32, 32 };
        using TileDescription = TextBackgroundTileDescription;
    };

    template<>
    struct ScreenSizeConstants<TextScreenSizeMode::W512_H256>
    {
        static constexpr Rectangle screenSizePixels{ 512, 256 };
        static constexpr Rectangle screenSizeTiles{ 64, 32 };
        using TileDescription = TextBackgroundTileDescription;
    };

    template<>
    struct ScreenSizeConstants<TextScreenSizeMode::W256_H512>
    {
        static constexpr Rectangle screenSizePixels{ 256, 512 };
        static constexpr Rectangle screenSizeTiles{ 32, 64 };
        using TileDescription = TextBackgroundTileDescription;
    };

    template<>
    struct ScreenSizeConstants<TextScreenSizeMode::W512_H512>
    {
        static constexpr Rectangle screenSizePixels{ 512, 512 };
        static constexpr Rectangle screenSizeTiles{ 64, 64 };
        using TileDescription = TextBackgroundTileDescription;
    };

    template<>
    struct ScreenSizeConstants<AffineScreenSizeMode::W128_H128>
    {
        static constexpr Rectangle screenSizePixels{ 256, 256 };
        static constexpr Rectangle screenSizeTiles{ 16, 16 };
        using TileDescription = AffineBackgroundTileDescription;
    };

    template<>
    struct ScreenSizeConstants<AffineScreenSizeMode::W256_H256>
    {
        static constexpr Rectangle screenSizePixels{ 512, 256 };
        static constexpr Rectangle screenSizeTiles{ 32, 32 };
        using TileDescription = AffineBackgroundTileDescription;
    };

    template<>
    struct ScreenSizeConstants<AffineScreenSizeMode::W512_H512>
    {
        static constexpr Rectangle screenSizePixels{ 256, 512 };
        static constexpr Rectangle screenSizeTiles{ 64, 64 };
        using TileDescription = AffineBackgroundTileDescription;
    };

    template<>
    struct ScreenSizeConstants<AffineScreenSizeMode::W1024_H1024>
    {
        static constexpr Rectangle screenSizePixels{ 1024, 1024 };
        static constexpr Rectangle screenSizeTiles{ 128, 128 };
        using TileDescription = AffineBackgroundTileDescription;
    };

    template<PaletteMode Mode>
    struct PaletteModeToType;

    template<>
    struct PaletteModeToType<PaletteMode::Color16_Palette16>
    {
        using View = PaletteView16;
        using CharacterTile = CharacterTile16;
    };
        
    template<>
    struct PaletteModeToType<PaletteMode::Color256_Palette1>
    {
        using View = PaletteView256;
        using CharacterTile = CharacterTile256;
    };

    template<TextScreenSizeMode Size, PaletteMode Palette>
    struct StaticTextBackgroundConstants
    {
        using SizeConstants = ScreenSizeConstants<Size>;
        
        static constexpr Rectangle screenSizePixels = SizeConstants::screenSizePixels;
        static constexpr Rectangle screenSizeTiles = SizeConstants::screenSizeTiles;
        using TileDescription = SizeConstants::TileDescription;
        using PaletteView = PaletteViewTemplate<Palette>;
        using CharacterTile = CharacterTileTemplate<Palette, false>;
    };

    template<bool Volatile>
    struct BackgroundControlRegisterTemplate 
    { 
        using Priority = u16PackedRegisterData<Range<u32, 0, 3>, 2, 0>;
        using Character_Base_Block = u16PackedRegisterData<Range<u32, 0, 3>, 2, 2>;
        using Enable_Mosaic = u16PackedRegisterData<WordBool, 1, 6>;
        using Palette_Mode = u16PackedRegisterData<PaletteMode, 1, 7>;
        using Screen_Base_Block = u16PackedRegisterData<Range<u32, 0, 31>, 5, 8>;
        using Display_Area_Overflow = u16PackedRegisterData<DisplayAreaOverflowMode, 1, 13>;
        using Screen_Size_Text = u16PackedRegisterData<TextScreenSizeMode, 2, 14>;
        using Screen_Size_Affine = u16PackedRegisterData<AffineScreenSizeMode, 2, 14>;

        ConditionallyVolatile_T<u16, Volatile> data;

        void SetPriority(Priority::type value)
        {
            Priority::Set(data, value);
        }

        Priority::type GetPriority() const
        {
            return Priority::Get(data);
        }
        
        void SetCharacterBaseBlock(Character_Base_Block::type value)
        {
            Character_Base_Block::Set(data, value);
        }

        Character_Base_Block::type GetCharacterBaseBlock() const
        {
            return Character_Base_Block::Get(data);
        }
        
        void EnableMosaic()
        {
            Enable_Mosaic::Set(data);
        }
        
        void DisableMosaic()
        {
            Enable_Mosaic::Reset(data);
        }

        Enable_Mosaic::type IsMosaicEnabled() const
        {
            return Enable_Mosaic::Get(data);
        }
        
        void SetPaletteMode(Palette_Mode::type value)
        {
            Palette_Mode::Set(data, value);
        }

        Palette_Mode::type GetPaletteMode() const
        {
            return Palette_Mode::Get(data);
        }
        
        void SetScreenBaseBlock(Screen_Base_Block::type value)
        {
            Screen_Base_Block::Set(data, value);
        }

        Screen_Base_Block::type GetScreenBaseBlock() const
        {
            return Screen_Base_Block::Get(data);
        }
        
        void SetDisplayOverflowMode(Display_Area_Overflow::type value)
        {
            Display_Area_Overflow::Set(data, value);
        }

        Display_Area_Overflow::type GetDisplayOverflowMode() const
        {
            return Display_Area_Overflow::Get(data);
        }
        
        void SetScreenSizeText(Screen_Size_Text::type value)
        {
            Screen_Size_Text::Set(data, value);
        }

        Screen_Size_Text::type GetScreenSizeText() const
        {
            return Screen_Size_Text::Get(data);
        }
        
        void SetScreenSizeAffine(Screen_Size_Affine::type value)
        {
            Screen_Size_Affine::Set(data, value);
        }

        Screen_Size_Affine::type GetScreenSizeAffine() const
        {
            return Screen_Size_Affine::Get(data);
        }
    };
    
    using BackgroundControlRegister = BackgroundControlRegisterTemplate<false>;
    using VolatileBackgroundControlRegister = BackgroundControlRegisterTemplate<true>;


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
        using ColorFormat = Palette256Index;

        //TODO: If this does unoptimal code gen, make this be some view class instead that is a wider type underneath and do
        //bit operations manually
        using PixelFormat = VolatilePalette256Index;
        
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
        VolatileBackgroundControlRegister& GetControlRegister()
        {
            return Memory<VolatileBackgroundControlRegister>(background_control_register_base_address + layer * sizeof(VolatileBackgroundControlRegister));
        }
        
        const VolatileBackgroundControlRegister& GetControlRegister() const
        {
            return Memory<VolatileBackgroundControlRegister>(background_control_register_base_address + layer * sizeof(VolatileBackgroundControlRegister));
        }

        void SetPriority(Range<u32, 0, 3> priority)
        {
            GetControlRegister().SetPriority(priority);
        }

        i32 GetPriority() const
        {
            return GetControlRegister().GetPriority();
        }
        
        void EnableMosaic()
        {
            GetControlRegister().EnableMosaic();
        }

        void DisableMosaic()
        {
            GetControlRegister().DisableMosaic();
        }

        bool IsMosaicEnabled() const
        {
            return GetControlRegister().IsMosaicEnabled();
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

        void SetDisplayOverflow(DisplayAreaOverflowMode mode)
        {
            GetControlRegister().SetDisplayOverflowMode(mode);
        }
        
        DisplayAreaOverflowMode GetDisplayOverflow() const
        {
            return GetControlRegister().GetDisplayOverflowMode();
        }
        
        void SetCharacterBaseBlock(Range<u32, 0, 3> value)
        {
            GetControlRegister().SetCharacterBaseBlock(value);
        }
        
        u32 GetCharacterBaseBlock() const
        {
            return GetControlRegister().GetCharacterBaseBlock();
        }
        
        void SetPaletteMode(PaletteMode mode)
        {
            GetControlRegister().SetPaletteMode(mode);
        }
        
        PaletteMode GetPaletteMode() const
        {
            return GetControlRegister().GetPaletteMode();
        }
        
        void SetScreenBaseBlock(Range<u32,0, 31> value)
        {
            GetControlRegister().SetScreenBaseBlock(value);
        }
        
        u32 GetScreenBaseBlock()
        {
            return GetControlRegister().GetScreenBaseBlock();
        }
        
        CharacterBlockView256 GetCharacterBlockData()
        {
            return CharacterBlockView256{ GetCharacterBaseBlock() };
        }

        PaletteView16 GetPalette16(Range<u32, 0, 15> palette)
        {
            return PaletteView16::MakeBackgroundView(palette);
        }        
        
        PaletteView256 GetPalette256()
        {
            return PaletteView256::MakeBackgroundView();
        }
    };

    struct CommonTileBackgroundView : public CommonBackgroundView
    {
    public:
        TextScreenBlockView GetScreenBlockData()
        {
            return TextScreenBlockView{ GetScreenBaseBlock() };
        }
        
        void SetScreenSize(TextScreenSizeMode mode)
        {
            GetControlRegister().SetScreenSizeText(mode);
        }

        TextScreenSizeMode GetScreenSize() const
        {
            return GetControlRegister().GetScreenSizeText();
        }

    private:
        using CommonBackgroundView::SetDisplayOverflow;
        using CommonBackgroundView::GetDisplayOverflow;
    };

    struct CommonTileAffineBackgroundView : public CommonBackgroundView
    {
    public:
        void SetScreenSize(AffineScreenSizeMode mode)
        {
            GetControlRegister().SetScreenSizeAffine(mode);
        }

        AffineScreenSizeMode GetScreenSize() const
        {
            return GetControlRegister().GetScreenSizeAffine();
        }
        //TODO: Implement GetScreenBlockData
        
    };

    class TileBackgroundView : public CommonTileBackgroundView
    {
    };

    class AffineTileBackgroundView : public CommonTileBackgroundView
    {
    };

    template<class Format>
    class BitmapBackgroundView : public CommonBackgroundView
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