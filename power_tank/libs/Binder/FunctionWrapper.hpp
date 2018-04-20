#ifndef _FUNCTION_WRAPPER_HPP_
#define _FUNCTION_WRAPPER_HPP_

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "../Std17/any.hpp"
#include "../Std17/function_traits.hpp"
#include "../Std17/optional.hpp"
#include "../Std17/type_traits.hpp"
#include "../Std17/move_on_copy.hpp"

#include "../TaskMgr/TaskMgr.h"
#include "../TaskMgr/TaskFuture.h"
#include "Bindable.h"

namespace detail
{


// Function wrapper used for storage and invoke
template <typename Serializer, typename Deserializer>
struct function_wrapper
{
    struct CallbackInfo
    {
        TaskMgr::Id taskId = 0;
        /// Subscription id
        std::uint64_t id = 0;
        /// Priority used for sorting
        std::uint32_t priority = 0;
        /// Sentinel used for life tracking
        nonstd::optional<Bindable::WeakPtr> sentinel;
        /// The function wrapper
        std::shared_ptr<detail::function_wrapper<Serializer, Deserializer>> function;
    };

    virtual ~function_wrapper() = default;

    virtual Deserializer invoke(Deserializer& params) const = 0;
    
    virtual Deserializer invoke_multithreaded(Deserializer& params, const CallbackInfo& callbackInfo, bool wait) const = 0;    
};

template <typename Serializer, typename Deserializer, typename F>
class function_wrapper_t : public function_wrapper<Serializer, Deserializer>
{
    using CallbackInfo = typename function_wrapper<Serializer, Deserializer>::CallbackInfo;
 
    Deserializer invoke_impl(Deserializer& params, const std::true_type&) const
    {
        typename nonstd::function_traits<F>::arg_types_decayed tuple;
        nonstd::for_each_in_tuple(tuple, [&params](auto& item)
        { 
            if(!(params >> item))
            {
                throw "cannot deserialize argument";
            }
        });
        nonstd::apply(m_function, tuple);
        return {};
    }
    Deserializer invoke_impl(Deserializer& params, const std::false_type&) const
    {
        typename nonstd::function_traits<F>::arg_types_decayed tuple;
        nonstd::for_each_in_tuple(tuple, [&params](auto& item) 
        { 
            if(!(params >> item))
            {
                throw "cannot deserialize argument";
            }
        });
        auto return_value = nonstd::apply(m_function, tuple);

        Serializer serializer;
        serializer << return_value;
        return Deserializer(std::move(serializer));
    }
    
    Deserializer invoke_multithreaded_impl(Deserializer& params, const CallbackInfo& callbackInfo, bool wait, const std::true_type&) const
    {
        typename nonstd::function_traits<F>::arg_types_decayed tuple;
        nonstd::for_each_in_tuple(tuple, [&params](auto& item) 
        {
            if(!(params >> item))
            {
                throw "cannot deserialize argument";
            }

        });

        auto promise = TaskMgr::Promise<R>();
        auto future = promise.GetFuture();
        if(callbackInfo.sentinel.has_value() == true)
        {
            TaskMgr::Invoke(
                callbackInfo.taskId, callbackInfo.sentinel.value(),
                [ function = m_function, args = std::move(tuple), promise = nonstd::monc(promise) ]() mutable
            {
                nonstd::apply(function, args);
                promise.get().Set();
            });
        }
        else
        {
            TaskMgr::Invoke(callbackInfo.taskId, [ function = m_function, args = std::move(tuple),
                            promise = nonstd::monc(promise) ]() mutable
            {
                nonstd::apply(function, args);
                promise.get().Set();
            });
        }
        if(wait)
        {
            future.Wait();
        }

        return {};
    }
    
    Deserializer invoke_multithreaded_impl(Deserializer& params, const CallbackInfo& callbackInfo, bool wait, const std::false_type&) const
    {
        typename nonstd::function_traits<F>::arg_types_decayed tuple;
        nonstd::for_each_in_tuple(tuple, [&params](auto& item) 
        {
            if(!(params >> item))
            {
                throw "cannot deserialize argument";
            }

        });

        
        auto promise = TaskMgr::Promise<R>();
        auto future = promise.GetFuture();        
        if(callbackInfo.sentinel.has_value() == true)
        {
            TaskMgr::Invoke(
                callbackInfo.taskId, callbackInfo.sentinel.value(),
                [ function = m_function, args = std::move(tuple), promise = nonstd::monc(promise) ]() mutable
            {
                promise.get().Set(nonstd::apply(function, args));
            });
        }
        else
        {
            TaskMgr::Invoke(callbackInfo.taskId, [ function = m_function, args = std::move(tuple),
                            promise = nonstd::monc(promise) ]() mutable
            {
                promise.get().Set(nonstd::apply(function, args));
            });
        }
        if(wait)
        {
            Serializer serializer;
            serializer << future.MoveGet();
            return Deserializer(std::move(serializer));
        }
        else
        {
            return {};
        }
    }
    
    
public:
    using delegate_t = std::function<typename nonstd::function_traits<F>::function_type>;
    using is_void = std::integral_constant<bool, std::is_same<void, typename nonstd::function_traits<F>::result_type>::value>;    
    using R = typename std::decay<typename nonstd::fn_result_of<F>>::type;
    
    ~function_wrapper_t() = default;

    template <typename C>
    function_wrapper_t(C* const object_ptr, F&& f)
        : m_function(nonstd::bind_mem_variadic(object_ptr, std::forward<F>(f)))
    {
    }

    function_wrapper_t(F&& f)
        : m_function(f)
    {
    }

    virtual Deserializer invoke(Deserializer& params) const
    {
        return invoke_impl(params, is_void());
    }
        
    virtual Deserializer invoke_multithreaded(Deserializer& params, const CallbackInfo& callbackInfo, bool wait) const
    {
        return invoke_multithreaded_impl(params, callbackInfo, wait, is_void());
    }

private:
    delegate_t m_function;
};

template <typename Serializer, typename Deserializer, typename F>
std::shared_ptr<function_wrapper<Serializer, Deserializer>> make_wrapper(F&& f)
{
    using wrapper = function_wrapper_t<Serializer, Deserializer, F>;
    return std::make_shared<wrapper>(std::forward<F>(f));
}

template <typename Serializer, typename Deserializer, typename C, typename F>
std::shared_ptr<function_wrapper<Serializer, Deserializer>> make_wrapper(C* const object_ptr, F&& f)
{
    using wrapper = function_wrapper_t<Serializer, Deserializer, F>;
    return std::make_shared<wrapper>(object_ptr, std::forward<F>(f));
}

} // namespace detail

#endif
