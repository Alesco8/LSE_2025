/*
 * ADC_POT.c
 *
 *  Created on: 18 nov. 2025
 *      Author: ArmJa
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"


// The next function sets the ADC input AIN3 PE0
void Setup_ADC_Pot(void)
{
// set the clock form port E
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // wait until port E is ready, his funcition is evoid acces to peripheric until its not ready

while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));// wait until port E is ready, his funcition is evoid acces to peripheric until its not ready



SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); //enable ADC0 clock, A/D converter we use
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));// wait until ADC0 clock is rrady


// PE0 ---> AIN3
GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0); //conf PE0 as analog input ADC


// Sequencer 3: a single sample
ADCSequenceDisable(ADC0_BASE, 3); // Disable sequencer 3 of ADC0 so that it can be configured without triggering.

ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);//conversion will init by software, ....trigger_processor..no timer no extern hardware
// priority=0, highest priority from the sequencer
ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END); //read channel 3 (AIN3=PE0), generate interrupt IE, end of sequence
ADCSequenceEnable(ADC0_BASE, 3); // enable the sequencer
ADCIntClear(ADC0_BASE, 3); //clean interruption flag

}


//the next function  will read and return the potenciometre value
uint32_t ADC_PotRead(void)
{
    uint32_t adcValue;

    // clean flag from sequencer 3
    ADCIntClear(ADC0_BASE, 3);

    //software conversion
    ADCProcessorTrigger(ADC0_BASE, 3);

    // wait until conversion finish
    while(!ADCIntStatus(ADC0_BASE, 3, false));

    // get converted data
    ADCSequenceDataGet(ADC0_BASE, 3, &adcValue);

    // return data between (0–4095)
    // 12-bits adc
    //resolution 3.3V/4096= ~0.8mV
    return adcValue;
}

