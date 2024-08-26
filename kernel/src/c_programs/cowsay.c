#include <stdint.h>
#include <stddef.h>
#include "shell.h"
#include <sprint.h>
#include <screen.h>
#include <include/constants.h>
#include <stdbool.h>
#include <drivers/keyboard.h>
#include <a_tools/convert_to_int.h>
#include <a_tools/timer.h>
#include <drivers/disk/ata.h>
#include <a_tools/random.h>

// FILE CONTAINS BOTH THE COWSAY BY Tony Monroe (https://github.com/tnalpgge/rank-amateur-cowsay)
// PORTED BY theoriginalgrasshopper
// AND THE ARGUMENT COMMAND INITIALIZATION

// COMMANDS WITH ARGUMENTS INIT


#define MAX_ARGUMENTS 10
#define MAX_ARGUMENT_LENGTH 50

char characters_before_argsym[MAX_ARGUMENT_LENGTH];
char characters_after_argsym[MAX_ARGUMENT_LENGTH];
char characters_after_argsym_second[MAX_ARGUMENT_LENGTH];
char characters_after_argsym_third[MAX_ARGUMENT_LENGTH];
char arguments[MAX_ARGUMENTS][MAX_ARGUMENT_LENGTH];
int argument_count = 0;
bool in_quotes = false;


void commands_with_argument_init(char string_to_say[]) {
    int i = 0;
    int char_index_before = 0;
    bool found_first_word = false;

    for (int j = 0; j < MAX_ARGUMENTS; j++) {
        for (int k = 0; k < MAX_ARGUMENT_LENGTH; k++) {
            arguments[j][k] = '\0';
        }
    }
    argument_count = 0;

    while (string_to_say[i] != '\0' && (string_to_say[i] != ' ' || in_quotes)) {
        if (string_to_say[i] == '\"') {
            in_quotes = !in_quotes;
        } else {
            characters_before_argsym[char_index_before++] = string_to_say[i];
        }
        i++;
    }
    characters_before_argsym[char_index_before] = '\0';

    if (string_to_say[i] == ' ') {
        i++;
    }
    // HANDE DOUBLE QUOTES
    while (string_to_say[i] != '\0') {
        int char_index = 0;
        in_quotes = false;

        while (string_to_say[i] == ' ') {
            i++;
        }

        while (string_to_say[i] != '\0' && (in_quotes || string_to_say[i] != ' ')) {
            if (string_to_say[i] == '\"') {
                in_quotes = !in_quotes;
            } else {
                arguments[argument_count][char_index++] = string_to_say[i];
            }
            i++;
        }
        arguments[argument_count][char_index] = '\0';
        argument_count++;

        if (string_to_say[i] == ' ') {
            i++;
        }
    }

    // COMPATIBILITY FOR THE FUNCTIONS I MADE
    // ONE
    if (argument_count > 0) {
        int j = 0;
        while (arguments[0][j] != '\0') {
            characters_after_argsym[j] = arguments[0][j];
            j++;
        }
        characters_after_argsym[j] = '\0';
    }
    // TWO
    if (argument_count > 1) {
        int j = 0;
        while (arguments[1][j] != '\0') {
            characters_after_argsym_second[j] = arguments[1][j];
            j++;
        }
        characters_after_argsym_second[j] = '\0';
    }
    // THREE
    if (argument_count > 2) {
        int j = 0;
        while (arguments[2][j] != '\0') {
            characters_after_argsym_third[j] = arguments[2][j];
            j++;
        }
        characters_after_argsym_third[j] = '\0';
    }
}   // CAN ADD MORE BUT DONT NEED FOR NOW


// ACTUAL COWSAY


void cowsay(){
    if ( string_same(characters_before_argsym, "cowsay") ){
        sprint("\n    < ", white);
        sprint(characters_after_argsym, white);
        sprint(" > \n", white);
        sprint("        \\   ^__^\n", white);
        sprint("         \\  (oo)\\_______\n", white);
        sprint("            (__)\\       )\\/\\ \n", white);
        sprint("               ||----W | \n", white);
        sprint("               ||     || \n", white);
    }
}
void say_times_what(){
    if ( string_same(characters_before_argsym, "say")){
        int number_of_sayings = string_to_int(characters_after_argsym_second);
        for(int h = 1; h < number_of_sayings + 1; h++){
            sprint(characters_after_argsym, white);
            sprint("\n", white);
        }
    }
}

