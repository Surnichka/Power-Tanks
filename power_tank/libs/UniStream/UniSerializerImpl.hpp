#include "UniSerializerImpl.h"
#include "UniTypeTools.h"


namespace serializer_impl
{
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    template<typename T>
    inline void SerializeNonArrayFundamental(const T* val, uint8_t* buff, uint8_t userMarker) noexcept
    {
        *buff++ = uni_tools::types::Code___REMOVE<T>::value | userMarker;
        const uint8_t* src = reinterpret_cast<const uint8_t*>(val);
        uni_tools::CopyToBuff<sizeof(T)>(buff, src);
    }

    inline void SerializeArrayHeader(uint8_t* buff, uint8_t typeCode, size_t size) noexcept
    {
        // write element type marker
        *buff = typeCode;

        // write elements count
        uint32_t s = static_cast<uint32_t>(size);
        uni_tools::CopyToBuff<4>(buff+1, reinterpret_cast<const uint8_t*>(&s));
    }

    /////////////////////////////////////////////////////////////////////
    template<typename T>
    inline void SerializeArrayFundamental(const T* val, uint8_t* buff, size_t count, uint8_t userMarker) noexcept
    {
        using namespace uni_tools::types;
        SerializeArrayHeader(buff, Code___REMOVE<T>::value | _markArray | userMarker, count);
        buff += _arrayHeaderSize;

        if (val == nullptr)
        {
            return;
        }
        // write elements
        const uint8_t* src = reinterpret_cast<const uint8_t*>(val);
#if defined(UNISTR_SHOULD_SWAP)
        auto arrayEnd = src + sizeof(T) * count;
        while(src < arrayEnd)
        {
            CopyToBuff<sizeof(T)>(buff, src);
            src += sizeof (T);
            buff += sizeof (T);
        }
#else
        memcpy(buff, src, sizeof(T) * count);
#endif
    }

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    //UNISTR_DECLARE_ALL_FUNDAMENTALS(void, SerializeNonArrayFundamental, const, *, uint8_t*)
    //UNISTR_DECLARE_ALL_FUNDAMENTALS(void, SerializeArrayFundamental, const, *, uint8_t*, size_t)
}
