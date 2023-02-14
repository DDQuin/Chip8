
#include <stdio.h>
#include "stack.h"

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
