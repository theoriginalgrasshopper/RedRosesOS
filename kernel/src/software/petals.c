#include "petals.h"
#include <drivers/keyboard.h>
#include <c_programs/shell.h>
#include <drivers/disk/fat.h>
#include <memory_management/pmm.h>
#include <sprint.h>
#include <include/constants.h>

bool exec_flag;
extern char input_buffer[256];

void run_script(char* filepath){
    exec_flag = 1;
    
    char* file = readfile_into_buffer(filepath);
    if (file == NULL) {
        exec_flag = 0;
        return;
    }

    char current_command[256];
    int command_index = 0;

    for (int i = 0; i < strlen(file); i++) {
        char current_char = file[i];

        if (current_char == 0x0A) {
            current_command[command_index] = '\0';  
            command_index = 0;  
            str_copy(current_command, input_buffer);
            command_init();
        } 
        
        else if (current_char == ';') {
            current_command[command_index] = '\0';   
            command_index = 0;
            str_copy(current_command, input_buffer);
            command_init();
        } 
        else {

            if (command_index < sizeof(current_command) - 1) {
                current_command[command_index++] = current_char;
            }

            else {
                sprint("command was too long.\n ", red);
                command_index = 0; 
            }
        }
    }

    // here do the thing, current_command contains the command needed
    if (command_index > 0) {
        current_command[command_index] = '\0';
        
        str_copy(current_command, input_buffer);
        command_init();
    }


    pmm_free_auto();
    exec_flag = 0;
}
