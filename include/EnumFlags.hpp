#pragma once
#include <type_traits>

namespace cgba
{
#define DECLARE_ENUM_FLAG_OPS2(Type, Type2) \
    constexpr Type operator|(const Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<std::underlying_type_t<Type>>(lh) | static_cast<std::underlying_type_t<Type2>>(rh));    \
    }   \
    constexpr Type operator&(const Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<std::underlying_type_t<Type>>(lh) & static_cast<std::underlying_type_t<Type2>>(rh));    \
    }   \
    constexpr Type operator^(const Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<std::underlying_type_t<Type>>(lh) ^ static_cast<std::underlying_type_t<Type2>>(rh));    \
    }   \
    constexpr Type operator|=(Type& lh, const Type2& rh)   \
    {   \
        return lh = lh | rh;    \
    }   \
    constexpr Type operator&=(Type& lh, const Type2& rh)   \
    {   \
        return lh = lh & rh;    \
    }   \
    constexpr Type operator^=(Type& lh, const Type2& rh)   \
    {   \
        return lh = lh ^ rh;    \
    }  

    #define DECLARE_ENUM_VOLATILE_FLAG_OPS2(Type, Type2) \
    inline Type operator|(const volatile Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<std::underlying_type_t<Type>>(lh) | static_cast<std::underlying_type_t<Type2>>(rh));    \
    }   \
    inline Type operator&(const volatile Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<std::underlying_type_t<Type>>(lh) & static_cast<std::underlying_type_t<Type2>>(rh));    \
    }   \
    inline Type operator^(const volatile Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<std::underlying_type_t<Type>>(lh) ^ static_cast<std::underlying_type_t<Type2>>(rh));    \
    }   \
    inline Type operator|=(volatile Type& lh, const Type2& rh)   \
    {   \
        Type temp = lh;\
        temp |= rh;\
        lh = temp;\
        return lh;    \
    }   \
    inline Type operator&=(volatile Type& lh, const Type2& rh)   \
    {   \
        Type temp = lh;\
        temp &= rh;\
        lh = temp;\
        return lh;    \
    }   \
    inline Type operator^=(volatile Type& lh, const Type2& rh)   \
    {   \
        Type temp = lh;\
        temp ^= rh;\
        lh = temp;\
        return lh;    \
    }
    
    #define DECLARE_ENUM_FLAG_OPS(Type) DECLARE_ENUM_FLAG_OPS2(Type, Type) \
        constexpr Type operator~(const Type& lh)    \
        {   \
            return static_cast<Type>(~static_cast<std::underlying_type_t<Type>>(lh));  \
        }   

    #define DECLARE_ENUM_VOLATILE_FLAG_OPS(Type) DECLARE_ENUM_VOLATILE_FLAG_OPS2(Type, Type)    \
        inline Type operator~(const volatile Type& lh)    \
        {   \
            return static_cast<Type>(~static_cast<std::underlying_type_t<Type>>(lh));  \
        }  
}