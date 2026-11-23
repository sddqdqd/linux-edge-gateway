#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace gateway {

// Fixed-capacity byte ring buffer for TCP stream reassembly.
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity = 16384);

    // Append up to `len` bytes into the buffer. Returns actual bytes written.
    size_t Write(const uint8_t* data, size_t len);

    // Peek `len` bytes starting at `offset` from read head without consuming.
    // Returns false if readable bytes < offset + len.
    bool Peek(uint8_t* out, size_t len, size_t offset = 0) const;

    // Discard `len` bytes from the read head.
    void Consume(size_t len);

    void Clear();

    size_t ReadableBytes() const;
    size_t WritableBytes() const;
    size_t Capacity() const;

private:
    std::vector<uint8_t> buffer_;
    size_t read_pos_ = 0;
    size_t write_pos_ = 0;
    size_t size_ = 0;
};

}  // namespace gateway
