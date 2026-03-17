# mechkey-configurator

> Universal mechanical keyboard configurator in C — RGB, macros, key remapping and profiles via USB HID.
> Includes both a **CLI** and a **GTK3 graphical interface**.

![License](https://img.shields.io/github/license/wendell0102/mechkey-configurator)
![Language](https://img.shields.io/badge/language-C-blue)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)
![GUI](https://img.shields.io/badge/GUI-GTK3-green)

---

## Features

| Feature | CLI | GUI |
|---|---|---|
| **RGB Control** | `mechkey rgb ...` | Color picker, sliders |
| **Lighting Effects** | `mechkey rgb mode ...` | Dropdown + speed slider |
| **Brightness** | `mechkey rgb brightness ...` | Slider 0–255 |
| **Macros** | `mechkey macro ...` | Table view + delete button |
| **Key Remapping** | `mechkey shortcut ...` | Form with hex inputs |
| **Profiles** | `mechkey profile ...` | Save/Load slot selector |
| **Device Listing** | `mechkey list` | Connection tab with status |
| **Cross-platform** | Linux / macOS / Windows | Linux / macOS / Windows |

---

## Screenshots (GUI)

```
+----------------------------------------------+
| MechKey Configurator              [x]        |
|----------------------------------------------|  
| Connection | RGB Lighting | Macros | Keymap  |
|----------------------------------------------|
| [Global RGB]                                  |
|  Color: [  ##FF0000  ] [Apply to All Keys]    |
|                                               |
| [Lighting Effects]                            |
|  Effect Mode: [ Breathing       v ]           |
|  Effect Speed: |--------o---------| 128       |
|  Brightness:   |------------------o| 255      |
|----------------------------------------------|
| Status: RGB: Applied color to all keys        |
+----------------------------------------------+
```

---

## Architecture

```
mechkey-configurator/
├── src/
│   ├── hid_keyboard.h   # Core structs, enums, device handle
│   ├── hid_keyboard.c   # HIDAPI wrapper: open/close/send/read/enumerate
│   ├── rgb.h / rgb.c    # RGB API
│   ├── macros.h / macros.c  # Macros, shortcuts, profiles API
│   ├── main.c           # CLI entry point
│   ├── gui.h            # GTK3 AppState struct + public GUI API
│   ├── gui.c            # GTK3 UI: tabs, callbacks, widget builders
│   └── gui_main.c       # GUI binary entry point
├── Makefile
├── LICENSE
└── README.md
```

---

## Dependencies

### CLI only
| OS | Command |
|---|---|
| Ubuntu/Debian | `sudo apt install libhidapi-dev` |
| Fedora | `sudo dnf install hidapi-devel` |
| macOS | `brew install hidapi` |

### GUI (GTK3 + HIDAPI)
| OS | Command |
|---|---|
| Ubuntu/Debian | `sudo apt install libhidapi-dev libgtk-3-dev` |
| Fedora | `sudo dnf install hidapi-devel gtk3-devel` |
| macOS | `brew install hidapi gtk+3` |
| Windows | MSYS2: `pacman -S mingw-w64-x86_64-hidapi mingw-w64-x86_64-gtk3` |

---

## Build

```bash
git clone https://github.com/wendell0102/mechkey-configurator.git
cd mechkey-configurator

# Build CLI only
make

# Build GUI only (requires GTK3)
make gui

# Build both
make all

# Install CLI system-wide
sudo make install

# Install GUI system-wide
sudo make install-gui

# Clean build artefacts
make clean
```

### Linux udev rule (no sudo needed)
```bash
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="046d", MODE="0666"' | sudo tee /etc/udev/rules.d/99-mechkey.rules
sudo udevadm control --reload-rules
```

---

## GUI Usage

```bash
./mechkey-gui
```

The graphical interface opens a window with four tabs:

| Tab | Contents |
|---|---|
| **Connection** | VID/PID fields, Connect button, status indicator |
| **RGB Lighting** | Color picker (all keys), effect combo-box, speed and brightness sliders |
| **Macros** | Table of stored macros, delete button |
| **Keymap** | Source/destination keycode form, apply/delete buttons |

All actions reflect immediately in the status bar at the bottom of the window.

---

## CLI Usage

```
mechkey [-v VID] [-p PID] <command> [args]
```

```bash
mechkey list                          # list known keyboards
mechkey rgb all 255 0 0               # all keys red
mechkey rgb key 0x04 0 0 255          # single key blue
mechkey rgb mode 1 200                # breathing effect speed 200
mechkey rgb brightness 180            # set brightness
mechkey rgb off                       # turn off LEDs
mechkey macro del 3                   # delete macro ID 3
mechkey macro list                    # list stored macros
mechkey shortcut set 0x39 0 0x29 0   # remap Caps Lock -> Escape
mechkey shortcut del 0x39 0           # remove remap
mechkey profile save 0                # save current config to slot 0
mechkey profile load 1                # load slot 1

# Specify keyboard by VID:PID
mechkey -v 1B1C -p 1B13 rgb all 0 255 0
```

---

## How it works

Most gaming keyboards expose a vendor HID interface (Usage Page `0xFF00`). This tool opens it via HIDAPI and sends 64-byte reports carrying a command byte and payload. `hid_keyboard.c` is the only layer that needs adjustment per brand; everything else is protocol-agnostic.

---

## Tested keyboards

| Keyboard | VID:PID | Status |
|---|---|---|
| Logitech G413 | `046D:C335` | Protocol template |
| Corsair K70 | `1B1C:1B13` | Protocol template |
| Redragon K552 | `04F2:0111` | Protocol template |
| Glorious GMMK | `258A:0049` | Protocol template |

> Because vendor protocols are proprietary, commands are based on community reverse-engineering. PRs welcome!

---

## Contributing

1. Fork the repository
2. `git checkout -b feature/your-feature`
3. Commit your changes
4. Open a Pull Request

---

## License

MIT — see [LICENSE](LICENSE).
