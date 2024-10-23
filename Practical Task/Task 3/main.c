#include "stdio.h"
#include "NUC1xx.h"
#include "GPIO.h"
#include "SYS.h"
#include "Scankey.h"
#include "LCD.h"

// Function to initialize the GPIO pins for LEDs (LED8~LED5) as outputs
void Init_LED()
{
    // Initialize GPC12~15 pins as output
    DrvGPIO_Open(E_GPC, 12, E_IO_OUTPUT); // LED8 (MSB)
    DrvGPIO_Open(E_GPC, 13, E_IO_OUTPUT); // LED7
    DrvGPIO_Open(E_GPC, 14, E_IO_OUTPUT); // LED6
    DrvGPIO_Open(E_GPC, 15, E_IO_OUTPUT); // LED5 (LSB)

    // Set all LEDs to OFF initially (output Hi, since low-active)
    DrvGPIO_SetBit(E_GPC, 12);
    DrvGPIO_SetBit(E_GPC, 13);
    DrvGPIO_SetBit(E_GPC, 14);
    DrvGPIO_SetBit(E_GPC, 15);
}

// Function to update LEDs based on binary input
void Display_Binary_On_LEDs(int number)
{
    // number is 4-bit binary, update each LED accordingly (low-active)
    if (number & 0x8) DrvGPIO_ClrBit(E_GPC, 12); // LED8 (MSB) on if bit 3 is set
    else DrvGPIO_SetBit(E_GPC, 12);              // otherwise off

    if (number & 0x4) DrvGPIO_ClrBit(E_GPC, 13); // LED7 on if bit 2 is set
    else DrvGPIO_SetBit(E_GPC, 13);

    if (number & 0x2) DrvGPIO_ClrBit(E_GPC, 14); // LED6 on if bit 1 is set
    else DrvGPIO_SetBit(E_GPC, 14);

    if (number & 0x1) DrvGPIO_ClrBit(E_GPC, 15); // LED5 (LSB) on if bit 0 is set
    else DrvGPIO_SetBit(E_GPC, 15);
}

int32_t main (void)
{
		char TEXT2[16] = "Keypad input   ";
		char TEXT1[16] = "number =       ";
    int8_t number;

    // System initialization
    UNLOCKREG();
    SYSCLK->PWRCON.XTL12M_EN = 1; // Enable 12MHz crystal clock
    SYSCLK->CLKSEL0.HCLK_S = 0;   // Set HCLK to 12MHz
    LOCKREG();

    init_LCD();
    clear_LCD();
    Init_LED(); // Initialize LEDs

    OpenKeyPad(); // Initialize keypad scanning
    print_Line(0, "Keypad input");

    while(1)
    {
        number = ScanKey(); // Get the keypad input (1 to 9)
				DrvSYS_Delay(5000);           // delay time for keeping 7-segment display 
        if (number != 0) // If a valid key is pressed
        {
            sprintf(TEXT1 + 9, "%d", number); // Display the pressed number
            print_Line(1, TEXT1);
						close_seven_segment();	      // turn off 7-segment LEDs	
						show_seven_segment(0,number); // display number on 7-segment LEDs
            // Convert the number (1 to 9) to a 4-bit binary output and display on LEDs
            Display_Binary_On_LEDs(number);
        }
        else
        {
            // If no key is pressed, turn off all LEDs
            Display_Binary_On_LEDs(0);
        }
											 
    }
}