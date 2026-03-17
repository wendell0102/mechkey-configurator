# mechkey-configurator

> Universal mechanical keyboard configurator in C — RGB, macros, key remapping and profiles via USB HID.
> Includes both a **CLI** and a **GTK3 graphical interface**.
> Developed and tested on **Arch Linux**.

![License](https://img.shields.io/github/license/wendell0102/mechkey-configurator)
![Language](https://img.shields.io/badge/language-C-blue)
![Platform](https://img.shields.io/badge/platform-Arch%20Linux-1793D1?logo=arch-linux&logoColor=white)
![GUI](https://img.shields.io/badge/GUI-GTK3-green)

---

## Instalação no Arch Linux (via PKGBUILD)

> **O PKGBUILD está incluso na raiz do repositório.**
> Para usá-lo, você precisa primeiro clonar o repositório e então rodar o `makepkg` dentro da pasta clonada.

```bash
# 1. Instale as dependências de build
sudo pacman -S --needed base-devel git pkg-config hidapi gtk3

# 2. Clone o repositório
git clone https://github.com/wendell0102/mechkey-configurator.git

# 3. Entre na pasta (o PKGBUILD está aqui)
cd mechkey-configurator

# 4. Verifique que o PKGBUILD existe
ls PKGBUILD

# 5. Build e instala com makepkg
makepkg -si
```

Isso instala `mechkey` (CLI) e `mechkey-gui` (GUI) e registra a regra udev automaticamente.

> **Atenção:** não rode `makepkg` fora da pasta `mechkey-configurator/`.
> O PKGBUILD deve estar no diretório atual onde o comando é executado.

---

## Build manual (sem makepkg)

```bash
git clone https://github.com/wendell0102/mechkey-configurator.git
cd mechkey-configurator

# CLI only
make

# GUI (requer GTK3)
make gui

# Ambos
make all

# Instalar
sudo make install
sudo make install-gui
sudo make install-udev
```

---

## Features

| Feature | CLI | GUI |
|---|---|---|
| **RGB Control** | `mechkey rgb ...` | Color picker, sliders |
| **Lighting Effects** | `mechkey rgb mode ...` | Dropdown + speed slider |
| **Brightness** | `mechkey rgb brightness ...` | Slider 0-255 |
| **Macros** | `mechkey macro ...` | Table view + delete button |
| **Key Remapping** | `mechkey shortcut ...` | Form with hex inputs |
| **Profiles** | `mechkey profile ...` | Save/Load slot selector |
| **Device Listing** | `mechkey list` | Connection tab with status |
| **Cross-platform** | Arch / Debian / macOS | Arch / Debian / macOS |

---

## Screenshots (GUI)

```
+----------------------------------------------+
| MechKey Configurator                    [x]  |
|----------------------------------------------|
| Connection | RGB Lighting | Macros | Keymap  |
|----------------------------------------------|
| [Global RGB]                                 |
|  Color: [ ##FF0000 ] [Apply to All Keys]     |
|                                              |
| [Lighting Effects]                           |
|  Effect Mode: [ Breathing            v ]     |
|  Effect Speed: |--------o---------| 128      |
|  Brightness:   |------------------o| 255     |
|----------------------------------------------|
| Status: RGB: Applied color to all keys       |
+----------------------------------------------+
```

---

## Architecture

```
mechkey-configurator/
├── src/
│   ├── hid_keyboard.h/c   # HIDAPI wrapper
│   ├── rgb.h/c            # RGB API
│   ├── macros.h/c         # Macros, shortcuts, profiles
│   ├── main.c             # CLI entry point
│   ├── gui.h/c            # GTK3 interface
│   └── gui_main.c         # GUI entry point
├── PKGBUILD               # Arch Linux package build script
├── Makefile
├── LICENSE
└── README.md
```

---

## Dependencies

### Arch Linux (recommended)

```bash
# CLI only
sudo pacman -S hidapi

# CLI + GUI
sudo pacman -S hidapi gtk3

# Build tools (if not already installed)
sudo pacman -S base-devel pkg-config
```

### udev rule (required on Arch to access USB without root)

```bash
# Create rule for your keyboard (replace 046d with your VID)
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="046d", MODE="0666", GROUP="input"' \
  | sudo tee /etc/udev/rules.d/99-mechkey.rules
sudo udevadm control --reload-rules
sudo udevadm trigger

# Add your user to the input group
sudo usermod -aG input $USER
# Log out and back in for the group change to take effect
```

### Other distros

| Distro | Command |
|---|---|
| Ubuntu/Debian | `sudo apt install libhidapi-dev libgtk-3-dev` |
| Fedora | `sudo dnf install hidapi-devel gtk3-devel` |
| macOS | `brew install hidapi gtk+3` |
| Windows | MSYS2: `pacman -S mingw-w64-x86_64-hidapi mingw-w64-x86_64-gtk3` |

---

## CLI Usage

```bash
mechkey list                        # list known keyboards
mechkey rgb all 255 0 0             # all keys red
mechkey rgb key 0x04 0 0 255        # single key blue
mechkey rgb mode 1 200              # breathing at speed 200
mechkey rgb brightness 180          # set brightness
mechkey rgb off                     # turn off LEDs
mechkey macro del 3                 # delete macro ID 3
mechkey macro list                  # list stored macros
mechkey shortcut set 0x39 0 0x29 0  # remap Caps Lock -> Escape
mechkey shortcut del 0x39 0         # remove remap
mechkey profile save 0              # save config to slot 0
mechkey profile load 1              # load slot 1

# Specify keyboard by VID:PID
mechkey -v 1B1C -p 1B13 rgb all 0 255 0
```

---

## GUI Usage

```bash
mechkey-gui
```

| Tab | Contents |
|---|---|
| **Connection** | VID/PID fields, Connect button, status indicator |
| **RGB Lighting** | Color picker, effect dropdown, speed + brightness sliders |
| **Macros** | Stored macro table, delete button |
| **Keymap** | Source/destination keycode remap form |

---

## How it works

Most gaming keyboards expose a vendor HID interface (Usage Page `0xFF00`).
This tool opens it via HIDAPI and sends 64-byte reports with a command byte + payload.
The file `hid_keyboard.c` is the only layer needing adjustment per brand — everything else is protocol-agnostic.

---

## Tested keyboards

| Keyboard | VID:PID | Status |
|---|---|---|
| Logitech G413 | `046D:C335` | Protocol template |
| Corsair K70 | `1B1C:1B13` | Protocol template |
| Redragon K552 | `04F2:0111` | Protocol template |
| Glorious GMMK | `258A:0049` | Protocol template |

> Vendor protocols are proprietary — commands are based on community reverse-engineering. PRs welcome!

---

## Contributing

1. Fork the repository
2. `git checkout -b feature/your-feature`
3. Commit your changes
4. Open a Pull Request

---

## License

MIT — see [LICENSE](LICENSE).
