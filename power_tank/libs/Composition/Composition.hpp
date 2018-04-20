#pragma once
//

#include <vector>
#include <set>
#include <map>
#include <cstdint>
#include <functional>
#include <atomic>
#include <memory>
#include <limits>
#include <algorithm>
#include <type_traits>

template <typename BaseObjType, size_t Size>
struct CompositionPrv;

template<typename BaseObjType, size_t Size = 32>
class Composition
{
public:
    //
    using BaseType = BaseObjType;
    using Ptr = std::shared_ptr<BaseType>;
    using WeakPtr = std::weak_ptr<BaseType>;
    static constexpr size_t CompositionSize = Size;

    Composition() = default;
    virtual ~Composition();

    Composition(const Composition& src);
    Composition& operator=(const Composition& src);
    //
    Composition(Composition&&);
    Composition& operator=(Composition&&);
    //
    template<typename T, typename ... Args>
    T& Add(Args&&...args);

    template<typename T>
    bool Has() const;

    template<typename ... Args>
    bool HasAll();

    template<typename ... Args>
    bool HasAny();

    template<typename T>
    T& Get();

    template<typename T>
    const T& ConstGet() const;

    template<typename T>
    void Remove();

    static std::shared_ptr<BaseObjType> Create()
    {
        return std::make_shared<BaseObjType>();
    }

private:
    using Private = CompositionPrv<BaseObjType, Size>;

    std::array<std::vector<uint8_t>, Size> m_mem;
    friend struct CompositionPrv<BaseObjType, Size>;
};

template <typename BaseObjType, size_t Size>
struct CompositionPrv
{
private:
    struct ComponentInfo
    {
        std::function<void(Composition<BaseObjType, Size>&)> dtorFunc;
        std::function<void(const Composition<BaseObjType, Size>&, Composition<BaseObjType, Size>&)> copyFunc;
    };

    static constexpr size_t INVALID_ID = std::numeric_limits<size_t>::max();

    using InfoArray = std::array<ComponentInfo, Size>;

    template<typename T>
    static size_t _getTypeIdx(bool createNew = true);

    template<typename T>
    static size_t _initType();

    template<typename T>
    static const typename std::decay<T>::type* _alignPointer(const uint8_t* ptr);

    template<typename T>
    static typename std::decay<T>::type* _alignPointer(uint8_t* ptr);

    static std::size_t _getFreeTypeId();

    static InfoArray& _getInfos();

    static bool _isEmpty(size_t idx);

    static void _clear(Composition<BaseObjType, Size>& obj);


    static bool _checkAllComponent(const Composition<BaseObjType, Size>&);
    template<typename T>
    static bool _checkAllComponent(const Composition<BaseObjType, Size>& obj);
    template<typename T1, typename T2, typename ... Args>
    static bool _checkAllComponent(const Composition<BaseObjType, Size>& obj);

    static bool _checkOneComponent(const Composition<BaseObjType, Size>&);
    template<typename T>
    static bool _checkOneComponent(const Composition<BaseObjType, Size>& obj);
    template<typename T1, typename T2, typename ... Args>
    static bool _checkOneComponent(const Composition<BaseObjType, Size>& obj);

    friend class Composition<BaseObjType, Size>;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

template <typename BaseObjType, size_t Size>
inline Composition<BaseObjType, Size>::~Composition()
{
    Private::_clear(*this);
}

/////////////////////////////////////////////////////////////////////
template <typename BaseObjType, size_t Size>
inline Composition<BaseObjType, Size>::Composition(const Composition& src)
{
    Private::_clear(*this);
    auto& infos = Private::_getInfos();
    for (size_t i = 0; i < src.m_mem.size(); ++i)
    {
        if (Private::_isEmpty(i))
        {
            break;
        }
        infos[i].copyFunc(src, *this);
    }
}

/////////////////////////////////////////////////////////////////////
template <typename BaseObjType, size_t Size>
inline Composition<BaseObjType, Size>& Composition<BaseObjType, Size>::operator=(const Composition& src)
{
    if (&src == this)
    {
        return *this;
    }

    Private::_clear(*this);

    auto& infos = Private::_getInfos();
    for (size_t i = 0; i < src.m_mem.size(); ++i)
    {
        if (Private::_isEmpty(i))
        {
            break;
        }
        infos[i].copyFunc(src, *this);
    }
    return *this;
}

/////////////////////////////////////////////////////////////////////
template <typename BaseObjType, size_t Size>
inline Composition<BaseObjType, Size>::Composition(Composition&& src) = default;

/////////////////////////////////////////////////////////////////////
template <typename BaseObjType, size_t Size>
inline Composition<BaseObjType, Size>& Composition<BaseObjType, Size>::operator=(Composition&& src)
{
    if (&src == this)
    {
        return *this;
    }

    Private::_clear(*this);
    m_mem = std::move(src.m_mem);
    return *this;
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T, typename ... Args>
inline T& Composition<BaseObjType, Size>::Add(Args&&...args)
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_same<T, ValueType>::value,
                      "Requires decayed value type!");

