#ifndef BINDABLE_H
#define BINDABLE_H

#include <memory>
#include <atomic>

class Bindable : public std::enable_shared_from_this<Bindable>
{
public:
    using WeakPtr = std::weak_ptr<Bindable>;
    using Ptr = std::shared_ptr<Bindable>;

    virtual ~Bindable() = default;
    
    virtual bool IsPaused() const;
    
    virtual void SetPaused(bool paused, uint64_t now);
    
    WeakPtr GetWeakPtr();

    virtual uint64_t GetLastPauseDuration() const;
    virtual uint64_t GetLastPauseStartTime() const;
    virtual void OnObjectPause() {}
    virtual void OnObjectResume() {}
protected:
    std::atomic_bool m_isPaused = {false};
    uint64_t m_lastPauseStart = 0;
    uint64_t m_lastPauseDuration = 0;
};

#endif
