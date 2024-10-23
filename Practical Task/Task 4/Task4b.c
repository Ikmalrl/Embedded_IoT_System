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

int32_t main (void) {
    int8_t x = 64, y = 32; // Starting position
    int8_t movX = 3, movY = 3; // Movement speed
    int8_t dirX, dirY; // Direction of movement

    // Initialize random seed (you might want to do this with a more variable seed)
    srand(0); // Change the seed to something like time or a hardware register for better randomness
    dirX = (rand() % 3) - 1; // Randomly -1, 0, or +1 for X direction
    dirY = (rand() % 3) - 1; // Randomly -1, 0, or +1 for Y direction

    UNLOCKREG();
    DrvSYS_Open(50000000);
    LOCKREG();

    init_LCD();  
    clear_LCD();
    print_Line(0, "Crosshair Bounce");
    print_Line(2, "Watch me move!");
    draw_Bmp16x16(x, y, FG_COLOR, BG_COLOR, bmp16x16);
    
    while (1) {
        draw_Bmp16x16(x, y, FG_COLOR, BG_COLOR, bmp16x16); // Draw Object
        DrvSYS_Delay(100000); // Delay for Vision
        draw_Bmp16x16(x, y, BG_COLOR, BG_COLOR, bmp16x16); // Erase Object
        
        // Update position
        x += dirX * movX; // Update X position
        y += dirY * movY; // Update Y position

        // Check boundaries for bouncing
        if (x < 0) {
            x = 0; // Reset to boundary
            dirX = 1; // Change direction
        }
        if (x > LCD_Xmax - 16) {
            x = LCD_Xmax - 16; // Reset to boundary
            dirX = -1; // Change direction
        }
        if (y < 0) {
            y = 0; // Reset to boundary
            dirY = 1; // Change direction
        }
        if (y > LCD_Ymax - 16) {
            y = LCD_Ymax - 16; // Reset to boundary
            dirY = -1; // Change direction
        }

        // Randomly change direction every few iterations
        if (rand() % 100 < 5) { // 5% chance to change direction
            dirX = (rand() % 3) - 1; // Randomly choose new direction for X
            dirY = (rand() % 3) - 1; // Randomly choose new direction for Y
        }
    }
}
