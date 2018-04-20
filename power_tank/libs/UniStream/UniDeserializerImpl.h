#ifndef UNISTR_UNI_DESERIALIZER_IMPL_H
#define UNISTR_UNI_DESERIALIZER_IMPL_H

#include <cstdint>
#include <cstddef>


namespace deserializer_impl
{
    //
    template<typename T>
    void DeserializeNonArrayFundamental(T* val, uint8_t remoteCode, const uint8_t* buff) noexcept;
    //
    template<typename T>
    void DeserializeArrayFundamental(T* val, uint8_t remoteCode, const uint8_t* buff, size_t count) noexcept;
    //
    void DeserializeArrayHeader(const uint8_t* buff, uint8_t& remoteCode, size_t& size);
}

#include "UniDeserializerImpl.hpp"
#endif //UNISTR_UNI_DESERIALIZER_FWDH

