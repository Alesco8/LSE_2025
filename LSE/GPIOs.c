/*
 * GPIOs.c
 *
 *  Created on: 22 oct. 2025
 *      Author: Alesco
 */


#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"


//*****************************************************************************
//
// GPIOs del Driver:
//
// -HAI,HBI,HCI: PWMs
// -LAI,LBI,LCI: Outputs
// -Sense_A, Sense_B, Sense_C: Inputs
// -POT: Input ADC
//
//*****************************************************************************

//Inicializa los pines GPIO utilizados por el driver del motor.
void Setup_GPIOs()
{
    //
    // El periférico PWM debe estar habilitado para su uso.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    //
    // El periférico de GPIO de los puertos A,B,C,D,E deben estar habilitados para su uso.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); 

    //
    // Comprobar si el acceso al periférico está habilitado.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Comprobar si el acceso al periférico está habilitado.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
    {
    }
    //
    // Configurar el pin PB4 como salida.
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_4);
    //
    // Configurar la función PWM para el pin PB5.
    //
    MAP_GPIOPinConfigure(GPIO_PB5_M0PWM3);
    MAP_GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_5);
    

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //
    // Comprobar si el acceso al periférico está habilitado.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    //
    // Comprobar si el acceso al periférico está habilitado.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
    {
    }
    // Configurar la función PWM para el pin PD0.
    MAP_GPIOPinConfigure(GPIO_PD0_M0PWM6);
    MAP_GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    //
    // Configurar el pin PD1 como salida.
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    //
    // Comprobar si el acceso al periférico está habilitado.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }
    //
    // Configurar el pin PE4 como salida.
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
    // Configurar la función PWM para el pin PE5.
    MAP_GPIOPinConfigure(GPIO_PE5_M0PWM5);
    MAP_GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_5);

    //-------------------------------------------------------------
    //Comparator GPIO : PC7(sense C), PD6(sense B), PD7(sense A)
    //Atention with PD7 GPIO reserved by TI for NMI, need to unlock
    //Page 682 TIVA datasheet info


    //sense_C PC7
    //configure digital input
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_7); //dir=0 input, den=1 digital enable, Afsel=0 mode GPIO
    MAP_GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_7,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //habilitem pull-up intern

    //sense_B PD6
    //configure digital input
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6);//dir=0 input, den=1 digital enable, Afsel=0 mode GPIO
    MAP_GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_6,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //habilitem pull-up intern


    // Sense_A  PD7---------------------------------------------
    //PD7 is locked by default because it is multiplexed with NMI.
    // To modify its configuration registers (AFSEL, DEN, PCTL, DIR),
    // the GPIOLOCK register must be written with the unlock key 0x4C4F434B.
    //Unlock first

    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;  //unlock key
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR)  |= GPIO_PIN_7; // allow configuration changes on PD/
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;  //lock again

    //configure a input

    MAP_GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_7);//dir=0 input, den=1 digital enable, Afsel=0 mode GPIO
    MAP_GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_7,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //habilitem pull-up intern




}

// HAI,HBI,HCI (PB5, PE5, PD0): **High-Side MOSFET PWMs** para el control de fase.
void Setup_PWMs()
{
    //
    // Configurar el reloj del sistema a 16MHz (XTAL_16MHZ) sin divisor.
    //
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    //
    // Configurar el reloj PWM al reloj del sistema.
    //
    MAP_SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    // Configurar PWM0 para contar hacia arriba/abajo sin sincronización.
    //
    MAP_PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_UP_DOWN |
                        PWM_GEN_MODE_NO_SYNC);
    MAP_PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN |
                            PWM_GEN_MODE_NO_SYNC);
    MAP_PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN |
                            PWM_GEN_MODE_NO_SYNC);
    //
    // Establecer el período de PWM a 20kHz. Para calcular el parámetro
    // apropiado use la siguiente ecuación: N = (1 / f) * SysClk. Donde N es el
    // parámetro de función, f es la frecuencia deseada y SysClk es la
    // frecuencia del reloj del sistema.
    //
    MAP_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, (SysCtlClockGet() / 20000));
    MAP_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, (SysCtlClockGet() / 20000));
    MAP_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, (SysCtlClockGet() / 20000));
    //
    // Establecer PWM0 a un duty cycle del 20%. El duty cycle se establece
    // como una función del período. Dado que el período se estableció antes, puede usar la
    // función PWMGenPeriodGet(). Para este ejemplo, el PWM estará en alto por
    // el 20% del tiempo o (Período PWM / 5).
    //
    MAP_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,
                         MAP_PWMGenPeriodGet(PWM0_BASE, PWM_GEN_1) / 5);
    MAP_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5,
                             MAP_PWMGenPeriodGet(PWM0_BASE, PWM_GEN_2) / 5);
    MAP_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6,
                             MAP_PWMGenPeriodGet(PWM0_BASE, PWM_GEN_3) / 5);
    //
    // Por seguridad contra cortocircuitos SIEMPRE
    // deshabilitar la señal de salida de los Bits 3, 5 y 6 de PWM Out,
    // (PB5(C)/PE5(A)/PD0(B)), de inicio.
    //
    MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_3_BIT), false);
    MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_5_BIT), false);
    MAP_PWMOutputState(PWM0_BASE, (PWM_OUT_6_BIT), false);
    //
    // Habilitar el bloque generador de PWM.
    //
    MAP_PWMGenEnable(PWM0_BASE, PWM_GEN_1);
    MAP_PWMGenEnable(PWM0_BASE, PWM_GEN_2);
    MAP_PWMGenEnable(PWM0_BASE, PWM_GEN_3);

}
