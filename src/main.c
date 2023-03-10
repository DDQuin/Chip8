#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <sys/time.h>
#include "audio.h"
#include "stack.h"
#include "display.h"

#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32
#define SCALE 16
#define TIMER_FREQ 60
#define MAIN_FREQ 700

unsigned char memory[4096] = {0}; // Read and set memory from rom
unsigned short pc = 512; // 512 Program counter
unsigned short i = 0; // address variable
unsigned char delay_timer = 0; 
unsigned char sound_timer = 0;
unsigned char registers[16] = {0}; // registers from V0 - VF
double total_elapsed_timer = 0;
double total_elapsed_main = 0;
unsigned char currentPressedScancode = 0xFF; // Currently selectted key
unsigned short opcode = 0;

// Config stuff, 0 is modern settings
int SHIFT_SETS_VX = 0;
int JUMP_WITH_VX = 0;
int STORE_LOAD_INCREMENT_I = 0;

int readRom(char *romName, int fileLen, int offset) {
   FILE *romFile;
   romFile = fopen(romName, "rt");  
   int r = fread(memory + offset, 1, fileLen, romFile);
   fclose(romFile);
   return r;
}

void printDebug() {
   printf("Opcode: %04x ", opcode);
   for (int iR = 0; iR < 16; iR++) {
      printf("V%x=%x ", iR, registers[iR]);
   }
   printf("PC=%x I=%x\n", pc, i);
   printf("--------\n");
}

