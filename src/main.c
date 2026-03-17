/*
 * mechkey-configurator - Universal Mechanical Keyboard Configurator
 * CLI entry point
 *
 * Usage:
 *   mechkey [options] <command> [args]
 *
 * Commands:
 *   list                            - List connected keyboards
 *   rgb all <R> <G> <B>             - Set all keys to color
 *   rgb key <KEY_ID> <R> <G> <B>   - Set a single key color
 *   rgb mode <MODE> [SPEED]         - Set lighting mode
 *   rgb brightness <0-255>          - Set brightness
 *   rgb off                         - Turn off RGB
 *   macro add <ID> <NAME> <steps>   - Add macro (steps from file)
 *   macro del <ID>                  - Delete macro
 *   macro list                      - List macros
 *   shortcut set <SRC> <MOD> <DST>  - Remap key
 *   shortcut del <SRC> <MOD>        - Delete remap
 *   profile save <SLOT>             - Save config to slot
 *   profile load <SLOT>             - Load config from slot
 *
 * Options:
 *   -v <VID>   Vendor ID  (hex, e.g. 046D)
 *   -p <PID>   Product ID (hex, e.g. C335)
 *   -h         Show this help
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "hid_keyboard.h"
#include "rgb.h"
#include "macros.h"

/* ------------------------------------------------------------------ */
/* Helpers                                                              */
/* ------------------------------------------------------------------ */

static uint16_t parse_hex(const char *s) {
    unsigned long v = strtoul(s, NULL, 16);
    return (uint16_t)(v & 0xFFFF);
}

static uint8_t parse_u8(const char *s) {
    return (uint8_t)(strtoul(s, NULL, 0) & 0xFF);
}

static void print_usage(const char *prog) {
    printf(
        "mechkey-configurator v1.0\n"
        "Universal mechanical keyboard configurator via USB HID\n\n"
        "Usage: %s [-v VID] [-p PID] <command> [args]\n\n"
        "Commands:\n"
        "  list                           List connected HID keyboards\n"
        "  rgb all <R> <G> <B>            Set all-key color (0-255 each)\n"
        "  rgb key <KEY_ID> <R> <G> <B>  Set per-key color\n"
        "  rgb mode <MODE> [SPEED]        Set mode (0=Static 1=Breathing\n"
        "                                 2=Wave 3=Reactive 4=Rainbow)\n"
        "  rgb brightness <0-255>         Set global brightness\n"
        "  rgb off                        Turn off all LEDs\n"
        "  macro del <ID>                 Delete macro by ID\n"
        "  macro list                     List stored macros\n"
        "  shortcut set <SRC> <MOD> <DST_KEY> <DST_MOD>  Remap key\n"
        "  shortcut del <SRC> <MOD>       Remove remap\n"
        "  profile save <SLOT>            Save config to profile slot\n"
        "  profile load <SLOT>            Load config from profile slot\n"
        "\nOptions:\n"
        "  -v <VID>   Vendor ID  in hex (default: 046D = Logitech)\n"
        "  -p <PID>   Product ID in hex (default: C335 = G413)\n"
        "  -h         Show this help\n",
        prog
    );
}

/* ------------------------------------------------------------------ */
/* Command handlers                                                     */
/* ------------------------------------------------------------------ */

static int cmd_list(void) {
    hid_keyboard_enumerate();
    return 0;
}

static int cmd_rgb(hid_device_t *dev, int argc, char **argv) {
    if (argc < 1) { fprintf(stderr, "rgb: missing subcommand\n"); return 1; }

    if (strcmp(argv[0], "all") == 0) {
        if (argc < 4) { fprintf(stderr, "rgb all <R> <G> <B>\n"); return 1; }
        return rgb_set_all(dev, parse_u8(argv[1]), parse_u8(argv[2]), parse_u8(argv[3]));

    } else if (strcmp(argv[0], "key") == 0) {
        if (argc < 5) { fprintf(stderr, "rgb key <KEY_ID> <R> <G> <B>\n"); return 1; }
        return rgb_set_key(dev, parse_u8(argv[1]),
                           parse_u8(argv[2]), parse_u8(argv[3]), parse_u8(argv[4]));

    } else if (strcmp(argv[0], "mode") == 0) {
        if (argc < 2) { fprintf(stderr, "rgb mode <MODE> [SPEED]\n"); return 1; }
        rgb_mode_t mode = (rgb_mode_t)parse_u8(argv[1]);
        uint8_t speed = (argc >= 3) ? parse_u8(argv[2]) : 128;
        return rgb_set_mode(dev, mode, speed);

    } else if (strcmp(argv[0], "brightness") == 0) {
        if (argc < 2) { fprintf(stderr, "rgb brightness <0-255>\n"); return 1; }
        return rgb_set_brightness(dev, parse_u8(argv[1]));

    } else if (strcmp(argv[0], "off") == 0) {
        return rgb_off(dev);
    }

    fprintf(stderr, "rgb: unknown subcommand '%s'\n", argv[0]);
    return 1;
}

