#include "protocol/frame_codec.hpp"

namespace gateway {

uint16_t FrameCodec::CalcCrc16Modbus(const uint8_t* data, size_t len) {
    (void)data;
    (void)len;
    // TODO (Week 1):
    // Implement standard CRC-16/MODBUS:
    // - Init crc = 0xFFFF
    // - For each byte: crc ^= byte; repeat 8 times: if (crc & 1) crc = (crc >> 1) ^ 0xA001 else crc >>= 1;
    return 0;
}

std::vector<uint8_t> FrameCodec::Encode(const Frame& frame) {
    (void)frame;
    // TODO (Week 1):
    // Pack fields in Big-Endian order according to docs/protocol.md:
    // Magic(0xAA, 0x55) | Version | Cmd | Seq(BE) | PayloadLen(BE) | Payload | CRC16(BE)
    return {};
}

DecodeResult FrameCodec::TryDecode(RingBuffer& rb, Frame& out_frame) {
    (void)rb;
    (void)out_frame;
    // TODO (Week 1):
    // Implement the sliding-window decoder described in docs/protocol.md:
    // 1. Scan for Magic (0xAA 0x55), dropping leading garbage bytes.
    // 2. Check header availability (kFrameHeaderLen), validate version & payload_len <= kMaxPayloadLen.
    // 3. Wait until full frame (kFrameMinLen + payload_len) is available.
    // 4. Verify CRC16; on failure consume 1 byte and return kCrcError to resync.
    // 5. On success populate out_frame, consume full frame, return kOk.
    return DecodeResult::kNeedMoreData;
}

}  // namespace gateway
