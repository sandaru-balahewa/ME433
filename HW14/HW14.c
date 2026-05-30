#include <stdio.h>
#include "pico/stdlib.h"

#define SCK_PIN 21
#define DATA_PIN 20
#define CLOCK_TIME_US 1

void init_hx711(void);
int hx711_read_raw(void);

int main()
{
    stdio_init_all();
    // Initialize HX711
    init_hx711();


    while (true) {
        int num = 0;
        int val_arr[1000];
        int raw_arr[1000];
        uint64_t t[1000];

        // Wait for the computer to send a number of samples to collect
        scanf("%d", &num);
        int avg = 570000;

        // Read and store the asked number of samples from HX711
        for (int i=0; i<num; i++){
            int val = -hx711_read_raw(); // Multiply by negative 1 because the sensor outputs negative numbers
            raw_arr[i] = val;
            // IIR filter
            avg = val*0.2 + avg*0.8;
            val_arr[i] = avg;
            t[i] = to_ms_since_boot(get_absolute_time());
        }

        // Print all the samples back to the serial monitor
        for (int i=0; i<num; i++){
            printf("%d %llu %d %d\n", i, t[i], raw_arr[i], val_arr[i]);
        }

    }
}


// Function to initialize the SCK and DT Pins of HX711
void init_hx711(void){
    gpio_init(SCK_PIN);
    gpio_set_dir(SCK_PIN, GPIO_OUT);
    gpio_put(SCK_PIN, 0);

    gpio_init(DATA_PIN);
    gpio_set_dir(DATA_PIN, GPIO_IN);
    gpio_pull_up(DATA_PIN);
}


// Function to read out 24 bits from HX711
int hx711_read_raw(void){
    // Wait until the data pin goes to low
    while (gpio_get(DATA_PIN)){
        tight_loop_contents();
    }
    sleep_us(1);

    unsigned int raw = 0;
    for (int i=0; i<24; i++){
        // Pulse the clock pin
        gpio_put(SCK_PIN, 1);
        sleep_us(CLOCK_TIME_US); // Short settle time

        // read the data pin
        raw = (raw << 1) | (gpio_get(DATA_PIN) ? 1 : 0);

        // put the clock pin low
        gpio_put(SCK_PIN, 0);
        sleep_us(CLOCK_TIME_US); // short settle time
    }

    // 25th pulse to set the gain to 128 for the next reading
    gpio_put(SCK_PIN, 1);
    sleep_us(CLOCK_TIME_US);
    gpio_put(SCK_PIN, 0);
    sleep_us(CLOCK_TIME_US);

    // sign-extend 24-bit two's complement to 32-bit signed int
    if (raw & 0x800000){
        raw = raw | 0xFF000000;
    }

    return (int) raw;

}

