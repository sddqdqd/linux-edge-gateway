#include "net/connection.hpp"

#include <unistd.h>

namespace gateway {

Connection::Connection(int fd)
    : fd_(fd), last_active_(std::chrono::steady_clock::now()) {}

Connection::~Connection() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

void Connection::AppendTxData(std::vector<uint8_t> data) {
    if (!data.empty()) {
        tx_queue_.push_back(std::move(data));
    }
}

bool Connection::HasPendingTx() const {
    return !tx_queue_.empty();
}

bool Connection::FlushTx() {
    // TODO (Week 2):
    // Loop while !tx_queue_.empty():
    // - Call ::write(fd_, front.data() + tx_head_offset_, remaining).
    // - If n > 0: advance tx_head_offset_; pop front when complete.
    // - If n < 0 and errno == EINTR: retry.
    // - If n < 0 and (errno == EAGAIN || errno == EWOULDBLOCK): break and wait for EPOLLOUT.
    // - Otherwise: return false so Reactor closes the connection.
    return true;
}

void Connection::UpdateLastActive() {
    last_active_ = std::chrono::steady_clock::now();
}

int64_t Connection::IdleSeconds() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - last_active_).count();
}

}  // namespace gateway
