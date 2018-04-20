#ifndef UNISTR_UNI_DESERIALIZER_H
#define UNISTR_UNI_DESERIALIZER_H

#include <vector>

#include "UniDeserializerImpl.h"
#include "UniTypeTools.h"
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <list>


class oUniStream;

class iUniStream
{
public:
    using StorageType = std::vector<uint8_t>;
    struct Inplace
    {
        Inplace(iUniStream &d) : deserializer(d){}
        iUniStream& deserializer;
    };

    iUniStream();
    iUniStream(const std::vector<uint8_t> &buff);
    iUniStream(std::vector<uint8_t> &buff);
    iUniStream(std::vector<uint8_t> &&buff);
    iUniStream(oUniStream &serializer);
    iUniStream(oUniStream &&serializer);

    //
    iUniStream(iUniStream&&);
    iUniStream& operator=(iUniStream&&);
    //
    iUniStream(const iUniStream&) = delete;
    iUniStream& operator=(const iUniStream&) = delete;

    std::vector<uint8_t> GetBuffer() const;

    template<typename T> typename
    std::enable_if
    <
            true == uni_tools::types::Info<T>::isArray
        ||  true == uni_tools::types::Info<T>::isFundamental
        ,   iUniStream&
    >::type
    operator>>(T& var)
    {
        _deserialize(var);
        return *this;
    }

    iUniStream& operator>>(iUniStream& var);
    iUniStream& operator>>(oUniStream& var);
    iUniStream& operator>>(Inplace&& inplace);

    inline explicit operator bool() const
    {
        auto res = m_lastOperationResult;
        if (res == false)
        {
            return false;
        }
        return m_lastOperationResult;
    }

    inline void Invalidate()
    {
        m_lastOperationResult = false;
    }

    inline bool IsEmpty() const
    {
        if (m_buffPtr == nullptr)
        {
            return true;
        }
        return m_payloadEndIdx == m_endIdx;
    }

    void Clear();

    template<typename T>
    iUniStream& Deserialize(T& t1)
    {
        *this >> t1;
        return *this;
    }

    template<typename T, typename ... Types>
    iUniStream& Deserialize(T& t1, Types& ...  args)
    {
        Deserialize(t1);
        Deserialize(args...);
        return *this;
    }

    inline uint8_t GetNextUserMarker()
    {
        if (IsEmpty() == true)
        {
            return 0;
        }
        const uint8_t* headerPtr = _internalBufferHead();
        if (nullptr == headerPtr)
        {
            return 0;
        }
        return (*headerPtr) & (uni_tools::types::_userDefinedMask);
    }

    //////////////////////////


    inline bool IsFundamental() const
    {
        if (IsEmpty() == true)
        {
            return false;
        }
        auto remoteCode = *(_internalBufferHead());
        return uni_tools::types::CodeParser::IsFundamental(remoteCode);
    }

    inline static bool IsFundamental(const std::vector<uint8_t>& buff)
    {
        if (buff.empty() == true)
        {
            return false;
        }
        auto remoteCode = buff[0];
        return uni_tools::types::CodeParser::IsFundamental(remoteCode);
    }

    inline bool IsIntegral() const
    {
        if (IsEmpty() == true)
        {
            return false;
        }
        auto remoteCode = *(_internalBufferHead());
        return uni_tools::types::CodeParser::IsIntegral(remoteCode);
    }

    inline static bool IsIntegral(const std::vector<uint8_t>& buff)
    {
        if (buff.empty() == true)
        {
            return false;
        }
        auto remoteCode = buff[0];
        return uni_tools::types::CodeParser::IsIntegral(remoteCode);
    }

    inline bool IsSigned() const
    {
        if (IsEmpty() == true)
        {
            return false;
        }
        auto remoteCode = *(_internalBufferHead());
        return uni_tools::types::CodeParser::IsSigned(remoteCode);
    }

    inline static bool IsSigned(const std::vector<uint8_t>& buff)
    {
        if (buff.empty() == true)
        {
            return false;
        }
        auto remoteCode = buff[0];
        return uni_tools::types::CodeParser::IsSigned(remoteCode);
    }

    inline bool IsArray() const
    {
        if (IsEmpty() == true)
        {
            return false;
        }
        auto remoteCode = *(_internalBufferHead());
        return uni_tools::types::CodeParser::IsArray(remoteCode);
    }

