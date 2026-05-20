CC=gcc
CFLAGS=-O2 -Wall -Wextra -Isrc
SRCS=src/main.c src/loader.c src/indexer.c src/search.c src/daemon.c src/fuzzy_match.c
OBJS=$(SRCS:src/%.c=build/%.o)
TARGET=build/bombini

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) build/*.o
