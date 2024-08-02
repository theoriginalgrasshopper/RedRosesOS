#include "cmd_cursor.h"
#include <sprint.h>
#include <screen.h>
#include <include/constants.h>
#include <stdbool.h>

bool cmd_cursor_delay;

void init_cmd_cursor(){
    print_char_at('_', cursor_pos_x, cursor_pos_y, white);
    cmd_cursor_delay = true;
}
void draw_cmd_cursor_animation(){
    print_char_at('_', cursor_pos_x, cursor_pos_y, black);
}
void draw_cmd_cursor_animation_white(){
    print_char_at('_', cursor_pos_x, cursor_pos_y, white);
}