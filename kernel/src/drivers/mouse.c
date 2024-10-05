#include <interrupts/irq.h>
#include <interrupts/idt.h>
#include "keyboard.h"
#include <interrupts/io.h>
#include <stdint.h>
#include <include/constants.h>
#include <screen.h>
#include "mouse.h"
#include <stdbool.h>
#include <sprint.h>
#include <a_tools/convert_to_int.h>
#include <drivers/disk/fat.h>

// Ports (same as keyboard)

#define MOUSE_DATA_PORT  0x60
#define MOUSE_STATUS_PORT  0x64

// the packets 1 to 3

static uint8_t mouse_cycle = 0;
static int8_t mouse_byte[3];

// Mouse pos

static int32_t mouse_x = 0;
static int32_t mouse_y = 0;

static int32_t prev_mouse_x = 0;
static int32_t prev_mouse_y = 0;

// mouse draw

char row1[10];
char row2[10];
char row3[10];
char row4[10];
char row5[10];
char row6[10];
char row7[10];
char row8[10];
char row9[10];
char row10[10];


// mouse l m r

static bool left_pressed = false;
static bool right_pressed = false;
static bool middle_pressed = false;

void fill_char_arr(const char* source, char* dest, int start, int length) {
    if (length > 0) {
        memcpy(dest, source + start, length);
        dest[length] = '\0'; 
    }
}

// the meat and potatoes, the handler

// basically the people who made the PS2 mouse just made it an overly complecated keyboard
// on irq12 it sends a bunch of bullshit to the cpu (packets)
// there are 3 packets in total, one for bs, one for x, one for y
// here we are browsing through each packet taking what we need

InterruptRegisters* mouse_handler(InterruptRegisters* regs) {
    switch (mouse_cycle) {
        case 0:
            mouse_byte[0] = inb(MOUSE_DATA_PORT);
            mouse_cycle++;
            break;
        case 1:
            mouse_byte[1] = inb(MOUSE_DATA_PORT);
            mouse_cycle++;
            break;
        case 2:
            mouse_byte[2] = inb(MOUSE_DATA_PORT);

            // POS

            int8_t x = mouse_byte[1];
            int8_t y = mouse_byte[2];

            // negative?

            if (mouse_byte[0] & 0x10) x |= 0xFFFFFF00;  
            if (mouse_byte[0] & 0x20) y |= 0xFFFFFF00;  

            mouse_x += x;
            mouse_y -= y;  // y is quirky

            // limit the thing

            if (mouse_x >= SCREEN_WIDTH){
                mouse_x = SCREEN_WIDTH-1;
            }
            else if (mouse_x <= 0){
                mouse_x = 1;
            }

            if (mouse_y >= SCREEN_HEIGHT){
                mouse_y = SCREEN_HEIGHT-1;
            }
            else if (mouse_y <= 0){
                mouse_y = 1;
            }

            // cursor
            cursor_draw();
            // mental overflow

            mouse_cycle = 0;
            
            // CLICKS
            
            left_pressed = mouse_byte[0] & 0x01;
            right_pressed = mouse_byte[0] & 0x02;
            middle_pressed = mouse_byte[0] & 0x03;

            break;      
    }
    return regs;
}
void mouse_init() {
    split_to_bitmap();
    // enable and wait for mouse
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0xA8);
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0x20);
    uint8_t status = inb(0x60) | 2;
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0x60);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, status);

    mouse_write(0xF6);
    mouse_read(); 

    // enable mouse
    mouse_write(0xF4);
    mouse_read(); 

    // the irq handler
    IRQ_installHandler(12, mouse_handler);
}

// the wait
void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((inb(MOUSE_STATUS_PORT) & 1) == 1) {
                return;
            }
        }
    } else {
        while (timeout--) {
            if ((inb(MOUSE_STATUS_PORT) & 2) == 0) {
                return;
            }
        }
    }
}

char* split_to_bitmap(){
    const char* cursor = readfile_into_buffer("/ROS_ICOS   /MOS_ICO RSI");
    // if no file exists like that
    if(cursor == NULL){
        str_copy("MQMQMQMQMQ", row1);
        str_copy("QMQMQMQMQM", row2);
        str_copy("MQMQMQMQMQ", row3);
        str_copy("QMQMQMQMQM", row4);
        str_copy("MQMQMQMQMQ", row5);
        str_copy("QMQMQMQMQM", row6);
        str_copy("MQMQMQMQMQ", row7);
        str_copy("QMQMQMQMQM", row8);
        str_copy("MQMQMQMQMQ", row9);
        str_copy("QMQMQMQMQM", row10);
        return;
    }
    // ill add more if i need lolololol 
    fill_char_arr(cursor, row1, 0, CURSOR_WIDTH);
    fill_char_arr(cursor, row2, 11, CURSOR_WIDTH);
    fill_char_arr(cursor, row3, 22, CURSOR_WIDTH);
    fill_char_arr(cursor, row4, 33, CURSOR_WIDTH);
    fill_char_arr(cursor, row5, 44, CURSOR_WIDTH);
    fill_char_arr(cursor, row6, 55, CURSOR_WIDTH);
    fill_char_arr(cursor, row7, 66, CURSOR_WIDTH);
    fill_char_arr(cursor, row8, 77, CURSOR_WIDTH);
    fill_char_arr(cursor, row9, 88, CURSOR_WIDTH);
    fill_char_arr(cursor, row10, 99, CURSOR_WIDTH);
}

