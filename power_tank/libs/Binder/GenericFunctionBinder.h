#ifndef GENERIC_FUNCTION_BINDER_H
#define GENERIC_FUNCTION_BINDER_H

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

#include "../TaskMgr/TaskMgr.h"
#include "../TaskMgr/TaskFuture.h"
#include "GlobalIds.h"
#include "Bindable.h"
#include "FunctionWrapper.hpp"


namespace slots
{
constexpr static std::uint32_t LOW_PRIORITY = 32;
constexpr static std::uint32_t NORMAL_PRIORITY = 64;
constexpr static std::uint32_t HIGH_PRIORITY = 96;
}



template <typename Serializer, typename Deserializer>
class GenericFunctionBinder
{
public:
    //-----------------------------------------------------------------------------
    /// Connects a slot to a signal.
    /// 'sentiel' - a weak_ptr for lifetime tracking and automatic disconnect.
    /// 'slot' - any void returning functor.
    /// 'priority' - with great value comes great priority.
    /// return - the id of the subscribed slot.
    //-----------------------------------------------------------------------------
    template <typename F>
    std::uint64_t ConnectSlot(const std::string& signalName, Bindable::WeakPtr sentinel, F&& slot,
                              std::uint32_t priority = slots::NORMAL_PRIORITY);

    //-----------------------------------------------------------------------------
    /// Connects a slot to a signal.
    /// 'sentiel' - a weak_ptr for lifetime tracking and automatic disconnect.
    /// 'object' - the object pointer to which the function belongs.
    /// 'slot' - any void returning member function.
    /// 'priority' - with great value comes great priority.
    /// return - the id of the subscribed slot.
    //-----------------------------------------------------------------------------
    template <typename C, typename F>
    std::uint64_t ConnectSlot(const std::string& signalName, Bindable::WeakPtr sentinel, C* const object,
                              F&& slot, std::uint32_t priority = slots::NORMAL_PRIORITY);
    
    //-----------------------------------------------------------------------------
    /// Connects a slot to a signal.
    /// 'object' - the object pointer to which the function belongs.
    /// 'slot' - any void returning member function.
    /// 'priority' - with great value comes great priority.
    /// return - the id of the subscribed slot.
    //-----------------------------------------------------------------------------
    template <typename F>
    std::uint64_t ConnectSlot(const std::string& signalName, F&& slot, 
                              std::uint32_t priority = slots::NORMAL_PRIORITY);
  
    //-----------------------------------------------------------------------------
    /// Connects a slot to a signal.
    /// 'object' - the object pointer to which the function belongs.
    /// 'slot' - any void returning member function.
    /// 'priority' - with great value comes great priority.
    /// return - the id of the subscribed slot.
    //-----------------------------------------------------------------------------
    template <typename C, typename F, typename std::enable_if<std::is_base_of<Bindable, C>::value>::type* = nullptr>
    std::uint64_t ConnectSlot(const std::string& signalName, C* const object, F&& slot,
                              std::uint32_t priority = slots::NORMAL_PRIORITY);
    
    //-----------------------------------------------------------------------------
    /// Connects a slot to a signal.
    /// 'object' - the object pointer to which the function belongs.
    /// 'slot' - any void returning member function.
    /// 'priority' - with great value comes great priority.
    /// return - the id of the subscribed slot.
    //-----------------------------------------------------------------------------
    template <typename C, typename F, typename std::enable_if<!std::is_base_of<Bindable, C>::value>::type* = nullptr>
    std::uint64_t ConnectSlot(const std::string& signalName, C* const object, F&& slot,
                              std::uint32_t priority = slots::NORMAL_PRIORITY);
    
    //-----------------------------------------------------------------------------
    /// Disonnects a slot from a signal.
    /// 'signalName' - the signal id to disconnect from.
    /// 'slotId' - the slot id to be disconnected from the signal.
    //-----------------------------------------------------------------------------
    void DisconnectSlot(const std::string& signalName, std::uint64_t slotId);

    //-----------------------------------------------------------------------------
    /// Emits a signal.
    /// 'signalName' - the signal to emit.
    /// 'args' - arguments passed to the slots.
    //-----------------------------------------------------------------------------
    template <typename... Args>
    void DispatchSignal(const std::string& signalName, Args&&... args) const;
    
    //-----------------------------------------------------------------------------
    /// Checks if a function is binded to a specific name.
    //-----------------------------------------------------------------------------
    bool HasBindedFunction(const std::string& functionName) const;

    //-----------------------------------------------------------------------------
    /// Binds a function to a name.
    //-----------------------------------------------------------------------------
    template <typename F>
    void BindFunction(const std::string& functionName, F&& function);

    //-----------------------------------------------------------------------------
    /// Binds a function to a name.
    /// 'sentiel' - a weak_ptr for lifetime tracking and automatic unbind.
    //-----------------------------------------------------------------------------
    template <typename F>
    void BindFunction(const std::string& functionName, Bindable::WeakPtr sentinel, F&& function);

    //-----------------------------------------------------------------------------
    /// Binds a member function to a name.
    /// 'sentiel' - a weak_ptr for lifetime tracking and automatic unbind.
    /// 'object' - the object pointer to which the function belongs.
    //-----------------------------------------------------------------------------
    template <typename C, typename F>
    void BindFunction(const std::string& functionName, Bindable::WeakPtr sentinel, C* const object,
                               F&& function);

    //-----------------------------------------------------------------------------
    /// Binds a member function to a name.
    /// 'object' - the object pointer to which the function belongs.
    //-----------------------------------------------------------------------------
    template <typename C, typename F>
    void BindFunction(const std::string& functionName, C* const object, F&& function);

    //-----------------------------------------------------------------------------
    /// Unbinds a function by name.
    //-----------------------------------------------------------------------------
    void UnbindFunction(const std::string& functionName);

    //-----------------------------------------------------------------------------
    /// Call a function by name
    //-----------------------------------------------------------------------------
    template <typename R, typename... Args,
              typename std::enable_if<!std::is_same<R, void>::value>::type* = nullptr>
    R CallFunction(const std::string& functionName, Args&&... args) const;

    //-----------------------------------------------------------------------------
    /// Call a function by name
    //-----------------------------------------------------------------------------
    template <typename R = void, typename... Args,
              typename std::enable_if<std::is_same<R, void>::value>::type* = nullptr>
    R CallFunction(const std::string& functionName, Args&&... args) const;

private:

    using CallbackInfo =
        typename ::detail::function_wrapper<Serializer, Deserializer>::CallbackInfo;
    using Bindable = ::Bindable;
    
    static void Sort(std::vector<CallbackInfo>& container);

    static void ClearGarbage(std::vector<CallbackInfo>& container);

    static std::shared_ptr<void> InitLifeguard();

    /// signal / slots
    mutable std::unordered_map<std::string, std::vector<CallbackInfo>> m_signals;
    /// binded fucntions
    mutable std::unordered_map<std::string, CallbackInfo> m_functions;
    
    std::thread::id m_threadId = std::this_thread::get_id();

    std::shared_ptr<void> m_lifetimeGuard = InitLifeguard();
        
};

#include "GenericFunctionBinder.hpp"

#endif
