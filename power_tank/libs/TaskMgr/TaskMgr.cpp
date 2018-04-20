#include "TaskMgr.h"

using namespace std::literals;

namespace TaskMgr
{

struct TaskContext
{
    TaskContext();
    ~TaskContext();

    void _register();
    void _unregister();
    //
    struct Invocation
    {
        Invocation() = default;
        Invocation(Invocation&&) = default;
        Invocation& operator=(Invocation&&) = default;
        //
        Invocation(const Invocation&) = delete;
        Invocation& operator=(const Invocation&) = delete;

        Function callback;
        std::weak_ptr<void> sentinel;
        bool hasSentinel = false;        
    };

    std::atomic_bool m_shouldStop = {false};
    std::mutex m_invokesLocker;
    std::vector<Invocation> m_invokes;
    std::vector<Invocation> m_processingInvokes;
    
    std::mutex m_callbacksLocker;
    std::vector<Invocation> m_onDestroyCallbacks;
    
    size_t m_invokesBegin = 0;
    //
    bool m_hasEmptyEvent = false;
    std::condition_variable m_wakeupEvent;
    //
    Id m_id;
    std::thread::id m_threadId;
    friend struct TaskMgr;
    bool m_registered = false;
};

static std::thread::id s_mainThreadId = std::this_thread::get_id();
static std::unordered_map<TaskMgr::Id, TaskMgr::TaskContext*> s_tasks[TASK_MGR_CONTENTION];
static std::unordered_map<std::thread::id, TaskMgr::Id> s_threadIds[TASK_MGR_CONTENTION];
static std::atomic_uint_fast64_t s_freeTaskId = {1};
static std::mutex s_mainLocker[TASK_MGR_CONTENTION];
static std::atomic_uint s_contentionIdx = {0};
static std::function<void(std::thread&, const std::string&)> s_changeThreadName;
static std::function<void(const std::string& msg)> s_logger;

namespace priv
{

thread_local static ::TaskMgr::TaskContext ___threadLockal_thisTask_;
constexpr static size_t _locksmitsLog2 = 10;
constexpr static size_t _locksmithsCount = (1 << _locksmitsLog2);
constexpr static uintptr_t _locksmithsHashMask = _locksmithsCount - 1;
//
static Locksmith _locksmiths[_locksmithsCount];

bool _invoke(TaskMgr::Id taskId, bool hasSentinel, std::weak_ptr<void> sentinel,
                      Function&& callback)
{
    if(callback == nullptr)
    {
        return false;
    }
    unsigned int idx = s_contentionIdx.fetch_add(1) % TASK_MGR_CONTENTION;
    s_mainLocker[idx].lock();
    auto it = s_tasks[idx].find(taskId);
    if(it == s_tasks[idx].end())
    {
        s_mainLocker[idx].unlock();
        return false;
    }
    auto& remoteTask = *(it->second);
    s_mainLocker[idx].unlock();
    
    remoteTask.m_invokesLocker.lock();
    remoteTask.m_invokes.emplace_back();
    remoteTask.m_invokes.back().callback = std::move(callback);
    if(true == hasSentinel)
    {
        remoteTask.m_invokes.back().sentinel = sentinel;
        remoteTask.m_invokes.back().hasSentinel = true;
    }
    //
    remoteTask.m_wakeupEvent.notify_all();
    remoteTask.m_invokesLocker.unlock();
    return true;
}

bool _add_callback(TaskMgr::Id taskId, bool hasSentinel, std::weak_ptr<void> sentinel,
                      Function&& callback)
{
    if(callback == nullptr)
    {
        return false;
    }
    unsigned int idx = s_contentionIdx.fetch_add(1) % TASK_MGR_CONTENTION;
    s_mainLocker[idx].lock();
    auto it = s_tasks[idx].find(taskId);
    if(it == s_tasks[idx].end())
    {
        s_mainLocker[idx].unlock();
        return false;
    }
    auto& remoteTask = *(it->second);
    s_mainLocker[idx].unlock();
    
    remoteTask.m_callbacksLocker.lock();
    remoteTask.m_onDestroyCallbacks.emplace_back();
    remoteTask.m_onDestroyCallbacks.back().callback = std::move(callback);
    if(true == hasSentinel)
    {
        remoteTask.m_onDestroyCallbacks.back().sentinel = sentinel;
        remoteTask.m_onDestroyCallbacks.back().hasSentinel = true;
    }
    //
    remoteTask.m_callbacksLocker.unlock();
    return true;
}

}




TaskContext::TaskContext() = default;
//
TaskContext::~TaskContext()
{
    m_callbacksLocker.lock();
    auto callbacks = std::move(m_onDestroyCallbacks);
    m_callbacksLocker.unlock();
    
    for(const auto& invocation : callbacks)
    {
        if(true == invocation.hasSentinel)
        {
            auto sentinelLock = invocation.sentinel.lock();
            if(nullptr != sentinelLock)
            {
                invocation.callback();
            }
        }
        else
        {
            invocation.callback();
        }
    }
    
    _unregister();
}

void TaskContext::_register()
{
    if(m_registered)
    {
        return;
    }
    m_id = TaskMgr::s_freeTaskId.fetch_add(1);
    for (unsigned int i = 0; i < TASK_MGR_CONTENTION; i++)
    {
        std::lock(TaskMgr::s_mainLocker[i], m_invokesLocker);
        std::lock_guard<std::mutex> lk1(TaskMgr::s_mainLocker[i], std::adopt_lock);
        std::lock_guard<std::mutex> lk2(m_invokesLocker, std::adopt_lock);
        
        //
        m_threadId = std::this_thread::get_id();

        TaskMgr::s_tasks[i][m_id] = this;
        TaskMgr::s_threadIds[i][m_threadId] = m_id;
    }
    m_registered = true;
}

void TaskContext::_unregister()
{
    for (unsigned int i = 0; i < TASK_MGR_CONTENTION; i++)
    {
        std::lock(TaskMgr::s_mainLocker[i], m_invokesLocker);
        std::lock_guard<std::mutex> lk1(TaskMgr::s_mainLocker[i], std::adopt_lock);
        std::lock_guard<std::mutex> lk2(m_invokesLocker, std::adopt_lock);
        
        //
        TaskMgr::s_tasks[i].erase(m_id);
        TaskMgr::s_threadIds[i].erase(m_threadId);
    }
    m_registered = false;
}

void AddDestructor(Id taskId, Function &&callback)
{
    priv::_add_callback(taskId, false, {}, std::move(callback));    
}


bool Invoke(Id taskId, Function&& callback)
{
    return priv::_invoke(taskId, false, {}, std::move(callback));
}

bool Invoke(Id taskId, std::weak_ptr<void> sentinel, Function&& callback)
{
    return priv::_invoke(taskId, true, sentinel, std::move(callback));
}

bool RunOrInvoke(TaskMgr::Id taskId, Function&& callback)
{
    auto& thisTask = priv::___threadLockal_thisTask_;
    if(taskId == thisTask.m_id)
    {
        if(callback != nullptr)
        {
            callback();
            return true;
        }
        return false;
    }
    else
    {
        return priv::_invoke(taskId, false, {}, std::move(callback));
    }
}

bool RunOrInvoke(TaskMgr::Id taskId, std::weak_ptr<void> sentinel, Function&& callback)
{
    auto& thisTask = priv::___threadLockal_thisTask_;
    if(taskId == thisTask.m_id)
    {
        if(callback != nullptr)
        {
            auto locker = sentinel.lock();
            if(nullptr != locker)
            {
                callback();
                return true;
            }
            return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return priv::_invoke(taskId, true, sentinel, std::move(callback));
    }
}

void PostEmptyEvent(Id taskId)
{
    unsigned int idx = s_contentionIdx.fetch_add(1) % TASK_MGR_CONTENTION;
    s_mainLocker[idx].lock();
    auto it = s_tasks[idx].find(taskId);
    if(it == s_tasks[idx].end())
    {
        s_mainLocker[idx].unlock();
        return;
    }
    auto& remoteTask = *(it->second);
    s_mainLocker[idx].unlock();
    
    remoteTask.m_invokesLocker.lock();
    remoteTask.m_hasEmptyEvent = true;
    remoteTask.m_wakeupEvent.notify_all();
    remoteTask.m_invokesLocker.unlock();
}

void Stop(Id taskId)
{
    unsigned int idx = s_contentionIdx.fetch_add(1) % TASK_MGR_CONTENTION;
    s_mainLocker[idx].lock();
    auto it = s_tasks[idx].find(taskId);
    if(it == s_tasks[idx].end())
    {
        s_mainLocker[idx].unlock();
        return;
    }
    auto& remoteTask = *(it->second);
    s_mainLocker[idx].unlock();
    
    remoteTask.m_invokesLocker.lock();
    remoteTask.m_hasEmptyEvent = true;
    remoteTask.m_shouldStop.store(true);
    remoteTask.m_wakeupEvent.notify_all();
    remoteTask.m_invokesLocker.unlock();
}

bool Invoke(std::thread::id threadId, Function&& callback)
{
    const auto p = GetId(threadId);
    if (p.first == false)
    {
        return false;
    }
    auto taskId = p.second;
    
    return priv::_invoke(taskId, false, {}, std::move(callback));

}

bool Invoke(std::thread::id threadId, std::weak_ptr<void> sentinel, Function&& callback)
{
    const auto p = GetId(threadId);
    if (p.first == false)
    {
        return false;
    }
    auto taskId = p.second;
    return priv::_invoke(taskId, true, sentinel, std::move(callback));
}

bool RunOrInvoke(std::thread::id threadId, Function&& callback)
{
    const auto p = GetId(threadId);
    if (p.first == false)
    {
        return false;
    }
    auto taskId = p.second;
    return RunOrInvoke(taskId, std::move(callback));
}

bool RunOrInvoke(std::thread::id threadId, std::weak_ptr<void> sentinel, Function&& callback)
{
    const auto p = GetId(threadId);
    if (p.first == false)
    {
        return false;
    }
    auto taskId = p.second;
    return RunOrInvoke(taskId, sentinel, std::move(callback));
}

void PostEmptyEvent(std::thread::id threadId)
{
    const auto p = GetId(threadId);
    if (p.first == false)
    {
        return;
    }
    auto taskId = p.second;
    PostEmptyEvent(taskId);
}

void Stop(std::thread::id threadId)
{
    const auto p = GetId(threadId);
    if (p.first == false)
    {
        return;
    }
    auto taskId = p.second;
    Stop(taskId);
}

Task::Ptr RunTask(const std::string& name)
{
    Task::Ptr task = std::make_shared<Task>();
    std::mutex lockMutex;
    std::unique_lock<std::mutex> locker(lockMutex);
    std::condition_variable cv;
    task->taskThread = std::thread([&lockMutex, &cv]() {
        RegisterThisThread();
        lockMutex.lock();
        cv.notify_all();
        lockMutex.unlock();
        //
        while(TaskMgr::ShouldStop() == false)
        {
            TaskMgr::WaitEvent();
        }
    });
    cv.wait(locker);
    //
    if(name.empty() == false)
    {
        if(s_changeThreadName)
        {
            s_changeThreadName(task->taskThread, name);
        } 
    }
    auto threadId = task->taskThread.get_id();
    const auto p = TaskMgr::GetId(threadId);
    
    if(p.first == false)
    {
        return nullptr;
    }
    task->taskId = p.second;
    return task;
}

void RegisterThisThread()
{
    auto& thisTask = priv::___threadLockal_thisTask_;
    thisTask._register();
}

std::thread::id MainThreadId()
{
    return s_mainThreadId;
}

Id GetId() noexcept
{
    auto& thisTask = priv::___threadLockal_thisTask_;
    return thisTask.m_id;
}

std::pair<bool, TaskMgr::Id> GetId(std::thread::id threadId)
{
    unsigned int idx = s_contentionIdx.fetch_add(1) % TASK_MGR_CONTENTION;
    std::lock_guard<std::mutex> locker(TaskMgr::s_mainLocker[idx]);
    auto it = TaskMgr::s_threadIds[idx].find(threadId);
    if(it == TaskMgr::s_threadIds[idx].end())
    {
        return {false, {}};
    }

    return {true, it->second};
}

std::pair<bool, std::thread::id> GetThreadId(TaskMgr::Id taskId)
{
    unsigned int idx = s_contentionIdx.fetch_add(1) % TASK_MGR_CONTENTION;
    std::lock_guard<std::mutex> locker(TaskMgr::s_mainLocker[idx]);
    auto it = TaskMgr::s_tasks[idx].find(taskId);
    if(it == TaskMgr::s_tasks[idx].end())
    {
        return {false, {}};
    }
    return {true, it->second->m_threadId};
}

bool ShouldStop()
{
    auto& thisTask = priv::___threadLockal_thisTask_;
    auto shouldStop = thisTask.m_shouldStop.load();
    return shouldStop;
}

bool _somethingForProcess(::TaskMgr::TaskContext& thisTask)
{
    return (false == (thisTask.m_invokes.empty() && thisTask.m_processingInvokes.size() == thisTask.m_invokesBegin));
}

bool _processQueueOne(std::unique_lock<std::mutex>& locker)
{
    auto& thisTask = priv::___threadLockal_thisTask_;
    if (false == _somethingForProcess(thisTask))
    {
        return false;
    }

    if (thisTask.m_processingInvokes.size() == thisTask.m_invokesBegin)
    {
        if (thisTask.m_invokes.empty())
        {
            return false;
        }
        // preserve the vector's capacity.
        auto tmp = std::move(thisTask.m_processingInvokes);
        thisTask.m_processingInvokes = std::move(thisTask.m_invokes);
        thisTask.m_invokes = std::move(tmp);
        thisTask.m_invokes.clear();

        thisTask.m_invokesBegin = 0;
    }
    locker.unlock();
    auto invocation = std::move(thisTask.m_processingInvokes[thisTask.m_invokesBegin]);
    thisTask.m_invokesBegin++;
    //
    if(true == invocation.hasSentinel)
    {
        auto sentinelLock = invocation.sentinel.lock();
        if(nullptr != sentinelLock)
        {
            invocation.callback();
        }
    }
    else
    {
        invocation.callback();
    }
    locker.lock();
    return true;
}

bool _processQueueAll(std::unique_lock<std::mutex>& locker)
{
    auto& thisTask = priv::___threadLockal_thisTask_;
    if (false == _somethingForProcess(thisTask))
    {
        return false;
    }

    while(false == thisTask.m_invokes.empty() || thisTask.m_processingInvokes.size() != thisTask.m_invokesBegin)
    {
        if (thisTask.m_processingInvokes.size() == thisTask.m_invokesBegin)
        {
            // preserve the vector's capacity.
            auto tmp = std::move(thisTask.m_processingInvokes);
            thisTask.m_processingInvokes = std::move(thisTask.m_invokes);
            thisTask.m_invokes = std::move(tmp);
            thisTask.m_invokes.clear();

            thisTask.m_invokesBegin = 0;
        }
        
        locker.unlock();
        while(thisTask.m_invokesBegin < thisTask.m_processingInvokes.size())
        {
            auto invocation = std::move(thisTask.m_processingInvokes[thisTask.m_invokesBegin]);
            thisTask.m_invokesBegin++;
            
            //
            if(true == invocation.hasSentinel)
            {
                auto sentinelLock = invocation.sentinel.lock();
                if(nullptr != sentinelLock)
                {
                    invocation.callback();
                }
            }
            else
            {
                invocation.callback();
            }
        }
        locker.lock();
        //
        
    }
    thisTask.m_processingInvokes.clear();
    thisTask.m_invokesBegin = 0;
    return true;
}

void Sleep(Duration duration)
{
    auto endTime = Clock::now() + duration;
    auto& thisTask = priv::___threadLockal_thisTask_;

    std::unique_lock<std::mutex> thisTaskLocker(thisTask.m_invokesLocker);
    _processQueueOne(thisTaskLocker);
    while(Clock::now() < endTime)
    {
        if(TaskMgr::ShouldStop() == true)
        {
            return;
        }
        auto timeLeft = endTime - Clock::now();
        if(timeLeft > Duration(0))
        {
            thisTask.m_wakeupEvent.wait_for(thisTaskLocker, timeLeft);
        }

        _processQueueOne(thisTaskLocker);
    }
}

void WaitEvent(Duration duration)
{
    auto& thisTask = priv::___threadLockal_thisTask_;

    std::unique_lock<std::mutex> thisTaskLocker(thisTask.m_invokesLocker);
    if(true == _processQueueOne(thisTaskLocker))
    {
        return;
    }
    if(TaskMgr::ShouldStop() == true)
    {
        return;
    }
    if (thisTask.m_hasEmptyEvent == true)
    {
        thisTask.m_hasEmptyEvent = false;        
        return;
    }
    thisTask.m_wakeupEvent.wait_for(thisTaskLocker, duration);
    _processQueueOne(thisTaskLocker);
}

void WaitEvent()
{
    auto& thisTask = priv::___threadLockal_thisTask_;

    std::unique_lock<std::mutex> thisTaskLocker(thisTask.m_invokesLocker);
    if(true == _processQueueOne(thisTaskLocker))
    {
        return;
    }
    if(TaskMgr::ShouldStop() == true)
    {
        return;
    }
    if (thisTask.m_hasEmptyEvent == true)
    {
        thisTask.m_hasEmptyEvent = false;
        return;
    }
    thisTask.m_wakeupEvent.wait(thisTaskLocker);
    _processQueueOne(thisTaskLocker);
}

void WaitEventNoProcessing(Duration duration)
{
    auto& thisTask = priv::___threadLockal_thisTask_;

    std::unique_lock<std::mutex> thisTaskLocker(thisTask.m_invokesLocker);
    if (true == _somethingForProcess(thisTask))
    {
        return;
    }

    if(TaskMgr::ShouldStop() == true)
    {
        return;
    }
    if (thisTask.m_hasEmptyEvent == true)
    {
        thisTask.m_hasEmptyEvent = false;
        return;
    }
    thisTask.m_wakeupEvent.wait_for(thisTaskLocker, duration);
}

void WaitEventNoProcessing()
{
    auto& thisTask = priv::___threadLockal_thisTask_;

    std::unique_lock<std::mutex> thisTaskLocker(thisTask.m_invokesLocker);
    if (true == _somethingForProcess(thisTask))
    {
        return;
    }
    if(TaskMgr::ShouldStop() == true)
    {
        return;
    }
    if (thisTask.m_hasEmptyEvent == true)
    {
        thisTask.m_hasEmptyEvent = false;
        return;
    }
    thisTask.m_wakeupEvent.wait(thisTaskLocker);
}

void WaitEmptyEvent(Duration duration)
{
    auto endTime = Clock::now() + duration;
    auto& thisTask = priv::___threadLockal_thisTask_;

    std::unique_lock<std::mutex> thisTaskLocker(thisTask.m_invokesLocker);

    while((thisTask.m_hasEmptyEvent == false) && (Clock::now() < endTime))
    {
        if(TaskMgr::ShouldStop() == true)
        {
            return;
        }
        auto timeLeft = endTime - Clock::now();
        if(timeLeft > Duration(0))
        {
            thisTask.m_wakeupEvent.wait_for(thisTaskLocker, timeLeft);
        }
    }
    thisTask.m_hasEmptyEvent = false;
}

void WaitEmptyEvent()
{
    auto& thisTask = priv::___threadLockal_thisTask_;

    std::unique_lock<std::mutex> thisTaskLocker(thisTask.m_invokesLocker);
    while(thisTask.m_hasEmptyEvent == false)
    {
        if(TaskMgr::ShouldStop() == true)
        {
            return;
        }
        thisTask.m_wakeupEvent.wait(thisTaskLocker);
    }
    thisTask.m_hasEmptyEvent = false;
}

Task::~Task()
{
    TaskMgr::Stop(taskId);
    if(taskThread.joinable() == true)
    {
        if (GetId() != taskId)
        {
            taskThread.join();
        }
        else
        {
            taskThread.detach();
        }
    }
}

bool Invoke(Task::Ptr task, Function &&callback)
{
    if (nullptr == task)
    {
        return false;
    }
    return Invoke(task->taskId, std::move(callback));
}

bool Invoke(Task::Ptr task, std::weak_ptr<void> sentinel, Function &&callback)
{
    if (nullptr == task)
    {
        return false;
    }
    return Invoke(task->taskId, sentinel, std::move(callback)); 
}

bool Invoke(const std::thread &thread, Function &&callback)
{
    return Invoke(thread.get_id(), std::move(callback)); 
}

bool Invoke(const std::thread &thread, std::weak_ptr<void> sentinel, Function &&callback)
{
    return Invoke(thread.get_id(), sentinel, std::move(callback)); 
}

bool RunOrInvoke(Task::Ptr task, Function &&callback)
{
    if (nullptr == task)
    {
        return false;
    }
    return RunOrInvoke(task->taskId, std::move(callback));
}

bool RunOrInvoke(Task::Ptr task, std::weak_ptr<void> sentinel, Function &&callback)
{
    if (nullptr == task)
    {
        return false;
    }
    return RunOrInvoke(task->taskId, sentinel, std::move(callback));
}

bool RunOrInvoke(const std::thread &thread, Function &&callback)
{
    return RunOrInvoke(thread.get_id(), std::move(callback));    
}

bool RunOrInvoke(const std::thread &thread, std::weak_ptr<void> sentinel, Function &&callback)
{
    return RunOrInvoke(thread.get_id(), sentinel, std::move(callback));    
}


void PostEmptyEvent(Task::Ptr task)
{
    if (nullptr == task)
    {
        return;
    }
    PostEmptyEvent(task->taskId);
}

void PostEmptyEvent(const std::thread &thread)
{
    PostEmptyEvent(thread.get_id());
}

void Stop(Task::Ptr task)
{
    if (nullptr == task)
    {
        return;
    }
    Stop(task->taskId);
}

void Stop(const std::thread &thread)
{
    Stop(thread.get_id());
}

bool IsValidTaskId(Id taskId) noexcept
{
    return (taskId != InvalidTaskId) && (taskId != EmptyTaskId);
}

void Process()
{
    auto& thisTask = priv::___threadLockal_thisTask_;


    std::unique_lock<std::mutex> thisTaskLocker(thisTask.m_invokesLocker);
    _processQueueAll(thisTaskLocker);
}

bool IsLocked(Locksmith& locksmith, uintptr_t obj)
{
    for(const auto& lockedObject : locksmith.objects)
    {
        if (lockedObject == obj)
        {
            return true;
        }
    }
    return false;
}

void RemoveClient(Locksmith& locksmith, TaskMgr::Id client)
{
    for (size_t i = 0; i < locksmith.clients.size(); i++)
    {
        if (locksmith.clients[i] == client)
        {
            locksmith.clients[i] = locksmith.clients.back();
            locksmith.clients.pop_back();
            return;
        }
    }
}

Locker LockNumber(uintptr_t numberObject, Duration timeout)
{
    size_t objHash = static_cast<size_t>(numberObject & priv::_locksmithsHashMask);
    auto& locksmith = priv::_locksmiths[objHash];

    //
    locksmith._locker.lock(); ///////////// LOCK ///////////////////
    //

    if (false == IsLocked(locksmith, numberObject))
    {
        locksmith.objects.push_back(numberObject);
        //
        locksmith._locker.unlock(); ///////////// UNLOCK ///////////////////
        //
        return {true, numberObject};
    }

    auto taskId = TaskMgr::GetId();
    locksmith.clients.push_back(taskId);
    //
    locksmith._locker.unlock(); ///////////// UNLOCK ///////////////////
    //

    auto now = TaskMgr::Clock::now();
    auto deadLine = now + timeout;

    while((now = TaskMgr::Clock::now()) < deadLine)
    {
        TaskMgr::WaitEvent(deadLine - now);
        locksmith._locker.lock(); ///////////// LOCK ///////////////////
        if (false == IsLocked(locksmith, numberObject))
        {
            RemoveClient(locksmith, taskId);
            locksmith.objects.push_back(numberObject);
            locksmith._locker.unlock(); ///////////// UNLOCK ///////////////////
            return {true, numberObject};
        }
        locksmith._locker.unlock(); ///////////// UNLOCK ///////////////////
    }
    return {false, numberObject};
}

Locker::~Locker()
{
    Unlock();
}

Locker::Locker(bool isLocked, uint64_t object)
    : _isLocked(isLocked)
    , _object(object)
{}

Locker::Locker(Locker &&rLocker)
{
    _isLocked = rLocker._isLocked;
    _object = rLocker._object;
    rLocker._isLocked = false;
}

Locker& Locker::operator=(Locker &&rLocker)
{
    _isLocked = rLocker._isLocked;
    _object = rLocker._object;
    rLocker._isLocked = false;
    return *this;
}

void Locker::Unlock()
{
    if (IsLocked() == false)
    {
        return;
    }
    _isLocked = false;

    size_t objHash = static_cast<size_t>(_object & priv::_locksmithsHashMask);
    auto& locksmith = priv::_locksmiths[objHash];
    //
    locksmith._locker.lock(); ///////////// LOCK ///////////////////
    for (size_t i = 0; i < locksmith.objects.size(); i++)
    {
        if (locksmith.objects[i] == _object)
        {
            locksmith.objects[i] = locksmith.objects.back();
            locksmith.objects.pop_back();
            break;
        }
    }
    for (auto& client : locksmith.clients)
    {
        TaskMgr::PostEmptyEvent(client);
    }
    locksmith._locker.unlock(); ///////////// UNLOCK ///////////////////
}

bool Locker::IsLocked() const
{
    return _isLocked;
}

void Init(std::function<void (const std::string &)> logger, 
          std::function<void (std::thread&, const std::string&)> changeThreadName)
{
    RegisterThisThread();
    s_logger = logger;
    s_changeThreadName = changeThreadName;   
}

void Log(const std::string &msg)
{
    if(s_logger)
    {
        s_logger(msg);
    }
}

bool IsMainThread()
{
    return std::this_thread::get_id() == MainThreadId();
}

}
