# Maintainer: Your Name <you@example.com>
# PKGBUILD for mechkey-configurator
# Install with: makepkg -si

pkgbase=mechkey-configurator
pkgname=('mechkey-configurator' 'mechkey-configurator-gui')
pkgver=1.0.0
pkgrel=1
pkgdesc='Universal mechanical keyboard configurator: RGB, macros, shortcuts via USB HID'
arch=('x86_64' 'aarch64')
url='https://github.com/wendell0102/mechkey-configurator'
license=('MIT')
depends=('hidapi')
makedepends=('base-devel' 'pkg-config')
options=(!strip)
source=("$pkgbase::git+$url.git")
sha256sums=('SKIP')

# mechkey-configurator (CLI) extra deps
_cli_deps=('hidapi')

# mechkey-configurator-gui extra deps
_gui_deps=('hidapi' 'gtk3')

prepare() {
  cd "$pkgbase"
}

build() {
  cd "$pkgbase"

  # Build CLI
  make cli

  # Build GUI (requires gtk3 at build time)
  if pkg-config --exists gtk+-3.0; then
    make gui
  else
    msg2 "GTK3 not found, skipping GUI build"
  fi
}

package_mechkey-configurator() {
  pkgdesc='Universal mechanical keyboard configurator (CLI)'
  depends=("${_cli_deps[@]}")

  cd "$pkgbase"

  # Install CLI binary
  install -Dm755 mechkey "$pkgdir/usr/bin/mechkey"

  # Install licence
  install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"

  # Install udev rules for all common gaming keyboard vendors
  install -Dm644 /dev/stdin \
    "$pkgdir/etc/udev/rules.d/99-mechkey.rules" <<'EOF'
# mechkey-configurator udev rules
# Allow members of the 'input' group to access HID devices without root

# Logitech
SUBSYSTEM=="usb", ATTRS{idVendor}=="046d", MODE="0660", GROUP="input"
KERNEL=="hidraw*", ATTRS{idVendor}=="046d", MODE="0660", GROUP="input"

# Corsair
SUBSYSTEM=="usb", ATTRS{idVendor}=="1b1c", MODE="0660", GROUP="input"
KERNEL=="hidraw*", ATTRS{idVendor}=="1b1c", MODE="0660", GROUP="input"

# Redragon
SUBSYSTEM=="usb", ATTRS{idVendor}=="04f2", MODE="0660", GROUP="input"
KERNEL=="hidraw*", ATTRS{idVendor}=="04f2", MODE="0660", GROUP="input"

# Glorious
SUBSYSTEM=="usb", ATTRS{idVendor}=="258a", MODE="0660", GROUP="input"
KERNEL=="hidraw*", ATTRS{idVendor}=="258a", MODE="0660", GROUP="input"
EOF

  # Install README
  install -Dm644 README.md "$pkgdir/usr/share/doc/$pkgname/README.md"
}

package_mechkey-configurator-gui() {
  pkgdesc='Universal mechanical keyboard configurator (GTK3 GUI)'
  depends=("${_gui_deps[@]}")

  cd "$pkgbase"

  # Install GUI binary (only if it was built)
  if [ -f mechkey-gui ]; then
    install -Dm755 mechkey-gui "$pkgdir/usr/bin/mechkey-gui"
  else
    msg2 "GUI binary not found, skipping"
    return 0
  fi

  # Desktop entry
  install -Dm644 /dev/stdin \
    "$pkgdir/usr/share/applications/mechkey-gui.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=MechKey Configurator
GenericName=Keyboard Configurator
Comment=Configure RGB, macros and shortcuts for mechanical keyboards
Exec=mechkey-gui
Icon=input-keyboard
Categories=Settings;HardwareSettings;
Keywords=keyboard;mechanical;rgb;macro;shortcut;
EOF

  # Install licence
  install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"

  # Install README
  install -Dm644 README.md "$pkgdir/usr/share/doc/$pkgname/README.md"
}
