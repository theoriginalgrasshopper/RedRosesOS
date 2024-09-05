#ifndef COWSAY_H
#define COWSAY_H
#pragma once
extern char input_buffer[];

void commands_with_argument_init( char string_to_say[] );
void cowsay();
void say_times_what();
void sound();
void sound_timed();
void pixel();
void random();
void math();
void diskr();
void diskw();
void ls();
void touch();
void write();
void mkdir();
void read();
#endif