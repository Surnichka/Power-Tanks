#ifndef EGT_TASK_MGR_H
#define EGT_TASK_MGR_H
//
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include <chrono>
//

using namespace std::chrono_literals;
static const int TASK_MGR_CONTENTION = 1;
namespace TaskMgr
{
    using Id = uint64_t;
    constexpr Id EmptyTaskId = 0ull;
    constexpr Id InvalidTaskId = ~(0ull);
    using Function = std::function<void()>;
    //
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::high_resolution_clock::duration;
    
    //
    struct Task : public std::enable_shared_from_this<Task>
    {
        using Ptr = std::shared_ptr<Task>;
        Id taskId;
        std::thread taskThread;
        ~Task();
        Task() = default;
        Task(const Task&) = delete;
        Task(Task&&) = default;
        Task& operator=(Task&&) = default;
        Task& operator=(const Task&) = delete;
    };
    //
    struct Locksmith
    {
        std::vector<TaskMgr::Id> clients;
        std::vector<uintptr_t> objects;
        std::mutex _locker;
    };

    struct Locker
    {
        ~Locker();
        Locker(bool isLocked, uint64_t object);
        Locker(Locker&&);
        Locker& operator=(Locker&&);
        Locker& operator=(const Locker&) = delete;
        Locker(const Locker&) = delete;
        void Unlock();
        bool IsLocked() const;

    private:

        bool _isLocked;
        uintptr_t _object;
    };


    Locker LockNumber(uintptr_t numberObject, Duration timeout = 999999h);

    template
    <
        typename T,
        class = typename std::enable_if
        <
            std::is_lvalue_reference<T>::value
        >::type
    >
    Locker LockObject(T&& obj, Duration timeout = 999999h)
    {
        return LockNumber(reinterpret_cast<uintptr_t>(&obj), timeout);
    }

    void Init(std::function<void(const std::string& msg)> logger,
              std::function<void(std::thread&, const std::string&)> changeThreadName);
    //
    void RegisterThisThread();
    // Ids
    std::thread::id MainThreadId();
    bool IsMainThread();
    Id GetId() noexcept;
    std::pair<bool, Id> GetId(std::thread::id threadId);
    std::pair<bool, std::thread::id> GetThreadId(Id taskId);
    // My task
    bool ShouldStop();
    //
    bool IsValidTaskId(Id taskId) noexcept;
    //
    void Process();
    void Sleep(Duration duration);
    void WaitEvent(Duration duration);
    void WaitEvent();
    void WaitEmptyEvent(Duration duration);
    void WaitEmptyEvent();
    void WaitEventNoProcessing(Duration duration);
    void WaitEventNoProcessing();
    // Remote task
    bool Invoke(Task::Ptr task, Function&& callback);
    bool Invoke(Task::Ptr task, std::weak_ptr<void> sentinel, Function&& callback);
    bool Invoke(const std::thread& thread, Function&& callback);
    bool Invoke(const std::thread& thread, std::weak_ptr<void> sentinel, Function&& callback);
    bool Invoke(Id taskId, Function&& callback);
    bool Invoke(Id taskId, std::weak_ptr<void> sentinel, Function&& callback);
    bool Invoke(std::thread::id threadId, Function&& callback);
    bool Invoke(std::thread::id threadId, std::weak_ptr<void> sentinel, Function&& callback);
    //
    bool RunOrInvoke(Task::Ptr task, Function&& callback);
    bool RunOrInvoke(Task::Ptr task, std::weak_ptr<void> sentinel, Function&& callback);
    bool RunOrInvoke(const std::thread& thread, Function&& callback);
    bool RunOrInvoke(const std::thread& thread, std::weak_ptr<void> sentinel, Function&& callback);
    bool RunOrInvoke(Id taskId, Function&& callback);
    bool RunOrInvoke(Id taskId, std::weak_ptr<void> sentinel, Function&& callback);
    bool RunOrInvoke(std::thread::id threadId, Function&& callback);
    bool RunOrInvoke(std::thread::id threadId, std::weak_ptr<void> sentinel, Function&& callback);
    
    //
    void AddDestructor(Id taskId, Function&& callback);
    //
    void PostEmptyEvent(Task::Ptr task);  
    void PostEmptyEvent(const std::thread& thread);
    void PostEmptyEvent(Id taskId);  
    void PostEmptyEvent(std::thread::id threadId);
    //
    void Stop(Task::Ptr task);
    void Stop(const std::thread& thread);
    void Stop(Id taskId);
    void Stop(std::thread::id threadId);
    //
    // New tasks
    Task::Ptr RunTask(const std::string& name = "");
    
    void Log(const std::string& msg);
}

#endif // EGT_TASK_MGR_H
