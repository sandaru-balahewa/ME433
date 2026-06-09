#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

// Encoder Registers and addresses
#define ENCODER_I2C_ADDRESS 0x36
#define STATUS_REG 0x0B
#define RAW_ANGLE_H 0x0C
#define ANGLE_H 0x0E


void initialize_encoder(void);


int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    sleep_ms(10);


    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}

void initialize_encoder(void){
    // Read the magnet status
    uint8_t reg = STATUS_REG;
    uint8_t val;
    i2c_write_blocking(I2C_PORT, ENCODER_I2C_ADDRESS, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, ENCODER_I2C_ADDRESS, &val, 1, false);
    
    if (val & (1 << 5)){
        // If true magnet is detected
        printf("Magnet Detected\n");
    }
    if (val & (1 << 4)){
        // If true, magnet too week
        printf("Magnet too weak\n");
    }
    if (val & (1 << 3)){
        // If true, magnet too strong
        printf("Magnet too strong\n");
    }
}