    static auto s_idx = Private::template _getTypeIdx<ValueType>(true);
    auto idx = s_idx;

    Private::_getInfos()[idx].dtorFunc(*this);
    m_mem[idx].resize(sizeof(ValueType) + alignof (ValueType));

    auto ptr = &m_mem[idx].front();
    auto alignedPtr = Private::template _alignPointer<ValueType>(ptr);
    new ( alignedPtr ) ValueType( std::forward<Args>(args)... );
    return *alignedPtr;
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T>
inline bool Composition<BaseObjType, Size>::Has() const
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_same<T, ValueType>::value,
                      "Requires decayed value type!");

    auto idx = Private::template _getTypeIdx<ValueType>(false);
    if (idx == Private::INVALID_ID)
    {
        return false;
    }

    return m_mem[idx].empty() == false;
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename ... Args>
inline bool Composition<BaseObjType, Size>::HasAll()
{
    return Private::template _checkAllComponent<Args...>(*this);
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename ... Args>
inline bool Composition<BaseObjType, Size>::HasAny()
{
    return Private::template _checkOneComponent<Args...>(*this);
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T>
inline T& Composition<BaseObjType, Size>::Get()
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_same<T, ValueType>::value,
                      "Requires decayed value type!");

    static auto s_idx = Private::template _getTypeIdx<ValueType>(true);
    auto idx = s_idx;
    if (m_mem[idx].empty() == false)
    {
        auto ptr = &m_mem[idx].front();
        auto alignedPtr = Private::template _alignPointer<ValueType>(ptr);
        return *alignedPtr;
    }
    //
    return Add<ValueType>();
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T>
inline const T& Composition<BaseObjType, Size>::ConstGet() const
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_same<T, ValueType>::value,
                      "Requires decayed value type!");

    const static ValueType empty = {};

    auto idx = Private::template _getTypeIdx<ValueType>(false);
    if (idx == Private::INVALID_ID)
    {
        return empty;
    }

    if(m_mem[idx].empty() == false)
    {
        auto ptr = &m_mem[idx].front();
        auto alignedPtr = Private::template _alignPointer<ValueType>(ptr);
        return *alignedPtr;
    }
    //
    return empty;
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T>
inline void Composition<BaseObjType, Size>::Remove()
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_same<T, ValueType>::value,
                      "Requires decayed value type!");

    auto idx = Private::template _getTypeIdx<ValueType>(false);
    if (Private::_isEmpty(idx))
    {
        return;
    }
    if (m_mem[idx].empty() == false)
    {
        auto& info = Private::_getInfos()[idx];
        info.dtorFunc(*this);
        m_mem[idx].clear();
    }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

