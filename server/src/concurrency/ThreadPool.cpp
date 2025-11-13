#include "concurrency/ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads) : m_stop(false)
{
    for (size_t i = 0; i < num_threads; ++i)
    {
        m_workers.emplace_back([this] {
            worker_thread();
        });
    }
}

ThreadPool::~ThreadPool()
{
    m_stop = true;

    // queue에 대기하고 있던 작업들 종료
    m_task_queue.notify_all();

    // Thread 종료
    for (std::thread &worker : m_workers)
        if (worker.joinable())
            worker.join();
}

void ThreadPool::worker_thread()
{
    while (!m_stop)
    {
        std::function<void()> task;

        m_task_queue.wait_and_pop(task);
        if (task)
            task(); 
    }
}
