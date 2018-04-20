#ifndef _GENERIC_MSG_H_
#define _GENERIC_MSG_H_

#include <string>
#include <vector>
#include <map>

template<typename Serializer, typename Deserializer>
struct GenericMsgPrivateAccess;

template<typename Serializer, typename Deserializer>
struct GenericMsg
{
    using ValueContainer = std::map<std::string, typename Serializer::StorageType>;
    //
    std::string             name;

    // Deprecated. Please do not use.
    std::string             description;
    int64_t                 gameId;
    //
    //
    std::vector<std::string> GetValueNames() const;
    //
    template<typename T>
    void AddValue(const std::string& valueName, T&& value);
    //
    template<typename ...Args>
    void AddValue(const std::string& valueName, Args&&... values);
    //
    template<typename T>
    bool GetValue(const std::string& valueName, T& value) const;
    //
    bool HasValue(const std::string& valueName) const;
    //
    template<typename T>
    bool HasValue(const std::string& valueName) const;
    //
    std::vector<uint8_t> Serialize() const;
    bool Deserialize( std::vector<uint8_t>&& buff );
    //
    const typename Serializer::StorageType& GetRawValue(const std::string& valueName) const;
    void SetRawValue(const std::string& valueName, typename Serializer::StorageType rawValue);
    //
    const ValueContainer& GetValueContainer() const;
    ValueContainer& GetMutableValueContainer();
    //
    friend struct GenericMsgPrivateAccess<Serializer, Deserializer>;
private:

    ValueContainer m_values;
};

template<typename Serializer, typename Deserializer>
struct GenericMsgPrivateAccess
{
    using Msg = GenericMsg<Serializer, Deserializer>;
    static typename Msg::ValueContainer& GetValues(Msg& msg) { return msg.m_values; }
    static const typename Msg::ValueContainer& GetValues(const Msg& msg) { return msg.m_values; }
};

#include "GenericMsg.hpp"
#endif // _GENERIC_MSG_H_
