/*
 * library.c
 *
 *  Created on: Jun 2, 2026
 *      Author: sanda
 */

#include "library.h"

uint32_t read_adc(ADC_HandleTypeDef *hadc){
	uint32_t raw;
	HAL_ADC_Start(hadc);                                 // start conversion
	if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK)  // wait (10 ms timeout)
	{
	    raw = HAL_ADC_GetValue(hadc);                   // read raw ADC value
	}
	else {
	    raw = 0;
	}
	HAL_ADC_Stop(hadc);
	return raw;
}

