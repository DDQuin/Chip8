chip8make: src/main.c src/audio.c src/stack.c src/display.c
	gcc src/main.c src/audio.c src/stack.c src/display.c -o chip8 -Wall -pedantic -lSDL2