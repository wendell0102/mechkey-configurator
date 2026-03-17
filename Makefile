# ============================================================
# mechkey-configurator — Universal Mechanical Keyboard Config
# ============================================================
# Requires HIDAPI: https://github.com/libusb/hidapi
#
# Linux install:   sudo apt install libhidapi-dev
# macOS install:   brew install hidapi
# Windows:         download pre-built binaries from HIDAPI repo
#
# Build:
#   make           (Linux/macOS)
#   make PLATFORM=windows
# ============================================================

CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -std=c99 -O2
BINARY   = mechkey
SRCDIR   = src
OBJDIR   = build

SRCS = $(SRCDIR)/main.c \
       $(SRCDIR)/hid_keyboard.c \
       $(SRCDIR)/rgb.c \
       $(SRCDIR)/macros.c

OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# ---- Platform detection ----------------------------------------
PLATFORM ?= $(shell uname -s | tr '[:upper:]' '[:lower:]')

ifeq ($(findstring linux,$(PLATFORM)),linux)
  LDFLAGS += -lhidapi-hidraw
  # Alternatively: -lhidapi-libusb
elseif ($(findstring darwin,$(PLATFORM)),darwin)
  LDFLAGS += -lhidapi
else ifneq (,$(findstring windows,$(PLATFORM)))
  LDFLAGS += -lhidapi
  BINARY  := mechkey.exe
else
  LDFLAGS += -lhidapi-hidraw
endif

# ---- Targets ---------------------------------------------------

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo ""
	@echo "  Build successful -> $(BINARY)"
	@echo ""

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(SRCDIR) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(BINARY) mechkey.exe

install: $(BINARY)
	cp $(BINARY) /usr/local/bin/
	@echo "Installed to /usr/local/bin/mechkey"

uninstall:
	rm -f /usr/local/bin/mechkey

.PHONY: all clean install uninstall
