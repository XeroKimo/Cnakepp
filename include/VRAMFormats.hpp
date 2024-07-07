#pragma once
#include <array>
#include "Math.hpp"
#include "Types.hpp"
#include "MemoryRegion.hpp"
#include "PackedRegister.hpp"

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
        
    struct Palette4Handle
    {
        u8 index;
    };
       
    struct Palette8Handle
    {
        u8 index;
    };
    
    struct VolatilePalette8Handle
    {
        volatile u8 index;

        VolatilePalette8Handle() = default;
        VolatilePalette8Handle(const Palette8Handle& v) :
            index{v.index}
        {   

        }

        operator Palette8Handle() const { return {index}; }
    };

    struct Tile4
    {
        std::array<u8, 8 * 8 / 2> data;

        constexpr Tile4() = default;

        //TODO: Implement this
        constexpr Tile4(const std::array<u8, 8 * 8>& _data);
        constexpr Tile4(const std::array<u8, 8 * 8 / 2>& _data) :
            data{ _data }
        {
            
        }
    };

    struct Tile8
    {
        std::array<u8, 8 * 8> data;
    };

    struct BackgroundMapTile
    {
        u16 data;
    };

    constexpr uintptr character_block_increments = 0x4000;
    class CharacterBlockView8
    {
        using tile_type = Tile8;

    private:
        tile_type* baseAddress;

    public:
        CharacterBlockView8(Range<u32, 0, 3> baseBlock) :
            baseAddress{ reinterpret_cast<tile_type*>(&VRAM<u8>(character_block_increments * baseBlock))}
        {
            
        }

        tile_type& operator[](u32 index)
        {
            return baseAddress[index];
        }
    };

    struct TextBackgroundTileDescription : PackedRegister<u16, false>
    {
        using PackedRegister<u16, false>::data;

        using Tile_Number = u16PackedRegisterData<Range<u32, 0, 1023>, 10, 0>;
        using Flip_Horizontal = u16PackedRegisterData<WordBool, 1, 10>; 
        using Flip_Vertical = u16PackedRegisterData<WordBool, 1, 11>; 
        using Palette_Number = u16PackedRegisterData<Range<u32, 0, 15>, 4, 12>;

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

    constexpr uintptr screen_block_increments = 0x0800;
    class TextScreenBlockView
    {
    private:
        using description_type = TextBackgroundTileDescription;

        description_type* baseAddress;

    public:
        TextScreenBlockView(Range<u32, 0, 31> baseBlock) :
            baseAddress{ reinterpret_cast<description_type*>(&VRAM<u8>(screen_block_increments * baseBlock))}
        {
            
        }
        
        description_type& operator[](u32 index)
        {
            return baseAddress[index];
        }
    };

    constexpr uintptr background_palettes_base_address = 0x0500'0000;
    constexpr uintptr background_palettes_block_increments = sizeof(RGB15) * 16;
    class BackgroundPaletteBlockView
    {
    private:
        VolatileRGB15* baseAddress;

    public:
        BackgroundPaletteBlockView(Range<u32, 0, 15> baseBlock = 0) :
            baseAddress{ &Memory<VolatileRGB15>(background_palettes_base_address + background_palettes_block_increments * baseBlock) }
        {

        }

        VolatileRGB15& operator[](u32 index)
        {
            return baseAddress[index];
        }
    };
}