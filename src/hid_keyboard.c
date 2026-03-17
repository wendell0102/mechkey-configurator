#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hid_keyboard.h"

/* ------------------------------------------------------------
 * Platform abstraction layer
 * This file uses HIDAPI (https://github.com/libusb/hidapi)
 * On Linux:   link with -lhidapi-hidraw  (or -lhidapi-libusb)
 * On macOS:   link with -lhidapi
 * On Windows: link with hidapi.lib
 * ------------------------------------------------------------ */

#ifdef _WIN32
#  include <windows.h>
#endif

/* Forward-declare the HIDAPI types so we don't need the header
 * directly — callers only see our opaque hid_device_t. */
typedef void  hid_device_raw;

extern hid_device_raw *hid_open(unsigned short vendor_id,
                                unsigned short product_id,
                                const wchar_t *serial);
extern void            hid_close(hid_device_raw *dev);
extern int             hid_write(hid_device_raw *dev,
                                 const unsigned char *data, size_t len);
extern int             hid_read_timeout(hid_device_raw *dev,
                                        unsigned char *data, size_t length,
                                        int milliseconds);
extern int             hid_init(void);
extern int             hid_exit(void);

struct hid_device_s {
    hid_device_raw *raw;
    uint16_t        vendor_id;
    uint16_t        product_id;
    uint8_t         report_buf[65]; /* 64-byte HID + 1 report-ID byte */
};

/* ------------------------------------------------------------------ */
/* Internal helpers                                                     */
/* ------------------------------------------------------------------ */

static void _hid_buf_clear(hid_device_t *dev) {
    memset(dev->report_buf, 0, sizeof(dev->report_buf));
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

hid_device_t *hid_keyboard_open(uint16_t vendor_id, uint16_t product_id) {
    static int hidapi_init = 0;
    if (!hidapi_init) {
        if (hid_init() != 0) {
            fprintf(stderr, "[mechkey] hid_init() failed\n");
            return NULL;
        }
        hidapi_init = 1;
    }

    hid_device_raw *raw = hid_open(vendor_id, product_id, NULL);
    if (!raw) {
        fprintf(stderr, "[mechkey] Cannot open device %04x:%04x\n",
                vendor_id, product_id);
        return NULL;
    }

    hid_device_t *dev = (hid_device_t *)calloc(1, sizeof(hid_device_t));
    if (!dev) {
        hid_close(raw);
        return NULL;
    }

    dev->raw        = raw;
    dev->vendor_id  = vendor_id;
    dev->product_id = product_id;
    printf("[mechkey] Opened device %04x:%04x\n", vendor_id, product_id);
    return dev;
}

void hid_keyboard_close(hid_device_t *dev) {
    if (!dev) return;
    hid_close(dev->raw);
    free(dev);
    hid_exit();
}

int hid_keyboard_send_cmd(hid_device_t *dev, uint8_t cmd,
                          const uint8_t *data, uint16_t len) {
    if (!dev || !dev->raw) return -1;
    if (len > 62) {
        fprintf(stderr, "[mechkey] Payload too large (%u bytes, max 62)\n", len);
        return -1;
    }

    _hid_buf_clear(dev);
    dev->report_buf[0] = 0x00; /* Report ID (0 = no report ID) */
    dev->report_buf[1] = cmd;
    dev->report_buf[2] = (uint8_t)(len & 0xFF);
    if (data && len > 0)
        memcpy(&dev->report_buf[3], data, len);

    int ret = hid_write(dev->raw, dev->report_buf, 65);
    if (ret < 0) {
        fprintf(stderr, "[mechkey] hid_write() failed for cmd 0x%02x\n", cmd);
        return -1;
    }
    return 0;
}

int hid_keyboard_read(hid_device_t *dev, uint8_t *buf, uint16_t maxlen) {
    if (!dev || !buf) return -1;
    int n = hid_read_timeout(dev->raw, buf,
                             maxlen < 64 ? maxlen : 64, 500);
    if (n < 0)
        fprintf(stderr, "[mechkey] hid_read_timeout() failed\n");
    return n;
}

void hid_keyboard_enumerate(void) {
    /* We use a simplified approach: try common gaming keyboard VID/PIDs */
    static const struct {
        uint16_t vid;
        uint16_t pid;
        const char *name;
    } known[] = {
        { 0x046D, 0xC335, "Logitech G413" },
        { 0x046D, 0xC336, "Logitech G512" },
        { 0x046D, 0xC33F, "Logitech G815" },
        { 0x1B1C, 0x1B13, "Corsair K70" },
        { 0x1B1C, 0x1B55, "Corsair K60" },
        { 0x04D9, 0x0169, "Drevo Excalibur" },
        { 0x04F2, 0x0111, "Redragon K552" },
        { 0x258A, 0x0049, "Glorious GMMK" },
        { 0x0000, 0x0000, NULL }
    };

    printf("[mechkey] Scanning for known mechanical keyboards...\n");
    int found = 0;
    for (int i = 0; known[i].name != NULL; i++) {
        hid_device_raw *d = hid_open(known[i].vid, known[i].pid, NULL);
        if (d) {
            printf("  [FOUND] %04x:%04x  %s\n",
                   known[i].vid, known[i].pid, known[i].name);
            hid_close(d);
            found++;
        }
    }
    if (found == 0)
        printf("  [NONE]  No known keyboard found. Connect device and retry.\n");
}
