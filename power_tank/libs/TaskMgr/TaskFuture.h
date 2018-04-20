#ifndef EGT_TASK_FUTURE_H
#define EGT_TASK_FUTURE_H

#include "TaskMgr.h"
#include <cassert>

namespace TaskMgr
{

enum class ResultStatus : unsigned
{
    NotReady,
    Ready,
    Error
};

template <typename R>
struct Result
{
    R value = {};

    std::atomic<Id> futureTaskId = {EmptyTaskId};
    std::atomic<ResultStatus> status = {ResultStatus::NotReady};
};

template <>
struct Result<void>
{
    std::atomic<Id> futureTaskId = {EmptyTaskId};
    std::atomic<ResultStatus> status = {ResultStatus::NotReady};
};

template <typename T>
class Promise;

template <typename T>
class Future
{
    friend class Promise<T>;
    using DurationType = std::chrono::high_resolution_clock::duration;
    using ResultType = Result<T>;

public:
    Future() = default;
    Future(Future&& rhs);
    Future& operator=(Future&& rhs);
    ~Future();
    
    //-----------------------------------------------------------------------------
    /// The purpose of the copy constructor and assignment
    /// is to allow promises and futures to be moved into a
    /// std::function. They are not actually invoked but are
    /// required for compilation by std::function
    //-----------------------------------------------------------------------------
    Future(const Future&) = delete;
    Future& operator=(const Future&) = delete;

    
    //-----------------------------------------------------------------------------
    /// Waits for the result to become available 
    //-----------------------------------------------------------------------------
    bool Wait();
    
    //-----------------------------------------------------------------------------
    /// waits for the result, returns if it is not available for 
    /// the specified timeout duration 
    //-----------------------------------------------------------------------------
    bool WaitFor(DurationType dur);

    //-----------------------------------------------------------------------------
    /// Returns a copy of the result. 
    /// Blocks until result is ready or error occurs. 
    //-----------------------------------------------------------------------------
    T CopyGet();
    
    //-----------------------------------------------------------------------------
    /// Returns the result and the shared state is invalidated.
    /// Blocks until result is ready or error occurs. 
    //-----------------------------------------------------------------------------
    T MoveGet();

    //-----------------------------------------------------------------------------
    /// Checks whether the result is ready. 
    //-----------------------------------------------------------------------------
    bool IsReady() const;
    
    //-----------------------------------------------------------------------------
    /// Checks whether an error occurred. 
    //-----------------------------------------------------------------------------
    bool HasError() const;
    
    //-----------------------------------------------------------------------------
    /// Checks if the future has an associated shared state.
    /// This is the case only for futures that were not default-constructed or
    /// moved from (i.e. returned by Promise::GetFuture()
    //-----------------------------------------------------------------------------
    bool IsValid() const;

    std::function<void()> onDestroy = nullptr;

private:
    std::shared_ptr<ResultType> result = nullptr;
};

template <>
class Future<void>
{
    friend class Promise<void>;
    using ResultType = Result<void>;
    using DurationType = std::chrono::high_resolution_clock::duration;

public:
    Future() = default;
    Future(Future&& rhs);
    Future& operator=(Future&& rhs);
    ~Future();
    
    //-----------------------------------------------------------------------------
    /// The purpose of the copy constructor and assignment
    /// is to allow promises and futures to be moved into a
    /// std::function. They are not actually invoked but are
    /// required for compilation by std::function
    //-----------------------------------------------------------------------------
    Future(const Future&) = delete;
    Future& operator=(const Future&) = delete;

    //-----------------------------------------------------------------------------
    /// Waits for the result to become available 
    //-----------------------------------------------------------------------------
    bool Wait();
    
    //-----------------------------------------------------------------------------
    /// waits for the result, returns if it is not available for 
    /// the specified timeout duration 
    //-----------------------------------------------------------------------------
    bool WaitFor(DurationType dur);

    //-----------------------------------------------------------------------------
    /// Checks whether the result is ready. 
    //-----------------------------------------------------------------------------
    bool IsReady() const;
    
    //-----------------------------------------------------------------------------
    /// Checks whether an error occurred. 
    //-----------------------------------------------------------------------------
    bool HasError() const;
    
    //-----------------------------------------------------------------------------
    /// Checks if the future has an associated shared state.
    /// This is the case only for futures that were not default-constructed or
    /// moved from (i.e. returned by Promise::GetFuture()
    //-----------------------------------------------------------------------------
    bool IsValid() const;

    std::function<void()> onDestroy = nullptr;

private:
    std::shared_ptr<ResultType> result = nullptr;
};

template <typename T>
class Promise
{
    using ResultType = Result<T>;

public:
    Promise() = default;
    Promise(Promise&& rhs);
    Promise& operator=(Promise&& rhs);
    ~Promise();
    
    //-----------------------------------------------------------------------------
    /// The purpose of the copy constructor and assignment
    /// is to allow promises and futures to be moved into a
    /// std::function. They are not actually invoked but are
    /// required for compilation by std::function
    //-----------------------------------------------------------------------------
    Promise(const Promise&) = delete;
    Promise& operator=(const Promise&) = delete;

    //-----------------------------------------------------------------------------
    /// Returns a future associated with the promised result 
    //-----------------------------------------------------------------------------
    Future<T> GetFuture();

    //-----------------------------------------------------------------------------
    /// Sets the result to specific value
    //-----------------------------------------------------------------------------
    void Set(T&& value);
    
    //-----------------------------------------------------------------------------
    /// Sets the result to specific value
    //-----------------------------------------------------------------------------
    void Set(const T& value);

    //-----------------------------------------------------------------------------
    /// User callback for change of status
    //-----------------------------------------------------------------------------
    std::function<void(ResultStatus)> onStatusChanged = nullptr;

private:
    void _setStatus(ResultStatus status);

    /// The shared state
    std::shared_ptr<ResultType> result = std::make_shared<ResultType>();
};

template <>
class Promise<void>
{
    using ResultType = Result<void>;

public:
    Promise() = default;
    Promise(Promise&& rhs);
    Promise& operator=(Promise&& rhs);
    ~Promise();
    
    //-----------------------------------------------------------------------------
    /// The purpose of the copy constructor and assignment
    /// is to allow promises and futures to be moved into a
    /// std::function. They are not actually invoked but are
    /// required for compilation by std::function
    //-----------------------------------------------------------------------------
    Promise(const Promise&) = delete;
    Promise& operator=(const Promise&) = delete;

    //-----------------------------------------------------------------------------
    /// Returns a future associated with the promised result 
    //-----------------------------------------------------------------------------
    Future<void> GetFuture();
    
    //-----------------------------------------------------------------------------
    /// Sets the result to specific value
    //-----------------------------------------------------------------------------
    void Set();
    
    //-----------------------------------------------------------------------------
    /// User callback for change of status
    //-----------------------------------------------------------------------------
    std::function<void(ResultStatus)> onStatusChanged = nullptr;

private:
    void _setStatus(ResultStatus status);

    /// The shared state
    std::shared_ptr<ResultType> result = std::make_shared<ResultType>();
};


}

#include "TaskFuture.hpp"

#endif // EGT_TASK_FUTURE_H
