#pragma once
#include <array>
#include "Math.hpp"
#include "Types.hpp"
#include "MemoryRegion.hpp"
#include "PackedRegister.hpp"

namespace cgba
{
    template<bool isVolatile>
    class RGB15Template
    {
        static constexpr u16 subColorMask = 0b0000'0000'0001'1111;
        static constexpr u16 u16RedBitShift = 0;
        static constexpr u16 u16GreenBitShift = 5;
        static constexpr u16 u16BlueBitShift = 10;

        static constexpr u16 u16RedMask = subColorMask << u16RedBitShift;
        static constexpr u16 u16GreenMask = subColorMask << u16GreenBitShift;
        static constexpr u16 u16BlueMask = subColorMask << u16BlueBitShift;

    private:
        ConditionallyVolatile_T<u16, isVolatile> data;

    public:
        constexpr RGB15Template() = default;
        
        constexpr explicit RGB15Template(u16 color) : data{ color }
        {

        } 

        constexpr RGB15Template(u32 r, u32 g, u32 b) :
            data{ static_cast<u16>(static_cast<u16>(r) | (static_cast<u16>(g) << u16GreenBitShift) | (static_cast<u16>(b) << u16BlueBitShift)) }
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
        
    enum class PaletteMode : u32
    {
        //Have 16 Palettes with 16 colors each
        Color16_Palette16 = 0,
        
        //Have 1 Palette with 256 colors
        Color256_Palette1 = 1
    };

    template<PaletteMode Mode, bool IsVolatile>
    struct PaletteIndexTemplate
    {
        ConditionallyVolatile_T<u8, IsVolatile> index;
        
        operator PaletteIndexTemplate<Mode, !IsVolatile>() const { return {index}; }
    };

    using Palette16Index = PaletteIndexTemplate<PaletteMode::Color16_Palette16, false>;
    using Palette256Index = PaletteIndexTemplate<PaletteMode::Color256_Palette1, false>;

    using VolatilePalette16Index = PaletteIndexTemplate<PaletteMode::Color16_Palette16, true>;
    using VolatilePalette256Index = PaletteIndexTemplate<PaletteMode::Color256_Palette1, true>;

    
    template<PaletteMode Mode>
    class PaletteViewTemplate
    {    
    private:
        VolatileRGB15* baseAddress;

    public:
        static PaletteViewTemplate MakeBackgroundView(Range<u32, 0, 15> paletteNumber) requires (Mode == PaletteMode::Color16_Palette16)
        {
            return { background_palettes, paletteNumber };
        }
        static PaletteViewTemplate MakeObjectView(Range<u32, 0, 15> paletteNumber) requires (Mode == PaletteMode::Color16_Palette16)
        {
            return { object_palettes, paletteNumber };
        }
        
        static PaletteViewTemplate MakeBackgroundView() requires (Mode == PaletteMode::Color256_Palette1)
        {
            return { background_palettes };
        }
        static PaletteViewTemplate MakeObjectView() requires (Mode == PaletteMode::Color256_Palette1)
        {
            return { object_palettes };
        }
        VolatileRGB15& operator[](u32 index)
        {
            return baseAddress[index];
        }

    private:
        PaletteViewTemplate(uintptr paletteAddress, Range<u32, 0, 15> paletteNumber) requires (Mode == PaletteMode::Color16_Palette16) :
            baseAddress{ &Memory<VolatileRGB15>(paletteAddress + palette_block_increments * paletteNumber) }
        {

        }

        PaletteViewTemplate(uintptr paletteAddress) requires (Mode == PaletteMode::Color256_Palette1) :
            baseAddress{ &Memory<VolatileRGB15>(paletteAddress) }
        {

        }
    };

    using PaletteView16 = PaletteViewTemplate<PaletteMode::Color16_Palette16>;
    using PaletteView256 = PaletteViewTemplate<PaletteMode::Color256_Palette1>;

    template<PaletteMode Mode, bool IsVolatile>
    struct CharacterTileTemplate;

    template<bool IsVolatile>
    struct CharacterTileTemplate<PaletteMode::Color16_Palette16, IsVolatile>
    {
        using PaletteIndex = PaletteIndexTemplate<PaletteMode::Color16_Palette16, IsVolatile>;
        std::array<PaletteIndex, 8 * 8 / 2> data;