void sound(){
    if ( string_same(characters_before_argsym, "sound-play") ){
        uint32_t the_frequency_of_sound = string_to_int(characters_after_argsym);
        playSound(the_frequency_of_sound);
    }
}
void sound_timed(){
    if ( string_same(characters_before_argsym, "sound-play-timed") ){
        uint32_t the_frequency_of_sound = string_to_int(characters_after_argsym);
        uint32_t the_time_of_sound = string_to_int(characters_after_argsym_second);
        playSoundTimed(the_frequency_of_sound, the_time_of_sound);
    }
}
void pixel(){
    if ( string_same(characters_before_argsym, "pixel") ){
        int x_coords = string_to_int(characters_after_argsym);
        int y_coords = string_to_int(characters_after_argsym_second);
        int colour = string_to_int(characters_after_argsym_third);
        set_pixel(x_coords, y_coords, colour);
    }
}
void random(){
    if (string_same(characters_before_argsym, "random")){
        int random_num_one = string_to_int(characters_after_argsym);
        int random_num_two = string_to_int(characters_after_argsym_second);         
        int random_num = give_random(random_num_one, random_num_two);
        char random_num_c[999999999];
        int_to_str(random_num, random_num_c);
        sprint(random_num_c, cyan);
        sprint("\n", white);
    }
}
int result;
void math(){
    if (string_same(characters_before_argsym, "math")){
        char result_c[256];
        if (string_same(characters_after_argsym_second, "+")){
            result = string_to_int(characters_after_argsym) + string_to_int(characters_after_argsym_third);
        }
        if (string_same(characters_after_argsym_second, "-")){
            result = string_to_int(characters_after_argsym) - string_to_int(characters_after_argsym_third);
        }
        if (string_same(characters_after_argsym_second, "*")){
            result = string_to_int(characters_after_argsym) * string_to_int(characters_after_argsym_third);
        }
        if (string_same(characters_after_argsym_second, "/")){
            if (string_same(characters_after_argsym_third, "0")){
                sprint("Undefined", red);
            }
            else{
            result = string_to_int(characters_after_argsym) / string_to_int(characters_after_argsym_third);
            }
        }
        if (string_same(characters_after_argsym_second, "^")){
            result = string_to_int(characters_after_argsym) ^ string_to_int(characters_after_argsym_third);
        }
        int_to_str(result, result_c);
        if (string_same(characters_after_argsym_third, "0")){    
        }
        else{
        sprint(result_c, cyan);
        }
        sprint("\n", white);
    }
}
void sprint_raw(uint8_t* data, int length, int color) {
    for (int i = 0; i < length; i++) {
        char text[2] = { data[i], '\0' }; // Ensure only one character is printed
        sprint(text, color);
    }
    sprint("\n", white); // Add a newline at the end
}
void diskr(){
    if (string_same(characters_before_argsym, "diskr")){
        if (string_same(characters_after_argsym, "")){
            sprint("WRONG SYNTAX.", red);
        } else{
            uint8_t sector = string_to_int(characters_after_argsym);
            int count = string_to_int(characters_after_argsym_second);
            uint8_t* data = ATA_Read28_PM(sector, count);
            sprint_raw(data, count, blue);
        }
    }
}
void diskw(){
    if (string_same(characters_before_argsym, "diskw")){
        if (string_same(characters_after_argsym, "")){
            sprint("WRONG SYNTAX.", red);
        } else{
            ATA_Flush_PM();
            uint32_t sector = string_to_int(characters_after_argsym);
            int count = string_to_int(characters_after_argsym_second);
            ATA_Write28_PM(sector, (uint8_t*)characters_after_argsym_third, count);
        }
    }
}