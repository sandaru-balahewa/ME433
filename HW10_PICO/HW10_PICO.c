#include <stdio.h> // set pico_enable_stdio_usb to 1 in CMakeLists.txt 
#include "pico/stdlib.h" // CMakeLists.txt must have pico_stdlib in target_link_libraries
#include "hardware/adc.h" // CMakeLists.txt must have hardware_adc in target_link_libraries

#define POT_PIN 26
#define PB_PIN 9


int main()
{
    stdio_init_all();

    // initialize the push button with pull up resistor
    gpio_init(PB_PIN);
    gpio_set_dir(PB_PIN, GPIO_IN);
    gpio_pull_up(PB_PIN);

    // turn on the adc
    adc_init();
    adc_gpio_init(POT_PIN); // pin GP26 is pin ADC0
    adc_select_input(0); // sample from ADC0

    while (true) {
        // Read ADC (potentiometer)
        uint16_t pot = adc_read();

        // Read the push button
        int pb_state = gpio_get(PB_PIN);

        // Send data to serial monitor
        printf("(%d,%d)\n", pot, pb_state);

        sleep_ms(1000/30); // send the numbers at 60Hz to the game
    }
}