#include "display.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define ORANGE_DARK  0x996600  
#define ORANGE_LIGHT 0xffcc00 
#define BLACK  0x0 
#define WHITE  0xffffff
#define BLUE  0x0000ff
#define DARK_BLUE  0x000055
#define WHITE  0xffffff

#define CLEAR_COLOUR DARK_BLUE 
#define SET_COLOUR BLUE

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Surface *window_surface;

int clearR, clearG, clearB;
int setR, setG, setB;

int width, height;
int scale;
int widthSurface;
unsigned int *pixels;

unsigned char scancodeMap[256] = {0x0}; // Map scancodes to keys

void setUpScancodes() {
   for (int iS = 0; iS < 256; iS++) {
      scancodeMap[iS] = 0xFF;
   }
   scancodeMap[0x1E] = 0x1;
   scancodeMap[0x1F] = 0x2;
   scancodeMap[0x20] = 0x3;
   scancodeMap[0x21] = 0xC;
   scancodeMap[0x14] = 0x4;
   scancodeMap[0x1A] = 0x5;
   scancodeMap[0x08] = 0x6;
   scancodeMap[0x15] = 0xD;
   scancodeMap[0x04] = 0x7;
   scancodeMap[0x16] = 0x8;
   scancodeMap[0x07] = 0x9;
   scancodeMap[0x09] = 0xE;
   scancodeMap[0x1D] = 0xA;
   scancodeMap[0x1B] = 0x0;
   scancodeMap[0x06] = 0xB;
   scancodeMap[0x19] = 0xF;
}


void setUpDisplay(int widthNew, int heightNew, int scaleNew) {
   setUpScancodes();
   scale = scaleNew;
   width = widthNew;
   height = heightNew;

   clearR = (CLEAR_COLOUR >> 16) & 0xff;
   clearG = (CLEAR_COLOUR >> 8) & 0xff;
   clearB = CLEAR_COLOUR  & 0xff;

   setR = (SET_COLOUR >> 16) & 0xff;
   setG = (SET_COLOUR >> 8) & 0xff;
   setB = SET_COLOUR  & 0xff;

   window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * scale, height * scale, SDL_WINDOW_RESIZABLE);
   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
   SDL_RenderSetScale(renderer, scale, scale);
   window_surface = SDL_GetWindowSurface(window);
   pixels = window_surface->pixels;
   widthSurface = window_surface->w;
}

void clearScreen() {
   SDL_SetRenderDrawColor(renderer, clearR, clearG, clearB, 0);
   SDL_RenderClear(renderer);
   SDL_RenderPresent(renderer);
}

void updateScreen() {
   SDL_RenderPresent(renderer);
}

void drawPixel(int x, int y) {
   SDL_SetRenderDrawColor(renderer, setR, setG, setB, 255);
   SDL_RenderDrawPoint(renderer, x, y);
}

void clearPixel(int x, int y) {
   SDL_SetRenderDrawColor(renderer, clearR, clearG, clearB, 0);
   SDL_RenderDrawPoint(renderer, x, y);
}
 
int isPixelOn(int x, int y) {
   unsigned int curPixel = pixels[(x * scale) + (y * scale) * widthSurface];
   if (curPixel == CLEAR_COLOUR) {
      return 0;
   }
   return 1;
}

void destroyDisplay() {
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
}

unsigned char getKeyFromScancode(unsigned char scancode) {
   return scancodeMap[scancode];
}

unsigned short getLetterSpriteAddress(unsigned char letter) {
   if (letter == 0x0) {return 0x50;}
   if (letter == 0x1) {return 0x55;}
   if (letter == 0x2) {return 0x5A;}
   if (letter == 0x3) {return 0x5F;}
   if (letter == 0x4) {return 0x64;}
   if (letter == 0x5) {return 0x69;}
   if (letter == 0x6) {return 0x6E;}
   if (letter == 0x7) {return 0x73;}
   if (letter == 0x8) {return 0x78;}
   if (letter == 0x9) {return 0x7D;}
   if (letter == 0xA) {return 0x82;}
   if (letter == 0xB) {return 0x87;}
   if (letter == 0xC) {return 0x8C;}
   if (letter == 0xD) {return 0x91;}
   if (letter == 0xE) {return 0x96;}
   if (letter == 0xF) {return 0x9B;}
   return 0x0;
}