    inline static bool IsArray(const std::vector<uint8_t>& buff)
    {
        if (buff.empty() == true)
        {
            return false;
        }
        auto remoteCode = buff[0];
        return uni_tools::types::CodeParser::IsArray(remoteCode);
    }

    inline bool IsString() const
    {
        if (IsEmpty() == true ||
            IsArray() == false ||
            IsFundamental() == false ||
            IsIntegral() == false ||
            IsSigned() == true ||
            GetTypeSize() != 1 ||
            (m_payloadEndIdx + uni_tools::types::_arrayHeaderSize >= m_buffPtr->size()))
        {
            return false;
        }

        uint8_t remoteCode;
        size_t arrayCount;
        auto head = _internalBufferHead();

        deserializer_impl::DeserializeArrayHeader(head, remoteCode, arrayCount);

        if (m_payloadEndIdx + uni_tools::types::_arrayHeaderSize + arrayCount > m_endIdx)
        {
            return false;
        }

        return (head + uni_tools::types::_arrayHeaderSize)[arrayCount-1] == 0; // check if is zero-terminated
    }

    inline static bool IsString(const std::vector<uint8_t>& buff)
    {
        if (buff.empty() == true ||
            IsArray(buff) == false ||
            IsFundamental(buff) == false ||
            IsIntegral(buff) == false ||
            IsSigned(buff) == true ||
            GetTypeSize(buff) != 1 ||
            (uni_tools::types::_arrayHeaderSize >= buff.size()))
        {
            return false;
        }

        uint8_t remoteCode;
        size_t arrayCount;
        auto headPtr = &(buff[0]);

        deserializer_impl::DeserializeArrayHeader(headPtr, remoteCode, arrayCount);

        if (uni_tools::types::_arrayHeaderSize + arrayCount > buff.size())
        {
            return false;
        }

        return (headPtr + uni_tools::types::_arrayHeaderSize)[arrayCount-1] == 0; // check if is zero-terminated
    }

    inline size_t GetTypeSize() const
    {
        if (IsEmpty() == true)
        {
            return 0;
        }
        auto remoteCode = *(_internalBufferHead());
        return uni_tools::types::CodeParser::TypeSize(remoteCode);
    }

    inline static size_t GetTypeSize(const std::vector<uint8_t>& buff)
    {
        if (buff.empty() == true)
        {
            return 0;
        }
        auto remoteCode = buff[0];
        return uni_tools::types::CodeParser::TypeSize(remoteCode);
    }

    inline size_t GetArraySize() const
    {
        if (IsEmpty() == true ||
            IsArray() == false ||
            (m_payloadEndIdx + uni_tools::types::_arrayHeaderSize >= m_buffPtr->size()))
        {
            return 0;
        }

        uint8_t remoteCode;
        size_t arrayCount;
        auto head = _internalBufferHead();

        deserializer_impl::DeserializeArrayHeader(head, remoteCode, arrayCount);
        return arrayCount;
    }

    inline size_t ConsumeArraySize()
    {
        if (IsEmpty() == true ||
            IsArray() == false)

        {
            return 0;
        }

        auto head = _consumeInternalMemory(uni_tools::types::_arrayHeaderSize);
        if (head == nullptr)
        {
            return 0;
        }
        uint8_t remoteCode;
        size_t arrayCount;
        deserializer_impl::DeserializeArrayHeader(head, remoteCode, arrayCount);
        return arrayCount;
    }

    template<typename T>
    inline bool CheckType(const T* nullPtr = nullptr) const
    {
        using TypeCode = decltype (uni_tools::types::Code___REMOVE<T>::value);
        if (_hasMemory(sizeof (TypeCode)) == false)
        {
            return false;
        }
        TypeCode remoteCode =  *(TypeCode*)(_internalBufferHead());

        return remoteCode == uni_tools::types::Code___REMOVE<T>::value;
    }

    //////////////////////////
private:
    //
    std::vector<uint8_t> m_internalBuff;
    const std::vector<uint8_t>* m_buffPtr = nullptr; // non owning
    size_t m_frontIdx = 0;
    size_t m_endIdx = 0; // not indexable (last idx + 1)
    size_t m_payloadEndIdx = 0;
    bool m_lastOperationResult = false;
    //

