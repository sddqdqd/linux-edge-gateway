#include "protocol/ring_buffer.hpp"

namespace gateway {

RingBuffer::RingBuffer(size_t capacity) : buffer_(capacity) {}

size_t RingBuffer::Write(const uint8_t* data, size_t len) {
    (void)data;
    (void)len;
    // TODO (Week 1):
    // Copy up to min(len, WritableBytes()) bytes starting at write_pos_,
    // handling wrap-around at buffer_.size() cleanly in at most two memcpy calls.
    return 0;
}

bool RingBuffer::Peek(uint8_t* out, size_t len, size_t offset) const {
    (void)out;
    (void)len;
    (void)offset;
    // TODO (Week 1):
    // Verify `offset + len <= size_`, then copy bytes starting from (read_pos_ + offset) % capacity
    // without modifying read_pos_ or size_.
    return false;
}

void RingBuffer::Consume(size_t len) {
    (void)len;
    // TODO (Week 1):
    // Advance read_pos_ by min(len, size_) modulo capacity and decrease size_.
}

void RingBuffer::Clear() {
    read_pos_ = 0;
    write_pos_ = 0;
    size_ = 0;
}

size_t RingBuffer::ReadableBytes() const {
    return size_;
}

size_t RingBuffer::WritableBytes() const {
    return buffer_.size() - size_;
}

size_t RingBuffer::Capacity() const {
    return buffer_.size();
}

}  // namespace gateway
