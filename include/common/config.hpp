#pragma once

#include <cstdint>
#include <string>

namespace gateway {

struct GatewayConfig {
    // Server settings
    std::string listen_ip = "0.0.0.0";
    uint16_t listen_port = 9000;
    size_t worker_threads = 4;
    int heartbeat_timeout_sec = 30;
    size_t max_connections = 128;

    // Logging settings
    std::string log_level = "INFO";
    std::string log_file_path = "./logs/gateway.log";
    bool log_enable_console = true;

    // HAL sysfs paths (V1)
    std::string thermal_sysfs_path = "/sys/class/thermal/thermal_zone0/temp";
    std::string led_sysfs_path = "/sys/class/leds/red/brightness";
};

// Load configuration from a JSON file.
// Returns true on success; on failure, retains default values and returns false.
bool LoadConfigFromFile(const std::string& file_path, GatewayConfig& out_config);

}  // namespace gateway
