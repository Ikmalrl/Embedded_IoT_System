#include <stdio.h>
#include "NUC1xx.h"
#include "SYS.h"
#include "GPIO.h"

// Function: Initialize ADC
void InitADC(void)
{
    // Initialize the ADC settings for the VR1
    GPIOA->OFFD |= 0x00800000;  // Disable digital input path
    SYS->GPAMFP.ADC7_SS21_AD6 = 1;  // Set ADC function 

    SYSCLK->CLKSEL1.ADC_S = 2;  // Select 22MHz for ADC
    SYSCLK->CLKDIV.ADC_N = 1;   // ADC clock source = 22MHz/2 = 11MHz
    SYSCLK->APBCLK.ADC_EN = 1;  // Enable clock source
    ADC->ADCR.ADEN = 1;         // Enable ADC module
    ADC->ADCR.DIFFEN = 0;       // Single-ended input
    ADC->ADCR.ADMD = 0;         // Single mode
    ADC->ADCHER.CHEN = 0x80;    // Enable channel 7 (VR1)
    ADC->ADSR.ADF = 1;          // Clear the A/D interrupt flags
    ADC->ADCR.ADST = 1;         // Start the ADC sample
}

// Function: Control Buzzer Volume based on VR1
void Buzz(int durationOn, int durationOff)
{
    DrvGPIO_ClrBit(E_GPB, 11); // GPB11 = 0 to turn on Buzzer
    DrvSYS_Delay(durationOn);   // Delay for durationOn
    DrvGPIO_SetBit(E_GPB, 11);  // GPB11 = 1 to turn off Buzzer
    DrvSYS_Delay(durationOff);   // Delay for durationOff
}

int main(void)
{
    int adcValue;       // ADC value from VR1
    int durationOn;     // Duration for buzzer ON
    int durationOff;    // Duration for buzzer OFF
    int buzzCount = 0;  // Counter for buzz iterations

    UNLOCKREG();
    SYSCLK->PWRCON.XTL12M_EN = 1; // Enable external clock (12MHz)
    SYSCLK->CLKSEL0.HCLK_S = 0;   // Select external clock (12MHz)
    LOCKREG();

    InitADC();           // Initialize ADC for VR1
    DrvGPIO_Open(E_GPB, 11, E_IO_OUTPUT); // Initialize GPIO pin GPB11 for controlling Buzzer

    while (1)
    {
        // Read the ADC value
        while (ADC->ADSR.ADF == 0); // Wait until conversion is done
        ADC->ADSR.ADF = 1;          // Clear the conversion flag
        adcValue = ADC->ADDR[7].RSLT; // Read ADC value into adcValue

        // Calculate buzzer duration based on ADC value (0 to 4095)
        durationOn = (4095 - adcValue) * 2000 / 4095; // Duration ON in milliseconds
        durationOff = (adcValue * 2000) / 4095;        // Duration OFF in milliseconds

        // Control the buzzer based on the adcValue
        if (adcValue < 100) // Adjust this threshold as needed
        {
            // If the ADC value is low, stop buzzing
            if (buzzCount > 0 )
            {
                DrvGPIO_SetBit(E_GPB, 11); // Turn off Buzzer
                buzzCount = 0; // Reset the buzz count
            }
        }
        else
        {
            // Buzz if ADC value indicates volume
            Buzz(durationOn, durationOff);
            buzzCount++;
            DrvSYS_Delay(20000); // Delay before the next reading
            ADC->ADCR.ADST = 1;   // Restart ADC sample
        }
    }
}
