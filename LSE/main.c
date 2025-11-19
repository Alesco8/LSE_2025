

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "GPIOs.h"
#include "Timers.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

void Timer0IntHandler();
extern volatile uint16_t count;

void main(void)
{
    bool closed_loop_mode = false;
    Setup_GPIOs();
    MAP_SysCtlDelay((MAP_SysCtlClockGet() * 2) / 3);
    Setup_PWMs();
    Setup_Timers();

    while(1)
    {
        if (!closed_loop_mode)
        {
            // --- MODO OPEN LOOP ---
            // El motor se mueve solo gracias a Timer0IntHandler

            // Verificamos si la rampa ha terminado (count > 1100)
            if (count > 1100)
            {
                // 1. Deshabilitamos la interrupción del Timer para que
                //    deje de conmutar ciegamente.
                MAP_IntDisable(INT_TIMER0A);
                MAP_TimerDisable(TIMER0_BASE, TIMER_A);

                // 2. Cambiamos el flag para entrar en modo lazo cerrado
                closed_loop_mode = true;
            }
            else
            {}
        }
        else
        {
            // --- MODO CLOSED LOOP ---
            // Leemos comparadores y conmutamos inmediatamente

        }
    }
}
