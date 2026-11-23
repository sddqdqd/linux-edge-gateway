#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/config.hpp"
#include "common/noncopyable.hpp"
#include "concurrency/thread_pool.hpp"
#include "net/connection.hpp"
#include "protocol/frame_codec.hpp"

namespace gateway {

// Single-threaded epoll Reactor coupled with a Worker ThreadPool.
// All socket I/O happens on the Reactor thread; worker threads send replies
// back via a thread-safe queue and wake the epoll loop through eventfd.
class ReactorServer : private NonCopyable {
public:
    explicit ReactorServer(const GatewayConfig& config);
    ~ReactorServer();

    // Initialize listening socket, epoll instance, and wakeup eventfd.
    bool Init();

    // Run the main epoll event loop until Stop() is called.
    void Run();

    // Signal-safe / thread-safe shutdown trigger.
    void Stop();

private:
    void HandleAccept();
    void HandleRead(int client_fd);
    void HandleWrite(int client_fd);
    void HandleWakeup();
    void CloseConnection(int client_fd);
    void SweepTimeoutConnections();

    // Business dispatcher executed inside Worker ThreadPool.
    Frame ProcessRequest(const Frame& req);

    // Enqueue encoded reply bytes from worker thread and wake up Reactor.
    void PostResponse(int client_fd, std::vector<uint8_t> encoded_frame);

    GatewayConfig config_;
    int listen_fd_ = -1;
    int epoll_fd_ = -1;
    int wakeup_fd_ = -1;
    std::atomic<bool> running_{false};

    std::unique_ptr<ThreadPool> thread_pool_;
    std::unordered_map<int, std::unique_ptr<Connection>> connections_;

    // Cross-thread response queue: (client_fd, encoded_bytes)
    std::mutex resp_mutex_;
    std::queue<std::pair<int, std::vector<uint8_t>>> resp_queue_;

    // Runtime statistics
    std::atomic<uint32_t> rx_frames_{0};
    std::atomic<uint32_t> crc_errors_{0};
};

}  // namespace gateway
