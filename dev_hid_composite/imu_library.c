#include "imu_library.h"
#include "hardware/i2c.h"

// Need to define one array for storing raw IMU data and one for processed data in the main
// unsigned char imu_raw_data[14];
// int16_t imu_proc_data[7];

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

void init_MPU6050(){
    set_pin(MPU6050_ADDRESS, PWR_MGMT_1, 0x00);
    set_pin(MPU6050_ADDRESS, ACCEL_CONFIG, 0x00); // set sensitivity of accelerometer to +-2g
    set_pin(MPU6050_ADDRESS, GYRO_CONFIG, 0b00011000); // set gyroscope sensitivity to +-2000 dps
}

void read_imu_data(unsigned char *imu_raw_data){
    unsigned char reg = ACCEL_XOUT_H;
    i2c_write_blocking(I2C_PORT, MPU6050_ADDRESS, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDRESS, imu_raw_data, 14, false);
}

void process_imu_data(int16_t *imu_proc_data_ptr, unsigned char *imu_raw_data){
    for (int i = 0; i < 7; i++){
        unsigned char first_byte = imu_raw_data[2 * i];
        unsigned char second_byte = imu_raw_data[(2 * i) + 1];
        imu_proc_data_ptr[i] =  (int16_t) (first_byte << 8) | second_byte;
    }
}