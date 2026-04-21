#include <stdio.h>
#include "ssd1306.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "font.h"

#define LED_DEBUG 18

// I2C defines
#define I2C_PORT i2c_default
#define I2C_SDA 12
#define I2C_SCL 13
#define TEST_PIXEL_X 64
#define TEST_PIXEL_Y 16

void draw_letter(unsigned char x, unsigned char y, char letter){
    int letter_num = (int) letter - 32;
    for (int i = 0; i < 5; i++){
        char col = ASCII[letter_num][i];
        for (int j = 0; j < 8; j++){
            unsigned char bit = (col >> j) & 0b1;
            ssd1306_drawPixel(x + i, y + j, bit);
        }
    }
    ssd1306_update();
}

int main()
{
    stdio_init_all();

    // Initialize the debug LED
    gpio_init(LED_DEBUG);
    gpio_set_dir(LED_DEBUG, GPIO_OUT);
    gpio_put(LED_DEBUG, 0);

    // I2C Initialisation. Using it at 1700Khz.
    i2c_init(I2C_PORT, 1700*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    // debug led state
    bool led_state = true;

    // test pixel state
    unsigned char pix_state = 1;

    while (true) {
        // blinking led on pico for debugging purposes
        gpio_put(LED_DEBUG, led_state);
        led_state = !led_state;

        // // blinking a test pixel
        // ssd1306_drawPixel(TEST_PIXEL_X, TEST_PIXEL_Y, pix_state);
        // ssd1306_update();
        // pix_state = !pix_state;

        // char let = 'A';
        draw_letter(TEST_PIXEL_X, TEST_PIXEL_Y, 'B');
        sleep_ms(1000);
    }
}
