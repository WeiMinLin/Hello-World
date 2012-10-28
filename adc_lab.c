//*****************************************************************************
//
// adc_lab.c - ADC Hands-on lab.
//
// Copyright (c) 2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// This is part of revision 8034 of the EK-LM4F232 Firmware Package.
//
//*****************************************************************************

#include "utils/ustdlib.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "grlib/grlib.h"
#include "drivers/cfal96x64x16.h"
#include "driverlib/gpio.h"


//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>ADC Lab (adc)</h1>
//!
//! ADC lab has following three objectives:
//! 1) Configure an ADC input channel in single-ended mode using Sequencer 3 and
//! capture accelerometer data for any one axis (x or y or z).
//! 2) Configure multiple (three) ADC channels in single-ended mode using
//! Sequencer 0 and capture accelerometer data for three axis (x, y and z).
//! 3) Configure ADC to use Hardware Averaging Sequencing Circuit.
//!
//! ADC value corresponding to the accelerometer voltage (which depends on
//! accelerometer's position) is displayed on the screen.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


// =======================================================================
// Defines
// -----------------------------------------------------------------------

//>>>>>>>>>>>>> SECTION: A
#define SEQUENCER 3

//*****************************************************************************
//
// ADC Lab
//
//*****************************************************************************
int
main(void)
{
    tContext sContext;
    char cText[8];
    unsigned long ulText;
    unsigned long ulADC0_Value[3];

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    //
    // Initialize the display driver.
    //
    CFAL96x64x16Init();

    //
    // Initialize the graphics context.
    //
    GrContextInit(&sContext, &g_sCFAL96x64x16);
    GrContextForegroundSet(&sContext, ClrWhite);

    //
    // Put application name on the screen.
    //
    GrContextFontSet(&sContext, &g_sFontFixed6x8);
	GrStringDraw(&sContext,"ADC Hands On Lab", -1,1, 5, 0);
	GrStringDraw(&sContext,"----------------", -1,1, 15, 0);
	GrStringDraw(&sContext,"AX :", -1,20, 25, 0);

	//>>>>>>>>>>>>> SECTION: B
	/*
	GrStringDraw(&sContext,"AY :", -1,20, 40, 0); // LABEL A
	GrStringDraw(&sContext,"AZ :", -1,20, 55, 0); // LABEL B
	*/

	//
	// Enable ADC0.
  	//
  	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

  	//
  	// Enable GPIO Port E.
  	//
  	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);


    // >>>>>>>>>>>>> SECTION: C
    //
    // Configure GPIO pins as ADC input channels.
    //
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

    //
    // Configure sequencer for trigger by processor.
    //
    ADCSequenceConfigure(ADC0_BASE, SEQUENCER, ADC_TRIGGER_PROCESSOR, 0);

    // >>>>>>>>>>>>> SECTIION: D
    //
    // Configure ADC to use Hardware Over Sampling Ciruit.
    //
    // ADCHardwareOversampleConfigure(ADC0_BASE, 64);

    // >>>>>>>>>>>>> SECTION: E
    //
    // Sample ADC channels in single ended mode & configure an interrupt flag
    // when sample is done. Configure the last step to cause an interrupt when
    // it is complete
    // ADCSequenceStepConfigure(ADC0_BASE, SEQUENCER, 0, ADC_CTL_CH8);
    // ADCSequenceStepConfigure(ADC0_BASE, SEQUENCER, 1, ADC_CTL_CH9);
	ADCSequenceStepConfigure(ADC0_BASE, SEQUENCER, 0, ADC_CTL_CH8 | ADC_CTL_IE
                       |ADC_CTL_END);

    //
    // Enable sample sequence.
    //
    ADCSequenceEnable(ADC0_BASE, SEQUENCER);

    //
    // Clear interrupt status flag.
    //
    ADCIntClear(ADC0_BASE, SEQUENCER);

    while(1)
    {
 		//
        // Trigger the ADC conversion.
        //
        ADCProcessorTrigger(ADC0_BASE, SEQUENCER);

        //
        // Wait for conversion to be completed.
        //
        while(!ADCIntStatus(ADC0_BASE, SEQUENCER, false))
        {
        }

        //
        // Read ADC Value.
        //
        ADCSequenceDataGet(ADC0_BASE, SEQUENCER, ulADC0_Value);

        //
        // Display X-axis values on screen .
        //
        usprintf(cText, " %d    ", ulADC0_Value[0]);
        GrStringDraw(&sContext, cText, -1, 45, 25, 1);


    	// >>>>>>>>>>>>> SECTION: F
	    //
        //
        // Display Y-axis values on screen.
        //
        // usprintf(cText, " %d    ", ulADC0_Value[1]);
        // GrStringDraw(&sContext, cText, -1, 45, 40, 1);

        //
        // Display Z-axis values on screen.
        //
        // usprintf(cText, " %d    ", ulADC0_Value[2]);
        // GrStringDraw(&sContext, cText, -1, 45, 55, 1);

        //
        // This function provides a means of generating a constant length
        // delay.  The function delay (in cycles) = 3 * parameter.  Delay
        // 250ms arbitrarily.
        //
        SysCtlDelay(SysCtlClockGet() / 12);

        //
    	// Flush any cached drawing operations.
    	//
    	//GrFlush(&sContext);
    }
}
