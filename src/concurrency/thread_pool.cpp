#include "concurrency/thread_pool.hpp"

namespace gateway {

ThreadPool::ThreadPool(size_t num_threads) {
    (void)num_threads;
    // TODO (Week 2):
    // Spawn `num_threads` std::thread instances running WorkerLoop().
}

ThreadPool::~ThreadPool() {
    Stop();
}

bool ThreadPool::Submit(std::function<void()> task) {
    (void)task;
    // TODO (Week 2):
    // Lock mutex_, check stopping_, push task, unlock, and call cv_.notify_one().
    return false;
}

void ThreadPool::Stop() {
    // TODO (Week 2):
    // Set stopping_ = true under lock (or atomic), cv_.notify_all(), and join all joinable threads.
}

void ThreadPool::WorkerLoop() {
    // TODO (Week 2):
    // Loop: wait on cv_ until stopping_ || !tasks_.empty();
    // exit if stopping_ && tasks_.empty();
    // pop task, release lock, execute task().
}

}  // namespace gateway
