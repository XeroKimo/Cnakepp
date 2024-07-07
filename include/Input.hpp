#pragma once
#include "Types.hpp"
#include "MemoryRegion.hpp"
#include "EnumFlags.hpp"

namespace cgba
{
    enum class Key : u16
    {
        A = 1 << 0,
        B= 1 << 1,
        Select= 1 << 2,
        Start= 1 << 3,
        Right= 1 << 4,
        Left= 1 << 5,
        Up= 1 << 6,
        Down= 1 << 7,
        R= 1 << 8,
        L= 1 << 9,
    };

    
    DECLARE_BIT_FLAG_OPS2(u16, Key);
    struct KeyInput
    {
        u16 data;
    };

    inline KeyInput PollInput()
    {
        KeyInput input = { Memory<volatile u16>(key_input_register) };
        input.data = ~input.data;
        return input;
    }
}