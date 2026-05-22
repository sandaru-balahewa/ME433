#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart0
#define BAUD_RATE 115200

// Use pins 0 and 1 for UART0
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 0
#define UART_RX_PIN 1



int main()
{
    stdio_init_all();

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // buffers
    char uart_buffer[1000];
    int uart_index = 0;

    char usb_buffer[1000];
    int usb_index = 0;
    
    // Send out a string, with CR/LF conversions
    // uart_puts(UART_ID, " Hello, UART!\n");
    

    while (true) {
        // Receive from STM32 (UART)
        while (uart_is_readable(UART_ID)){
            char c = uart_getc(UART_ID);

            // End of message?
            if (c == '\n'){
                uart_buffer[uart_index] = '\0';
                printf("%s\n", uart_buffer);
                uart_index = 0;
            }
            else{
                uart_buffer[uart_index] = c;
                uart_index++;

                // Prevent overflow
                if (uart_index == 1000){
                    uart_index = 0;
                }
            }
        }

        // Receive from USB
        int ch = getchar_timeout_us(10);

        if (ch != PICO_ERROR_TIMEOUT){
            char c = (char) ch;

            // End of line?
            if (c == '\n'){
                usb_buffer[usb_index] = '\0';

                // send to STM32 via UART
                uart_puts(UART_ID, usb_buffer);
                uart_puts(UART_ID, "\n");

                // Testing
                printf("%s\n", usb_buffer);

                usb_index = 0;
            }
            else{
                usb_buffer[usb_index] = c;
                usb_index++;

                //Prevent overflow
                if (usb_index == 1000){
                    usb_index = 0;
                }
            }
        }
        sleep_ms(1);
    }
}
