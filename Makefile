chip8make: src/main.c src/audio.c src/stack.c
	gcc src/main.c src/audio.c src/stack.c  -o chip8 -Wall -pedantic -lSDL2