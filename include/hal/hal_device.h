#ifndef GATEWAY_HAL_DEVICE_H
#define GATEWAY_HAL_DEVICE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* thermal_sysfs_path;
    const char* led_sysfs_path;
} hal_config_t;

typedef struct {
    int32_t cpu_temp_millic;  /* CPU temperature in milli-Celsius (e.g., 45200 = 45.2 C) */
    uint32_t mem_total_kb;    /* Total memory in KB */
    uint32_t mem_avail_kb;    /* Available memory in KB */
} hal_sys_status_t;

/*
 * Initialize HAL layer.
 * V1: store sysfs node paths (hal_sysfs.c).
 * V2: open custom /dev/xxx character/I2C/SPI device nodes (hal_driver.c).
 * Returns 0 on success, negative errno-like code on failure.
 */
int hal_init(const hal_config_t* cfg);

/* Release HAL resources on shutdown. */
void hal_deinit(void);

/* Read CPU temperature and memory info. Returns 0 on success, < 0 on error. */
int hal_get_sys_status(hal_sys_status_t* out_status);

/* Control board LED (state: 0 = OFF, 1 = ON). Returns 0 on success, < 0 on error. */
int hal_set_led(uint8_t led_id, uint8_t state, uint8_t* out_actual_state);

#ifdef __cplusplus
}
#endif

#endif /* GATEWAY_HAL_DEVICE_H */
