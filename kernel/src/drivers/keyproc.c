#include <drivers/keyproc.h>
#include <drivers/keybuff.h>
#include <sprint.h>
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

void keyboard_flush(void) {
	sc_flush();
}

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
    return char_is_lowercase(c) ? (c - 'a' + 'A') : c;
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

// define keys
#define BACKSPACE 		0x0E
#define ENTER 			0x1C
#define LSHIFT 			0x2A
#define LCTRL 			0x1D
#define CAPSLOCK        0x0F // <------- TAB ACTUALLY
#define DOWN            0x50

char ascii_US[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
        'u', 'i', 'o', 'p', '[', ']', '?', '{', 'a', 's', 'd', 'f', 'g',
        'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' ', '}'};        // { - CTRL } - CAPS


char input_buffer[256];
char input_buffer_graphic[256];
char input_buffer_history[256];
bool shift_pressed;
bool backspace_pressed;
bool enter_pressed;
bool caps_pressed;

// deprecated behaviour. Should be ceased as soon as possible
#define SC_MAX 			80

// Process keys until buffer is emptied
void process_keys(void) {
    // Process scan codes until buffer is empty
	while(!sc_isempty())
		process_key();
}

// Process a single key
void process_key(void) {
	uint8_t scancode;

    // If scancode buffer is empty there s nothing to do
	if (sc_isempty())
		return;

    // Retrieve the next scancode from the scancode buffer
	scancode = sc_getelement();

    // if we press unknown
    if (scancode > SC_MAX && !(scancode == LSHIFT || scancode == (LSHIFT | 0x80))) return;

    switch (scancode) {     // if pressed
        case LSHIFT:
            shift_pressed = true;
            shift_button_pressed();
        break;

    }

	if (scancode | 0x80) {	// if released
		switch (scancode) {

			case BACKSPACE:
                if (strlen(input_buffer) > 0) {
                    backspace_pressed = true;
                    backspace_button_pressed();
                    init_cmd_cursor();
                    print_char_at('_', cursor_pos_x + 1, cursor_pos_y, black);
                }
			break;


            case ENTER:
                if (mode == 1){
                    enter_pressed = true;
                    print_char_at('_', cursor_pos_x, cursor_pos_y, black);
                    command_init();
                    str_copy(input_buffer, input_buffer_history);
                    input_buffer[0] = '\0';
                }
                if (mode == 2){
                    stop_cmd_cursor();
                    user_input(input_buffer_graphic);
                    input_buffer_graphic[0] = '\0';
                }
            break;
            case DOWN:
                scroll_pixel_line();
            break;
            

            case LSHIFT | 0x80:     // released shift
                shift_pressed = false;
                shift_button_pressed();
            break;




            // CAPS LOCK WORKS BUT IN A VERY BAD AND BAD AND HORRIBLE WAY

			default:
                char c = getch(scancode); // <-------------- horrible
                
                if (scancode == 72){ // ^ key
                    sprint(input_buffer_history, white);
                    str_copy(input_buffer_history, input_buffer);
                }
                
                if (c == 125){ 
                    if (caps_pressed == true){
                        caps_pressed = false;
                    }
                    else{
                        caps_pressed = true;
                    }
                    c = 1;                    
                }
                if (start_menu_active == true){
                    clear_and_print();
                    start_menu_active = false;
                    sprint_char(TTY_CHAR, red);
                }
                if (caps_pressed == true) c = caps_char(c);
                if (shift_pressed) c = caps_char(c);
                if (shift_pressed != true && caps_pressed != true) c = lower_char(c); 
                // hardcoding !@#$%^&*()_+ because yes
                // https://www.cs.cmu.edu/~pattis/15-1XX/common/handouts/ascii.html <------ useful website
                if (shift_pressed && c == 49) c = 33; // !, 1
                if (shift_pressed && c == 50) c = 64; // @, 2
                if (shift_pressed && c == 51) c = 35; // #, 3
                if (shift_pressed && c == 52) c = 36; // $, 4
                if (shift_pressed && c == 53) c = 37; // %, 5
                if (shift_pressed && c == 54) c = 94; // ^, 6
                if (shift_pressed && c == 55) c = 38; // &, 7
                if (shift_pressed && c == 56) c = 42; // *, 8
                if (shift_pressed && c == 57) c = 40; // (, 9
                if (shift_pressed && c == 48) c = 41; // ), 0
                if (shift_pressed && c == 45) c = 95; // _, -
                if (shift_pressed && c == 61) c = 43; // +, =
                // and the special ones
                if (shift_pressed && c == 96) c = 126; // ~, `
                if (shift_pressed && c == 92) c = 124; // |, / but mirrored
                if (shift_pressed && c == 91) c = 123; // [, { 
                if (shift_pressed && c == 93) c = 125; // ], }
                if (shift_pressed && c == 44) c = 60; // <, ,
                if (shift_pressed && c == 46) c = 62; // >, .
                if (shift_pressed && c == 39) c = 34; // ", '
                //if (c == 'h') createfile("    HELP", "TXT"); 
                if (scancode != LSHIFT && scancode != 72) { // <-------- ^ < > keys
				    if(mode == 1){
                        append(input_buffer, c);
                        sprint_char(c, white);
                        init_cmd_cursor();
                    }
                    if(mode == 2){
                        append(input_buffer_graphic, c);
                    }
                }

        }
	}
	

	return;
}
// i do not know why it repeats the text at a specific speed but i am not complaining

char getch(uint8_t scancode) {
	return ascii_US[scancode];
}


void user_input(char* input) {
    lower(input);
};
