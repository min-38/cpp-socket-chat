#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue
{
public:
    void push(T value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(value));
        m_cond.notify_one();
    }
    
    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
    
        m_cond.wait(lock, [this] { return !m_queue.empty(); });
        
        value = std::move(m_queue.front());
        m_queue.pop();
    }

    void notify_all()
    {
        m_cond.notify_all();
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::queue<T> m_queue;
};
