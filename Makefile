CFLAGS = -O2

bombini: main.c
	gcc $(CFLAGS) -o build/bombini main.c

clean:
	rm -f build/bombini

