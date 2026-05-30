CC?=gcc

CFLAGS?=-O2
CFLAGS+=-Wall -Wextra -Isrc
LDFLAGS ?=

SRCS=src/main.c src/indexer.c src/search.c src/daemon.c src/fuzzy_match.c
OBJS=$(SRCS:src/%.c=build/%.o)
TARGET=build/bombini

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) build/*.o

install: all
	@echo "Instalando en $(DESTDIR)$(BINDIR)..."
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/bombini

uninstall:
	@echo "Desinstalando de $(DESTDIR)$(BINDIR)..."
	rm -f $(DESTDIR)$(BINDIR)/bombini
