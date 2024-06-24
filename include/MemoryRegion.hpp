#pragma once
#include <concepts>
#include "Types.hpp"

namespace cgba
{
    constexpr uintptr io_registers = 0x0400'0000;
    constexpr uintptr display_control_register = 0x0400'0000;
    constexpr uintptr display_status_register = 0x0400'0004;
    constexpr uintptr vertical_counter_register = 0x0400'0006;
    constexpr uintptr key_input_register = 0x0400'0130;
    constexpr uintptr background_pallettes = 0x0500'0000;
    constexpr uintptr object_pallettes = 0x0500'0200;
    constexpr uintptr vram = 0x0600'0000;

    template<class Ty, class Ty2>
    concept integral_compatible = std::integral<Ty2> 
        && (sizeof(Ty) == sizeof(Ty2) && alignof(Ty) == alignof(Ty2));

    template<class Ty>
    concept integral_like = integral_compatible<Ty, u8> 
        || integral_compatible<Ty, i8> 
        || integral_compatible<Ty, u16> 
        || integral_compatible<Ty, i16> 
        || integral_compatible<Ty, u32> 
        || integral_compatible<Ty, i32>; 

    template<integral_like Ty>
    Ty& Memory(uintptr location)
    {
        return *reinterpret_cast<Ty*>(location);
    }

    //Returns a reference to a memory location with the background pallette as it's base address
    //the offset will respect the alignment of the type passed in as a template parameter
    template<integral_like Ty>
    Ty& BackgroundPallettes(uintptr offset)
    {
        return (&Memory<Ty>(background_pallettes))[offset];
    }

    //Returns a reference to a memory location with the object pallette as it's base address
    //the offset will respect the alignment of the type passed in as a template parameter
    template<integral_like Ty>
    Ty& ObjectPallettes(uintptr offset)
    {
        return (&Memory<Ty>(object_pallettes))[offset];
    }

    //Returns a reference to a memory location with the VRAM as it's base address
    //the offset will respect the alignment of the type passed in as a template parameter
    template<integral_compatible<u16> Ty = u16>
    Ty& VRAM(uintptr offset)
    {
        return (&Memory<Ty>(vram))[offset];
    }

    template<integral_like Ty>
    volatile Ty& VolatileMemory(uintptr location)
    {
        return Memory<Ty>(location);
    }

    //Returns a reference to a memory location with the background pallette as it's base address
    //the offset will respect the alignment of the type passed in as a template parameter
    template<integral_like Ty>
    volatile Ty& VolatileBackgroundPallettes(uintptr offset)
    {
        return BackgroundPallettes<Ty>(offset);
    }

    //Returns a reference to a memory location with the object pallette as it's base address
    //the offset will respect the alignment of the type passed in as a template parameter
    template<integral_like Ty>
    volatile Ty& VolatileObjectPallettes(uintptr offset)
    {
        return ObjectPallettes<Ty>(offset);
    }

    //Returns a reference to a memory location with the VRAM as it's base address
    //the offset will respect the alignment of the type passed in as a template parameter
    template<integral_compatible<u16> Ty = u16>
    volatile Ty& VolatileVRAM(uintptr offset)
    {
        return VRAM<Ty>(offset);
    }
    
}