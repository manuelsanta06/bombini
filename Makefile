CFLAGS = -O2

bombini: main.c
	gcc $(CFLAGS) -o build/bombini main.c

.PHONY: test clean

test: build/bombini
	./build/bombini

clean:
	rm -f build/bombini

