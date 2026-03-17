#include <stdio.h>
#include <string.h>
#include "rgb.h"
#include "hid_keyboard.h"

/* ------------------------------------------------------------------ */
/* Mode name table                                                      */
/* ------------------------------------------------------------------ */

static const char *mode_names[RGB_MODE_COUNT] = {
    "Static",
    "Breathing",
    "Wave",
    "Reactive",
    "Rainbow",
    "Custom"
};

const char *rgb_mode_name(rgb_mode_t mode) {
    if ((int)mode < 0 || mode >= RGB_MODE_COUNT)
        return "Unknown";
    return mode_names[mode];
}

/* ------------------------------------------------------------------ */
/* Internal builder                                                     */
/* ------------------------------------------------------------------ */

/**
 * Build a CMD_RGB_SET_COLOR payload:
 *   byte 0   : key_id  (0xFF = all keys)
 *   bytes 1-3: R, G, B
 */
static int _send_color(hid_device_t *dev, uint8_t key_id,
                       uint8_t r, uint8_t g, uint8_t b) {
    uint8_t payload[4] = { key_id, r, g, b };
    return hid_keyboard_send_cmd(dev, CMD_RGB_SET_COLOR, payload, 4);
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

int rgb_set_all(hid_device_t *dev, uint8_t r, uint8_t g, uint8_t b) {
    printf("[rgb] Set all keys -> #%02X%02X%02X\n", r, g, b);
    return _send_color(dev, 0xFF, r, g, b);
}

int rgb_set_key(hid_device_t *dev, uint8_t key_id,
                uint8_t r, uint8_t g, uint8_t b) {
    printf("[rgb] Set key 0x%02X -> #%02X%02X%02X\n", key_id, r, g, b);
    return _send_color(dev, key_id, r, g, b);
}

int rgb_set_keys(hid_device_t *dev, const key_rgb_t *keys, uint16_t count) {
    if (!keys || count == 0) return -1;
    int err = 0;
    for (uint16_t i = 0; i < count; i++) {
        int ret = _send_color(dev, keys[i].key_id,
                              keys[i].color.r,
                              keys[i].color.g,
                              keys[i].color.b);
        if (ret < 0) {
            fprintf(stderr, "[rgb] Failed on key 0x%02X\n", keys[i].key_id);
            err = -1;
        }
    }
    return err;
}

int rgb_set_mode(hid_device_t *dev, rgb_mode_t mode, uint8_t speed) {
    if ((int)mode < 0 || mode >= RGB_MODE_COUNT) {
        fprintf(stderr, "[rgb] Invalid mode %d\n", (int)mode);
        return -1;
    }
    printf("[rgb] Set mode: %s  speed=%u\n", rgb_mode_name(mode), speed);
    uint8_t payload[2] = { (uint8_t)mode, speed };
    return hid_keyboard_send_cmd(dev, CMD_RGB_SET_MODE, payload, 2);
}

int rgb_set_brightness(hid_device_t *dev, uint8_t brightness) {
    printf("[rgb] Brightness: %u/255\n", brightness);
    return hid_keyboard_send_cmd(dev, CMD_RGB_BRIGHTNESS, &brightness, 1);
}

int rgb_off(hid_device_t *dev) {
    printf("[rgb] Turning off all LEDs\n");
    return rgb_set_all(dev, 0, 0, 0);
}
