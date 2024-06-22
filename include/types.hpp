#pragma once
#include "bn_cstring.h"
#include <concepts>

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

    static_assert(sizeof(i32) == 4);
    static_assert(sizeof(u32) == 4);
    static_assert(sizeof(i16) == 2);
    static_assert(sizeof(u16) == 2);
    static_assert(sizeof(i8) == 1);
    static_assert(sizeof(u8) == 1);
};