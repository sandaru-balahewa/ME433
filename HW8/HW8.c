#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "math.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
#define SPI_PORT spi0
#define PIN_SDI 16
#define DAC_PIN_CS 17
#define RAM_PIN_CS 20
#define PIN_SCK  18
#define PIN_SDO 19

static inline void cs_deselect(uint cs_pin);
static inline void cs_select(uint cs_pin);
void writeDAC(int channel, float v);
void spi_ram_init();

int main()
{
    stdio_init_all();

    // SPI initialization for DAC
    spi_init(SPI_PORT, 20000*1000); // the baud, or bits per second
    gpio_set_function(PIN_SDI, GPIO_FUNC_SPI);
    gpio_set_function(DAC_PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_SDO, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(DAC_PIN_CS, GPIO_OUT);
    gpio_put(DAC_PIN_CS, 1);

    // Sine wave at 2 Hz
    int sine_freq = 2;

    // Create an array to store the single cycle sine wave as values required by the DAC
    uint16_t sine_wave_arr[1000];

    float t = 0;
    float dt = 0.0005;
    for (int i=0; i<1000; i++){
        uint16_t sine_voltage_dac = (sin(2*M_PI*sine_freq*t) + 1) / 2 * 1023;

        uint8_t data[2];
        data[0] = 0b01110000;
    
        // putting the channel bit in
        data[0] = data[0] | ((0 & 0b1) << 7);

        // put the first 4 bits of the 10bit voltage into data[0]'s last four bits
        data[0] = data[0] | ((sine_voltage_dac >> 6) & 0b00001111);

        // put the last 6 bits of the 10 bit voltage into data[1]'s first 6 bits
        data[1] = (sine_voltage_dac << 2) & 0b11111111;

        sine_wave_arr[i] = (data[0] << 8) | data[1];
        t += dt;
    }
    
    float sine_voltage;

    while (true) {
        

        // call writeDAC
        writeDAC(0, sine_voltage);
        t += dt;
        sleep_ms(1);
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

    cs_select(DAC_PIN_CS);
    spi_write_blocking(SPI_PORT, data, 2); // where data is a uint8_t array with length len
    cs_deselect(DAC_PIN_CS);
}

void spi_ram_init(){
    uint8_t init_data[2];

    init_data[0] = 0b00000001; //Write Status register instruction
    init_data[1] = 0b01000000; // Sequential operation

    cs_select(RAM_PIN_CS);
    spi_write_blocking(SPI_PORT, init_data, 2);
    cs_deselect(RAM_PIN_CS);
}

void spi_ram_write(uint16_t address, uint8_t *data, int len){
    uint8_t command[3];

    command[0] = 0b00000010;
    command[1] = (address >> 8) & 0xFF; // high byte of the address
    command[2] = address & 0xFF;

    cs_select(RAM_PIN_CS);
    spi_write_blocking(SPI_PORT, command, 3);
    spi_write_blocking(SPI_PORT, data, len);
    cs_deselect(RAM_PIN_CS);
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
