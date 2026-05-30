#include <stdio.h>
#include "pico/stdlib.h"

#define SCK_PIN 21
#define DATA_PIN 20

void init_hx711(void);
int hx711_read_raw(void);

int main()
{
    stdio_init_all();
    // Initialize SCK and DT pins on HX711
    init_pins();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}


// Function to initialize the SCK and DT Pins of HX711
void init_hx711(void){
    gpio_init(SCK_PIN);
    gpio_set_dir(SCK_PIN, GPIO_OUT);
    gpio_put(SCK_PIN, 0);

    gpio_init(DATA_PIN);
    gpio_set_dir(DATA_PIN, GPIO_IN);
    gpio_pull_up(DATA_PIN)
}


// Function to read out 24 bits from HX711
int hx711_read_raw(void){

}

