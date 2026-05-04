#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "math.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
#define SPI_PORT spi0
#define PIN_SDI 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_SDO 19

static inline void cs_deselect(uint cs_pin);
static inline void cs_select(uint cs_pin);
void writeDAC(int channel, float v);

int main()
{
    stdio_init_all();

    // SPI initialization
    spi_init(SPI_PORT, 1000*1000); // the baud, or bits per second
    gpio_set_function(PIN_SDI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_SDO, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    // Sine wave at 2 Hz and triangle wave at 1 Hz
    int sine_freq = 2;
    int tri_freq = 1;

    //Update 50x faster than the frequency of the waves

    float t = 0;
    float dt = 0.01;
    float sine_voltage;
    float tri_voltage = 0;

    float tri_step = 3.3 / (((1.0 / tri_freq) / 2) / dt);
    bool tri_going_up = true;
    while (true) {
        // Calculate the sine wave voltage
        sine_voltage = (sin(2*M_PI*sine_freq*t) + 1) / 2 * 3.3;

        // Calculate the triangle wave voltage
        if (tri_going_up){
            tri_voltage += tri_step;
            if (tri_voltage >= 3.3){
                tri_voltage = 3.3;
                tri_going_up = false;
            }
        }
        else{
            tri_voltage -= tri_step;
            if (tri_voltage <= 0){
                tri_voltage = 0;
                tri_going_up = true;
            }
        }

        // call writeDAC
        writeDAC(0, sine_voltage);
        writeDAC(1, tri_voltage);
        t += 0.01;
        sleep_ms(10);
    }
}

void writeDAC(int channel, float v){

    uint8_t data[2];

    data[0] = 0b01110000;
    
    // putting the channel bit in
    data[0] = data[0] | ((channel & 0b1) << 7);

    uint16_t voltage = v / 3.3 * 1023; // 0b1111111111 10 bit number

    // put the first 4 bits of the 10bit voltage into data[0]'s last four bits
    data[0] = data[0] | ((voltage >> 6) & 0b00001111);

    // put the last 6 bits of the 10 bit voltage into data[1]'s first 6 bits
    data[1] = (voltage << 2) & 0b11111111;

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}
