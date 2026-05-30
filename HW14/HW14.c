#include <stdio.h>
#include "pico/stdlib.h"

#define SCK_PIN 21
#define DATA_PIN 20

void init_pins(void);

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


// Function to initialize the SCK and DT Pins
void init_pins(void){
    gpio_init(SCK_PIN);
    gpio_set_dir(GPIO_OUT);
    gpio_init(DATA_PIN);
    gpio_set_dir(GPIO_IN);
}


