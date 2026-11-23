#include "common/config.hpp"

namespace gateway {

bool LoadConfigFromFile(const std::string& file_path, GatewayConfig& out_config) {
    (void)file_path;
    (void)out_config;
    // TODO (Week 3):
    // 1. Read entire JSON file into std::string.
    // 2. Parse with cJSON_Parse() (wrap cJSON* in a small RAII guard or std::unique_ptr with custom deleter).
    // 3. Extract "server", "log", "hal" fields into `out_config` with type checks.
    return false;
}

}  // namespace gateway
