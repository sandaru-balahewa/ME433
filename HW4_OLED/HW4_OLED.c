#include <stdio.h>
#include "ssd1306.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define LED_DEBUG 18

// I2C defines
#define I2C_PORT i2c_default
#define I2C_SDA 12
#define I2C_SCL 13
#define TEST_PIXEL_X 64
#define TEST_PIXEL_Y 16


int main()
{
    stdio_init_all();

    // Initialize the debug LED
    gpio_init(LED_DEBUG);
    gpio_set_dir(LED_DEBUG, GPIO_OUT);
    gpio_put(LED_DEBUG, 0);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    ssd1306_setup();

    // debug led state
    bool led_state = true;

    // test pixel state
    unsigned char pix_state = 1;

    while (true) {
        // blinking led on pico for debugging purposes
        gpio_put(LED_DEBUG, led_state);
        ssd1306_drawPixel(TEST_PIXEL_X, TEST_PIXEL_Y, pix_state);
        ssd1306_update();
        led_state = !led_state;
        pix_state = !pix_state;
        sleep_ms(1000);
    }
}
