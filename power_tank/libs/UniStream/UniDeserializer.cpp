#include "UniDeserializer.h"
#include "UniSerializer.h"

iUniStream::iUniStream()
    : m_internalBuff()
    , m_buffPtr(nullptr)
    , m_frontIdx(0)
    , m_endIdx(0)
    , m_payloadEndIdx(0)
    , m_lastOperationResult(false)
{

}

iUniStream::iUniStream(const std::vector<uint8_t> &buff)
    : m_internalBuff()
    , m_buffPtr(&buff)
    , m_frontIdx(0)
    , m_endIdx(m_buffPtr->size())
    , m_payloadEndIdx(m_frontIdx)
    , m_lastOperationResult(true)
{

}

iUniStream::iUniStream(std::vector<uint8_t> &buff)
    : iUniStream(std::move(buff))
{

}

iUniStream::iUniStream(std::vector<uint8_t> &&buff)
    : m_internalBuff(std::move(buff))
    , m_buffPtr(&m_internalBuff)
    , m_frontIdx(0)
    , m_endIdx(m_buffPtr->size())
    , m_payloadEndIdx(m_frontIdx)
    , m_lastOperationResult(true)
{

}

iUniStream::iUniStream(oUniStream &serializer)
    : iUniStream(std::move(serializer.GetBuffer()))
{
    serializer.Invalidate();
}

iUniStream::iUniStream(oUniStream &&serializer)
    : iUniStream(std::move(serializer.GetBuffer()))
{
    serializer.Invalidate();
}

iUniStream::iUniStream(iUniStream &&rhs)
{
    m_internalBuff = std::move(rhs.m_internalBuff);
    m_frontIdx = rhs.m_frontIdx;
    m_endIdx = rhs.m_endIdx;
    m_payloadEndIdx = rhs.m_payloadEndIdx;
    m_lastOperationResult = rhs.m_lastOperationResult;

    if (rhs.m_buffPtr == &(rhs.m_internalBuff))
    {
        m_buffPtr = &(m_internalBuff);
    }
    else
    {
        m_buffPtr = rhs.m_buffPtr;
    }

    rhs.m_buffPtr = nullptr;
    rhs.m_frontIdx = 0;
    rhs.m_endIdx = 0;
    rhs.m_payloadEndIdx = 0;
    rhs.m_lastOperationResult = false;
}

iUniStream &iUniStream::operator=(iUniStream &&rhs)
{
    m_internalBuff = std::move(rhs.m_internalBuff);
    m_frontIdx = rhs.m_frontIdx;
    m_endIdx = rhs.m_endIdx;
    m_payloadEndIdx = rhs.m_payloadEndIdx;
    m_lastOperationResult = rhs.m_lastOperationResult;

    if (rhs.m_buffPtr == &(rhs.m_internalBuff))
    {
        m_buffPtr = &(m_internalBuff);
    }
    else
    {
        m_buffPtr = rhs.m_buffPtr;
    }

    rhs.m_buffPtr = nullptr;
    rhs.m_frontIdx = 0;
    rhs.m_endIdx = 0;
    rhs.m_payloadEndIdx = 0;
    rhs.m_lastOperationResult = false;
    return *this;
}

void iUniStream::Clear()
{
    *this = iUniStream();
}

iUniStream &iUniStream::operator>>(iUniStream::Inplace &&inplace)
{
    const uint8_t *headerPtr = _consumeInternalMemory( uni_tools::types::_arrayHeaderSize );
    if (nullptr == headerPtr)
    {
        inplace.deserializer = iUniStream();
        return *this;
    }
    uint8_t remoteCode;
    size_t arrayCount;

    deserializer_impl::DeserializeArrayHeader(headerPtr, remoteCode, arrayCount);
    remoteCode = remoteCode & (~uni_tools::types::_userDefinedMask);
    //
    if (true == uni_tools::types::Checker<std::vector<uint8_t>>::CastNeeded(remoteCode))
    {
        inplace.deserializer = iUniStream();
        if (true == m_lastOperationResult)
        {
            m_lastOperationResult = false;
            return *this;
        }
        throw "Implacing deserializer from non compatible type.";
    }

    size_t inplacedFrontIdx = m_payloadEndIdx;
    size_t inplacedEndIdx = inplacedFrontIdx + arrayCount;
    size_t inplacedPayloadEndIdx = inplacedFrontIdx;

    if (nullptr == _consumeInternalMemory( arrayCount ))
    {
        inplace.deserializer = iUniStream();
        if (true == m_lastOperationResult)
        {
            m_lastOperationResult = false;
            return *this;
        }
        throw "Implacing deserializer from broken stream.";
    }

    auto& inplaced = inplace.deserializer;
    //
    //inplaced.m_internalBuff = m_internalBuff;
    inplaced.m_buffPtr = m_buffPtr;
    inplaced.m_frontIdx = inplacedFrontIdx;
    inplaced.m_endIdx = inplacedEndIdx;
    inplaced.m_payloadEndIdx = inplacedPayloadEndIdx;
    inplaced.m_lastOperationResult = true;
    return *this;
}


std::vector<uint8_t> iUniStream::GetBuffer() const
{
    if (m_buffPtr == nullptr || m_buffPtr->empty())
    {
        return {};
    }
    if (m_endIdx <= m_payloadEndIdx)
    {
        return {};
    }
    if (m_payloadEndIdx >= m_buffPtr->size() || m_endIdx > m_buffPtr->size())
    {
        return {};
    }

    std::vector<uint8_t> res(m_endIdx - m_payloadEndIdx);

    memcpy(res.data(), m_buffPtr->data() + m_payloadEndIdx, res.size());
    return res;
}


iUniStream &iUniStream::operator>>(iUniStream &var)
{
    std::vector<uint8_t> buff;
    if (!(*this >> buff))
    {
        var = iUniStream();
    }
    else
    {
        var = iUniStream(std::move(buff));
    }
    return *this;
}

iUniStream &iUniStream::operator>>(oUniStream &var)
{
    std::vector<uint8_t> buff;
    if (!(*this >> buff))
    {
        var = oUniStream();
    }
    else
    {
        var = oUniStream(std::move(buff));
    }
    return *this;
}
