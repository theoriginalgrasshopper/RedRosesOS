#ifndef CMD_CURSOR_H
#define CMD_CURSOR_H
#pragma once
extern int cursor_pos_y;
extern int cursor_pos_x;

void init_cmd_cursor();
void draw_cmd_cursor_animation();
void draw_cmd_cursor_animation_white();
void stop_cmd_cursor();
#endif