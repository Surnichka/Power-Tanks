// Signals.h ----------------------------------------------------------------------

#ifndef EGT_GLOBAL_IDS_H
#define EGT_GLOBAL_IDS_H

#include <atomic>
#include <string>
#include "DemangleTypes.h"

template<typename T>
using Id = uint64_t;

namespace Ids
{


namespace internals
{
    struct IdInfo
    {
        std::atomic<uint64_t> *idPtr;
        std::string name;
    };

    static IdInfo info[2048];
    static std::atomic<size_t> infoIdx = {0};

    inline void AddIdInfo(std::atomic<uint64_t>* idPtr, const std::string& idName)
    {
        auto idx = infoIdx.fetch_add(1);
        if (idx >= sizeof (info) / sizeof (IdInfo))
        {
            // assert
            return;
        }
        info[idx] = {idPtr, idName};
    }
}


template<typename T>
inline Id<T> GetFree()
{
    static std::atomic<uint64_t> id = {1};
    static bool isReady = (void(void(internals::AddIdInfo(&id, demangle::TypeName<T>()))), (void)isReady, true);

    return id.fetch_add(1ull);
}


namespace type_id_prv {

inline std::atomic<uint64_t>& _getStaticTypeId()
{
    static std::atomic<uint64_t> s_typeId = {1};
    return s_typeId;
}
}
//
template<typename T>
inline Id<T> _getTypeId()
{
    static uint64_t id = []() -> uint64_t
    {
        return type_id_prv::_getStaticTypeId().fetch_add(1ull);
    }();

    return id;
}

template<typename T>
inline Id<T> GetTypeId()
{
    return _getTypeId<typename std::remove_cv<T>::type>();
}

template<typename T>
inline void Invalidate(Id<T>& id)
{
    id = 0ull;
}

template <typename T>
inline Id<T> GetInvalid()
{
    return 0ull;
}

template<typename T>
inline bool IsValid(const Id<T>& id)
{
    return id > 0ull;
}

}

#endif //EGT_GLOBAL_IDS_H
