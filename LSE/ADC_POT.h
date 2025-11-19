/*
 * ADC_POT.h
 *
 *  Created on: 18 nov. 2025
 *      Author: ArmJa
 */

#ifndef ADC_POT_H_
#define ADC_POT_H_


// set the input AIN3-PE0 to read the potenciometer
void Setup_ADC_Pot(void);


// read the potenciometer and returns a value from it between 0-4095
uint32_t ADC_PotRead(void);


#endif /* ADC_POT_H_ */
