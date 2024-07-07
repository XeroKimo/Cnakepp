#pragma once
#include <concepts>
#include "Types.hpp"

namespace cgba
{
    constexpr uintptr io_registers = 0x0400'0000;
    constexpr uintptr display_control_register = 0x0400'0000;
    constexpr uintptr display_status_register = 0x0400'0004;
    constexpr uintptr vertical_counter_register = 0x0400'0006;
    constexpr uintptr background_control_register_base_address = 0x0400'0008;
    constexpr uintptr background_scroll_offset_register_base_address = 0x0400'0010;
    constexpr uintptr background_rotation_scale_register_base_address = 0x0400'0020;
    constexpr uintptr key_input_register = 0x0400'0130;
    constexpr uintptr background_palettes = 0x0500'0000;
    constexpr uintptr object_palettes = 0x0500'0200;
    constexpr uintptr palette_block_increments = 0x0020;
    constexpr uintptr vram = 0x0600'0000;
    constexpr uintptr screen_block_increments = 0x0800;
    constexpr uintptr character_block_increments = 0x4000;


    template<class Ty>
    Ty& Memory(uintptr location)
    {
        return *reinterpret_cast<Ty*>(location);
    }    
    
    //Offset will offset the location equivalent to indexing an array of Ty (aka Ty[offset]);
    template<class Ty>
    Ty& Memory(uintptr location, uintptr offset)
    {
        return *reinterpret_cast<Ty*>(location + sizeof(Ty) * offset);
    }

}