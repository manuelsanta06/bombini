CC?=gcc
VERSION ?= 1.2.0

CFLAGS?=-O2
CFLAGS+=-Wall -Wextra -Isrc -std=gnu11 -D_GNU_SOURCE -DVERSION=\"$(VERSION)\"
LDFLAGS ?=

SRCS=src/main.c src/indexer.c src/search.c src/daemon.c src/fuzzy_match.c
OBJS=$(SRCS:src/%.c=build/%.o)
TARGET=build/bombini

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin

SYSTEMD_USER_DIR = $(PREFIX)/lib/systemd/user

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) build/*.o

install: all
	@echo "Installing in $(DESTDIR)$(BINDIR)..."
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/bombini
	@echo "Installing systemd service on $(DESTDIR)$(SYSTEMD_USER_DIR)..."
	install -d $(DESTDIR)$(SYSTEMD_USER_DIR)
	install -m 644 bombini.service $(DESTDIR)$(SYSTEMD_USER_DIR)/bombini.service

uninstall:
	@echo "Uninstalling from $(DESTDIR)$(BINDIR)..."
	rm -f $(DESTDIR)$(BINDIR)/bombini
	rm -f $(DESTDIR)$(SYSTEMD_USER_DIR)/bombini.service

.PHONY: all clean install uninstall test
