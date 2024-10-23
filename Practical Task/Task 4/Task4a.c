#include <stdio.h>  
#include <string.h>                                                     
#include "NUC1xx.h"
#include "SYS.h"
#include "LCD.h"
#include "Scankey.h"
#include <stdlib.h> // Include for rand()

unsigned char bmp16x16[32] = {
    // UPPER PART
    0xE0, 0x98,
    0x84, 0x82,
    0x82, 0x81,
    0x81, 0xFF,
    0xFF, 0x81,
    0x81, 0x82,  
    0x82, 0x84,
    0x98, 0xE0,
    // LOWER PART
    0x07, 0x19,
    0x21, 0x41,
    0x41, 0x81,
    0x81, 0xFF,
    0xFF, 0x81,
    0x81, 0x41,
    0x41, 0x21,
    0x19, 0x07
};

int32_t main (void)
{
  int8_t x=0, y=0;
  int8_t keyin=0;
  int8_t lastKeyin=0;  // Variable to store the last key pressed
  int8_t movX=3, dirX=0;
  int8_t movY=3, dirY=0;

  UNLOCKREG();
  DrvSYS_Open(50000000);
  LOCKREG();

  init_LCD();  
  clear_LCD();
  OpenKeyPad();

  x=64; y=32;
  print_Line(0,"LCD_BmpMove");
  print_Line(2,"to move");  
  print_Line(3,"press Keypad 1~9");
  draw_Bmp16x16(x,y,FG_COLOR,BG_COLOR,bmp16x16);
  
  while(keyin==0) keyin=ScanKey(); // wait till key is pressed
  clear_LCD();
  
  while(1) {
    draw_Bmp16x16(x,y,FG_COLOR,BG_COLOR,bmp16x16); // Draw Object
    DrvSYS_Delay(100000);                          // Delay for Vision
    draw_Bmp16x16(x,y,BG_COLOR,BG_COLOR,bmp16x16); // Erase Object
    
    keyin=ScanKey(); // Scan 3x3 keypad
    // Check if a new key is pressed and it's different from the last key pressed
    if (keyin != lastKeyin) {
      switch(keyin){   // input 1~9 to change (x,y) direction
        case 1:  
          dirX=-1; dirY=-1; 
        break;
        
        case 2: 
          dirX=0;  dirY=-1; 
        break;
        
        case 3: 
          dirX=+1; dirY=-1; 
        break;
        
        case 4:  
          dirX=-1; dirY=0;  
        break;
        
        case 5: 
          dirX=0;  dirY=0;  
        break;
        
        case 6: 
          dirX=+1; dirY=0;  
        break;
        
        case 7:  
          dirX=-1; dirY=+1; 
        break;
        
        case 8: 
          dirX=0;  dirY=+1; 
        break;
        
        case 9: 
          dirX=+1; dirY=+1; 
        break; 
        
        default: 
          break;
      }
      
      x = x + dirX * movX; // increment/decrement X
      y = y + dirY * movY; // increment/decrement Y

      // Check boundaries
      if (x < 0)           x = LCD_Xmax - 16; // check X boundary Min
      if (x > LCD_Xmax - 16) x = 0;           // check X boundary Max
      if (y < 0)           y = LCD_Ymax - 16; // check Y boundary Min
      if (y > LCD_Ymax - 16) y = 0;            // check Y boundary Max
      
    } 
    
    // Add a small delay to reduce flickering and improve responsiveness
    DrvSYS_Delay(10000);
  }
}
