#ifndef UNISTR_UNI_TYPE_TOOLS_H
#define UNISTR_UNI_TYPE_TOOLS_H

#include <vector>
#include <string>
#include <cstring>
#include <type_traits>
#include <array>

//#define UNISTR_SHOULD_SWAP 1

#if defined(UNISTR_SHOULD_SWAP)
    #if defined(__clang__)
        #define UNISTR_SWAP_2(val) (__builtin_bswap16(val))
        #define UNISTR_SWAP_4(val) (__builtin_bswap32(val))
        #define UNISTR_SWAP_8(val) (__builtin_bswap64(val))
    #elif defined(__GNUC__) || defined(__GNUG__)
        #define UNISTR_SWAP_2(val) (__builtin_bswap16(val))
        #define UNISTR_SWAP_4(val) (__builtin_bswap32(val))
        #define UNISTR_SWAP_8(val) (__builtin_bswap64(val))
    #elif defined(_MSC_VER)
        #define UNISTR_SWAP_2(val) (_byteswap_ushort(val))
        #define UNISTR_SWAP_4(val) (_byteswap_ulong(val))
        #define UNISTR_SWAP_8(val) (_byteswap_uint64(val))
    #endif
#else
    #define UNISTR_SWAP_2(val) (val)
    #define UNISTR_SWAP_4(val) (val)
    #define UNISTR_SWAP_8(val) (val)
#endif

/*
#define UNISTR_DECLARE_FUNDAMENTAL(funamental, returnType, funcName, constantness, refType, ...)       \
    template returnType funcName<funamental>(constantness funamental refType, __VA_ARGS__);

#define UNISTR_DECLARE_ALL_FUNDAMENTALS(returnType, funcName, constantness, refType, ...)                   \
        UNISTR_DECLARE_FUNDAMENTAL(char,                returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(char16_t,            returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(char32_t,            returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(signed char,         returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(signed short,        returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(signed long,         returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(signed long long,    returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(signed int,          returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(unsigned char,       returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(unsigned short,      returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(unsigned long,       returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(unsigned long long,  returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(unsigned int,        returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(float,               returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(double,              returnType, funcName, constantness, refType, __VA_ARGS__)        \
        UNISTR_DECLARE_FUNDAMENTAL(bool,                returnType, funcName, constantness, refType, __VA_ARGS__)        \
*/
namespace uni_tools
{
    template<size_t sz>
    void CopyFromBuff(void* dst, const uint8_t* buff) noexcept;
    template<size_t sz>
    void CopyToBuff(void* dst, const uint8_t* buff) noexcept;
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////
    template<>
    inline void CopyFromBuff<1>(void* dst, const uint8_t* buff) noexcept
    {
        *(reinterpret_cast<uint8_t*>(dst)) = *buff;
    }

    /////////////////////////////////////////////////////////////////////
    template<>
    inline void CopyFromBuff<2>(void* dst, const uint8_t* buff) noexcept
    {
        uint16_t tmp;
        memcpy(&tmp, buff, sizeof(tmp));
        *(reinterpret_cast<uint16_t*>(dst)) = UNISTR_SWAP_2(tmp);
    }

    /////////////////////////////////////////////////////////////////////
    template<>
    inline void CopyFromBuff<4>(void* dst, const uint8_t* buff) noexcept
    {
        uint32_t tmp;
        memcpy(&tmp, buff, sizeof(tmp));
        *(reinterpret_cast<uint32_t*>(dst)) = UNISTR_SWAP_4(tmp);
    }

    /////////////////////////////////////////////////////////////////////
    template<>
    inline void CopyFromBuff<8>(void* dst, const uint8_t* buff) noexcept
    {
        uint64_t tmp;
        memcpy(&tmp, buff, sizeof(tmp));
        *(reinterpret_cast<uint64_t*>(dst)) = UNISTR_SWAP_8(tmp);
    }

    /////////////////////////////////////////////////////////////////////
    template<>
    inline void CopyToBuff<1>(void* dst, const uint8_t* buff) noexcept
    {
        *(reinterpret_cast<uint8_t*>(dst)) = *buff;
    }

