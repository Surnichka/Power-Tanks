#ifndef GENERIC_FUNCTION_BINDER_HPP
#define GENERIC_FUNCTION_BINDER_HPP
#include "GenericFunctionBinder.h"


template<typename Serializer, typename Deserializer>
template<typename F>
inline std::uint64_t GenericFunctionBinder<Serializer, Deserializer>::ConnectSlot(
        const std::string& signalName, Bindable::WeakPtr sentinel, F&& slot, std::uint32_t priority)
{
    static_assert(std::is_same<void, typename nonstd::function_traits<F>::result_type>::value,
                  "signals cannot have a return type different from void");
    
    auto id = Ids::GetFree<CallbackInfo>();
    CallbackInfo info;
    info.taskId = TaskMgr::GetId();
    info.id = id;
    info.priority = priority;
    if(sentinel.expired() == false)
    {
        info.sentinel = sentinel;
    }
    info.function = detail::make_wrapper<Serializer, Deserializer>(std::forward<F>(slot));

    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard, [ this, signalName, info = std::move(info) ]() 
    {
        auto& container = m_signals[signalName];
        container.emplace_back(std::move(info));
        Sort(container);
    });

    return id;
}


template<typename Serializer, typename Deserializer>
template<typename C, typename F>
inline std::uint64_t GenericFunctionBinder<Serializer, Deserializer>::ConnectSlot(
        const std::string& signalName, Bindable::WeakPtr sentinel, C* const object,
                          F&& slot, std::uint32_t priority)
{
    static_assert(std::is_same<void, typename nonstd::function_traits<F>::result_type>::value,
                  "signals cannot have a return type different from void");

    auto id = Ids::GetFree<CallbackInfo>();
    CallbackInfo info;
    info.taskId = TaskMgr::GetId();
    info.id = id;
    info.priority = priority;
    if(sentinel.expired() == false)
    {
        info.sentinel = sentinel;
    }
    
    info.function = detail::make_wrapper<Serializer, Deserializer>(object, std::forward<F>(slot));

    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard, [ this, signalName, info = std::move(info) ]() 
    {
        auto& container = m_signals[signalName];
        container.emplace_back(std::move(info));
        Sort(container);
    });

    return id;
}


template<typename Serializer, typename Deserializer>
template<typename F>
inline std::uint64_t GenericFunctionBinder<Serializer, Deserializer>::ConnectSlot(
        const std::string& signalName, F&& slot, std::uint32_t priority)
{
    return ConnectSlot(signalName, Bindable::WeakPtr(), std::forward<F>(slot), priority);
}

template<typename Serializer, typename Deserializer>
template<typename C, typename F, typename std::enable_if<std::is_base_of<Bindable, C>::value>::type*>
inline std::uint64_t GenericFunctionBinder<Serializer, Deserializer>::ConnectSlot(
        const std::string& signalName, C* const object, F&& slot, std::uint32_t priority)
{
    auto bindable = static_cast<Bindable*>(object);
    auto sentinel = bindable->GetWeakPtr();
    return ConnectSlot(signalName, sentinel, object, std::forward<F>(slot), priority);
}

template<typename Serializer, typename Deserializer>
template<typename C, typename F, typename std::enable_if<!std::is_base_of<Bindable, C>::value>::type*>
inline std::uint64_t GenericFunctionBinder<Serializer, Deserializer>::ConnectSlot(
        const std::string& signalName, C* const object, F&& slot, std::uint32_t priority)
{
    return ConnectSlot(signalName, Bindable::WeakPtr(), object, std::forward<F>(slot), priority);
}

template<typename Serializer, typename Deserializer>
inline void GenericFunctionBinder<Serializer, Deserializer>::DisconnectSlot(
        const std::string &signalName, uint64_t slotId)
{
    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard, [this, signalName, slotId]() 
    {
        auto& container = m_signals[signalName];
        
        auto it = std::find_if(std::begin(container), std::end(container),
                               [&slotId](const CallbackInfo& info) { return info.id == slotId; });
        
        if(it != std::end(container))
        {
            auto& info = *it;
            info.sentinel = Bindable::WeakPtr();
        }
    });
}


