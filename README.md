## Chip-8 Interpreter 

A simple Chip-8 interpreter made in C as a fun project.

To run clone project and compile with `make`. Make sure you have SDL installed as well.

and run `./chip8 roms/{ROM}` (boot.ch8 is a base ROM for storing the first 512 bytes with font data, dont modify)

Simple audio library taken from https://github.com/jakebesworth/Simple-SDL2-Audio by Jake Besworth. 

Thanks! (I didn't want to spend another hour or two figuring out how to work with SDL audio)

Also `python hex.py {file}` can be ran to convert a file with hexacedimal strings to byte file eg. 0xAA 0xBB... 
This was created primarly to convert the hexadecimal compile output from Octo http://johnearnest.github.io/Octo/ into raw bytes that can be used by the interpreter
![Flight Runner](https://github.com/DDQuin/Chip8/blob/master/images/flightrunner.png)
![Space Invaders](https://github.com/DDQuin/Chip8/blob/master/images/spaceinvacer.png)
