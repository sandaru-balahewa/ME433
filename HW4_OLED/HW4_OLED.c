#include <stdio.h>
#include "ssd1306.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
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
    // iterate for each column of ascii bitmap
    for (int i = 0; i < 5; i++){
        char col = ASCII[letter_num][i];
        // draw each bit in the column
        for (int j = 0; j < 8; j++){
            unsigned char bit = (col >> j) & 0b1;
            ssd1306_drawPixel(x + i, y + j, bit);
        }
    }
}

void draw_message(unsigned char x, unsigned char y, char *message){
    int i = 0;
    unsigned char new_x = x;
    unsigned char new_y = y;
    while (message[i] != 0){
        draw_letter(new_x, new_y, message[i]);
        i++;
        // calculate new_x and new_y positions
        if (new_x + 9 > 127){
            new_x = 0;
            new_y = new_y + 8;
        }
        else {
            new_x = new_x + 5;
        }
        
    }
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

    // initialize the display
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    // initialize adc
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // debug led state
    bool led_state = true;

    // test pixel state
    unsigned char pix_state = 1;

    absolute_time_t t_start, t_end;
    uint64_t t;
    float fps = 0.0;

    char line1[30];
    char line2[30];
    char line3[30];
    char line4[30];

    while (true) {
        // get start time
        t_start = get_absolute_time();
        ssd1306_clear();

        // blinking led on pico for debugging purposes
        gpio_put(LED_DEBUG, led_state);
        led_state = !led_state;

        // Read ADC
        uint16_t pot = adc_read();
        float voltage = pot * 3.3 / 4095.0;

        sprintf(line1, "ADC0 voltage is %.2f V", voltage);
        sprintf(line2, "Go! U Northwestern!");
        sprintf(line3, "Fight for victory.");
        sprintf(line4, "FPS = %7.4f", fps);

        // write to oled screen
        draw_message(0, 0, line1);
        draw_message(0, 8, line2);
        draw_message(0, 16, line3);
        draw_message(0, 24, line4);
        ssd1306_update();

        // get end time
        t_end = get_absolute_time();

        t = to_us_since_boot(t_end) - to_us_since_boot(t_start);

        // calculate fps
        fps = 1.0 / (t / 1000000.0);

        sleep_ms(500);
    }
}
