/*
 * GPIOs.c
 *
 *  Created on: 22 oct. 2025
 *      Author: Alesco
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/pwm.h"

uint8_t state_pwm;
uint8_t state_low;
uint16_t count;
// armando culiao
void
Timer0IntHandler(void)
{
    //
    // Clear de la interrupcion del Timer_0 A
    //
    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if (count<=1100)
    {
        MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, MAP_SysCtlClockGet()/(400+count));   // Cambio de Periodo de fase x ciclo
        MAP_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,
                             MAP_PWMGenPeriodGet(PWM0_BASE, PWM_GEN_1) / (5+count*0.002)); // Cambio de duty x ciclo
        MAP_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5,
                             MAP_PWMGenPeriodGet(PWM0_BASE, PWM_GEN_2) / (5+count*0.002));
        MAP_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6,
                             MAP_PWMGenPeriodGet(PWM0_BASE, PWM_GEN_3) / (5+count*0.002));
    }
    else
    {   }
    if (state_pwm==1 && state_low==1)
    {
        MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_3_BIT), false); 
        MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_5_BIT), true);  //HAI activo
        MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_6_BIT), false);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4); //LCI activo
    }
    else if (state_pwm==1 && state_low==2)
    {
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0);          //LCI a 0
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1); //LBI activo
        state_pwm++; // Suma de estado high
    }
    else if (state_pwm==2 && state_low==3)
    {
        MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_3_BIT), true);  //HCI activo
        MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_5_BIT), false); //HAI a 0
        MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_6_BIT), false);
    }
    else if (state_pwm==2 && state_low==4)
    {
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);          //LBI a 0
        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4); //LAI activo
        state_pwm++; // Suma de estado high
    }
    else if (state_pwm==3 && state_low==5)
    {
        MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_3_BIT), false); // HCI a 0
        MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_5_BIT), false);
        MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_6_BIT), true);  // HBI activo
    }
    else if (state_pwm==3 && state_low==6)
    {
        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0);          //LAI a 0
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4); //LCI activo
        state_pwm=1;                                           // Reinicio de bucle 
        state_low=0;
        count++; // Suma de cuentas de ciclo entero
    }
    state_low++; // Suma de estado low


}


void Setup_Timers(void)
{
    //
    // Enable the peripherals used by this example.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //
    // Enable processor interrupts.
    //
    MAP_IntMasterEnable();

    //
    // Configure the two 32-bit periodic timers.
    //
    MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, MAP_SysCtlClockGet()/400);
    //
    // Setup the interrupts for the timer timeouts.
    //
    MAP_IntEnable(INT_TIMER0A);
    MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //
    // Enable the timers.
    //
    MAP_TimerEnable(TIMER0_BASE, TIMER_A);
    state_pwm=1;
    state_low=1;
    count=1;
}