    /////////////////////////////////////////////////////////////////////
    template<>
    inline void CopyToBuff<2>(void* dst, const uint8_t* buff) noexcept
    {
#if UNISTR_SHOULD_SWAP
        uint16_t tmp;
        memcpy(&tmp, buff, sizeof(tmp));
        tmp = UNISTR_SWAP_2(tmp);
        memcpy(dst, &tmp, sizeof(tmp));
#else
        uint8_t* dstPtr = static_cast<uint8_t*>(dst);
        *dstPtr++ = *buff++;
        *dstPtr = *buff;
#endif
    }

    /////////////////////////////////////////////////////////////////////
    template<>
    inline void CopyToBuff<4>(void* dst, const uint8_t* buff) noexcept
    {
#if UNISTR_SHOULD_SWAP
        uint32_t tmp;
        memcpy(&tmp, buff, sizeof(tmp));
        tmp = UNISTR_SWAP_4(tmp);
        memcpy(dst, &tmp, sizeof(tmp));
#else
        uint8_t* dstPtr = static_cast<uint8_t*>(dst);
        *dstPtr++ = *buff++;
        *dstPtr++ = *buff++;
        *dstPtr++ = *buff++;
        *dstPtr = *buff;
#endif
    }

    /////////////////////////////////////////////////////////////////////
    template<>
    inline void CopyToBuff<8>(void* dst, const uint8_t* buff) noexcept
    {
#if UNISTR_SHOULD_SWAP
        uint64_t tmp;
        memcpy(&tmp, buff, sizeof(tmp));
        tmp = UNISTR_SWAP_8(tmp);
        memcpy(dst, &tmp, sizeof(tmp));
#else
        uint8_t* dstPtr = static_cast<uint8_t*>(dst);
        *dstPtr++ = *buff++;
        *dstPtr++ = *buff++;
        *dstPtr++ = *buff++;
        *dstPtr++ = *buff++;
        *dstPtr++ = *buff++;
        *dstPtr++ = *buff++;
        *dstPtr++ = *buff++;
        *dstPtr = *buff;
#endif
    }

    /////////////////////////////////////////////////////////////////////
    namespace types
    {
        template<typename T>
        using Vec = std::vector<T>;
        //
        template<typename T>
        using Str = std::basic_string<T>;
        //
                                                        // 7 6 5 4 3 2 1 0
        constexpr uint8_t _typeSizeMask     = (3 << 0); // 0 0 0 0 0 0 1 1
        constexpr uint8_t _fundamentalMask  = (1 << 2); // 0 0 0 0 0 1 0 0
        constexpr uint8_t _integralMask     = (1 << 3); // 0 0 0 0 1 0 0 0
        constexpr uint8_t _signedMask       = (1 << 4); // 0 0 0 1 0 0 0 0
        constexpr uint8_t _arrayMask        = (1 << 5); // 0 0 1 0 0 0 0 0
        constexpr uint8_t _userDefinedMask  = (0xC0);   // 1 1 0 0 0 0 0 0
        //
        constexpr uint8_t _noCastMask       = (_fundamentalMask | _integralMask | _typeSizeMask); // 0 0 0 0 1 1 1 1 (0x0F)
        //
        constexpr uint8_t _markTypeSize_1   = (0 << 0); // 0 0 0 0 0 0 0 0
        constexpr uint8_t _markTypeSize_2   = (1 << 0); // 0 0 0 0 0 0 0 1
        constexpr uint8_t _markTypeSize_4   = (2 << 0); // 0 0 0 0 0 0 1 0
        constexpr uint8_t _markTypeSize_8   = (3 << 0); // 0 0 0 0 0 0 1 1
        constexpr uint8_t _markFundamental  = (1 << 2); // 0 0 0 0 0 1 0 0
        constexpr uint8_t _markIntegral     = (1 << 3); // 0 0 0 0 1 0 0 0
        constexpr uint8_t _markSigned       = (1 << 4); // 0 0 0 1 0 0 0 0
        constexpr uint8_t _markArray        = (1 << 5); // 0 0 1 0 0 0 0 0
        constexpr uint8_t _markUserMark_1   = (0x40);   // 0 1 0 0 0 0 0 0
        constexpr uint8_t _markUserMark_2   = (0x80);   // 1 0 0 0 0 0 0 0
        constexpr uint8_t _markUserMark_3   = (0xC0);   // 1 1 0 0 0 0 0 0
        //
        //
        constexpr uint8_t _markNonFundamental   = 0;
        constexpr uint8_t _markNonIntegral      = 0;
        constexpr uint8_t _markUnsigned         = 0;
        //
        constexpr size_t _arrayHeaderSize = 5;
        //
        constexpr uint8_t _markTypeSize(size_t typeSize)
        {
            return  (1 == typeSize) ? _markTypeSize_1 :
                    (2 == typeSize) ? _markTypeSize_2 :
                    (4 == typeSize) ? _markTypeSize_4 :
                    (8 == typeSize) ? _markTypeSize_8 : 
                    0;
        }
        //
        template<typename T, bool b> struct RemoveEnumHelper{};
        template<typename T> struct RemoveEnumHelper<T, false>{using Type = T;};
        template<typename T> struct RemoveEnumHelper<T, true>{ using Type = typename std::underlying_type<T>::type;};
        template<typename T> struct RemoveEnum { using Type = typename RemoveEnumHelper<T, std::is_enum<T>::value>::Type; };
        //
        template<typename T>
        struct ArrayHelper
        {
            using Type = T;
            static constexpr bool isArray = false;
//            static constexpr void Resize(T&, size_t);
//            static constexpr size_t GetSize(const T &);
//            static constexpr size_t GetSerializedSize(const T &);

        };
        template<typename T, size_t N>
        struct ArrayHelper<T[N]>
        {
            using ArrType = T[N];
            using Type = T;
            static constexpr bool isArray = true;
            static void Resize(ArrType&, size_t) noexcept {}
            static constexpr size_t GetSize(const ArrType &)
            {
                return N;
            }
            static constexpr size_t GetSerializedSize(const ArrType &)
            {
                return sizeof(Type) * N + _arrayHeaderSize;
            }
            static const Type* FrontConst(const ArrType &arr) noexcept
            {
                return &(arr[0]);
            }
            static Type* FrontMutable(ArrType& arr)
            {
                return &(arr[0]);
            }
        };

