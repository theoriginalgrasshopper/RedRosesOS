#include <stdint.h>
#include <stddef.h>
#include <sprint.h>
#include <include/constants.h>
#include <c_programs/clear_and_print.h>


// A BETTER FUCTION BY ABDOOOWD, NOT USABLE THOUGH 

// void sprint(char* str, int x, int y, uint32_t color) {
//   int i = 0;
//   while (str[i]) {
//     if (str[i] == '\n') y++;
//     print_char_at(str[i], x + i, y, color);
//     i++;
//   }
// }


int cursor_pos_y = 0;
int cursor_pos_x = 0;

// PRINT STRING

void sprint(char string_to_print[], uint32_t color) {
    int length = 0;
    while (string_to_print[length] != '\0') {
        length++;
    }

    for (int i = 0; i < length; i++) {
        print_char_at(string_to_print[i], cursor_pos_x, cursor_pos_y, color);
    if (string_to_print[i] == '\n') {
        cursor_pos_x = 0; 
        cursor_pos_y++;   
    } 
    else {
      cursor_pos_x++; 
    }
        // print the cursor for debug
        
        //print_char_at('>', cursor_pos_x, cursor_pos_y, 0xffffff); 
    }

}

// PRINT ONE CHARACTER

void sprint_char(char char_to_print, uint32_t color) {
    int length_of_char = 1;
    for (int i = 0; i < length_of_char; i++) {
        print_char_at(char_to_print, cursor_pos_x, cursor_pos_y, color); 
        cursor_pos_x++;  
        // print the cursor for debug
        
        //print_char_at('>', cursor_pos_x, cursor_pos_y, 0xffffff);
    }
}


