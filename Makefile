# ============================================================
# mechkey-configurator — Universal Mechanical Keyboard Config
# ============================================================
# Primary platform: Arch Linux
#
# Arch Linux install:
#   sudo pacman -S base-devel pkg-config hidapi gtk3
#
# Other Linux:
#   sudo apt install libhidapi-dev libgtk-3-dev   (Debian/Ubuntu)
#   sudo dnf install hidapi-devel gtk3-devel       (Fedora)
#
# macOS:   brew install hidapi gtk+3
# Windows: MSYS2 pacman -S mingw-w64-x86_64-hidapi mingw-w64-x86_64-gtk3
#
# Targets:
#   make           -> CLI  binary  (mechkey)
#   make gui       -> GUI  binary  (mechkey-gui)  requires GTK3
#   make all       -> both
#   make install   -> /usr/local/bin (CLI)
#   make install-gui -> /usr/local/bin (GUI)
#   make install-udev -> install udev rule for USB access without root
#   make clean     -> remove build artefacts
# ============================================================

CC       ?= gcc
CFLAGS   ?= -Wall -Wextra -std=c99 -O2
SRCDIR    = src
OBJDIR    = build
PREFIX   ?= /usr/local

BINARY_CLI = mechkey
BINARY_GUI = mechkey-gui

UDEV_RULE  = 99-mechkey.rules
UDEV_DIR   = /etc/udev/rules.d

# ---- Shared sources (no main) ----------------------------------
SRCS_SHARED = $(SRCDIR)/hid_keyboard.c \
              $(SRCDIR)/rgb.c \
              $(SRCDIR)/macros.c

# ---- CLI sources -----------------------------------------------
SRCS_CLI = $(SRCS_SHARED) $(SRCDIR)/main.c
OBJS_CLI = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/cli_%.o, $(SRCS_CLI))

# ---- GUI sources -----------------------------------------------
SRCS_GUI = $(SRCS_SHARED) $(SRCDIR)/gui.c $(SRCDIR)/gui_main.c
OBJS_GUI = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/gui_%.o, $(SRCS_GUI))

# ---- Platform detection ----------------------------------------
# Detect Arch Linux via /etc/arch-release
IS_ARCH := $(shell [ -f /etc/arch-release ] && echo yes)
PLATFORM ?= $(shell uname -s | tr '[:upper:]' '[:lower:]')

ifeq ($(IS_ARCH),yes)
  LDFLAGS_CLI += -lhidapi
  LDFLAGS_GUI += -lhidapi
  $(info [mechkey] Platform: Arch Linux)
elseif ($(findstring darwin,$(PLATFORM)),darwin)
  LDFLAGS_CLI += -lhidapi
  LDFLAGS_GUI += -lhidapi
else ifneq (,$(findstring windows,$(PLATFORM)))
  LDFLAGS_CLI += -lhidapi
  LDFLAGS_GUI += -lhidapi
  BINARY_CLI  := mechkey.exe
  BINARY_GUI  := mechkey-gui.exe
else
  # Generic Linux (Ubuntu, Debian, Fedora, etc.)
  LDFLAGS_CLI += -lhidapi-hidraw
  LDFLAGS_GUI += -lhidapi-hidraw
endif

# GTK3 via pkg-config (works on Arch and most distros)
GTK_CFLAGS  := $(shell pkg-config --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS    := $(shell pkg-config --libs   gtk+-3.0 2>/dev/null)

# ---- Targets ---------------------------------------------------

.DEFAULT_GOAL := cli

.PHONY: all cli gui clean install install-gui install-udev uninstall

all: cli gui

cli: $(BINARY_CLI)

gui: $(BINARY_GUI)

# ---- Link CLI --------------------------------------------------
$(BINARY_CLI): $(OBJS_CLI)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS_CLI)
	@echo ""
	@echo "  CLI build done -> $(BINARY_CLI)"
	@echo "  Run: sudo ./$(BINARY_CLI) list"
	@echo "  (or install udev rule with: sudo make install-udev)"
	@echo ""

# ---- Link GUI --------------------------------------------------
$(BINARY_GUI): $(OBJS_GUI)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -o $@ $^ $(LDFLAGS_GUI) $(GTK_LIBS)
	@echo ""
	@echo "  GUI build done -> $(BINARY_GUI)"
	@echo "  Run: ./$(BINARY_GUI)"
	@echo ""

# ---- Compile rules ---------------------------------------------
$(OBJDIR)/cli_%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(SRCDIR) -c -o $@ $<

$(OBJDIR)/gui_%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -I$(SRCDIR) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

# ---- Install ---------------------------------------------------
install: cli
	install -Dm755 $(BINARY_CLI) $(DESTDIR)$(PREFIX)/bin/$(BINARY_CLI)
	@echo "Installed CLI -> $(PREFIX)/bin/$(BINARY_CLI)"

install-gui: gui
	install -Dm755 $(BINARY_GUI) $(DESTDIR)$(PREFIX)/bin/$(BINARY_GUI)
	@echo "Installed GUI -> $(PREFIX)/bin/$(BINARY_GUI)"

# Install udev rule so USB device is accessible without root
install-udev:
	@echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="046d", MODE="0666", GROUP="input"' \
	  > /tmp/$(UDEV_RULE)
	@echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="1b1c", MODE="0666", GROUP="input"' \
	  >> /tmp/$(UDEV_RULE)
	@echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="04f2", MODE="0666", GROUP="input"' \
	  >> /tmp/$(UDEV_RULE)
	@echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="258a", MODE="0666", GROUP="input"' \
	  >> /tmp/$(UDEV_RULE)
	@echo 'KERNEL=="hidraw*", ATTRS{idVendor}=="046d", MODE="0666", GROUP="input"' \
	  >> /tmp/$(UDEV_RULE)
	install -Dm644 /tmp/$(UDEV_RULE) $(UDEV_DIR)/$(UDEV_RULE)
	udevadm control --reload-rules
	udevadm trigger
	@echo "udev rule installed. Reconnect your keyboard."
	uninstall:
	rm -f $(PREFIX)/bin/$(BINARY_CLI) $(PREFIX)/bin/$(BINARY_GUI)
	rm -f $(UDEV_DIR)/$(UDEV_RULE)

clean:
	rm -rf $(OBJDIR) $(BINARY_CLI) $(BINARY_GUI) mechkey.exe mechkey-gui.exe
