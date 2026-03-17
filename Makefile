# ============================================================
# mechkey-configurator — Universal Mechanical Keyboard Config
# ============================================================
# Requires HIDAPI: https://github.com/libusb/hidapi
#
# Linux install:   sudo apt install libhidapi-dev libgtk-3-dev
# macOS install:   brew install hidapi gtk+3
# Windows:         download pre-built binaries from HIDAPI repo
#
# Targets:
#   make           -> CLI binary  (mechkey)
#   make gui       -> GUI binary  (mechkey-gui)  requires GTK3
#   make all       -> both CLI and GUI
#   make clean     -> remove build artefacts
#   make install   -> install CLI to /usr/local/bin
# ============================================================

CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -std=c99 -O2
SRCDIR   = src
OBJDIR   = build

BINARY_CLI = mechkey
BINARY_GUI = mechkey-gui

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
PLATFORM ?= $(shell uname -s | tr '[:upper:]' '[:lower:]')

ifeq ($(findstring linux,$(PLATFORM)),linux)
  LDFLAGS_CLI += -lhidapi-hidraw
  LDFLAGS_GUI += -lhidapi-hidraw
elseif ($(findstring darwin,$(PLATFORM)),darwin)
  LDFLAGS_CLI += -lhidapi
  LDFLAGS_GUI += -lhidapi
else ifneq (,$(findstring windows,$(PLATFORM)))
  LDFLAGS_CLI += -lhidapi
  LDFLAGS_GUI += -lhidapi
  BINARY_CLI  := mechkey.exe
  BINARY_GUI  := mechkey-gui.exe
else
  LDFLAGS_CLI += -lhidapi-hidraw
  LDFLAGS_GUI += -lhidapi-hidraw
endif

# GTK3 flags (auto-detected via pkg-config)
GTK_CFLAGS  := $(shell pkg-config --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS    := $(shell pkg-config --libs   gtk+-3.0 2>/dev/null)

# ---- Targets ---------------------------------------------------

.DEFAULT_GOAL := cli

.PHONY: all cli gui clean install uninstall

all: cli gui

cli: $(BINARY_CLI)

gui: $(BINARY_GUI)

# ---- Link CLI --------------------------------------------------
$(BINARY_CLI): $(OBJS_CLI)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS_CLI)
	@echo ""
	@echo "  CLI build done -> $(BINARY_CLI)"
	@echo ""

# ---- Link GUI --------------------------------------------------
$(BINARY_GUI): $(OBJS_GUI)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -o $@ $^ $(LDFLAGS_GUI) $(GTK_LIBS)
	@echo ""
	@echo "  GUI build done -> $(BINARY_GUI)"
	@echo ""

# ---- Compile rules ---------------------------------------------
$(OBJDIR)/cli_%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(SRCDIR) -c -o $@ $<

$(OBJDIR)/gui_%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -I$(SRCDIR) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

# ---- Utility ---------------------------------------------------
clean:
	rm -rf $(OBJDIR) $(BINARY_CLI) $(BINARY_GUI) mechkey.exe mechkey-gui.exe

install: cli
	cp $(BINARY_CLI) /usr/local/bin/
	@echo "Installed CLI to /usr/local/bin/mechkey"

install-gui: gui
	cp $(BINARY_GUI) /usr/local/bin/
	@echo "Installed GUI to /usr/local/bin/mechkey-gui"

uninstall:
	rm -f /usr/local/bin/mechkey /usr/local/bin/mechkey-gui
