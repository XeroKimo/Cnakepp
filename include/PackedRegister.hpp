#pragma once
#include <concepts>
#include <climits>
#include "Types.hpp"

namespace cgba
{
    
    template<std::integral MaskType, class IOType, MaskType MaskSize, MaskType MaskShift>
        requires (MaskShift < sizeof(MaskType) * CHAR_BIT)
    struct PackedRegisterData
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
            if constexpr(std::is_enum_v<IOType> && MaskSize == 1)
            {
                _register |= static_cast<std::underlying_type_t<IOType>>(value) << MaskShift;
            }
            else
            {
                Reset(_register);
                if constexpr(std::is_enum_v<IOType>)
                    _register |= static_cast<std::underlying_type_t<IOType>>(value) << MaskShift;
                else
                    _register |= value << MaskShift;
            }
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
            return IOType{ (_register & bitMask) >> bitShift };
        }
    };
    
    template<class IOType, auto MaskSize, auto MaskShift>
    using u16PackedRegisterData = PackedRegisterData<u16, IOType, MaskSize, MaskShift>;

    template<class Ty, bool Volatile>
    struct PackedRegister
    {
        Ty data {};
    };

    template<class Ty>
    struct PackedRegister<Ty, true>
    {
        volatile Ty data {};
    };
}