#include "UniSerializer.h"
#include "UniDeserializer.h"

oUniStream::oUniStream()
    : m_internalBuff(256)
    , m_buffPtr(&m_internalBuff)
    , m_frontIdx(0)
    , m_endIdx(m_buffPtr->size())
    , m_payloadEndIdx(m_frontIdx)
{
}

oUniStream::oUniStream(oUniStream && rhs)
{
    m_internalBuff = rhs.m_internalBuff;
    m_frontIdx = rhs.m_frontIdx;
    m_endIdx = rhs.m_endIdx;
    m_payloadEndIdx = rhs.m_payloadEndIdx;

    if (rhs.m_buffPtr == &(rhs.m_internalBuff))
    {
        m_buffPtr = &(m_internalBuff);
    }
    else
    {
        m_buffPtr = rhs.m_buffPtr;
    }

    rhs.m_frontIdx = 0;
    rhs.m_endIdx = 0;
    rhs.m_payloadEndIdx = 0;
    rhs.m_buffPtr = nullptr;
}

oUniStream::oUniStream(oUniStream::InplaceMarker, oUniStream &host)
    : m_internalBuff()
    , m_buffPtr(host.m_buffPtr)
    , m_frontIdx(host.m_payloadEndIdx)
    , m_endIdx(host.m_endIdx)
    , m_payloadEndIdx(m_frontIdx)
{
    _getInternalMemory(uni_tools::types::_arrayHeaderSize);
    m_frontIdx += uni_tools::types::_arrayHeaderSize;
}

oUniStream::oUniStream(std::vector<uint8_t>&& buff)
    : m_internalBuff(std::move(buff))
    , m_buffPtr(&m_internalBuff)
    , m_frontIdx(0)
    , m_endIdx(m_buffPtr->size())
    , m_payloadEndIdx(m_endIdx)
{

}

oUniStream &oUniStream::operator=(oUniStream &&rhs)
{
    m_internalBuff = rhs.m_internalBuff;
    m_frontIdx = rhs.m_frontIdx;
    m_endIdx = rhs.m_endIdx;
    m_payloadEndIdx = rhs.m_payloadEndIdx;

    if (rhs.m_buffPtr == &(rhs.m_internalBuff))
    {
        m_buffPtr = &(m_internalBuff);
    }
    else
    {
        m_buffPtr = rhs.m_buffPtr;
    }

    rhs.m_frontIdx = 0;
    rhs.m_endIdx = 0;
    rhs.m_payloadEndIdx = 0;
    rhs.m_buffPtr = nullptr;

    return *this;
}

oUniStream &oUniStream::operator<<(const oUniStream &var)
{
    size_t arrayCount = var.m_payloadEndIdx - var.m_frontIdx;
    size_t streamSize = arrayCount + uni_tools::types::_arrayHeaderSize;
    //
    if (var.m_buffPtr == m_buffPtr)
    {
        uint8_t remoteCode = uni_tools::types::Code___REMOVE<std::vector<uint8_t>>::value | uni_tools::types::_markUserMark_3;
        // flush inplace
        uint8_t* headerStart = m_buffPtr->data() + m_payloadEndIdx;

        serializer_impl::SerializeArrayHeader(headerStart, remoteCode, arrayCount);

        m_endIdx = m_buffPtr->size();
        m_payloadEndIdx = var.m_payloadEndIdx;
    }
    else
    {
        // copy extern stream
        uint8_t *freeMem = _getInternalMemory( streamSize );

        const uint8_t *frontPtr = var.m_buffPtr->data() + var.m_frontIdx;
        serializer_impl::SerializeArrayFundamental(frontPtr, freeMem, arrayCount, uni_tools::types::_markUserMark_3 );
    }

    return *this;
}

oUniStream &oUniStream::operator>>(oUniStream::Inplace&& inplace)
{
    inplace.serializer = oUniStream(InplaceMarker(), *this);
    return *this;
}

void oUniStream::Invalidate()
{
    m_internalBuff.clear();
    m_buffPtr = nullptr;
    m_frontIdx = 0;
    m_endIdx = 0;
    m_payloadEndIdx = 0;
}

void oUniStream::Clear()
{
    *this = oUniStream();
}

std::vector<uint8_t> &oUniStream::GetBuffer()
{
    size_t payloadSize = m_payloadEndIdx - m_frontIdx;
    if (m_frontIdx != 0)
    {
        // we are inplaced
        m_inplacedBufferCopy.resize( payloadSize );
        memcpy(m_inplacedBufferCopy.data(), m_buffPtr->data() + m_frontIdx, payloadSize);
        return m_inplacedBufferCopy;
    }

    m_buffPtr->resize( payloadSize );
    m_endIdx = m_buffPtr->size();
    return *m_buffPtr;
}


oUniStream &oUniStream::operator<<(const iUniStream &var)
{
    return *this << var.GetBuffer();
}
