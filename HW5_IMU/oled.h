#ifndef OLED_H__
#define OLED_H__

// I2C defines
#define TEST_PIXEL_X 64
#define TEST_PIXEL_Y 16

#define LED_DEBUG 18

void draw_letter(unsigned char x, unsigned char y, char letter);
void draw_message(unsigned char x, unsigned char y, char *message);
void draw_line(unsigned char start_x, unsigned char start_y, unsigned char end_x, unsigned char end_y);
void init_display();

#endif