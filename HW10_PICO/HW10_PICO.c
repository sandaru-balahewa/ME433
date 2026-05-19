#include <stdio.h> // set pico_enable_stdio_usb to 1 in CMakeLists.txt 
#include "pico/stdlib.h" // CMakeLists.txt must have pico_stdlib in target_link_libraries
#include "hardware/adc.h" // CMakeLists.txt must have hardware_adc in target_link_libraries

#define POT_PIN 16


int main()
{
    stdio_init_all();

    // turn on the adc
    adc_init();
    adc_gpio_init(POT_PIN); // pin GP26 is pin ADC0
    adc_select_input(0); // sample from ADC0

    while (true) {
        // Read ADC
        uint16_t pot = adc_read();
        float voltage = pot * 3.3 / 4095.0;
    }
}