#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

// Defines for the load sensor
#define SCK_PIN 21
#define DATA_PIN 20
#define CLOCK_TIME_US 1

// Encoder Registers and addresses
#define ENCODER_I2C_ADDRESS 0x36
#define STATUS_REG 0x0B
#define RAW_ANGLE_H 0x0C
#define ANGLE_H 0x0E

// Encoder function prototypes
void initialize_encoder(void);
uint16_t read_raw_angle(void);

// Load sensor function prototypes
void init_hx711(void);
int hx711_read_raw(void);

int main()
{
    stdio_init_all();

    // Initialize force sensor
    init_hx711();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    sleep_ms(10);
    initialize_encoder();

    while (true) {
        uint16_t raw_angle = read_raw_angle();
        printf("%d\n", raw_angle);
        sleep_ms(10);

        // Force sensor existing code
        // int num = 0;
        // int val_arr[1000];
        // int raw_arr[1000];
        // uint64_t t[1000];

        // // Wait for the computer to send a number of samples to collect
        // scanf("%d", &num);
        // int avg = 580000;

        // // Read and store the asked number of samples from HX711
        // for (int i=0; i<num; i++){
        //     int val = -hx711_read_raw(); // Multiply by negative 1 because the sensor outputs negative numbers
        //     raw_arr[i] = val;
        //     // IIR filter
        //     avg = val*0.2 + avg*0.8;
        //     val_arr[i] = avg;
        //     t[i] = to_ms_since_boot(get_absolute_time());
        // }

        // // Print all the samples back to the serial monitor
        // for (int i=0; i<num; i++){
        //     printf("%d %llu %d %d\n", i, t[i], raw_arr[i], val_arr[i]);
        // }

        
    }
}

void initialize_encoder(void){
    // Read the magnet status
    uint8_t reg = STATUS_REG;
    uint8_t val;
    i2c_write_blocking(I2C_PORT, ENCODER_I2C_ADDRESS, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, ENCODER_I2C_ADDRESS, &val, 1, false);
    
    if (val & (1 << 5)){
        // If true magnet is detected
        printf("Magnet Detected\n");
    }
    if (val & (1 << 4)){
        // If true, magnet too week
        printf("Magnet too weak\n");
    }
    if (val & (1 << 3)){
        // If true, magnet too strong
        printf("Magnet too strong\n");
    }
}

uint16_t read_raw_angle(void){
    uint8_t reg = RAW_ANGLE_H;
    uint8_t buf[2];
    i2c_write_blocking(i2c0, ENCODER_I2C_ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c0, ENCODER_I2C_ADDRESS, buf, 2, false);

    // Return the raw angle by stitching the two bytes
    // buf[0]'s bit0-bit3 are needed
    // buf[1]'s all bits are needed
    return ((uint16_t)(buf[0] & 0x0F) << 8) | buf[1];
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
