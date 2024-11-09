#include <interrupts/irq.h>
#include "keyboard.h"
#include <sprint.h>
#include <interrupts/io.h>
#include <interrupts/pic.h>
#include <interrupts/cpu.h>
#include <screen.h>
#include <stdbool.h>
#include <stddef.h>
#include <include/constants.h>
#include <c_programs/shell.h>
#include <a_tools/convert_to_int.h>
#include <c_programs/cmd_cursor.h>
#include <c_programs/clear_and_print.h>
#include <gui/mode.h>
#include <multitasking/multitasking.h>
#include <drivers/disk/fat.h>


// SHELL JUNK. MOVE TO SHELL.C IN THE FUTURE

char lower_char(char character) {  
    if (character >= 'A' && character <= 'Z') { 
        character += 32; 
    }
    return character;  
} 

void str_copy(char *source, char *destination) {
    while (*source != '\0') {
        *destination = *source;
        source++;
        destination++;
    }
    *destination = '\0';
}
bool char_is_lowercase(char c) {
    return c >= 'a' && c <= 'z';
}

char caps_char(char c) {
    if (char_is_lowercase(c)){
        return c - 'a' + 'A';
    } else{
        switch (c){
            case '1': return '!';
            case '2': return '@';
            case '3': return '#';
            case '4': return '$';
            case '5': return '%';
            case '6': return '^';
            case '7': return '&';
            case '8': return '*';
            case '9': return '(';
            case '0': return ')';
            case '-': return '_';
            case '=': return '+';
            case '\'': return '\"';
            case '\\': return '|';
            default: return c;
        }
    }
}

int string_compare(char* s1, char* s2) {
    size_t i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}
bool string_same(char* str1, char* str2) {
    return string_compare(str1, str2) == 0;
}
bool string_contains(char* str1, char* str2) {
    if (str2[0] == '\0') {
        return true;
    }

    for (size_t i = 0; str1[i] != '\0'; i++) {
        if (string_compare(&str1[i], str2) == 0) {
            return true;
        }
    }
    return false;
}

size_t strlen(char* str) {
    size_t i = 0;
    while (str[i] != '\0') ++i;
    return i;
}

void append(char* str, char c) {
    size_t len = strlen(str);
    str[len] = c;
    str[len + 1] = '\0';
}
void lower(char* str) {
    for (size_t i = 0; i < strlen(str); i++)
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 0x20;
}

// init keyboard
void keyboard_init() {
    IRQ_installHandler(1, keyboard_handler);
}

// the data port for the PS/2 keyboard
#define KBD_DATA_PORT 	0x60

// define keys
#define BACKSPACE 		0x0E
#define ENTER 			0x1C
#define LSHIFT 			0x2A
#define LCTRL 			0x1D
#define CAPSLOCK        0x0F
#define DOWN            0x50

char ascii_US[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
        'u', 'i', 'o', 'p', '[', ']', '?', '{', 'a', 's', 'd', 'f', 'g',
        'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' ', '}'};        // { - CTRL } - CAPS

bool shift_pressed;
bool backspace_pressed;
bool enter_pressed;
bool caps_pressed;
bool up_pressed;

extern int mode;
char current_character;
uint8_t current_scancode;

// deprecated behaviour. Should be ceased as soon as possible
#define SC_MAX 			80

// THE DRIVER PART WITH HANDLER
InterruptRegisters* keyboard_handler(InterruptRegisters* regs) {
	uint8_t scancode = inb(KBD_DATA_PORT);
    // if we press unknown
    if (scancode > SC_MAX && !(scancode == LSHIFT || scancode == (LSHIFT | 0x80))) return regs;
    
    if (scancode & 0x80) { // released
        uint8_t released_key = scancode & ~0x80;
        
        current_scancode = '\0';
        current_character = '\0';

        if (released_key == LSHIFT) {
            shift_pressed = false;
        } else {
            current_character = '\0';
        }

    } else { // pressed
        if (scancode == LSHIFT) {
            shift_pressed = true;
        } else if (getch(scancode) == 125){
            scancode = BACKSPACE; // cannot believe this worked acutally
            caps_pressed = !caps_pressed;
        } else {
            current_character = getch(scancode);
            current_scancode = scancode;
        }
    }
	return regs;
}
uint8_t get_current_character() {
    if (shift_pressed) {
        return caps_char(current_character);
    }
    if (caps_pressed) {
        return caps_char(current_character);
    } else {
        return current_character;
    }
    return '\0';
}
uint8_t get_current_scancode() {
    return current_scancode;
}
// i do not know why it repeats the text at a specific speed but i am not complaining
char getch(uint8_t scancode) {
	return ascii_US[scancode];
}