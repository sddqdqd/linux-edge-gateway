#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <vector>

#include "common/noncopyable.hpp"
#include "protocol/ring_buffer.hpp"

namespace gateway {

// Encapsulates a single client TCP connection owned exclusively by the Reactor thread.
class Connection : private NonCopyable {
public:
    explicit Connection(int fd);
    ~Connection();

    int fd() const { return fd_; }
    RingBuffer& rx_buffer() { return rx_buffer_; }

    // Queue outbound bytes to be flushed when socket becomes writable.
    void AppendTxData(std::vector<uint8_t> data);
    bool HasPendingTx() const;

    // Non-blocking flush of queued outbound bytes.
    // Returns false if a fatal write error occurred and the connection should be closed.
    bool FlushTx();

    void UpdateLastActive();
    int64_t IdleSeconds() const;

private:
    int fd_;
    RingBuffer rx_buffer_;
    std::deque<std::vector<uint8_t>> tx_queue_;
    size_t tx_head_offset_ = 0;
    std::chrono::steady_clock::time_point last_active_;
};

}  // namespace gateway
