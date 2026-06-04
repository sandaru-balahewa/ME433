/*
 * library.c
 *
 *  Created on: Jun 2, 2026
 *      Author: sanda
 */

#include "library.h"

extern I2C_HandleTypeDef hi2c2;

#define INA219_ADDR 0b1000000 // I2C address
#define INA219_REG_CONFIG 0x00 // config register address
#define INA219_REG_CURRENT 0x04 // current register
#define INA219_REG_CALIBRATION 0x05 // calibration register

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


void init_ina219(){
    // set the INA219 sensitivity - 10 bit, plus/minus160mV, 148us per sample
    unsigned short ina219_calValue = 1024;
    unsigned short ina219_config = 0b0011000010001111;
    writeINA219(INA219_REG_CALIBRATION, ina219_calValue);
    writeINA219(INA219_REG_CONFIG, ina219_config);
}

float read_ina219(){
    float ma = 0;
    signed short value = readINA219(INA219_REG_CURRENT);
    ma = value / 3.0;
    return ma;
}

// write 2 bytes
void writeINA219(int reg, int value){
    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = value>>8;
    buf[2] = value&0xff;

    HAL_I2C_Master_Transmit(&hi2c2, INA219_ADDR<<1, buf, 3, 10);
}

// read 2 bytes
signed short readINA219(unsigned char reg){
    HAL_I2C_Master_Transmit(&hi2c2, INA219_ADDR<<1, &reg, 1, 10);
    uint8_t buffer[2];
    HAL_I2C_Master_Receive(&hi2c2, INA219_ADDR<<1, buffer, 2, 10);

    signed short value = (buffer[0]<<8)|buffer[1];
    return value;
}
