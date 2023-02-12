## Chip-8 Interpeter 

A simple Chip-8 intepreter made in C as a fun project.
Clone project and then
Compile with gcc main.c audio.c -o chip8 -Wall -pedantic -lSDL2
and run ./chip8 roms/{ROM}

Simple audio library taken from https://github.com/jakebesworth/Simple-SDL2-Audio by Jake Besworth. 
Thanks! (I didn't want to spend another hour or two figuring out how to work with SDL audio)