CFLAGS = -O2

bombini: src/main.c
	gcc $(CFLAGS) src/main.c -o build/bombini

clean:
	rm -f build/bombini

