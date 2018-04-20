#ifndef UNISTR_UNI_SERIALIZER_H
#define UNISTR_UNI_SERIALIZER_H

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <list>

#include "../Std17/type_traits.hpp"
#include "UniSerializerImpl.h"
#include "UniTypeTools.h"

class iUniStream;
class oUniStream
{
public:
    using StorageType = std::vector<uint8_t>;
    struct Inplace
    {
        Inplace(oUniStream &s) : serializer(s){}
        oUniStream& serializer;
    };

    oUniStream();

    //
    oUniStream(oUniStream&&);
    oUniStream(std::vector<uint8_t>&& buff);
    oUniStream& operator=(oUniStream&&);
    //
    oUniStream(const oUniStream&) = delete;
    oUniStream& operator=(const oUniStream&) = delete;

    inline explicit operator bool() const
    {
        return true;
    }

    template<typename T> typename
    std::enable_if
    <
            uni_tools::types::Info<T>::isArray
        ||  uni_tools::types::Info<T>::isFundamental
        ,   oUniStream&
    >::type
    operator<<(const T& var)
    {
        _serialize(var);
        return *this;
    }


    oUniStream& operator<<(const oUniStream& var);
    oUniStream& operator<<(const iUniStream& var);
    oUniStream& operator>>(Inplace&& inplace);

    oUniStream& Serialize() { return *this; }

