#include <stdio.h>
#include "pico/stdlib.h"
#include "can.h"
 
#define CAN_ID 0x150
 
int main(void) {
    stdio_init_all();
    sleep_ms(100);
 
    can_init();
 
    const uint32_t PERIOD_US = 1000;  // 1000Hz
    uint32_t next_time = time_us_32();
 
    float desired_current = 200.0f;  // replace with your value
 
    while (true) {
        bool acked = can_send_float(CAN_ID, desired_current);
 
        if (!acked) {
            printf("CAN no ACK\n");
        }
 
        next_time += PERIOD_US;
        uint32_t now = time_us_32();
        if ((int32_t)(next_time - now) > 0) {
            sleep_us(next_time - now);
        }
    }
}