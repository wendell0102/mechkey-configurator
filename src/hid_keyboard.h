#ifndef HID_KEYBOARD_H
#define HID_KEYBOARD_H

#include <stdint.h>

/* HID Usage Page and Usage IDs for keyboards */
#define HID_USAGE_PAGE_GENERIC_DESKTOP  0x01
#define HID_USAGE_PAGE_KEYBOARD         0x07
#define HID_USAGE_PAGE_LED              0x08
#define HID_USAGE_PAGE_BUTTON           0x09
#define HID_USAGE_PAGE_VENDOR           0xFF00

/* Vendor-specific command types */
#define CMD_RGB_SET_COLOR    0x01
#define CMD_RGB_SET_MODE     0x02
#define CMD_RGB_BRIGHTNESS   0x03
#define CMD_MACRO_SET        0x10
#define CMD_MACRO_DELETE     0x11
#define CMD_MACRO_LIST       0x12
#define CMD_SHORTCUT_SET     0x20
#define CMD_SHORTCUT_DELETE  0x21
#define CMD_PROFILE_SAVE     0x30
#define CMD_PROFILE_LOAD     0x31
#define CMD_DEVICE_INFO      0x40

/* RGB modes */
typedef enum {
    RGB_MODE_STATIC = 0,
    RGB_MODE_BREATHING,
    RGB_MODE_WAVE,
    RGB_MODE_REACTIVE,
    RGB_MODE_RAINBOW,
    RGB_MODE_CUSTOM,
    RGB_MODE_COUNT
} rgb_mode_t;

/* Key action types */
typedef enum {
    KEY_ACTION_STANDARD = 0,
    KEY_ACTION_MACRO,
    KEY_ACTION_SHORTCUT,
    KEY_ACTION_MEDIA,
    KEY_ACTION_LAYER
} key_action_t;

/* RGB color struct */
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_color_t;

/* Per-key RGB config */
typedef struct {
    uint8_t key_id;
    rgb_color_t color;
} key_rgb_t;

/* Macro step (keypress event) */
typedef struct {
    uint8_t  keycode;
    uint8_t  modifiers; /* Ctrl, Shift, Alt, GUI */
    uint16_t delay_ms;
    uint8_t  down;      /* 1=press, 0=release */
} macro_step_t;

/* Macro definition */
#define MAX_MACRO_STEPS  64
#define MAX_MACRO_NAME   32
typedef struct {
    uint8_t      id;
    char         name[MAX_MACRO_NAME];
    uint8_t      step_count;
    macro_step_t steps[MAX_MACRO_STEPS];
} macro_t;

/* Shortcut / key remap */
typedef struct {
    uint8_t      src_keycode;
    uint8_t      src_modifiers;
    key_action_t action_type;
    uint8_t      dst_keycode;
    uint8_t      dst_modifiers;
    uint8_t      macro_id;   /* used when action_type == KEY_ACTION_MACRO */
} shortcut_t;

/* HID device handle (opaque) */
typedef struct hid_device_s hid_device_t;

/* Open / close a keyboard by vendor+product ID */
hid_device_t *hid_keyboard_open(uint16_t vendor_id, uint16_t product_id);
void          hid_keyboard_close(hid_device_t *dev);

/* Send a raw vendor command (cmd, payload, len) */
int hid_keyboard_send_cmd(hid_device_t *dev, uint8_t cmd,
                          const uint8_t *data, uint16_t len);

/* Read a response (returns bytes read, or -1 on error) */
int hid_keyboard_read(hid_device_t *dev, uint8_t *buf, uint16_t maxlen);

/* Enumerate connected HID keyboards */
void hid_keyboard_enumerate(void);

#endif /* HID_KEYBOARD_H */