int main (int argc, char *argv[]) {
   if (argc <= 1) {
      printf("No ROM supplied!\n");
      exit(1);
   }
   srand(time(NULL));
   SDL_Event event;

   SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

   /* Init Simple-SDL2-Audio */
   initAudio();
   Audio * music = createAudio("./sound/beep.wav", 1, SDL_MIX_MAXVOLUME);
    
   playMusicFromMemory(music, SDL_MIX_MAXVOLUME);
   pauseAudio();

   setUpDisplay(WINDOW_WIDTH, WINDOW_HEIGHT, SCALE);
   clearScreen();

   readRom("./roms/boot.ch8", 512, 0); // Read boot ROM
   readRom(argv[1], 4096, 512); // read given ROM
    
   while(1) {
      clock_t start = clock();
      if (total_elapsed_timer > (double)1/(double)TIMER_FREQ) {
         total_elapsed_timer = 0;
      
         if (delay_timer > 0) {
            delay_timer = delay_timer - 1;
         }
         if (sound_timer > 0) {
            unpauseAudio();
            sound_timer = sound_timer - 1;
         } else {
            pauseAudio();
         }
      }
      if (SDL_PollEvent(&event)) { //Get current key press incase needed in instruction.
         if (event.type == SDL_QUIT) {
            break;
         }
         if (event.type == SDL_KEYDOWN) {
            pressKey(event.key.keysym.scancode);
         } else if (event.type == SDL_KEYUP) {
            releaseKey(event.key.keysym.scancode);
         }
      }
      
      if (total_elapsed_main > (double)1/(double)MAIN_FREQ) { 
         total_elapsed_main = 0;
        
         opcode |= (memory[pc] & 0xFF) << 8; //Big endian so first byte in memory is most significant
         opcode |= (memory[pc + 1] & 0xFF); // second byte in memory is least significant
         unsigned char firstNibble = (opcode & 0xF000) >> 12; //shift 0x1N part of 0x1NNN 
         unsigned char X = (opcode & 0x0F00) >> 8;
         unsigned char Y = (opcode & 0x00F0) >> 4;
         unsigned char N = (opcode & 0x000F);
         opcode = 0;

         unsigned char NN = Y << 4 | N;
         unsigned short NNN = X << 8 | Y << 4 | N;
         pc = pc + 2;

         switch (firstNibble) {
            case 0x0:
               if (X == 0x0 && Y == 0xE && N == 0x0) { //  00E0 clear screen
                  clearScreen();
               }
               if (X == 0x0 && Y == 0xE && N == 0xE) { //  00EE return from sub routine
                  pc = pop();
               }
            break;

            case 0x1:
                pc = NNN; // Jump to address NNN 0x1NNN 
            break;

            case 0x2:
                push(pc);
                pc = NNN; //Subroutine called NNN 0x2NNN
            break;

            case 0x3:
               if (registers[X] == NN) { //Skip if VX = NN, 0x3XNN
                  pc = pc + 2;
               }
            break;

            case 0x4:
               if (registers[X] != NN) { //Skip if VX != NN, 0x4XNN
                  pc = pc + 2;
               }
            break;

            case 0x5:
               if (N == 0x0) {
                  if (registers[X] == registers[Y]) { //Skip if VX = VY, 0x5XY0
                     pc = pc + 2;
                  }
               }
            break;

            case 0x6:
               registers[X] = NN; //Set register VX to NN  6XNN
            break;

            case 0x7:
               registers[X] += NN; //Add NN to register VX 7XNN
            break;

            case 0x8:
               switch(N) {
                  case 0x0: //8XY0 set VX = VY
                     registers[X] = registers[Y];
                  break;

                  case 0x1: // 8XY1 set VX = VX | VY
                     registers[X] = registers[X] | registers[Y];
                  break;

                  case 0x2: // 8XY2 set VX = VX & VY
                     registers[X] = registers[X] & registers[Y];
                  break;

                  case 0x3: // 8XY3 set VX = VX ^ (XOR) VY
                     registers[X] = registers[X] ^ registers[Y];
                  break;

                  case 0x4: // 8XY4 set VX = VX + VY
                     registers[0xF] = 0x0;
                     if ((unsigned char)(registers[X] + registers[Y]) < registers[X] ||
                         (unsigned char)(registers[X] + registers[Y]) < registers[Y]) {
                        registers[0xF] = 0x1;
                     } 
                     registers[X] = registers[X] + registers[Y];
                  break;

                  case 0x5: // 8XY5 set VX = VX - VY
                     registers[0xF] = 0x1;
                     if (registers[Y] > registers[X]) {
                        registers[0xF] = 0x0;
                     }
                     registers[X] = registers[X] - registers[Y];
                  break;

                  case 0x7: // 8XY7 set VX = VY - VX
                     registers[0xF] = 0x1;
                     if (registers[X] > registers[Y]) {
                        registers[0xF] = 0x0;
                     }
                     registers[X] = registers[Y] - registers[X];
                  break;

                  case 0x6: // 8XY6 shift x rightt 1
                     if (SHIFT_SETS_VX == 0x1) registers[X] = registers[Y];
                     registers[0xF] = registers[X] & 0x1;
                     registers[X] = registers[X] >> 1;
                  break;

                  case 0xE:  // 8XY6 shift x left 1
                     if (SHIFT_SETS_VX == 0x1) registers[X] = registers[Y];
                     registers[0xF] = (registers[X] >> 7) & 0x1;
                     registers[X] = registers[X] << 1;
                  break;
               }
            break;

            case 0x9:
               if (N == 0) {
                  if (registers[X] != registers[Y]) { //Skip if VX != VY, 0x9XY0
                     pc = pc + 2;
                  }
               }
            break;

            case 0xA:
               i = NNN; // Sets address i to NNN // ANNN
            break;

            case 0xB: // jump to BNNN with V0 offsett orr VX
               if (JUMP_WITH_VX == 1) {
                  pc = NNN + registers[X];
               } else { 
                  pc = NNN + registers[0x0];
               }
            break;

            case 0xC: { // set arndom num on VX rand () & NN CXNN
               unsigned int random = rand() % 255;
               registers[X] = random & NN;
            }

            break;

            case 0xD: {
               unsigned char xCoord = registers[X] % 64;
               unsigned char yCoord = registers[Y] % 32;
               registers[0xF] = 0;
               for (int row = 0; row < N; row++) {
                  xCoord = registers[X] % 64;
                  unsigned char sprite = memory[i + row];
                  for (int iP=7; iP>=0; iP--) {
                     if ((((sprite>>iP) & 1) == 1) && !isPixelOn(xCoord, yCoord)) { // and x, y is off. Set x, y
                        drawPixel(xCoord, yCoord);              
                     } else if ((((sprite>>iP) & 1) == 1)) { // and x, y is on. Turn x, y off             
                        clearPixel(xCoord, yCoord);
                        registers[0xF] = 1;
                     }
                     if (xCoord == 63) break; //go to next row
                     xCoord++;
                  }
                  yCoord++;
                  if (yCoord == 32) break; //Edge screen reached
               }
               updateScreen();
            }
            break;

            case 0xE:
               if (Y == 0x9 && N == 0xE) { //EX9E skip  if current key = key in vx
                  if (registers[X] < 16 && isKeyPressed(registers[X])) {
                     pc = pc + 2;
                  }
               }

               if (Y == 0xA && N == 0x1) { //EXA1 skip if current key != key in vx
                  if (registers[X] < 16) { // Make sure register stores key
                     if (!isKeyPressed(registers[X])) {
                        pc = pc + 2;
                     }
                  } else {
                     pc = pc + 2;
                  }
               }
            break;

            case 0xF:
            if (N == 0xA && Y == 0x0) { //FX0A get key
               pc = pc - 2;
               unsigned char currentPressedKey = getKeyPressed();
               if (currentPressedKey != 0xff) {
                  registers[X] = currentPressedKey;
                  pc = pc + 2;
               }
            }

            if (Y == 0x0 && N == 0x7) { //FX07 sets VX to currnet val of dealy timer
               registers[X] = delay_timer;
            }

            if (Y == 0x1 && N == 0x5) { //FX15 sets delay timer to value in VX
               delay_timer = registers[X];
            }

            if (Y == 0x1 && N == 0x8) { //FX18 sets sound ttimterr to val in VX
               sound_timer = registers[X];
            }

            if (Y == 0x1 && N == 0xE) { //FX1E add to index i vx
               if (i + registers[X] > 0xFFF) { // overflowed occured
                  registers[0xF] = 1;
               }
               i = i + registers[X];
            }

            if (Y == 0x2 && N == 0x9) { //FX29 font, i is set to hex font at X
               unsigned char lastNib = registers[X] & 0x0F;
               i = getLetterSpriteAddress(lastNib);
            }

            if (N == 0x3 && Y == 0x3) { //FX33 BCD, set VX to I, I + 1, I + 2 digits.
               unsigned char num = registers[X];
               int units = num % 10;
               int tens = ((num - units) % 100);
               int tensExtract = tens / 10;
               int hundreds = (num - units - tens) % 1000;
               int hundredsExtract = hundreds / 100;
               memory[i] = (unsigned char) hundredsExtract;
               memory[i + 1] = (unsigned char) tensExtract;
               memory[i + 2] = (unsigned char) units;
            }

            if (N == 0x5 && Y == 0x5) { //FX55 value from V0 to VX is stored in  I - I + X
               for (int iA = 0; iA <= X; iA++) {
                  if (STORE_LOAD_INCREMENT_I == 1) {
                     memory[i] = registers[iA];
                     i++;
                  } else {
                     memory[i + iA] = registers[iA];
                  }
               }
            }

            if (Y == 0x6 && N == 0x5) { //FX65 value from V0 to VX is loaded from  I - I + X
               for (int iA = 0; iA <= X; iA++) {
                  if (STORE_LOAD_INCREMENT_I == 1) {
                     registers[iA] = memory[i];
                     i++;
                  } else {
                     registers[iA] = memory[i + iA];
                  }
               }
            }
            break;
        }
      }
      clock_t end = clock();
      total_elapsed_timer = total_elapsed_timer + ((double)(end - start)/CLOCKS_PER_SEC);
      total_elapsed_main = total_elapsed_main + ((double)(end - start)/CLOCKS_PER_SEC);
    }
   endAudio();
   freeAudio(music);
   destroyDisplay();
   SDL_Quit();
   return EXIT_SUCCESS;
}
