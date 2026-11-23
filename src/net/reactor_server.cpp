#include "net/reactor_server.hpp"

#include <unistd.h>

namespace gateway {

ReactorServer::ReactorServer(const GatewayConfig& config) : config_(config) {}

ReactorServer::~ReactorServer() {
    Stop();
    if (wakeup_fd_ >= 0) ::close(wakeup_fd_);
    if (epoll_fd_ >= 0) ::close(epoll_fd_);
    if (listen_fd_ >= 0) ::close(listen_fd_);
}

bool ReactorServer::Init() {
    // TODO (Week 2):
    // 1. Create non-blocking listening TCP socket (SO_REUSEADDR), bind(), listen().
    // 2. Create epoll_fd_ via epoll_create1(EPOLL_CLOEXEC).
    // 3. Create wakeup_fd_ via eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC).
    // 4. Register listen_fd_ and wakeup_fd_ into epoll_fd_ with EPOLLIN.
    // 5. Create thread_pool_ with config_.worker_threads workers.
    return true;
}

void ReactorServer::Run() {
    running_.store(true);
    // TODO (Week 2):
    // Main epoll_wait loop (e.g. 1000 ms timeout for periodic heartbeat sweeping):
    // - Dispatch listen_fd_ -> HandleAccept()
    // - Dispatch wakeup_fd_ -> HandleWakeup()
    // - Dispatch client fd EPOLLIN -> HandleRead(fd), EPOLLOUT -> HandleWrite(fd), errors -> CloseConnection(fd)
    // - Periodically call SweepTimeoutConnections()
    running_.store(false);
}

void ReactorServer::Stop() {
    bool expected = true;
    if (running_.compare_exchange_strong(expected, false)) {
        // TODO (Week 2): Write uint64_t(1) to wakeup_fd_ to unblock epoll_wait immediately.
    }
}

void ReactorServer::HandleAccept() {
    // TODO (Week 2):
    // Accept loop (non-blocking) until EAGAIN; enforce max_connections;
    // set client fd non-blocking; create Connection and register EPOLLIN in epoll_fd_.
}

void ReactorServer::HandleRead(int client_fd) {
    (void)client_fd;
    // TODO (Week 2):
    // Read available bytes into Connection::rx_buffer() (handle EOF / EAGAIN / error).
    // Loop FrameCodec::TryDecode():
    // - On kOk: update active time, increment rx_frames_, submit ProcessRequest() to thread_pool_.
    // - On kCrcError: increment crc_errors_ and continue loop.
}

void ReactorServer::HandleWrite(int client_fd) {
    (void)client_fd;
    // TODO (Week 2):
    // Call conn->FlushTx(); if empty, remove EPOLLOUT from epoll interest list;
    // if fatal error, CloseConnection(client_fd).
}

void ReactorServer::HandleWakeup() {
    // TODO (Week 2):
    // Drain wakeup_fd_ (read uint64_t), swap resp_queue_ under resp_mutex_,
    // append bytes to target connections (if still alive), flush or arm EPOLLOUT.
}

void ReactorServer::CloseConnection(int client_fd) {
    (void)client_fd;
    // TODO (Week 2):
    // epoll_ctl(DEL) and erase from connections_ (Connection destructor closes fd automatically).
}

void ReactorServer::SweepTimeoutConnections() {
    // TODO (Week 2):
    // Iterate connections_ safely; close any connection with IdleSeconds() > config_.heartbeat_timeout_sec.
}

Frame ReactorServer::ProcessRequest(const Frame& req) {
    Frame resp;
    resp.version = kProtocolVersionV1;
    resp.cmd = static_cast<uint8_t>(req.cmd | static_cast<uint8_t>(Command::kResponseMask));
    resp.seq = req.seq;

    // TODO (Week 2):
    // Dispatch based on req.cmd (kHeartbeatReq, kSysStatusReq, kLedControlReq, kGatewayStatsReq)
    // and call hal_get_sys_status() / hal_set_led() for hardware commands.
    resp.payload.push_back(static_cast<uint8_t>(StatusCode::kErrUnknownCmd));
    return resp;
}

void ReactorServer::PostResponse(int client_fd, std::vector<uint8_t> encoded_frame) {
    {
        std::lock_guard<std::mutex> lock(resp_mutex_);
        resp_queue_.emplace(client_fd, std::move(encoded_frame));
    }
    // TODO (Week 2): Write uint64_t(1) to wakeup_fd_ so Reactor thread wakes up to send.
}

}  // namespace gateway
