#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "protocol/ring_buffer.hpp"

namespace gateway {

// Protocol constants (see docs/protocol.md)
constexpr uint8_t kFrameMagicHigh = 0xAA;
constexpr uint8_t kFrameMagicLow = 0x55;
constexpr uint8_t kProtocolVersionV1 = 0x01;
constexpr size_t kFrameHeaderLen = 8;   // Magic(2) + Ver(1) + Cmd(1) + Seq(2) + Len(2)
constexpr size_t kFrameCrcLen = 2;      // CRC16(2)
constexpr size_t kFrameMinLen = kFrameHeaderLen + kFrameCrcLen;
constexpr uint16_t kMaxPayloadLen = 4096;

enum class Command : uint8_t {
    // V1 Request Commands
    kHeartbeatReq = 0x01,
    kSysStatusReq = 0x02,
    kLedControlReq = 0x03,
    kGatewayStatsReq = 0x04,

    // V2 Reserved Request Commands
    kSensorDataReq = 0x10,
    kCacheStatusReq = 0x20,
    kCacheFlushReq = 0x21,

    // Response flag bit (RespCmd = ReqCmd | 0x80)
    kResponseMask = 0x80,
};

enum class StatusCode : uint8_t {
    kOk = 0x00,
    kErrUnknownCmd = 0x01,
    kErrInvalidParam = 0x02,
    kErrHalFailed = 0x03,
    kErrInternal = 0x04,
};

struct Frame {
    uint8_t version = kProtocolVersionV1;
    uint8_t cmd = 0;
    uint16_t seq = 0;
    std::vector<uint8_t> payload;
};

enum class DecodeResult {
    kOk,            // One complete and valid frame decoded
    kNeedMoreData,  // Incomplete frame; wait for more bytes from socket
    kCrcError,      // Frame header matched but CRC mismatch (byte dropped to resync)
    kInvalidHeader, // Invalid version or payload length (byte dropped to resync)
};

class FrameCodec {
public:
    // Calculate CRC-16/MODBUS (poly 0xA001, init 0xFFFF).
    static uint16_t CalcCrc16Modbus(const uint8_t* data, size_t len);

    // Encode a Frame into wire bytes (Big-Endian).
    static std::vector<uint8_t> Encode(const Frame& frame);

    // Attempt to decode a single Frame from RingBuffer.
    // Call in a loop until kNeedMoreData to drain all sticky packets.
    static DecodeResult TryDecode(RingBuffer& rb, Frame& out_frame);
};

}  // namespace gateway