        template<typename T, size_t N>
        struct ArrayHelper<std::array<T, N>>
        {
            using ArrType = std::array<T, N>;
            using Type = T;
            static constexpr bool isArray = true;
            static void Resize(ArrType&, size_t) noexcept {}
            static constexpr size_t GetSize(const ArrType &)
            {
                return N;
            }
            static constexpr size_t GetSerializedSize(const ArrType &)
            {
                return sizeof(Type) * N + _arrayHeaderSize;
            }
            static const Type* FrontConst(const ArrType &arr) noexcept
            {
                return &(arr[0]);
            }
            static Type* FrontMutable(ArrType& arr)
            {
                return &(arr[0]);
            }
        };
        template<typename T>
        struct ArrayHelper<std::vector<T>>
        {
            using ArrType = std::vector<T>;
            using Type = T;
            static constexpr bool isArray = true;
            static void Resize(ArrType& vec, size_t sz)
            {
                vec.resize(sz);
            }
            static size_t GetSize(const ArrType &vec) noexcept
            {
                return vec.size();
            }
            static size_t GetSerializedSize(const ArrType &vec) noexcept
            {
                return sizeof(Type) * vec.size() + _arrayHeaderSize;
            }
            static const Type* FrontConst(const ArrType &arr) noexcept
            {
                return arr.data();
            }
            static Type* FrontMutable(ArrType& arr)
            {
                return arr.data();
            }
        };



