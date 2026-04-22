#include <stdio.h>
#include <stdlib.h>
#include "ssd1306.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "font.h"
#include "oled.h"

void draw_letter(unsigned char x, unsigned char y, char letter){
    int letter_num = (int) letter - 32;
    // iterate for each column of ascii bitmap
    for (int i = 0; i < 5; i++){
        char col = ASCII[letter_num][i];
        // draw each bit in the column
        for (int j = 0; j < 8; j++){
            unsigned char bit = (col >> j) & 0b1;
            ssd1306_drawPixel(x + i, y + j, bit);
        }
    }
}

void draw_message(unsigned char x, unsigned char y, char *message){
    int i = 0;
    unsigned char new_x = x;
    unsigned char new_y = y;
    while (message[i] != 0){
        draw_letter(new_x, new_y, message[i]);
        i++;
        // calculate new_x and new_y positions
        if (new_x + 9 > 127){
            new_x = 0;
            new_y = new_y + 8;
        }
        else {
            new_x = new_x + 5;
        }
        
    }
}

void draw_line(unsigned char start_x, unsigned char start_y, unsigned char end_x, unsigned char end_y){
    // Using Bresenham's Line Algorithm
    int x0 = start_x;
    int y0 = start_y;
    int x1 = end_x;
    int y1 = end_y;

    int incr_x, incr_y;

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    if (x1 - x0 > 0){
        incr_x = 1;
    }
    else{
        incr_x = -1;
    }

    if (y1 - y0 > 0){
        incr_y = 1;
    }
    else{
        incr_y = -1;
    }

    int err = dx - dy;

    while (true) {
        ssd1306_drawPixel(x0, y0, 1);

        if (x0 == x1 && y0 == y1){
            break;
        }
        int e2 = 2 * err;

        if (e2 > -dy){
            err -= dy;
            x0 += incr_x;
        }
        if (e2 < dx){
            err += dx;
            y0 += incr_y;
        }

    }
}

void init_display(){
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();
}