void draw_bitmap(char* bitmap, int x, int y, uint32_t colour) {
    int length = strlen(bitmap); 
    for (int i = 0; i < length; i++) {
        if (bitmap[i] == 'W') {
            set_pixel(x + i, y, colour); 
        }
        if (bitmap[i] == 'Q') {
            set_pixel(x + i, y, black);
        }
        if (bitmap[i] == 'R') {
            set_pixel(x + i, y, red);
        }
        if (bitmap[i] == 'G') {
            set_pixel(x + i, y, green);
        }
        if (bitmap[i] == 'B') {
            set_pixel(x + i, y, blue);
        }
        if (bitmap[i] == 'Y') {
            set_pixel(x + i, y, yellow);
        }
        if (bitmap[i] == 'M') {
            set_pixel(x + i, y, magenta);
        }
    }
}
uint32_t read_bitmap(char* bitmap, int x, int y) {
    int length = strlen(bitmap); 
    for (int i = 0; i < length; i++) {
        if (bitmap[i] == 'W') {
            read_pixel(x + i, y); 
        }
        if (bitmap[i] == 'Q') {
            read_pixel(x + i, y);
        }
        if (bitmap[i] == 'R') {
            read_pixel(x + i, y);
        }
        if (bitmap[i] == 'G') {
            read_pixel(x + i, y);
        }
        if (bitmap[i] == 'B') {
            read_pixel(x + i, y);
        }
        if (bitmap[i] == 'Y') {
            read_pixel(x + i, y);
        }
        if (bitmap[i] == 'M') {
            read_pixel(x + i, y);
        }
    }
}

uint32_t original_pixels[CURSOR_WIDTH * CURSOR_HEIGHT]; 

void store_original_pixels(int x, int y) {
    int index = 0;
    for (int row = 0; row < CURSOR_HEIGHT; row++) {
        for (int col = 0; col < CURSOR_WIDTH; col++) {
            original_pixels[index++] = read_pixel(x + col, y + row);
        }
    }
}

void restore_original_pixels(int x, int y) {
    int index = 0;
    for (int row = 0; row < CURSOR_HEIGHT; row++) {
        for (int col = 0; col < CURSOR_WIDTH; col++) {
            set_pixel(x + col, y + row, original_pixels[index++]);
        }
    }
}

void cursor_draw() {
    // erase cursor
    restore_original_pixels(prev_mouse_x, prev_mouse_y);

    // set the colour to our mouse pos
    store_original_pixels(mouse_x, mouse_y);

    // draw cursor

    // clicks
    uint32_t cursor_colour;
    if (left_pressed) {
        cursor_colour = yellow;
    } else
    if (right_pressed) {
        cursor_colour = cyan;
    } else
    if (middle_pressed) {
        cursor_colour = magenta;
    } else{
        cursor_colour = white;
    }
    draw_bitmap(row1, mouse_x, mouse_y, cursor_colour);
    draw_bitmap(row2, mouse_x, mouse_y+1, cursor_colour);
    draw_bitmap(row3, mouse_x, mouse_y+2, cursor_colour);
    draw_bitmap(row4, mouse_x, mouse_y+3, cursor_colour);
    draw_bitmap(row5, mouse_x, mouse_y+4, cursor_colour);
    draw_bitmap(row6, mouse_x, mouse_y+5, cursor_colour);
    draw_bitmap(row7, mouse_x, mouse_y+6, cursor_colour);
    draw_bitmap(row8, mouse_x, mouse_y+7, cursor_colour);
    draw_bitmap(row9, mouse_x, mouse_y+8, cursor_colour);
    draw_bitmap(row10, mouse_x, mouse_y+9, cursor_colour);


    // trickery
    prev_mouse_x = mouse_x;
    prev_mouse_y = mouse_y;
}


void mouse_write(uint8_t data) {
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0xD4);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, data);
}

uint8_t mouse_read() {
    mouse_wait(0);
    return inb(MOUSE_DATA_PORT);
}