template<typename Serializer, typename Deserializer>
template <typename... Args>
inline void GenericFunctionBinder<Serializer, Deserializer>::DispatchSignal(
        const std::string& signalName, Args&&... args) const
{
    Serializer serialziedArgs;
    serialziedArgs.Serialize(std::forward<Args>(args)...);
    auto& buffer = serialziedArgs.GetStorage();

    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard,
    [ this, signalName, bufferRvalue = std::move(buffer) ]() mutable
    {
        const auto taskId = TaskMgr::GetId();
        bool collectGarbage = false;
    
        const auto& buffer = bufferRvalue;
    
        auto& container = m_signals[signalName];
        // Iterate this way to allow addition
        for(size_t i = 0; i < container.size(); ++i)
        {
            const auto& info = container[i];
            Bindable::Ptr sentinel = nullptr;
            if(info.sentinel.has_value())
            {
                sentinel = info.sentinel.value().lock();
                if(nullptr == sentinel)
                {
                    collectGarbage = true;
                }
                if(nullptr == sentinel || sentinel->IsPaused())
                {
                    continue;
                }
            }
            Deserializer params(buffer);
    
            if(info.taskId == taskId)
            {
                info.function->invoke(params);
            }
            else
            {
                info.function->invoke_multithreaded(params, info, false);
            }
        }
    
        if(collectGarbage)
        {
            ClearGarbage(container);
            // If it was the last one, remove the whole container
            if(container.empty())
                m_signals.erase(signalName);
        }
    });
}

template<typename Serializer, typename Deserializer>
inline bool GenericFunctionBinder<Serializer, Deserializer>::HasBindedFunction(const std::string& functionName) const
{
    auto promise = TaskMgr::Promise<bool>();
    auto future = promise.GetFuture();

    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard,
    [ this, functionName, promise = nonstd::monc(promise) ]() mutable
    {
        auto& info = m_functions[functionName];

        Bindable::Ptr sentinel = nullptr;
        if(info.sentinel.has_value())
        {
            sentinel = info.sentinel.value().lock();
            if(nullptr == sentinel)
            {
                m_functions.erase(functionName);
                promise.get().Set(false);
                return;
            }
            if(nullptr == sentinel)
            {
                promise.get().Set(false);
                return;
            }
        }
        promise.get().Set(true);
    });
    return future.CopyGet();
}

template<typename Serializer, typename Deserializer>
template <typename F>
inline void GenericFunctionBinder<Serializer, Deserializer>::BindFunction(
        const std::string& functionName, F&& function)
{
    auto id = Ids::GetFree<CallbackInfo>();
    CallbackInfo info;
    info.taskId = TaskMgr::GetId();
    info.id = id;
    info.function = detail::make_wrapper<Serializer, Deserializer>(std::forward<F>(function));

    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard, [ this, functionName, info = std::move(info) ]() 
    {
        m_functions.emplace(functionName, std::move(info));
    });
}

template<typename Serializer, typename Deserializer>
template <typename F>
inline void GenericFunctionBinder<Serializer, Deserializer>::BindFunction(
        const std::string& functionName, Bindable::WeakPtr sentinel, F&& function)
{    
    auto id = Ids::GetFree<CallbackInfo>();
    CallbackInfo info;
    info.taskId = TaskMgr::GetId();
    info.id = id;
    info.sentinel = sentinel;
    info.function = detail::make_wrapper<Serializer, Deserializer>(std::forward<F>(function));

    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard, [ this, functionName, info = std::move(info) ]() 
    {
        m_functions.emplace(functionName, std::move(info));
    });
}

template<typename Serializer, typename Deserializer>
template <typename C, typename F>
inline void GenericFunctionBinder<Serializer, Deserializer>::BindFunction(
        const std::string& functionName, Bindable::WeakPtr sentinel, C* const object, F&& function)
{
    auto id = Ids::GetFree<CallbackInfo>();
    CallbackInfo info;
    info.taskId = TaskMgr::GetId();
    info.id = id;
    info.sentinel = sentinel;
    info.function = detail::make_wrapper<Serializer, Deserializer>(object, std::forward<F>(function));

    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard, [ this, functionName, info = std::move(info) ]() 
    {
        m_functions.emplace(functionName, std::move(info));
    });
}

template<typename Serializer, typename Deserializer>
template <typename C, typename F>
inline void GenericFunctionBinder<Serializer, Deserializer>::BindFunction(
        const std::string& functionName, C* const object, F&& function)
{
    auto id = Ids::GetFree<CallbackInfo>();
    CallbackInfo info;
    info.taskId = TaskMgr::GetId();
    info.id = id;
    info.function = detail::make_wrapper<Serializer, Deserializer>(object, std::forward<F>(function));

    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard, [ this, functionName, info = std::move(info) ]() 
    {
        m_functions.emplace(functionName, std::move(info));
    });
}


template<typename Serializer, typename Deserializer>
inline void GenericFunctionBinder<Serializer, Deserializer>::UnbindFunction(
        const std::string &functionName)
{
    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard, [ this, functionName]() 
    {
        m_functions.erase(functionName);
    });
}

