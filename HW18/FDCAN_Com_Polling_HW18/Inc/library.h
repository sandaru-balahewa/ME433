/*
 * library.h
 *
 *  Created on: Jun 2, 2026
 *      Author: sanda
 */

#ifndef APPLICATION_USER_LIBRARY_H_
#define APPLICATION_USER_LIBRARY_H_

#include <stdint.h>
#include "stm32c0xx_hal.h"

uint32_t read_adc(ADC_HandleTypeDef *hadc);
void init_ina219();
float read_ina219();
void writeINA219(int reg, int value);
signed short readINA219(unsigned char reg);




#endif /* APPLICATION_USER_LIBRARY_H_ */
