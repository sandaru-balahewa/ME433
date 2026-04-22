#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
#define I2C_PORT i2c0
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

char imu_raw_data[14];
int16_t imu_proc_data[7];


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
    i2c_write_blocking(I2C_PORT, MPU6050_ADDRESS, ACCEL_XOUT_H, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDRESS, &imu_raw_data, 14, false);
}

void process_imu_data(){
    for (int i = 0; i < 7; i++){
        char first_byte = imu_raw_data[2 * i];
        char second_byte = imu_raw_data[2 * i + 1];
        imu_proc_data[i] = (first_byte << 8) | second_byte;
    }
}

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA);
    // gpio_pull_up(I2C_SCL);

    init_MCP6050();

    while (true) {
        printf("Hello, world!\n");
        read_imu_data();
        process_imu_data();

        print(imu_proc_data[0], imu_proc_data[1], imu_proc_data[2]);
        sleep_ms(100);
    }
}