template<typename Serializer, typename Deserializer>
template <typename R, typename... Args,
          typename std::enable_if<!std::is_same<R, void>::value>::type*>
inline R GenericFunctionBinder<Serializer, Deserializer>::CallFunction(
        const std::string& functionName, Args&&... args) const
{
    static_assert(!std::is_reference<R>::value, 
                  "Trying to get a reference to a temporary deserialized object.");
    
    using ReturnValue = typename std::decay<R>::type;
    
    Serializer serialziedArgs;
    serialziedArgs.Serialize(std::forward<Args>(args)...);
    auto& buffer = serialziedArgs.GetStorage();
    
    auto promise = TaskMgr::Promise<ReturnValue>();
    auto future = promise.GetFuture();
    
    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard,
    [ this, functionName, bufferRvalue = std::move(buffer), promise = nonstd::monc(promise) ]() mutable
    {
        const auto taskId = TaskMgr::GetId();
    
        const auto& buffer = bufferRvalue;
    
        auto& info = m_functions[functionName];

        Bindable::Ptr sentinel = nullptr;
        if(info.sentinel.has_value())
        {
            sentinel = info.sentinel.value().lock();
            if(nullptr == sentinel)
            {
                m_functions.erase(functionName);
                promise.get().Set({});
                return;
            }
            if(nullptr == sentinel)
            {
                promise.get().Set({});
                return;
            }
        }
        Deserializer params(buffer);

        if(info.taskId == taskId)
        {
            Deserializer result = info.function->invoke(params);
            ReturnValue res;
            result >> res;
            promise.get().Set(std::move(res));
        }
        else
        {
            Deserializer result = info.function->invoke_multithreaded(params, info, true);
            ReturnValue res;
            result >> res;
            promise.get().Set(std::move(res));
        }
        
    });
    
    return future.MoveGet();

}

template<typename Serializer, typename Deserializer>
template <typename R, typename... Args,
          typename std::enable_if<std::is_same<R, void>::value>::type*>
inline R GenericFunctionBinder<Serializer, Deserializer>::CallFunction(
        const std::string& functionName, Args&&... args) const
{
    using ReturnValue = typename std::decay<R>::type;
    
    Serializer serialziedArgs;
    serialziedArgs.Serialize(std::forward<Args>(args)...);
    auto& buffer = serialziedArgs.GetStorage();
    
    auto promise = TaskMgr::Promise<ReturnValue>();
    auto future = promise.GetFuture();
    TaskMgr::RunOrInvoke(m_threadId, m_lifetimeGuard,
    [ this, functionName, bufferRvalue = std::move(buffer), promise = nonstd::monc(promise) ]() mutable
    {
        const auto taskId = TaskMgr::GetId();
    
        const auto& buffer = bufferRvalue;
    
        auto& info = m_functions[functionName];

        Bindable::Ptr sentinel = nullptr;
        if(info.sentinel.has_value())
        {
            sentinel = info.sentinel.value().lock();
            if(nullptr == sentinel)
            {
                m_functions.erase(functionName);
            }
            if(nullptr == sentinel)
            {
                promise.get().Set();
                return;
            }
        }
        Deserializer params(buffer);

        if(info.taskId == taskId)
        {
            info.function->invoke(params);
        }
        else
        {
            info.function->invoke_multithreaded(params, info, true);
        }
        promise.get().Set();
        
    });
    
    future.Wait();
}


template<typename Serializer, typename Deserializer>
inline void GenericFunctionBinder<Serializer, Deserializer>::Sort(
        std::vector<GenericFunctionBinder::CallbackInfo> &container)
{
    std::sort(std::begin(container), std::end(container),
              [](const auto& info1, const auto& info2) { return info1.priority > info2.priority; });
}

template<typename Serializer, typename Deserializer>
inline void GenericFunctionBinder<Serializer, Deserializer>::ClearGarbage(
        std::vector<GenericFunctionBinder::CallbackInfo> &container)
{
    // remove expired subscribers
    container.erase(std::remove_if(std::begin(container), std::end(container),
                                   [](const auto& info) 
    {
        return info.sentinel.has_value() &&
                info.sentinel.value().expired();
    }),
                    std::end(container));
}

template<typename Serializer, typename Deserializer>
inline std::shared_ptr<void> GenericFunctionBinder<Serializer, Deserializer>::InitLifeguard()
{
    TaskMgr::RegisterThisThread();
    auto var = std::make_shared<bool>();
    return var;
}
#endif
