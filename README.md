# mechkey-configurator

> Universal mechanical keyboard configurator in C — RGB, macros, key remapping and profiles via USB HID.

![License](https://img.shields.io/github/license/wendell0102/mechkey-configurator)
![Language](https://img.shields.io/badge/language-C-blue)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)

---

## Features

| Feature | Description |
|---|---|
| **RGB Control** | Set per-key or all-key colors, choose effects (Static, Breathing, Wave, Reactive, Rainbow, Custom), set brightness |
| **Macros** | Define keystroke sequences with timing delays, send to firmware, delete or list |
| **Key Remapping** | Remap any key to another key, macro, media command or layer action |
| **Profiles** | Save and load full configurations to/from firmware profile slots |
| **Device Listing** | Scan USB HID bus for known mechanical keyboards |
| **Cross-platform** | Works on Linux, macOS and Windows via HIDAPI |

---

## Architecture

```
mechkey-configurator/
├── src/
│   ├── hid_keyboard.h   # Core structs, enums, device handle (opaque)
│   ├── hid_keyboard.c   # HIDAPI wrapper: open/close/send/read/enumerate
│   ├── rgb.h            # RGB API declarations + color macros
│   ├── rgb.c            # RGB command builders and senders
│   ├── macros.h         # Macro / shortcut / profile API
│   ├── macros.c         # Macro serialisation + HID command dispatch
│   └── main.c           # CLI entry point with full argument parser
├── Makefile
├── LICENSE
└── README.md
```

---

## Dependencies

This project uses **[HIDAPI](https://github.com/libusb/hidapi)** for cross-platform USB HID communication.

| OS | Install |
|---|---|
| Ubuntu/Debian | `sudo apt install libhidapi-dev` |
| Fedora/RHEL | `sudo dnf install hidapi-devel` |
| macOS | `brew install hidapi` |
| Windows | Download pre-built from [HIDAPI releases](https://github.com/libusb/hidapi/releases) |

---

## Build

```bash
# Clone
git clone https://github.com/wendell0102/mechkey-configurator.git
cd mechkey-configurator

# Build (Linux/macOS)
make

# Install system-wide (optional)
sudo make install

# Clean
make clean
```

On Linux you may need to add a udev rule to access the keyboard without root:

```bash
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="046d", MODE="0666"' | sudo tee /etc/udev/rules.d/99-mechkey.rules
sudo udevadm control --reload-rules
```

---

## Usage

```
mechkey [-v VID] [-p PID] <command> [args]
```

### Options

| Flag | Description |
|---|---|
| `-v <VID>` | Keyboard vendor ID in hex (default: `046D` = Logitech) |
| `-p <PID>` | Keyboard product ID in hex (default: `C335` = G413) |
| `-h` | Show help |

### Commands

#### List devices
```bash
mechkey list
```

#### RGB
```bash
# Set all keys to solid red
mechkey rgb all 255 0 0

# Set a single key (by HID key ID) to blue
mechkey rgb key 0x04 0 0 255

# Set breathing mode at speed 200
mechkey rgb mode 1 200

# Available modes: 0=Static  1=Breathing  2=Wave  3=Reactive  4=Rainbow  5=Custom

# Set brightness (0-255)
mechkey rgb brightness 180

# Turn all LEDs off
mechkey rgb off
```

#### Macros
```bash
# Delete macro by ID
mechkey macro del 3

# List stored macros
mechkey macro list
```

#### Key Remapping (Shortcuts)
```bash
# Remap Caps Lock (0x39, no modifier) to Escape (0x29)
mechkey shortcut set 0x39 0x00 0x29 0x00

# Delete the remap
mechkey shortcut del 0x39 0x00
```

#### Profiles
```bash
# Save current config to slot 0
mechkey profile save 0

# Load slot 1
mechkey profile load 1
```

---

## How it works

Most gaming mechanical keyboards expose a **vendor-specific HID interface** (Usage Page `0xFF00`) alongside the standard keyboard interface. This tool:

1. Opens that vendor interface with HIDAPI.
2. Sends 64-byte HID reports with a 1-byte command code + payload.
3. Reads the firmware acknowledgement.

Because each brand uses its own proprietary protocol, **`hid_keyboard.c` is the layer you extend per-brand**. The `rgb.c` and `macros.c` modules build the payload and call `hid_keyboard_send_cmd()`, so adding support for a new keyboard means only modifying the low-level serialisation.

---

## Adding a new keyboard

1. Find your keyboard's VID and PID with `lsusb` (Linux) or Device Manager (Windows).
2. Add it to the list in `hid_keyboard_enumerate()`.
3. Capture HID traffic from the official software (Wireshark + USBPcap).
4. Map the captured bytes to the command constants in `hid_keyboard.h`.
5. Adjust the payload layout in `hid_keyboard_send_cmd()` if needed.

---

## Tested keyboards

| Keyboard | VID:PID | Status |
|---|---|---|
| Logitech G413 | `046D:C335` | Protocol template |
| Corsair K70 | `1B1C:1B13` | Protocol template |
| Redragon K552 | `04F2:0111` | Protocol template |
| Glorious GMMK | `258A:0049` | Protocol template |

> **Note:** Because vendor protocols are proprietary and undocumented, the commands are based on community reverse-engineering. Contributions welcome!

---

## Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/corsair-k100`
3. Commit your changes
4. Open a Pull Request

---

## License

MIT License — see [LICENSE](LICENSE) for details.
