#include <SDL2/SDL.h>

void setUpScancodes();

void setUpDisplay(int width, int height, int scale); //Initilases display

void clearScreen(); // Clears screen and updates change

void updateScreen(); // Updates changes made after drawing/clearing

void drawPixel(int x, int y); // Sets pixel on

void clearPixel(int x, int y); // Clears pixel 
 
int isPixelOn(int x, int y); // Returns 1 if pixel is on, 0 otherwise

void destroyDisplay(); // Exit and finish display

unsigned char getKeyFromScancode(unsigned char scancode);

void pressKey(unsigned char scancode);

void releaseKey(unsigned char scancode);

int isKeyPressed(unsigned char key);

unsigned char getKeyPressed();

unsigned short getLetterSpriteAddress(unsigned char letter);