    template<typename T>
    oUniStream& Serialize(T&& t1)
    {
        (*this) << std::forward<T>(t1);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename T, typename ... Types>
    oUniStream& Serialize(T&& t1, Types&& ...  args)
    {
        (*this) << std::forward<T>(t1);
        (*this).Serialize(std::forward<Types>(args)...);
        return *this;
    }

    void Invalidate();
    void Clear();

    std::vector<uint8_t> &GetBuffer();
    inline StorageType &GetStorage()
    {
        auto& storage = GetBuffer();
        return storage;
    }

    inline void SetNextItemUserMarker(uint8_t userMarker)
    {
        m_userMarker = userMarker & uni_tools::types::_userDefinedMask;
    }

    inline void InplaceNonFundamentalArrayHeade(size_t size)
    {
        uint8_t* memPtr = _getInternalMemory(uni_tools::types::_arrayHeaderSize);
        serializer_impl::SerializeArrayHeader(memPtr, uni_tools::types::_arrayMask , size);
    }

private:
    //
    std::vector<uint8_t> m_internalBuff;
    std::vector<uint8_t> m_inplacedBufferCopy;
    std::vector<uint8_t>* m_buffPtr = nullptr; // non owning
    size_t m_frontIdx = 0;
    size_t m_endIdx = 0; // not indexable (last idx + 1)
    size_t m_payloadEndIdx = 0;
    uint8_t m_userMarker = 0;
    //

    struct InplaceMarker{};
    oUniStream(InplaceMarker, oUniStream& host);
    //void _flush

    inline uint8_t* _getInternalMemory(size_t size)
    {
        if((m_endIdx - m_payloadEndIdx) < size)
        {
            if (m_endIdx != m_buffPtr->size())
            {
                throw "Probably bad using of inplaced streams";
            }
            size_t missing = size - (m_endIdx - m_payloadEndIdx);
            m_buffPtr->resize( m_buffPtr->size() * 2 +  missing);
            m_endIdx = m_buffPtr->size();
        }
        uint8_t* memPtr = m_buffPtr->data() + m_payloadEndIdx;
        m_payloadEndIdx += size;
        return memPtr;
    }
    //
    template<typename T> inline typename
    std::enable_if
    <
            true == uni_tools::types::Info<T>::isFundamental
        &&  false == uni_tools::types::Info<T>::isArray
        ,   void
    >::type
    _serialize(const T& var)
    {
        // Fundamental, NO array
        uint8_t* memPtr = _getInternalMemory(uni_tools::types::Info<T>::typeSerializedSize);
        serializer_impl::SerializeNonArrayFundamental(&var, memPtr, m_userMarker);
        m_userMarker = 0;
    }
    //
    template<typename T> inline typename
    std::enable_if
    <
            true == uni_tools::types::Info<T>::isFundamental
        &&  true == uni_tools::types::Info<T>::isArray
        &&  false == std::is_same<T, std::vector<bool>>::value
        ,   void
    >::type
    _serialize(const T& var)
    {
        // Fundamental array
        uint8_t* memPtr = _getInternalMemory(uni_tools::types::ArrayHelper<T>::GetSerializedSize(var));
        size_t arraySize = uni_tools::types::ArrayHelper<T>::GetSize(var);
        serializer_impl::SerializeArrayFundamental(uni_tools::types::ArrayHelper<T>::FrontConst(var), memPtr, arraySize, m_userMarker);
        m_userMarker = 0;
    }
    //
    // std::vector<bool> is a shit !
    void _serialize(const std::vector<bool>& var)
    {
        // Fundamental array
        std::vector<uint8_t> copy(var.size());
        size_t idx = 0;
        for (auto b : var)
        {
            copy[idx] = static_cast<uint8_t>(b);
            idx++;
        }
        _serialize(copy);
    }
    //
    template<typename T> inline typename
    std::enable_if
    <
            false == uni_tools::types::Info<T>::isFundamental
        &&  true == uni_tools::types::Info<T>::isArray
        ,   void
    >::type
    _serialize(const T& var)
    {
        // Non fundamental array
        uint8_t* memPtr = _getInternalMemory( uni_tools::types::_arrayHeaderSize );
        size_t arraySize = uni_tools::types::ArrayHelper<T>::GetSize(var);
        serializer_impl::SerializeArrayHeader(memPtr, uni_tools::types::Code___REMOVE<T>::value | m_userMarker, arraySize);
        m_userMarker = 0;
        const typename uni_tools::types::ArrayHelper<T>::Type* elPtr = uni_tools::types::ArrayHelper<T>::FrontConst(var);
        auto arrEnd = elPtr + arraySize;
        while(elPtr < arrEnd)
        {
            *this << (*elPtr);
            elPtr++;
        }
    }
};

template<typename T1, typename T2>
oUniStream& operator <<(oUniStream& serializer, const std::pair<T1, T2>& p)
{
    return serializer << p.first << p.second;
}

template<typename KeyType, typename ValueType>
oUniStream& operator <<(oUniStream& serializer, const std::map<KeyType, ValueType>& m)
{
    serializer << m.size();
    for (const auto& p : m)
    {
        serializer << p;
    }
    return serializer;
}

template<typename KeyType, typename ValueType>
oUniStream& operator <<(oUniStream& serializer, const std::unordered_map<KeyType, ValueType>& m)
{
    serializer << m.size();
    for (const auto& p : m)
    {
        serializer << p;
    }
    return serializer;
}

template<typename T>
oUniStream& operator <<(oUniStream& serializer, const std::list<T>& l)
{
    serializer << l.size();
    for (const auto& e : l)
    {
        serializer << e;
    }
    return serializer;
}

template<typename T>
oUniStream& operator <<(oUniStream& serializer, const std::set<T>& s)
{
    serializer << s.size();
    for (const auto& e : s)
    {
        serializer << e;
    }
    return serializer;
}

template<typename T>
oUniStream& operator <<(oUniStream& serializer, const std::unordered_set<T>& s)
{
    serializer << s.size();
    for (const auto& e : s)
    {
        serializer << e;
    }
    return serializer;
}
//
template<typename T> typename
std::enable_if
<
        false == uni_tools::types::Info<T>::isArray
    &&  false == uni_tools::types::Info<T>::isFundamental
    ,   oUniStream&
>::type
operator<<(oUniStream&, const T& var);

template<typename ...Args>
oUniStream& operator <<(oUniStream& serializer, const std::tuple<Args...>& tup)
{
    nonstd::for_each_in_tuple(tup,
    [&serializer](const auto& e)
    {
        serializer << e;
    });
    return serializer;
}

#endif // UNISTR_UNI_SERIALIZER_H
