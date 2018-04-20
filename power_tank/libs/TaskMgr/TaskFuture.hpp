#pragma once

#include "TaskFuture.h"

namespace TaskMgr
{

template <typename T>
inline Future<T>::~Future()
{
    if(onDestroy != nullptr)
    {
        onDestroy();
    }
}

template <typename T>
inline Future<T>::Future(Future<T>&& rhs)
  : onDestroy(std::move(rhs.onDestroy))
  , result{rhs.result}
{
  rhs.result = nullptr;
  rhs.onDestroy = nullptr;
}

template <typename T>
inline Future<T>& Future<T>::operator=(Future<T>&& rhs)
{
    result = std::move(rhs.result);
    onDestroy = std::move(rhs.onDestroy);
    //
    rhs.result = nullptr;
    rhs.onDestroy = nullptr;
    return *this;
}

template <typename T>
inline bool Future<T>::Wait()
{
    auto myTaskId = result->futureTaskId.exchange(TaskMgr::GetId());
    if(myTaskId == TaskMgr::EmptyTaskId)
    {
        while(result->status.load() == ResultStatus::NotReady)
        {
            TaskMgr::WaitEvent();
        }
    }

    return result->status.load() == ResultStatus::Ready;
}

template <typename T>
inline bool Future<T>::WaitFor(DurationType dur)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto endTime = now + dur;
    auto myTaskId = result->futureTaskId.exchange(TaskMgr::GetId());
    if(myTaskId == TaskMgr::EmptyTaskId)
    {
        while(result->status.load() == ResultStatus::NotReady)
        {
            if(now > endTime)
            {
                break;
            }
            TaskMgr::WaitEvent(endTime - now);
            now = std::chrono::high_resolution_clock::now();
        }
    }

    return result->status.load() == ResultStatus::Ready;
}

template <typename T>
inline T Future<T>::CopyGet()
{
    Wait();
    return result->value;
}

template <typename T>
inline T Future<T>::MoveGet()
{
    Wait();
    return std::move(result->value);
}

template <typename T>
inline bool Future<T>::IsReady() const
{
    return result->status.load() == ResultStatus::Ready;
}

template <typename T>
inline bool Future<T>::HasError() const
{
    return result->status.load() == ResultStatus::Error;
}

template <typename T>
inline bool Future<T>::IsValid() const
{
    return result != nullptr;
}

inline Future<void>::Future(Future<void>&& rhs)
    : onDestroy(std::move(rhs.onDestroy))
    , result(std::move(rhs.result))
{
    rhs.result = nullptr;
    rhs.onDestroy = nullptr;
}

inline Future<void>& Future<void>::operator=(Future<void>&& rhs)
{
    result = std::move(rhs.result);
    onDestroy = std::move(rhs.onDestroy);
    //
    rhs.result = nullptr;
    rhs.onDestroy = nullptr;
    return *this;
}

inline Future<void>::~Future()
{
    if(onDestroy != nullptr)
    {
        onDestroy();
    }
}

inline bool Future<void>::Wait()
{
    auto myTaskId = result->futureTaskId.exchange(TaskMgr::GetId());
    if(myTaskId == TaskMgr::EmptyTaskId)
    {
        while(result->status.load() == ResultStatus::NotReady)
        {
            TaskMgr::WaitEvent();
        }
    }

    return result->status.load() == ResultStatus::Ready;
}

inline bool Future<void>::WaitFor(DurationType dur)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto endTime = now + dur;
    auto myTaskId = result->futureTaskId.exchange(TaskMgr::GetId());
    if(myTaskId == TaskMgr::EmptyTaskId)
    {
        while(result->status.load() == ResultStatus::NotReady)
        {
            if(now > endTime)
            {
                break;
            }
            TaskMgr::WaitEvent(endTime - now);
            now = std::chrono::high_resolution_clock::now();
        }
    }

    return result->status.load() == ResultStatus::Ready;
}

inline bool Future<void>::IsReady() const
{
    return result->status.load() == ResultStatus::Ready;
}

inline bool Future<void>::HasError() const
{
    return result->status.load() == ResultStatus::Error;
}

inline bool Future<void>::IsValid() const
{
    return result != nullptr;
}

template <typename T>
inline Promise<T>::Promise(Promise<T>&& rhs)
  :onStatusChanged(std::move(rhs.onStatusChanged))
  ,result(std::move(rhs.result))
{
    rhs.result = nullptr;
    rhs.onStatusChanged = nullptr;
}

template <typename T>
inline Promise<T>& Promise<T>::operator=(Promise<T>&& rhs)
{
    result = std::move(rhs.result);
    onStatusChanged = std::move(rhs.onStatusChanged);
    //
    rhs.result = nullptr;
    rhs.onStatusChanged = nullptr;

    return *this;
}

template <typename T>
inline Promise<T>::~Promise()
{
    if(result && result->status == ResultStatus::NotReady)
    {
        _setStatus(ResultStatus::Error);
    }
}

template <typename T>
inline Future<T> Promise<T>::GetFuture()
{
    Future<T> future;
    future.result = result;
    return future;
}

template <typename T>
inline void Promise<T>::Set(T&& value)
{
    if(nullptr == result)
    {
        return;
    }
    result->value = std::move(value);
    _setStatus(ResultStatus::Ready);
}

template <typename T>
inline void Promise<T>::Set(const T& value)
{
    if(nullptr == result)
    {
        return;
    }
    result->value = value;
    _setStatus(ResultStatus::Ready);
}

template <typename T>
inline void Promise<T>::_setStatus(ResultStatus status)
{
    if(nullptr == result)
    {
        return;
    }
    result->status = status;
    auto futureTaskId = result->futureTaskId.exchange(TaskMgr::InvalidTaskId);
    //
    if(TaskMgr::IsValidTaskId(futureTaskId))
    {
        TaskMgr::PostEmptyEvent(futureTaskId);
    }
    if(onStatusChanged != nullptr)
    {
        onStatusChanged(status);
    }
}

inline Promise<void>::Promise(Promise<void>&& rhs)
    : onStatusChanged(std::move(rhs.onStatusChanged))
    , result(std::move(rhs.result))
{
    rhs.result = nullptr;
    rhs.onStatusChanged = nullptr;
}

inline Promise<void>& Promise<void>::operator=(Promise<void>&& rhs)
{
    result = std::move(rhs.result);
    onStatusChanged = std::move(rhs.onStatusChanged);
    //
    rhs.result = nullptr;
    rhs.onStatusChanged = nullptr;
    return *this;
}

inline Promise<void>::~Promise()
{
    if(result && result->status == ResultStatus::NotReady)
    {
        _setStatus(ResultStatus::Error);
    }
}

inline Future<void> Promise<void>::GetFuture()
{
    Future<void> future;
    future.result = result;
    return future;
}

inline void Promise<void>::Set()
{
    _setStatus(ResultStatus::Ready);
}

inline void Promise<void>::_setStatus(ResultStatus status)
{
    if(nullptr == result)
    {
        return;
    }
    result->status = status;
    auto futureTaskId = result->futureTaskId.exchange(TaskMgr::InvalidTaskId);
    //
    if(TaskMgr::IsValidTaskId(futureTaskId))
    {
        TaskMgr::PostEmptyEvent(futureTaskId);
    }

    if(onStatusChanged != nullptr)
    {
        onStatusChanged(status);
    }
}

}
