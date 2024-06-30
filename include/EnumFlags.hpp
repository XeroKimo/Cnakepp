#pragma once
#include <type_traits>

namespace cgba
{
    template<class Ty>
    struct UnderlyingType
    {
        using type = Ty;
    };

    template<class Ty>
        requires std::is_class_v<Ty>
    struct UnderlyingType<Ty>
    {
        using type = Ty::type;
    };

    template<class Ty>
        requires std::is_enum_v<Ty>
    struct UnderlyingType<Ty>
    {
        using type = std::underlying_type_t<Ty>;
    };

    template<class Ty>
    using UnderlyingTypeT = UnderlyingType<Ty>::type;

    #define DECLARE_BIT_SHIFT_OPS2(Type, Type2) \
        constexpr Type operator<<(const Type& lh, const Type2& rh)    \
        {   \
            return static_cast<Type>(static_cast<UnderlyingTypeT<Type>>(lh) << static_cast<UnderlyingTypeT<Type2>>(rh));  \
        }   \
        constexpr Type operator>>(const Type& lh, const Type2& rh)    \
        {   \
            return static_cast<Type>(static_cast<UnderlyingTypeT<Type>>(lh) >> static_cast<UnderlyingTypeT<Type2>>(rh));  \
        }  

    #define DECLARE_VOLATILE_BIT_SHIFT_OPS2(Type, Type2) \
        inline Type operator<<(const volatile Type& lh, const Type2& rh)    \
        {   \
            return static_cast<Type>(static_cast<UnderlyingTypeT<Type>>(lh) << static_cast<UnderlyingTypeT<Type2>>(rh));  \
        }   \
        inline Type operator>>(const volatile Type& lh, const Type2& rh)    \
        {   \
            return static_cast<Type>(static_cast<UnderlyingTypeT<Type>>(lh) >> static_cast<UnderlyingTypeT<Type2>>(rh));  \
        }  

#define DECLARE_BIT_FLAG_OPS2(Type, Type2) \
    constexpr Type operator|(const Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<UnderlyingTypeT<Type>>(lh) | static_cast<UnderlyingTypeT<Type2>>(rh));    \
    }   \
    constexpr Type operator&(const Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<UnderlyingTypeT<Type>>(lh) & static_cast<UnderlyingTypeT<Type2>>(rh));    \
    }   \
    constexpr Type operator^(const Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<UnderlyingTypeT<Type>>(lh) ^ static_cast<UnderlyingTypeT<Type2>>(rh));    \
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

#define DECLARE_VOLATILE_BIT_FLAG_OPS2(Type, Type2) \
    inline Type operator|(const volatile Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<UnderlyingTypeT<Type>>(lh) | static_cast<UnderlyingTypeT<Type2>>(rh));    \
    }   \
    inline Type operator&(const volatile Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<UnderlyingTypeT<Type>>(lh) & static_cast<UnderlyingTypeT<Type2>>(rh));    \
    }   \
    inline Type operator^(const volatile Type& lh, const Type2& rh)  \
    {   \
        return static_cast<Type>(static_cast<UnderlyingTypeT<Type>>(lh) ^ static_cast<UnderlyingTypeT<Type2>>(rh));    \
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
    
    #define DECLARE_BIT_FLAG_OPS(Type) DECLARE_BIT_FLAG_OPS2(Type, Type) \
        constexpr Type operator~(const Type& lh)    \
        {   \
            return static_cast<Type>(~static_cast<UnderlyingTypeT<Type>>(lh));  \
        }   \
        DECLARE_BIT_SHIFT_OPS2(Type, i32)

    #define DECLARE_VOLATILE_BIT_FLAG_OPS(Type) DECLARE_VOLATILE_BIT_FLAG_OPS2(Type, Type)    \
        inline Type operator~(const volatile Type& lh)    \
        {   \
            return static_cast<Type>(~static_cast<UnderlyingTypeT<Type>>(lh));  \
        }     \
        DECLARE_VOLATILE_BIT_SHIFT_OPS2(Type, i32)
}