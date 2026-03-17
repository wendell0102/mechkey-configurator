#ifndef MACROS_H
#define MACROS_H

#include <stdint.h>
#include "hid_keyboard.h"

/* ------------------------------------------------------------------ */
/* Macro management API                                                 */
/* ------------------------------------------------------------------ */

/**
 * Send a macro definition to the keyboard firmware.
 * The firmware stores it and can replay it on trigger.
 */
int macro_send(hid_device_t *dev, const macro_t *macro);

/**
 * Delete a macro by ID from the keyboard firmware.
 */
int macro_delete(hid_device_t *dev, uint8_t macro_id);

/**
 * Request a list of stored macros (reads response into out_macros).
 * Returns number of macros retrieved, or -1 on error.
 */
int macro_list(hid_device_t *dev, macro_t *out_macros, uint8_t max_count);

/**
 * Print a macro's details to stdout (for debugging / CLI display).
 */
void macro_print(const macro_t *macro);

/* ------------------------------------------------------------------ */
/* Shortcut (key remap) API                                            */
/* ------------------------------------------------------------------ */

/**
 * Program a key remap / shortcut on the keyboard.
 */
int shortcut_set(hid_device_t *dev, const shortcut_t *sc);

/**
 * Remove a previously programmed shortcut by source keycode.
 */
int shortcut_delete(hid_device_t *dev, uint8_t src_keycode,
                    uint8_t src_modifiers);

/**
 * Print shortcut info to stdout.
 */
void shortcut_print(const shortcut_t *sc);

/* ------------------------------------------------------------------ */
/* Profile management                                                   */
/* ------------------------------------------------------------------ */

/** Save current configuration to profile slot (0-based). */
int profile_save(hid_device_t *dev, uint8_t slot);

/** Load configuration from profile slot. */
int profile_load(hid_device_t *dev, uint8_t slot);

#endif /* MACROS_H */
