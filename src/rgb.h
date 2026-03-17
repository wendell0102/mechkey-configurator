#ifndef RGB_H
#define RGB_H

#include <stdint.h>
#include "hid_keyboard.h"

/* ------------------------------------------------------------------ */
/* RGB control API                                                      */
/* ------------------------------------------------------------------ */

/**
 * Set a uniform color for all keys.
 * @param dev   Open keyboard handle
 * @param r,g,b Color values (0-255)
 * @return 0 on success, -1 on error
 */
int rgb_set_all(hid_device_t *dev, uint8_t r, uint8_t g, uint8_t b);

/**
 * Set color for a single key identified by key_id.
 */
int rgb_set_key(hid_device_t *dev, uint8_t key_id,
                uint8_t r, uint8_t g, uint8_t b);

/**
 * Apply a batch of per-key colors.
 * @param keys     Array of key_rgb_t structs
 * @param count    Number of entries
 */
int rgb_set_keys(hid_device_t *dev, const key_rgb_t *keys, uint16_t count);

/**
 * Set the RGB lighting mode.
 * @param mode   One of rgb_mode_t values
 * @param speed  Effect speed (0=slowest, 255=fastest)
 */
int rgb_set_mode(hid_device_t *dev, rgb_mode_t mode, uint8_t speed);

/**
 * Set global brightness.
 * @param brightness 0=off, 255=maximum
 */
int rgb_set_brightness(hid_device_t *dev, uint8_t brightness);

/**
 * Turn off all RGB lighting.
 */
int rgb_off(hid_device_t *dev);

/* ------------------------------------------------------------------ */
/* Convenience: predefined colors                                       */
/* ------------------------------------------------------------------ */

#define RGB_RED       255, 0,   0
#define RGB_GREEN     0,   255, 0
#define RGB_BLUE      0,   0,   255
#define RGB_WHITE     255, 255, 255
#define RGB_OFF       0,   0,   0
#define RGB_CYAN      0,   255, 255
#define RGB_MAGENTA   255, 0,   255
#define RGB_YELLOW    255, 255, 0
#define RGB_ORANGE    255, 128, 0
#define RGB_PURPLE    128, 0,   255

/* Helper macro: create an rgb_color_t literal */
#define RGB_COLOR(r,g,b)  ((rgb_color_t){(r),(g),(b)})

/* Return mode name as a C string */
const char *rgb_mode_name(rgb_mode_t mode);

#endif /* RGB_H */
