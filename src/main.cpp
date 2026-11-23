#include <csignal>
#include <iostream>
#include <string>

#include "common/config.hpp"
#include "hal/hal_device.h"
#include "net/reactor_server.hpp"

namespace {

gateway::ReactorServer* g_server = nullptr;

void SignalHandler(int /*signum*/) {
    // Note: Only async-signal-safe operations should happen here.
    if (g_server != nullptr) {
        g_server->Stop();
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string config_path = "config/gateway.json";
    bool run_as_daemon = false;

    // Simple CLI argument parsing: --config <path> --daemon
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            config_path = argv[++i];
        } else if (arg == "--daemon") {
            run_as_daemon = true;
        }
    }

    gateway::GatewayConfig config;
    if (!gateway::LoadConfigFromFile(config_path, config)) {
        std::cerr << "[WARN] Failed to load config from " << config_path
                  << ", falling back to defaults." << std::endl;
    }

    if (run_as_daemon) {
        // TODO (Week 3): Implement daemonize() using fork(), setsid(), chdir(), close stdio.
        std::cout << "[INFO] Daemon mode requested (TODO)." << std::endl;
    }

    // TODO (Week 3): Initialize logging library (rxi/log.c) here.

    hal_config_t hal_cfg{};
    hal_cfg.thermal_sysfs_path = config.thermal_sysfs_path.c_str();
    hal_cfg.led_sysfs_path = config.led_sysfs_path.c_str();
    if (hal_init(&hal_cfg) != 0) {
        std::cerr << "[WARN] HAL init returned non-zero (expected on x86 host without board sysfs)."
                  << std::endl;
    }

    gateway::ReactorServer server(config);
    g_server = &server;

    // Register graceful shutdown handlers and ignore SIGPIPE to prevent abrupt exit on broken socket.
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    std::signal(SIGPIPE, SIG_IGN);

    if (!server.Init()) {
        std::cerr << "[ERROR] Failed to initialize ReactorServer." << std::endl;
        hal_deinit();
        return 1;
    }

    std::cout << "[INFO] Edge Gateway starting on " << config.listen_ip << ":"
              << config.listen_port << std::endl;
    server.Run();

    g_server = nullptr;
    hal_deinit();
    std::cout << "[INFO] Edge Gateway exited cleanly." << std::endl;
    return 0;
}