        constexpr CharacterTileTemplate() = default;

        //TODO: Implement this
        constexpr CharacterTileTemplate(const std::array<Palette256Index, 8 * 8>& _data);
        constexpr CharacterTileTemplate(const std::array<Palette16Index, 8 * 8 / 2>& _data) :
            data{ _data }
        {
            
        }
    };

    template<bool IsVolatile>
    struct CharacterTileTemplate<PaletteMode::Color256_Palette1, IsVolatile>
    {
        using PaletteIndex = PaletteIndexTemplate<PaletteMode::Color256_Palette1, IsVolatile>;
        std::array<PaletteIndex, 8 * 8> data;
    };
    

    using CharacterTile16 = CharacterTileTemplate<PaletteMode::Color16_Palette16, false>;
    using CharacterTile256 = CharacterTileTemplate<PaletteMode::Color256_Palette1, false>;

    
    template<PaletteMode Mode>
    class CharacterBlockViewTemplate
    {
        using tile_type = CharacterTileTemplate<Mode, true>;

    private:
        tile_type* baseAddress;
        
    public:
        CharacterBlockViewTemplate(Range<u32, 0, 3> baseBlock) :
            baseAddress{ &Memory<tile_type>(vram + character_block_increments * baseBlock)}
        {
            
        }

        tile_type& operator[](u32 index)
        {
            return baseAddress[index];
        }
    };

    using CharacterBlockView16 = CharacterBlockViewTemplate<PaletteMode::Color16_Palette16>;
    using CharacterBlockView256 = CharacterBlockViewTemplate<PaletteMode::Color256_Palette1>;

    struct TextBackgroundTileDescription
    {
        using Tile_Number = u16PackedRegisterData<Range<u32, 0, 1023>, 10, 0>;
        using Flip_Horizontal = u16PackedRegisterData<WordBool, 1, 10>; 
        using Flip_Vertical = u16PackedRegisterData<WordBool, 1, 11>; 
        using Palette_Number = u16PackedRegisterData<Range<u32, 0, 15>, 4, 12>;
                
        ConditionallyVolatile_T<u16, false> data;

        constexpr TextBackgroundTileDescription() = default;
        constexpr TextBackgroundTileDescription(Tile_Number::type tileNumber, Flip_Horizontal::type flipHorizontal, Flip_Vertical::type flipVertical, Palette_Number::type Palette)
        {
            SetTileNumber(tileNumber);
            Flip_Horizontal::Set(data, flipHorizontal);
            Flip_Vertical::Set(data, flipVertical);
            Palette_Number::Set(data, Palette);
        }

        constexpr void SetTileNumber(Tile_Number::type value)
        {
            Tile_Number::Set(data, value);
        }

        constexpr Tile_Number::type GetTileNumber() const
        {
            return Tile_Number::Get(data);
        }
                
        constexpr void SetFlipHorizontal(Flip_Horizontal::type value)
        {
            Flip_Horizontal::Set(data, value);
        }

        constexpr Flip_Horizontal::type GetFlipHorizontal() const
        {
            return Flip_Horizontal::Get(data);
        }
                
        constexpr void SetFlipVertical(Flip_Vertical::type value)
        {
            Flip_Vertical::Set(data, value);
        }

        constexpr Flip_Vertical::type GetFlipVertical() const
        {
            return Flip_Vertical::Get(data);
        }
                
        constexpr void SetPaletteNumber(Palette_Number::type value)
        {
            Palette_Number::Set(data, value);
        }

        constexpr Palette_Number::type GetPaletteNumber() const
        {
            return Palette_Number::Get(data);
        }
    };

    class TextScreenBlockView
    {
    private:
        using description_type = TextBackgroundTileDescription;

        description_type* baseAddress;

    public:
        TextScreenBlockView(Range<u32, 0, 31> baseBlock) :
            baseAddress{ &Memory<description_type>(vram + screen_block_increments * baseBlock)}
        {
            
        }
        
        description_type& operator[](u32 index)
        {
            return baseAddress[index];
        }
    };
}