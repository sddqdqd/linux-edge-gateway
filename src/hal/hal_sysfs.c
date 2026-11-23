#include "hal/hal_device.h"

#include <stdio.h>
#include <string.h>

static char g_thermal_path[256] = "/sys/class/thermal/thermal_zone0/temp";
static char g_led_path[256] = "/sys/class/leds/red/brightness";

int hal_init(const hal_config_t* cfg) {
    if (cfg != NULL) {
        if (cfg->thermal_sysfs_path != NULL && cfg->thermal_sysfs_path[0] != '\0') {
            snprintf(g_thermal_path, sizeof(g_thermal_path), "%s", cfg->thermal_sysfs_path);
        }
        if (cfg->led_sysfs_path != NULL && cfg->led_sysfs_path[0] != '\0') {
            snprintf(g_led_path, sizeof(g_led_path), "%s", cfg->led_sysfs_path);
        }
    }
    return 0;
}

void hal_deinit(void) {
    /* Nothing to release in V1 sysfs backend. In V2, close(/dev/xxx) descriptors here. */
}

int hal_get_sys_status(hal_sys_status_t* out_status) {
    if (out_status == NULL) {
        return -1;
    }
    memset(out_status, 0, sizeof(*out_status));

    /*
     * TODO (Week 2):
     * 1. Open and read integer milli-Celsius value from `g_thermal_path`.
     *    (On x86 VM where thermal_zone0 may not exist, gracefully return a fallback/error.)
     * 2. Parse `MemTotal:` and `MemAvailable:` from `/proc/meminfo`.
     */
    return 0;
}

int hal_set_led(uint8_t led_id, uint8_t state, uint8_t* out_actual_state) {
    (void)led_id;
    /*
     * TODO (Week 2):
     * 1. Validate state (0 or 1).
     * 2. Write "0" or "1" to `g_led_path` via open()/write()/close() or fopen()/fputs()/fclose().
     * 3. Read back actual state into `*out_actual_state` if non-NULL.
     */
    if (out_actual_state != NULL) {
        *out_actual_state = (state != 0) ? 1 : 0;
    }
    return 0;
}