        /// NOTE : DO NOT USE std::string for binary data container
        /// USE std::vector<uin8_t> for binary serialization.
        /// std::string will strip the tail zeroes if there are any.
        template<typename T>
        struct ArrayHelper<std::basic_string<T>>
        {
            using ArrType = std::basic_string<T>;
            using Type = T;
            static constexpr bool isArray = true;
            static void Resize(ArrType& str, size_t sz)
            {
                str.resize(sz);
            }
            static size_t GetSize(const ArrType &str) noexcept
            {
                return str.size();
            }
            static size_t GetSerializedSize(const ArrType &str) noexcept
            {
                return sizeof(Type) * str.size() + _arrayHeaderSize;
            }
            static const Type* FrontConst(const ArrType &str) noexcept
            {
                return str.data();
            }
            static Type* FrontMutable(ArrType& str) noexcept
            {
                if (true == str.empty())
                {
                    return nullptr;
                }
                return &(str[0]);
            }
        };
        //
        template<>
        struct ArrayHelper<const char*>
        {
            using ArrType = const char*;
            using Type = char;
            static constexpr bool isArray = true;
            static size_t GetSize(const ArrType &str) noexcept
            {
                return std::strlen(str);
            }
            static size_t GetSerializedSize(const ArrType &str) noexcept
            {
                return sizeof(Type) * std::strlen(str) + _arrayHeaderSize;
            }
            static const Type* FrontConst(const ArrType &str) noexcept
            {
                return str;
            }
        };
        //
        template<typename T> struct ArrayHelper<T&> : ArrayHelper<T> {};
        template<typename T> struct ArrayHelper<T&&> : ArrayHelper<T> {};

        //
        template<typename T> struct RawType 
        {
            using NoArrayType = typename ArrayHelper<T>::Type;
            //
            using Type = typename RemoveEnum<NoArrayType>::Type;
            static constexpr bool wasArray = ArrayHelper<T>::isArray;
            static constexpr bool wasEnum = std::is_enum<NoArrayType>::value;
        };
        //
        template<typename T>
        struct Info
        {
            static_assert(false == std::is_void<T>::value,          "UniStream Type Info requires not void types.");
            static_assert(false == std::is_null_pointer<T>::value,  "UniStream Type Info cannot work with nullptr.");

            using Type = typename RawType<T>::Type;

            static constexpr bool isFundamental = std::is_fundamental<Type>::value || std::is_enum<Type>::value;
            static constexpr bool isIntegral    = std::is_integral<Type>::value || std::is_enum<Type>::value;
            static constexpr size_t typeSize    = isFundamental ? sizeof(Type) : 0;
            static constexpr size_t typeSerializedSize = isFundamental ? (typeSize + 1) : 0;
            static constexpr bool isSigned      = std::is_signed<Type>::value;
            static constexpr bool isArray       = RawType<T>::wasArray;
        };

        template<typename T>
        struct Code___REMOVE
        {
            using Info = types::Info<T>;
            static constexpr uint8_t value =
                (Info::isFundamental    ? _markFundamental  : 0) |
                (Info::isIntegral       ? _markIntegral     : 0) |
                (Info::isSigned         ? _markSigned       : 0) |
                (Info::isArray          ? _markArray        : 0) |
                _markTypeSize(Info::typeSize);

            static constexpr uint8_t noCastCondition = value & _noCastMask;
        };

        template<typename T>
        struct Checker
        {
            using Code = Code___REMOVE<T>;
            static constexpr bool CastNeeded(uint8_t remoteCode)
            {
                return (remoteCode & _noCastMask) != Code::noCastCondition;
            }
        };

        struct CodeParser
        {
            inline static constexpr bool IsFundamental(uint8_t remoteCode)
            {
                return (0 != (remoteCode & _fundamentalMask));
            }
            //
            inline static constexpr bool IsIntegral(uint8_t remoteCode)
            {
                return (0 != (remoteCode & _integralMask));
            }
            //
            inline static constexpr bool IsSigned(uint8_t remoteCode)
            {
                return (0 != (remoteCode & _signedMask));
            }
            //
            inline static constexpr bool IsArray(uint8_t remoteCode)
            {
                return (0 != (remoteCode & _arrayMask));
            }
            //
            inline static constexpr uint8_t ArrayElementCode(uint8_t remoteCode)
            {
                return remoteCode & (~_arrayMask);
            }
            //
            inline static constexpr size_t TypeSize(uint8_t remoteCode)
            {
                // (remoteCode & _typeSizeMask) :
                // 0 -> 1 byte
                // 1 -> 2 bytes
                // 2 -> 4 bytes
                // 3 -> 8 bytes
                return 1 << (remoteCode & _typeSizeMask);
            }
            //
        };
    }
}

#endif //UNISTR_UNI_TYPE_TOOLS_H
