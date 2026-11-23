#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "common/noncopyable.hpp"

namespace gateway {

// Fixed-size C++11 thread pool for executing business tasks off the Reactor thread.
class ThreadPool : private NonCopyable {
public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();

    // Enqueue a task for asynchronous execution. Returns false if pool is stopped.
    bool Submit(std::function<void()> task);

    // Gracefully stop all worker threads after draining queued tasks.
    void Stop();

private:
    void WorkerLoop();

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stopping_{false};
};

}  // namespace gateway
