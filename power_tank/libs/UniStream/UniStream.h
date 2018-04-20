#ifndef UNISTR_UNI_STREAM_H
#define UNISTR_UNI_STREAM_H

#include "UniDeserializer.h"
#include "UniSerializer.h"
#include "UniTypeTools.h"
/*
    (0)         is known type   (0 = unknown, 1 = known)
    (1)         is integral type(0 = non int, 1 = int)
    (2, 3)      type size       (0 = 1, 1 = 2, 2 = 4, 3 = 8) or (1 << n times)
    (4)         is signed       (0 = unsigned, 1 = signed)
    (5)         is array type   (0 = not array, 1 = array)
*/

/*====================================================
*
*   Stream buffer description
*
*   Everything is in little endian format.
*
*   Stream buffer format:
*
*   Header (#headerSize = 4)
*   4 bytes (#bufferSize) - size of buffer including stream header
*   --------
*   Payload (#payloadSize = #bufferSize - 4):
*   1 byte (#itemType) item type
*====================================================*/

using UniSerializer = oUniStream;
using oUniInplace = oUniStream::Inplace;
using UniDeserializer = iUniStream;
using iUniInplace = UniDeserializer::Inplace;

namespace UniStream
{
constexpr uint8_t UserMarker_1 = ::uni_tools::types::_markUserMark_1;
constexpr uint8_t UserMarker_2 = ::uni_tools::types::_markUserMark_2;
}

// This only simulates serialization deserialization
// via std::vector<uin8_t> type
class dUniStream
{

public:
    using StorageType = std::vector<uint8_t>;
    dUniStream() = default;
    dUniStream(std::vector<uint8_t>&& buff)
        : m_buffer(std::move(buff))
    {
    }

    std::vector<uint8_t> GetBuffer()
    {
        return std::move(m_buffer);
    }
    StorageType& GetStorage()
    {
        return m_buffer;
    }

    dUniStream& operator<<(StorageType&& var)
    {
        m_buffer = std::move(var);
        return *this;
    }

    dUniStream& operator>>(StorageType& var)
    {
        var = std::move(m_buffer);
        return *this;
    }

    template <typename MsgType>
    dUniStream& operator<<(std::pair<MsgType, StorageType>&& var)
    {
        auto& rawBuffer = var.second;
        size_t rawBufferSize = rawBuffer.size();

        auto msgBuffer = var.first.Serialize();
        uint32_t msgBufferSz = static_cast<uint32_t>(msgBuffer.size());

        m_buffer = std::move(rawBuffer);

        m_buffer.resize(rawBufferSize + size_t(msgBufferSz) + sizeof(msgBufferSz));

        uint8_t* dst = reinterpret_cast<uint8_t*>(m_buffer.data() + rawBufferSize);
        std::memcpy(dst, msgBuffer.data(), msgBufferSz);

        dst += msgBufferSz;
        const uint8_t* src = reinterpret_cast<const uint8_t*>(&msgBufferSz);
        uni_tools::CopyToBuff<sizeof(uint32_t)>(dst, src);
        return *this;
    }

    template <typename MsgType>
    dUniStream& operator>>(std::pair<MsgType, StorageType>& var)
    {
        uint32_t msgBufferSz = 0;
        uni_tools::CopyFromBuff<sizeof(uint32_t)>(&msgBufferSz,
                                                  m_buffer.data() + m_buffer.size() - sizeof(msgBufferSz));

        m_buffer.resize(m_buffer.size() - sizeof(uint32_t));

        std::vector<uint8_t> msgBuffer(msgBufferSz, 0);
        std::memcpy(msgBuffer.data(), m_buffer.data() + m_buffer.size() - msgBufferSz, msgBufferSz);

        m_buffer.resize(m_buffer.size() - msgBufferSz);
        
        var.first.Deserialize(std::move(msgBuffer));
        
        var.second = std::move(m_buffer);

        return *this;
    }

private:
    StorageType m_buffer;
};

#endif // UNISTR_UNI_STREAM_H
