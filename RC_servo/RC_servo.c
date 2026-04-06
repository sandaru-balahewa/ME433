#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PWMPIN 18
#define WRAP 60000
#define SERVO_PULSE_L_BOUND 0.05
#define SERVO_PULSE_U_BOUND 0.10

void set_servo(int angle){
    float duty = ((SERVO_PULSE_U_BOUND-SERVO_PULSE_L_BOUND)* angle/180.0 + SERVO_PULSE_L_BOUND) * WRAP;
    pwm_set_gpio_level(PWMPIN, (int) duty);
}

int main()
{
    stdio_init_all();

    // turn on the pwm
    gpio_set_function(PWMPIN, GPIO_FUNC_PWM); // Set the Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(PWMPIN); // Get PWM slice number

    float div = 50; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // sets the clock speed
    uint16_t wrap = WRAP; // when to rollover, must be less than 65535

    pwm_set_wrap(slice_num, wrap); 
    pwm_set_enabled(slice_num, true); // turn on the PWM

    pwm_set_gpio_level(PWMPIN, 0); 


    while (true) {
        for (int i=0; i<=180; i++){
            set_servo(i);
            sleep_ms(10);
        }
        for (int j=180; j>=0; j--){
            set_servo(j);
            sleep_ms(10);
        }
    }
}