    struct InplaceMarker{};
    iUniStream(InplaceMarker, iUniStream& host);
    //
    inline const uint8_t* _internalBufferHead() const
    {
        const uint8_t* memPtr = m_buffPtr->data() + m_payloadEndIdx;
        return memPtr;
    }
    //
    inline const bool _hasMemory(size_t size) const
    {
        return m_endIdx >= size + m_payloadEndIdx;
    }
    //
    inline const uint8_t* _consumeInternalMemory(size_t size)
    {
        if(false == _hasMemory(size))
        {
            if (true == m_lastOperationResult)
            {
                m_lastOperationResult = false;
                return nullptr;
            }
            throw "bad stream deserialization";
        }

        auto memPtr = _internalBufferHead();
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
    _deserialize(T& var)
    {
        // Fundamental, NO array
        const uint8_t* remoteCodePtr = _consumeInternalMemory(1);
        if (nullptr == remoteCodePtr)
        {
            return;
        }
        uint8_t remoteCode = *remoteCodePtr;
        if ( false == uni_tools::types::CodeParser::IsFundamental(remoteCode) ||
             true == uni_tools::types::CodeParser::IsArray(remoteCode))
        {
            if (true == m_lastOperationResult)
            {
                m_lastOperationResult = false;
                return;
            }
            throw "Reading array or non fundamental type as fundamental.";
        }
        size_t remoteSize = uni_tools::types::CodeParser::TypeSize(remoteCode);
        const uint8_t* dumpPtr = _consumeInternalMemory(remoteSize);
        if (nullptr == dumpPtr)
        {
            return;
        }

        deserializer_impl::DeserializeNonArrayFundamental(&var, remoteCode, dumpPtr);
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
    _deserialize(T& var)
    {
        // Fundamental array
        const uint8_t *headerPtr = _consumeInternalMemory( uni_tools::types::_arrayHeaderSize );
        if (nullptr == headerPtr)
        {
            return;
        }
        uint8_t remoteCode;
        size_t arrayCount;

        deserializer_impl::DeserializeArrayHeader(headerPtr, remoteCode, arrayCount);

        if ( false == uni_tools::types::CodeParser::IsFundamental(remoteCode) ||
             false == uni_tools::types::CodeParser::IsArray(remoteCode))
        {
            if (true == m_lastOperationResult)
            {
                m_lastOperationResult = false;
                return;
            }
            throw "Reading non array or non fundamental type as array of fundamentals.";
        }

        uni_tools::types::ArrayHelper<decltype (var)>::Resize(var, arrayCount);
        auto sz = uni_tools::types::ArrayHelper<decltype (var)>::GetSize(var);
        if (sz < arrayCount)
        {
            if (true == m_lastOperationResult)
            {
                m_lastOperationResult = false;
                return;
            }
            throw "Reading wrong size for array.";
        }

        size_t arraySize = arrayCount * uni_tools::types::CodeParser::TypeSize(remoteCode);

        const uint8_t *dumpPtr = _consumeInternalMemory( arraySize );
        if (nullptr == dumpPtr)
        {
            return;
        }
        typename uni_tools::types::ArrayHelper<T>::Type* elPtr = uni_tools::types::ArrayHelper<T>::FrontMutable(var);

        deserializer_impl::DeserializeArrayFundamental(elPtr, remoteCode, dumpPtr, arrayCount);

        _clearStringZeroes(var);
    }
    //
    // std::vector<bool> is shit
    void _deserialize(std::vector<bool>& var)
    {
        std::vector<uint8_t> copy;
        _deserialize(copy);
        var.resize(copy.size());
        size_t idx = 0;
        for(auto b : copy)
        {
            var[idx] = static_cast<bool>(b);
            idx++;
        }
    }
    //
    template<typename T> inline typename
    std::enable_if
    <
            false == uni_tools::types::Info<T>::isFundamental
        &&  true == uni_tools::types::Info<T>::isArray
        ,   void
    >::type
    _deserialize(T& var)
    {
        // Non fundamental array
        const uint8_t *headerPtr = _consumeInternalMemory( uni_tools::types::_arrayHeaderSize );
        if (nullptr == headerPtr)
        {
            return;
        }
        uint8_t remoteCode;
        size_t arrayCount;

        deserializer_impl::DeserializeArrayHeader(headerPtr, remoteCode, arrayCount);

        if ( true == uni_tools::types::CodeParser::IsFundamental(remoteCode) ||
             false == uni_tools::types::CodeParser::IsArray(remoteCode))
        {
            if (true == m_lastOperationResult)
            {
                m_lastOperationResult = false;
                return;
            }
            throw "Reading array of non fundamental types as something else.";
        }

        uni_tools::types::ArrayHelper<decltype (var)>::Resize(var, arrayCount);
        auto sz = uni_tools::types::ArrayHelper<decltype (var)>::GetSize(var);
        if (sz < arrayCount)
        {
            if (true == m_lastOperationResult)
            {
                m_lastOperationResult = false;
                return;
            }
            throw "Reading non array or non fundamental type as array of fundamentals.";
        }

        typename uni_tools::types::ArrayHelper<T>::Type* elPtr = uni_tools::types::ArrayHelper<T>::FrontMutable(var);
        auto arrEnd = elPtr + arrayCount;
        while(elPtr < arrEnd)
        {
            *this >> (*elPtr);
            elPtr++;
        }
    }


    template<typename T>
    void _clearStringZeroes(T& var)
    {

    }

    /// NOTE : DO NOT USE std::string for binary data container
    /// USE std::vector<uin8_t> for binary serialization.
    /// std::string will strip the tail zeroes if there are any.
    void _clearStringZeroes(std::string& var)
    {
        while(var.empty() == false && var.back() == 0)
        {
            var.pop_back();
        }
    }
};


template<typename T1, typename T2>
iUniStream& operator >>(iUniStream& deserializer, std::pair<T1, T2>& p)
{
    return deserializer >> p.first >> p.second;
}

template<typename KeyType, typename ValueType>
iUniStream& operator >>(iUniStream& deserializer, std::map<KeyType, ValueType>& m)
{
    size_t size = 0;
    if (!(deserializer >> size))
    {
        return deserializer;
    }
    m.clear();
    for (size_t i = 0; i < size; ++i)
    {
        std::pair<KeyType, ValueType> p;
        if (!(deserializer >> p))
        {
            return deserializer;
        }
        m.emplace(std::move(p));
    }
    return deserializer;
}

template<typename KeyType, typename ValueType>
iUniStream& operator >>(iUniStream& deserializer, std::unordered_map<KeyType, ValueType>& m)
{
    size_t size = 0;
    if (!(deserializer >> size))
    {
        return deserializer;
    }
    m.clear();
    for (size_t i = 0; i < size; ++i)
    {
        std::pair<KeyType, ValueType> p;
        if (!(deserializer >> p))
        {
            return deserializer;
        }
        m.emplace(std::move(p));
    }
    return deserializer;
}

template<typename T>
iUniStream& operator <<(iUniStream& deserializer, std::list<T>& l)
{
    size_t size = 0;
    if (!(deserializer >> size))
    {
        return deserializer;
    }
    l.clear();
    for (size_t i = 0; i < size; ++i)
    {
        T e;
        if (!(deserializer >> e))
        {
            return deserializer;
        }
        l.emplace_back(std::move(e));
    }
    return deserializer;
}

template<typename T>
iUniStream& operator >>(iUniStream& deserializer, std::set<T>& s)
{
    size_t size = 0;
    if (!(deserializer >> size))
    {
        return deserializer;
    }
    s.clear();
    for (size_t i = 0; i < size; ++i)
    {
        T e;
        if (!(deserializer >> e))
        {
            return deserializer;
        }
        s.emplace(std::move(e));
    }
    return deserializer;
}

template<typename T>
iUniStream& operator >>(iUniStream& deserializer, std::unordered_set<T>& s)
{
    size_t size = 0;
    if (!(deserializer >> size))
    {
        return deserializer;
    }
    s.clear();
    for (size_t i = 0; i < size; ++i)
    {
        T e;
        if (!(deserializer >> e))
        {
            return deserializer;
        }
        s.emplace(std::move(e));
    }
    return deserializer;
}
//
template<typename T> typename
std::enable_if
<
        false == uni_tools::types::Info<T>::isArray
    &&  false == uni_tools::types::Info<T>::isFundamental
    ,   iUniStream&
>::type
operator>>(iUniStream&, T& var);

#endif // UNISTR_UNI_DESERIALIZER_H
