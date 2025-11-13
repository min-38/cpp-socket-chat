#pragma once

#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include "../containers/SafeQueue.h" // 이전에 만든 스레드 안전 큐

class ThreadPool {
public:
    ThreadPool(size_t num_threads);

    ~ThreadPool();

    void enqueue(std::function<void()> task) {
        m_task_queue.push(std::move(task));
    }

private:
    void worker_thread();

    std::vector<std::thread> m_workers;
    SafeQueue<std::function<void()>> m_task_queue;
    std::atomic<bool> m_stop;
};
