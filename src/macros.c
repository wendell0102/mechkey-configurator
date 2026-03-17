#include <stdio.h>
#include <string.h>
#include "macros.h"
#include "hid_keyboard.h"

/* ------------------------------------------------------------------ */
/* Macro serialisation helpers                                          */
/* ------------------------------------------------------------------ */

/**
 * Serialise a macro_t into a flat byte buffer for the HID payload.
 * Layout:
 *   [0]      macro id
 *   [1]      step_count
 *   [2..33]  name (MAX_MACRO_NAME bytes)
 *   [34+]    steps: each step = 5 bytes (keycode, modifiers, delay_lo,
 *                                        delay_hi, down)
 * Returns total bytes written, or -1 if buffer too small.
 */
static int _macro_serialise(const macro_t *m, uint8_t *buf, int buflen) {
    int required = 2 + MAX_MACRO_NAME + (int)m->step_count * 5;
    if (required > buflen) return -1;

    int pos = 0;
    buf[pos++] = m->id;
    buf[pos++] = m->step_count;
    memcpy(&buf[pos], m->name, MAX_MACRO_NAME);
    pos += MAX_MACRO_NAME;

    for (uint8_t i = 0; i < m->step_count; i++) {
        const macro_step_t *s = &m->steps[i];
        buf[pos++] = s->keycode;
        buf[pos++] = s->modifiers;
        buf[pos++] = (uint8_t)(s->delay_ms & 0xFF);
        buf[pos++] = (uint8_t)((s->delay_ms >> 8) & 0xFF);
        buf[pos++] = s->down;
    }
    return pos;
}

/* ------------------------------------------------------------------ */
/* Macro API                                                            */
/* ------------------------------------------------------------------ */

int macro_send(hid_device_t *dev, const macro_t *macro) {
    if (!macro) return -1;

    uint8_t payload[62]; /* max HID vendor payload */
    int len = _macro_serialise(macro, payload, (int)sizeof(payload));
    if (len < 0) {
        fprintf(stderr, "[macro] Macro '%s' too large to fit in one HID packet\n",
                macro->name);
        return -1;
    }

    printf("[macro] Sending macro id=%u name='%s' steps=%u\n",
           macro->id, macro->name, macro->step_count);
    return hid_keyboard_send_cmd(dev, CMD_MACRO_SET, payload, (uint16_t)len);
}

int macro_delete(hid_device_t *dev, uint8_t macro_id) {
    printf("[macro] Deleting macro id=%u\n", macro_id);
    return hid_keyboard_send_cmd(dev, CMD_MACRO_DELETE, &macro_id, 1);
}

int macro_list(hid_device_t *dev, macro_t *out_macros, uint8_t max_count) {
    (void)out_macros; (void)max_count;
    int ret = hid_keyboard_send_cmd(dev, CMD_MACRO_LIST, NULL, 0);
    if (ret < 0) return -1;
    /* In a real driver we would read and parse the firmware response here. */
    printf("[macro] List requested (parse firmware response to populate macros)\n");
    return 0;
}

void macro_print(const macro_t *macro) {
    if (!macro) return;
    printf("Macro id=%u  name='%s'  steps=%u\n",
           macro->id, macro->name, macro->step_count);
    for (uint8_t i = 0; i < macro->step_count; i++) {
        const macro_step_t *s = &macro->steps[i];
        printf("  [%2u] %s key=0x%02X mod=0x%02X delay=%ums\n",
               i, s->down ? "DOWN" : "UP  ",
               s->keycode, s->modifiers, s->delay_ms);
    }
}

/* ------------------------------------------------------------------ */
/* Shortcut API                                                         */
/* ------------------------------------------------------------------ */

int shortcut_set(hid_device_t *dev, const shortcut_t *sc) {
    if (!sc) return -1;
    printf("[shortcut] Map key=0x%02X mod=0x%02X -> action=%d key=0x%02X mod=0x%02X\n",
           sc->src_keycode, sc->src_modifiers,
           (int)sc->action_type,
           sc->dst_keycode, sc->dst_modifiers);

    uint8_t payload[6] = {
        sc->src_keycode,
        sc->src_modifiers,
        (uint8_t)sc->action_type,
        sc->dst_keycode,
        sc->dst_modifiers,
        sc->macro_id
    };
    return hid_keyboard_send_cmd(dev, CMD_SHORTCUT_SET, payload, 6);
}

int shortcut_delete(hid_device_t *dev, uint8_t src_keycode,
                    uint8_t src_modifiers) {
    printf("[shortcut] Delete key=0x%02X mod=0x%02X\n",
           src_keycode, src_modifiers);
    uint8_t payload[2] = { src_keycode, src_modifiers };
    return hid_keyboard_send_cmd(dev, CMD_SHORTCUT_DELETE, payload, 2);
}

void shortcut_print(const shortcut_t *sc) {
    if (!sc) return;
    const char *types[] = { "Standard", "Macro", "Shortcut", "Media", "Layer" };
    printf("Shortcut: src=0x%02X+0x%02X  action=%s  dst=0x%02X+0x%02X  macro=%u\n",
           sc->src_keycode, sc->src_modifiers,
           (sc->action_type < 5 ? types[sc->action_type] : "?"),
           sc->dst_keycode, sc->dst_modifiers, sc->macro_id);
}

/* ------------------------------------------------------------------ */
/* Profile API                                                          */
/* ------------------------------------------------------------------ */

int profile_save(hid_device_t *dev, uint8_t slot) {
    printf("[profile] Saving to slot %u\n", slot);
    return hid_keyboard_send_cmd(dev, CMD_PROFILE_SAVE, &slot, 1);
}

int profile_load(hid_device_t *dev, uint8_t slot) {
    printf("[profile] Loading slot %u\n", slot);
    return hid_keyboard_send_cmd(dev, CMD_PROFILE_LOAD, &slot, 1);
}
