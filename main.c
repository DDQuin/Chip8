#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <sys/time.h>
#include "audio.h"

#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32
#define SCALE 16
#define TIMER_FREQ 60
#define MAIN_FREQ 700
//Stack implementation

int MAXSIZE = 16;       
short stack[16];     
int top = -1;            

short isEmpty() {
   if(top == -1)
      return 1;
   else
      return 0;
}
   
short isFull() {

   if(top == MAXSIZE)
      return 1;
   else
      return 0;
}

short peek() {
   return stack[top];
}

short pop() {
   short data;
	
   if(!isEmpty()) {
      data = stack[top];
      top = top - 1;   
      return data;
   } else {
      printf("Could not retrieve data, Stack is empty.\n");
      return -1;
   }
}

short push(short data) {

   if(!isFull()) {
      top = top + 1;   
      stack[top] = data;
      return 0;
   } else {
      printf("Could not insert data, Stack is full.\n");
      return -1;
   }
}


int main (int argc, char *argv[]) {
   if (argc <= 1) {
      printf("No ROM supplied!\n");
      exit(1);
   }

   srand(time(NULL));
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    /* Init Simple-SDL2-Audio */
    initAudio();
    Audio * music = createAudio("sound/beep.wav", 1, SDL_MIX_MAXVOLUME);
    playMusicFromMemory(music, SDL_MIX_MAXVOLUME);
    pauseAudio();

    window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH * SCALE, WINDOW_HEIGHT* SCALE, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_RenderSetScale(renderer,
                       SCALE, SCALE);
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    printf("%s", SDL_GetError());
    unsigned int *pixels = window_surface->pixels;
    int width = window_surface->w;
   //  unsigned int pixelsNew[32][64] = {0};
   //  for (int pJ = 0; pJ < )
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  
    
    //Read and set memory from rom
    unsigned char memory[4096] = {0};
   int fileLen = 512;
   FILE *fBoot;
    fBoot = fopen ("boot.ch8", "rt");  /* open the boot for reading */
   int r = fread(memory, 1, fileLen, fBoot);
   if(r != fileLen) {
      fprintf(stderr, "error reading key: read %d chars, expected %d\n", r, fileLen);
      exit(1);
   }
   fclose(fBoot);

   int romFileLen = 4096 - 0;
   FILE *fRom;
   fRom = fopen (argv[1], "rt");  /* open the file for reading */
   int rRom = fread(memory + 512, 1, romFileLen, fRom);
   if(rRom > romFileLen) {
      fprintf(stderr, "error reading key: read %d chars, expected at max %d\n", rRom, romFileLen);
      exit(1);
   }
   fclose(fRom);
   for (int iMem = 0; iMem < 4096; iMem++) {
      //printf("%x ", memory[iMem]);
   }
   
   
   //Set up values
    unsigned short pc = 512; //512 Program counter
    unsigned short i = 0; // address variable
    unsigned char delay_timer = 0; 
    unsigned char sound_timer = 0;
    unsigned char registers[16] = {0}; // registers from V0 - VF
    //double period = (1/700) * 1000000 700 Hz
    //unsigned int period = 1000;
    double total_elapsed_timer = 0;
    double total_elapsed_main = 0;
    //struct timespec begin, end; 

    //Config stuff 
    int SHIFT_SETS_VX = 0;
    int JUMP_WITH_VX = 0;
    int STORE_LOAD_INCREMENT_I = 0;
    unsigned char scancode = 0xFF;
    
    
    while(1) {
   //  window_surface = SDL_GetWindowSurface(window);
   //  //printf("%s", SDL_GetError());
   //  pixels = window_surface->pixels;
    
   //  width = window_surface->w;
      
      //clock_gettime(CLOCK_REALTIME, &begin);
      clock_t start = clock();
      
      if (total_elapsed_timer > (double)1/(double)TIMER_FREQ) {
         
         total_elapsed_timer = 0;
      
         if (delay_timer > 0) {
            delay_timer = delay_timer - 1;
         }
         if (sound_timer > 0) {
            unpauseAudio();
            //printf("Sound cur timer %d", sound_timer);
            sound_timer = sound_timer - 1;
         } else {
            pauseAudio();
         }
      }
      if (SDL_PollEvent(&event)) {
         if (event.type == SDL_QUIT) {
            break;
         }
         if (event.type == SDL_KEYDOWN) {
            scancode = event.key.keysym.scancode;
         }
      }
       //usleep(1);
      
        if (total_elapsed_main > (double)1/(double)MAIN_FREQ) {
         window_surface = SDL_GetWindowSurface(window);
         pixels = window_surface->pixels;
        
         total_elapsed_main = 0;
        
        unsigned short opcode = 0; //opcode is 2 bytes long
        
        opcode |= (memory[pc] & 0xFF) << 8; //Big endian so first byte in memory is most significant
        opcode |= (memory[pc + 1] & 0xFF); // second byte in memory is least significant
        unsigned char firstNibble = (opcode & 0xF000) >> 12; //shift 0x1N part of 0x1NNN 
        unsigned char X = (opcode & 0x0F00) >> 8;
        unsigned char Y = (opcode & 0x00F0) >> 4;
        unsigned char N = (opcode & 0x000F);

        unsigned char NN = Y << 4 | N;
        unsigned short NNN = X << 8 | Y << 4 | N;

        unsigned char xCoord; 
        unsigned char yCoord; 
        unsigned char sprite; 
        unsigned int curPixel;
        unsigned int gettingKey = 1;
        unsigned int random = 0;
        

      //printf("Opcode: %04x ", opcode);
        for (int iR = 0; iR < 16; iR++) {
        //printf("V%x=%x ", iR, registers[iR]);
         //printf("S%x=%x ", i, stack[i]);
        }
        //printf("PC=%x I=%x\n", pc, i);
        //printf("--------\n");
        pc = pc + 2;
       
        switch (firstNibble) {
            case 0x0:
               if (X == 0x0 && Y == 0xE && N == 0x0) { //  00E0 clear screen
                  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                  SDL_RenderClear(renderer);
                  SDL_RenderPresent(renderer);
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

            case 0xC: // set arndom num on VX rand () & NN CXNN
               random = rand() % 255;
               registers[X] = random & NN;

            break;

            case 0xD:
               xCoord = registers[X] % 64;
               yCoord = registers[Y] % 32;
               registers[0xF] = 0;
               for (int row = 0; row < N; row++) {
                  xCoord = registers[X] % 64;
                  sprite = memory[i + row];
                  for (int iP=7; iP>=0; iP--) {
                     // get pixel at xCoord and yCoord
                     curPixel = pixels[(xCoord * SCALE) + (yCoord * SCALE) * width];
                     if ((((sprite>>iP) & 1) == 1) && curPixel == 0x0) { // and x, y is off
                     //set pixel x,y on
                    // printf("pixeeel at (%d, %d) is off pc %d", xCoord, yCoord, pc);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderDrawPoint(renderer, xCoord, yCoord);
                        //SDL_RenderPresent(renderer);
                        
                     } else if ((((sprite>>iP) & 1) == 1)) { // and x, y is on
                        //turn off pixel at x,y              
                      //  printf("pixeeel at (%d, %d) is on pc %d ", xCoord, yCoord, pc);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                        SDL_RenderDrawPoint(renderer, xCoord, yCoord);
                        //SDL_RenderPresent(renderer);
                        registers[0xF] = 1;
                     }
                     if (xCoord == 63) break; //go to nextt row
                     xCoord++;
                  }
                  yCoord++;
                  if (yCoord == 32) break; //Edge scren rrechead
               }
               SDL_RenderPresent(renderer);
               
            break;

            case 0xE:
               if (N == 0xE && Y == 0x9) { //EX9E skip  if current key = key in vx
               //printf("scancode is %x\n", scancode);
               switch (scancode) {
                  case 0x1E:
                     if (registers[X] == 0x1) {pc = pc + 2;}
                     break;
                  case 0x1F:
                     if (registers[X] == 0x2) {pc = pc + 2;}
                     break;
                  case 0x20:
                     if (registers[X] == 0x3) {pc = pc + 2;}
                     break;
                  case 0x21:
                     if (registers[X] == 0xC) {pc = pc + 2;}
                     break;
                  case 0x14:
                     if (registers[X] == 0x4) {pc = pc + 2;}
                     break;
                  case 0x1A:
                     if (registers[X] == 0x5) {pc = pc + 2;}
                     break;
                  case 0x08:
                     if (registers[X] == 0x6) {pc = pc + 2;}
                     break;
                  case 0x15:
                     if (registers[X] == 0xD) {pc = pc + 2;}
                     break;
                  case 0x04:
                     if (registers[X] == 0x7) {pc = pc + 2;}
                     break;
                  case 0x16:
                     if (registers[X] == 0x8) {pc = pc + 2;}
                     break;
                  case 0x07:
                     if (registers[X] == 0x9) {pc = pc + 2;}
                     break;
                  case 0x09:
                     if (registers[X] == 0xE) {pc = pc + 2;}
                     break;
                  case 0x1D:
                     if (registers[X] == 0xA) {pc = pc + 2;}
                     break;
                  case 0x1B:
                     if (registers[X] == 0x0) {pc = pc + 2;}
                     break;
                  case 0x06:
                     if (registers[X] == 0xB) {pc = pc + 2;}
                     break;
                  case 0x19:
                     if (registers[X] == 0xF) {pc = pc + 2;}
                     break;
                  default:
                     break;
               }
               }
               if (N == 0x1 && Y == 0xA) { //EXA1 skip iv current key != key in vx
               //printf("scancode is %x\n", scancode);
               switch (scancode) {
                  case 0x1E:
                     if (registers[X] != 0x1) {pc = pc + 2;}
                     break;
                  case 0x1F:
                     if (registers[X] != 0x2) {pc = pc + 2;}
                     break;
                  case 0x20:
                     if (registers[X] != 0x3) {pc = pc + 2;}
                     break;
                  case 0x21:
                     if (registers[X] != 0xC) {pc = pc + 2;}
                     break;
                  case 0x14:
                     if (registers[X] != 0x4) {pc = pc + 2;}
                     break;
                  case 0x1A:
                     if (registers[X] != 0x5) {pc = pc + 2;}
                     break;
                  case 0x08:
                     if (registers[X] != 0x6) {pc = pc + 2;}
                     break;
                  case 0x15:
                     if (registers[X] != 0xD) {pc = pc + 2;}
                     break;
                  case 0x04:
                     if (registers[X] != 0x7) {pc = pc + 2;}
                     break;
                  case 0x16:
                     if (registers[X] != 0x8) {pc = pc + 2;}
                     break;
                  case 0x07:
                     if (registers[X] != 0x9) {pc = pc + 2;}
                     break;
                  case 0x09:
                     if (registers[X] != 0xE) {pc = pc + 2;}
                     break;
                  case 0x1D:
                     if (registers[X] != 0xA) {pc = pc + 2;}
                     break;
                  case 0x1B:
                     if (registers[X] != 0x0) {pc = pc + 2;}
                     break;
                  case 0x06:
                     if (registers[X] != 0xB) {pc = pc + 2;}
                     break;
                  case 0x19:
                     if (registers[X] != 0xF) {pc = pc + 2;}
                     break;
                  default:
                     pc = pc + 2;
                     break;
               }
            }
            break;

            case 0xF:
            if (N == 0xA && Y == 0x0) { //FX0A get key
               pc = pc - 2;
               if (scancode == 0x1E) {registers[X] = 0x1; pc = pc + 2; gettingKey = 0; } 
               if (scancode == 0x1F) {registers[X] = 0x2; pc = pc + 2; gettingKey = 0; }
               if (scancode == 0x20) {registers[X] = 0x3; pc = pc + 2; gettingKey = 0; } 
               if (scancode == 0x21) {registers[X] = 0xC; pc = pc + 2; gettingKey = 0; }

               if (scancode == 0x14) {registers[X] = 0x4; pc = pc + 2; gettingKey = 0; }
               if (scancode == 0x1A) {registers[X] = 0x5; pc = pc + 2; gettingKey = 0; }
               if (scancode == 0x08) {registers[X] = 0x6; pc = pc + 2; gettingKey = 0; }
               if (scancode == 0x15) {registers[X] = 0xD; pc = pc + 2; gettingKey = 0; }

               if (scancode == 0x04) {registers[X] = 0x7; pc = pc + 2; gettingKey = 0; }
               if (scancode == 0x16) {registers[X] = 0x8; pc = pc + 2; gettingKey = 0; }
               if (scancode == 0x07) {registers[X] = 0x9; pc = pc + 2; gettingKey = 0; }
               if (scancode == 0x09) {registers[X] = 0xE; pc = pc + 2; gettingKey = 0; }

               if (scancode == 0x1D) {registers[X] = 0xA; pc = pc + 2; gettingKey = 0; }
               if (scancode == 0x1B) {registers[X] = 0x0; pc = pc + 2; gettingKey = 0; }
               if (scancode == 0x06) {registers[X] = 0xB; pc = pc + 2; gettingKey = 0; }
               if (scancode == 0x19) {registers[X] = 0xF; pc = pc + 2; gettingKey = 0; }
            }

            if (N == 0x7 && Y == 0x0) { //FX07 sets VX to currnet val of dealy timer
               registers[X] = delay_timer;
            }

            if (N == 0x5 && Y == 0x1) { //FX15 sets delay timer to value in VX
               delay_timer = registers[X];
            }

            if (N == 0x8 && Y == 0x1) { //FX18 sets sound ttimterr to val in VX
               sound_timer = registers[X];
            }

            if (N == 0xE && Y == 0x1) { //FX1E add to index i vx
               if (i + registers[X] > 0xFFF) { // overflowed occured
                  registers[0xF] = 1;
               }
               i = i + registers[X];
            }

            if (N == 0x9 && Y == 0x2) { //FX29 font, i is set to hex font at X
               unsigned char lastNib = registers[X] & 0x0F;
               //printf("last nib is %x \n", lastNib);
               if (lastNib == 0x0) {i = 0x50;}
               if (lastNib == 0x1) {i = 0x55;}
               if (lastNib == 0x2) {i = 0x5A;}
               if (lastNib == 0x3) {i = 0x5F;}
               if (lastNib == 0x4) {i = 0x64;}
               if (lastNib == 0x5) {i = 0x69;}
               if (lastNib == 0x6) {i = 0x6E;}
               if (lastNib == 0x7) {i = 0x73;}
               if (lastNib == 0x8) {i = 0x78;}
               if (lastNib == 0x9) {i = 0x7D;}
               if (lastNib == 0xA) {i = 0x82;}
               if (lastNib == 0xB) {i = 0x87;}
               if (lastNib == 0xC) {i = 0x8C;}
               if (lastNib == 0xD) {i = 0x91;}
               if (lastNib == 0xE) {i = 0x96;}
               if (lastNib == 0xF) {i = 0x9B;}
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
               //printf("Memory location hun %x ten %x unit %x", memory[i], memory[i + 1], memory[i + 2]);
               //printf("units %d tens %d hundreds %d\n", units, tensExtract,  hundredsExtract);
            }

            if (N == 0x5 && Y == 0x5) { //FX55 value from V0 to VX is stored in  I - I + X
               for (int iA = 0; iA <= X; iA++) {
                  if (STORE_LOAD_INCREMENT_I == 1) {
                     memory[i] = registers[iA];
                     i++;
                  } else {
                     memory[i + iA] = registers[iA];
                  }
                  //printf("Current register %x, currrent memory loc %x after putting in %x\n", iA, i + iA, memory[i + iA]);
               }
            }

            if (N == 0x5 && Y == 0x6) { //FX65 value from V0 to VX is loaded from  I - I + X
               for (int iA = 0; iA <= X; iA++) {
                  if (STORE_LOAD_INCREMENT_I == 1) {
                     registers[iA] = memory[i];
                     i++;
                  } else {
                     registers[iA] = memory[i + iA];
                  }
                  //printf("Current register %x, currrent memory loc %x after putting in %x\n", iA, i + iA, memory[i + iA]);
               }
            }
            break;
        }
      }
      clock_t end = clock();
        //clock_gettime(CLOCK_REALTIME, &end);
        //long seconds = end.tv_sec - begin.tv_sec;
        // long nanoseconds = end.tv_nsec - begin.tv_nsec;
         //double elapsed = seconds + nanoseconds*1e-9;
        total_elapsed_timer = total_elapsed_timer + ((double)(end - start)/CLOCKS_PER_SEC);
        total_elapsed_main = total_elapsed_main + ((double)(end - start)/CLOCKS_PER_SEC);
        //total_elapsed_timer = total_elapsed_timer + elapsed;
       // total_elapsed_main = total_elapsed_main + elapsed;
         
    }
    endAudio();
    freeAudio(music);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}



