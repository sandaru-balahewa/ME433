#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <math.h>
#include "hardware/uart.h"

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
#define HX711_OFFSET 593000   // zero-load raw value (calibrate this)


// Encoder Registers and addresses
#define ENCODER_I2C_ADDRESS 0x36
#define STATUS_REG 0x0B
#define RAW_ANGLE_H 0x0C
#define ANGLE_H 0x0E

// Angle thresholds (raw 0-4095 units)
#define ANGLE_FREE_LOW      2450     // below this: ramp force up (left wall)
#define ANGLE_FREE_HIGH     3050     // above this: ramp force up (right wall)
#define ANGLE_WALL_LOW      2050     // at/below this: full left wall force
#define ANGLE_WALL_HIGH     3450     // at/above this: full right wall force
#define WALL_FORCE_MAX_MA   500.0f  // max desired force in mA (tune to motor)

// PD controller gains
#define KP  0.1f
#define KD  0.05f

// Current clamp sent to STM32 (mA)
// ---------------------------------------------------------------------------
#define CURRENT_MAX_MA  800.0f

// UART to STM32
// Pico UART1: TX=GP8, RX=GP9  →  wire GP8 to STM32 UART RX, share GND
// ---------------------------------------------------------------------------
#define UART_ID     uart1
#define UART_TX     8
#define UART_RX     9
#define UART_BAUD   115200

// UART function prototypes
static void uart_to_stm32_init();
static void uart_send_float(float value);

// Encoder function prototypes
void initialize_encoder(void);
uint16_t read_raw_angle(void);

// Load sensor function prototypes
void init_hx711(void);
int hx711_read_raw(void);

// Compute desired force from force-angle plot
static float compute_desired_force(uint16_t angle);


int main()
{
    stdio_init_all();

    // Initialize force sensor
    init_hx711();

    // I2C and encoder initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    sleep_ms(10);
    initialize_encoder();

    // UART initialization
    uart_to_stm32_init();

    float prev_error = 0.0f;
    const float dt = 0.01f;   // 100 Hz loop (sleep_ms(10) below)

    while (true) {
        uint16_t angle       = read_raw_angle();
        float actual_force   = (float)hx711_read_raw();
        float desired_force  = compute_desired_force(angle);
 
        // printf("%f\n", actual_force);
        // PD controller
        float error      = desired_force - actual_force;
        float derivative = (error - prev_error) / dt;
        float desired_current_ma = KP * error + KD * derivative;
        prev_error = error;
 
        // Clamp
        if (desired_current_ma >  CURRENT_MAX_MA) desired_current_ma =  CURRENT_MAX_MA;
        if (desired_current_ma < -CURRENT_MAX_MA) desired_current_ma = -CURRENT_MAX_MA;
 
        // Send to STM32 over UART
        uart_send_float(desired_current_ma);
 
        // Debug to serial (comment out if too slow)
        printf("angle=%4u  f_des=%7.1f  f_act=%7.1f  i_des=%7.1f mA\n",
               angle, desired_force, actual_force, desired_current_ma);
 
        sleep_ms(10);   // ~100 Hz
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

    raw = raw * (-1);

    int force_calibrated = raw - HX711_OFFSET;

    return force_calibrated;

}


// ---------------------------------------------------------------------------
// Haptic wall: compute desired force from raw angle
//
//  ANGLE_WALL_LOW    ANGLE_FREE_LOW     ANGLE_FREE_HIGH    ANGLE_WALL_HIGH
//       |<--- ramp up force --->|<--- zero force --->|<--- ramp up force --->|
// ---------------------------------------------------------------------------
static float compute_desired_force(uint16_t angle) {
    if (angle <= ANGLE_WALL_LOW) {
        return -WALL_FORCE_MAX_MA;   // full left wall
    }
    if (angle >= ANGLE_WALL_HIGH) {
        return  WALL_FORCE_MAX_MA;   // full right wall
    }
    if (angle >= ANGLE_FREE_LOW && angle <= ANGLE_FREE_HIGH) {
        return 0.0f;                 // free middle zone
    }
    if (angle < ANGLE_FREE_LOW) {
        // left ramp: ANGLE_WALL_LOW -> ANGLE_FREE_LOW maps to -MAX -> 0
        float t = (float)(angle - ANGLE_WALL_LOW) /
                  (float)(ANGLE_FREE_LOW - ANGLE_WALL_LOW);
        return -WALL_FORCE_MAX_MA * (1.0f - t);
    }
    // right ramp: ANGLE_FREE_HIGH -> ANGLE_WALL_HIGH maps to 0 -> +MAX
    float t = (float)(angle - ANGLE_FREE_HIGH) /
              (float)(ANGLE_WALL_HIGH - ANGLE_FREE_HIGH);
    return WALL_FORCE_MAX_MA * t;
}

// UART Functions

static void uart_to_stm32_init() {
    uart_init(UART_ID, UART_BAUD);
    gpio_set_function(UART_TX, GPIO_FUNC_UART);
    gpio_set_function(UART_RX, GPIO_FUNC_UART);
}
 
// Send desired current as 4 raw float bytes
static void uart_send_float(float value) {
    uart_write_blocking(UART_ID, (uint8_t *)&value, sizeof(float));
}