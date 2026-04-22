#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "oled.h"
#include "ssd1306.h"

// I2C defines
#define I2C_PORT i2c_default
// #define I2C_SDA 4
// #define I2C_SCL 5
#define I2C_SDA 4
#define I2C_SCL 5

// MPU6050 defines
#define MPU6050_ADDRESS 0x68

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C

// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

unsigned char imu_raw_data[14];
int16_t imu_proc_data[7];

#define LED_DEBUG 18

void set_pin(unsigned char address, unsigned char reg, unsigned char val){
    unsigned char buf[2] = {reg, val};
    i2c_write_blocking(I2C_PORT, address, buf, 2, false);
}

unsigned char read_pin(unsigned char address, unsigned char reg){
    unsigned char buf;
    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, address, &buf, 1, false);
    return buf;
}

void init_MCP6050(){
    set_pin(MPU6050_ADDRESS, PWR_MGMT_1, 0x00);
    set_pin(MPU6050_ADDRESS, ACCEL_CONFIG, 0x00); // set sensitivity of accelerometer to +-2g
    set_pin(MPU6050_ADDRESS, GYRO_CONFIG, 0b00011000); // set gyroscope sensitivity to +-2000 dps
}

void read_imu_data(){
    unsigned char reg = ACCEL_XOUT_H;
    i2c_write_blocking(I2C_PORT, MPU6050_ADDRESS, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDRESS, imu_raw_data, 14, false);
}

void process_imu_data(){
    for (int i = 0; i < 7; i++){
        unsigned char first_byte = imu_raw_data[2 * i];
        unsigned char second_byte = imu_raw_data[(2 * i) + 1];
        imu_proc_data[i] =  (int16_t) (first_byte << 8) | second_byte;
    }
}

int main()
{
    stdio_init_all();

    // Initialize the debug LED
    gpio_init(LED_DEBUG);
    gpio_set_dir(LED_DEBUG, GPIO_OUT);
    gpio_put(LED_DEBUG, 0);
    
    // setting SDA and SCL pins for I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SDA_OLED, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SCL_OLED, GPIO_FUNC_I2C);

    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // I2C Initialisation. Using it at 1700Khz.
    i2c_init(I2C_PORT, 400*1000);

    // initialize the display
    init_display();

    //initialize the IMU
    init_MCP6050();
    unsigned char who_am_i = read_pin(MPU6050_ADDRESS, WHO_AM_I);
    if (who_am_i != 0x68){
        while (true)
        {
            printf("IMU not correctly initialized\n");
            gpio_put(LED_DEBUG, 1);
        }
    }

    float accel[3];
    float gyro[3];
    float temp;

    while (true) {
        ssd1306_clear();
        read_imu_data();
        process_imu_data();

        for (int i=0; i<3; i++){
            accel[i] = (float) imu_proc_data[i] * 0.000061;
        }

        int j = 0;
        for (int i=4; i<7; i++){
            gyro[j] = (float) imu_proc_data[i] * 0.007630;
            j++;
        }
        temp = (float) imu_proc_data[3] / 340.0 + 36.53;

        printf("X_Accel: %i, Y_Accel: %i, Z_Accel: %i\n", imu_proc_data[0], imu_proc_data[1], imu_proc_data[2]);

        printf("X_Accel: %.3f g, Y_Accel: %.3f g, Z_Accel: %.3f g\n", accel[0], accel[1], accel[2]);
        printf("Gyro X: %.3f dps, Gyro Y: %.3f dps, Gyro Z: %.3f dps\n", gyro[0], gyro[1], gyro[2]);
        printf("Temperature: %.1f C\n", temp);

        // Drawing lines
        int dx = imu_proc_data[0] * 64 / 32768;
        int dy = imu_proc_data[1] * 16 / 32768;

        int x1 = 64 + dx;
        int y1 = 16 - dy; // oled screen's y axis points down

        draw_line(64, 16, x1, y1);
        ssd1306_update();
        sleep_ms(10);
    }
}