template<typename BaseObjType, size_t Size>
template<typename T>
inline size_t CompositionPrv<BaseObjType, Size>::_getTypeIdx(bool createNew)
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_same<T, ValueType>::value,
                      "Requires decayed value type!");

    static size_t s_myId = INVALID_ID;

    static std::atomic<bool> s_inited = {false};

    if (createNew == false)
    {
        return (s_inited == true) ? s_myId : INVALID_ID;
    }

    static bool runOnce = (s_myId = _initType<T>(),      
                           s_inited.store(true),
                           true);
    (void)runOnce;
    return s_myId;
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T>
inline size_t CompositionPrv<BaseObjType, Size>::_initType()
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_same<T, ValueType>::value,
                      "Requires decayed value type!");

    auto id = _getFreeTypeId();
    //
    auto& infos = _getInfos();
    if(id >= infos.size())
    {
        throw " O NEEE, I should increase Composition size :(";
    }

    auto& info = infos[id];
    info.dtorFunc = [id](Composition<BaseObjType, Size>& go)
    {
        if (go.m_mem[id].empty() == true)
        {
            return;
        }
        auto ptr = &go.m_mem[id].front();
        auto& value = *_alignPointer<ValueType>(ptr);
        value.~ValueType();
    };
    info.copyFunc = [id](const Composition<BaseObjType, Size>& src, Composition<BaseObjType, Size>& dst)
    {
        //
        if (src.m_mem[id].empty() == true)
        {
            return;
        }
        //
        dst.m_mem[id].resize(sizeof(ValueType) + alignof (ValueType));
        //
        const auto& alignedSrc = *_alignPointer<ValueType>(&src.m_mem[id].front());
        auto alignedDstPtr = _alignPointer<ValueType>(&dst.m_mem[id].front());
        new ( alignedDstPtr ) ValueType( alignedSrc );
    };
    //
    return id;
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T>
inline const typename std::decay<T>::type* CompositionPrv<BaseObjType, Size>::_alignPointer(const uint8_t* ptr)
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_same<T, ValueType>::value,
                      "Requires decayed value type!");

    return (ValueType*)((uintptr_t(ptr) + alignof(ValueType) - 1) & (~(alignof(ValueType) - 1)));
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T>
inline typename std::decay<T>::type* CompositionPrv<BaseObjType, Size>::_alignPointer(uint8_t* ptr)
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_same<T, ValueType>::value,
                      "Requires decayed value type!");

    return (ValueType*)((uintptr_t(ptr) + alignof(ValueType) - 1) & (~(alignof(ValueType) - 1)));
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
inline std::size_t CompositionPrv<BaseObjType, Size>::_getFreeTypeId()
{
    static std::atomic<size_t> s_freeId = {0};
    return s_freeId.fetch_add(1);
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
inline typename CompositionPrv<BaseObjType, Size>::InfoArray& CompositionPrv<BaseObjType, Size>::_getInfos()
{
    static InfoArray s_info;
    return s_info;
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
bool CompositionPrv<BaseObjType, Size>::_isEmpty(size_t idx)
{
    if (idx >= Size)
    {
        return true;
    }
    return _getInfos()[idx].dtorFunc == nullptr;
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
inline void CompositionPrv<BaseObjType, Size>::_clear(Composition<BaseObjType, Size>& obj)
{
    auto& infos = _getInfos();
    for (size_t i = 0; i < obj.m_mem.size(); ++i)
    {
        if (_isEmpty(i))
        {
            break;
        }
        infos[i].dtorFunc(obj);
    }
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
inline bool CompositionPrv<BaseObjType, Size>::_checkAllComponent(const Composition<BaseObjType, Size>&)
{
    return true;
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T>
inline bool CompositionPrv<BaseObjType, Size>::_checkAllComponent(const Composition<BaseObjType, Size>& obj)
{
    return obj.template Has<T>();
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T1, typename T2, typename ... Args>
inline bool CompositionPrv<BaseObjType, Size>::_checkAllComponent(const Composition<BaseObjType, Size>& obj)
{
    if (obj.template Has<T1>() == false)
    {
        return false;
    }
    return _checkAllComponent<T2, Args...>(obj);
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
inline bool CompositionPrv<BaseObjType, Size>::_checkOneComponent(const Composition<BaseObjType, Size>&)
{
    return false;
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T>
inline bool CompositionPrv<BaseObjType, Size>::_checkOneComponent(const Composition<BaseObjType, Size>& obj)
{
    return obj.template Has<T>();
}

/////////////////////////////////////////////////////////////////////
template<typename BaseObjType, size_t Size>
template<typename T1, typename T2, typename ... Args>
inline bool CompositionPrv<BaseObjType, Size>::_checkOneComponent(const Composition<BaseObjType, Size>& obj)
{
    if (obj.template Has<T1>() == true)
    {
        return true;
    }
    return _checkOneComponent<T2, Args...>(obj);
}