static int cmd_macro(hid_device_t *dev, int argc, char **argv) {
    if (argc < 1) { fprintf(stderr, "macro: missing subcommand\n"); return 1; }

    if (strcmp(argv[0], "del") == 0) {
        if (argc < 2) { fprintf(stderr, "macro del <ID>\n"); return 1; }
        return macro_delete(dev, parse_u8(argv[1]));

    } else if (strcmp(argv[0], "list") == 0) {
        return macro_list(dev, NULL, 0);
    }

    fprintf(stderr, "macro: unknown subcommand '%s'\n", argv[0]);
    return 1;
}

static int cmd_shortcut(hid_device_t *dev, int argc, char **argv) {
    if (argc < 1) { fprintf(stderr, "shortcut: missing subcommand\n"); return 1; }

    if (strcmp(argv[0], "set") == 0) {
        if (argc < 5) {
            fprintf(stderr, "shortcut set <SRC_KEY> <SRC_MOD> <DST_KEY> <DST_MOD>\n");
            return 1;
        }
        shortcut_t sc = {0};
        sc.src_keycode   = parse_u8(argv[1]);
        sc.src_modifiers = parse_u8(argv[2]);
        sc.action_type   = KEY_ACTION_SHORTCUT;
        sc.dst_keycode   = parse_u8(argv[3]);
        sc.dst_modifiers = parse_u8(argv[4]);
        return shortcut_set(dev, &sc);

    } else if (strcmp(argv[0], "del") == 0) {
        if (argc < 3) {
            fprintf(stderr, "shortcut del <SRC_KEY> <SRC_MOD>\n");
            return 1;
        }
        return shortcut_delete(dev, parse_u8(argv[1]), parse_u8(argv[2]));
    }

    fprintf(stderr, "shortcut: unknown subcommand '%s'\n", argv[0]);
    return 1;
}

static int cmd_profile(hid_device_t *dev, int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "profile <save|load> <SLOT>\n"); return 1; }
    uint8_t slot = parse_u8(argv[1]);
    if (strcmp(argv[0], "save") == 0) return profile_save(dev, slot);
    if (strcmp(argv[0], "load") == 0) return profile_load(dev, slot);
    fprintf(stderr, "profile: unknown subcommand '%s'\n", argv[0]);
    return 1;
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */

int main(int argc, char **argv) {
    uint16_t vid = 0x046D; /* Logitech (default) */
    uint16_t pid = 0xC335; /* G413           */
    int i;

    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
            vid = parse_hex(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            pid = parse_hex(argv[++i]);
        } else {
            break; /* remaining args are the command */
        }
    }

    if (i >= argc) {
        print_usage(argv[0]);
        return 1;
    }

    const char *cmd = argv[i];
    int cmd_argc    = argc - i - 1;
    char **cmd_argv = argv + i + 1;

    /* Commands that don't need an open device */
    if (strcmp(cmd, "list") == 0) {
        return cmd_list();
    }

    /* All other commands require an open device */
    hid_device_t *dev = hid_keyboard_open(vid, pid);
    if (!dev) {
        fprintf(stderr,
                "\nTip: Run 'mechkey list' to see available devices.\n"
                "     Use -v and -p to specify your keyboard's VID:PID.\n");
        return 2;
    }

    int ret = 0;
    if      (strcmp(cmd, "rgb")      == 0) ret = cmd_rgb(dev, cmd_argc, cmd_argv);
    else if (strcmp(cmd, "macro")    == 0) ret = cmd_macro(dev, cmd_argc, cmd_argv);
    else if (strcmp(cmd, "shortcut") == 0) ret = cmd_shortcut(dev, cmd_argc, cmd_argv);
    else if (strcmp(cmd, "profile")  == 0) ret = cmd_profile(dev, cmd_argc, cmd_argv);
    else {
        fprintf(stderr, "Unknown command: '%s'\n", cmd);
        ret = 1;
    }

    hid_keyboard_close(dev);
    return ret;
}
