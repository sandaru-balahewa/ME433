#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define LED_DEBUG 18
#define CHIP_ADDRESS 0b0100000
#define IODIR_REG 0x00
#define OLAT_REG 0x0A
#define GPIO_REG 0x09

// I2C defines
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

void set_pin(unsigned char address, unsigned char reg, unsigned char val){
    unsigned char buf[2] = {reg, val};
    i2c_write_blocking(i2c_default, address, buf, 2, false);
}

unsigned char read_pin(unsigned char address, unsigned char reg){
    unsigned char buf;
    i2c_write_blocking(i2c_default, address, &reg, 1, true);
    i2c_read_blocking(i2c_default, address, &buf, 1, false);
    return buf;
}

int main()
{
    stdio_init_all();

    gpio_init(LED_DEBUG);
    gpio_set_dir(LED_DEBUG, GPIO_OUT);
    gpio_put(LED_DEBUG, 0);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // Initialize the MCP23008 to set GP7 to output (LED) and GP0 to input (PB)
    set_pin(CHIP_ADDRESS, IODIR_REG, 0b01111111);

    // variable to store the value of GPIO reg
    unsigned char temp_reg;

    // debug led state
    bool state = true;

    while (true) {
        // blinking led on pico for debugging purposes
        gpio_put(LED_DEBUG, state);
        // read from GPIO register
        temp_reg = read_pin(CHIP_ADDRESS, GPIO_REG);

        // check if PB is pushed
        if ((temp_reg & 1) == 0){
            // turn on led
            set_pin(CHIP_ADDRESS, OLAT_REG, 0b10000000);
            sleep_ms(100);
            set_pin(CHIP_ADDRESS, OLAT_REG, 0x00000000);
        }
        else{
            sleep_ms(100);
        }
        state = !state;
    }
}
