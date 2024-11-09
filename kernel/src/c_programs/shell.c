#include "shell.h"
#include <sprint.h>
#include <include/constants.h>
#include "clear_and_print.h"
#include <screen.h>
#include "reboot.h"
#include "cmd_arg.h"
#include <a_tools/timer.h>
#include <a_tools/convert_to_int.h>
#include "rosefetch.h"
#include <interrupts/idt.h>
#include <c_programs/draw_image.h>
#include <clear_screen.h>
#include <a_tools/random.h>
#include "start_menu.h"
#include "cmd_cursor.h"
#include <memory_management/pmm.h>
#include <gui/gui_lib.h>
#include <gui/mode.h>
#include <drivers/disk/mbr.h>
#include <drivers/disk/fat.h>
#include <interrupts/io.h>
#include <drivers/keyboard.h>
#include <software/petals.h>
#include <a_tools/clock.h>
#include <multitasking/multitasking.h>
#include <interrupts/pic.h>

// FLAGS
extern int mode;
extern bool exec_flag;

// KEYS
extern bool shift_pressed;
extern bool backspace_pressed;
extern bool enter_pressed;
extern bool caps_pressed;
extern bool up_pressed;


// CHARACTER
extern char current_character;
char input_buffer[256] = {0};
char input_buffer_history[256] = {0};

// THE ACTUAL SHELL
// ABSOLUTELY NO CLUE WHERE THE \n COMES FROM WHEN ENTER PRESSED, DO NOT CARE.

void bouquet_shell_task() {
    char ch = get_current_character();
    uint8_t scncd = get_current_scancode();

    if (ch != '\0' && scncd != 65) { // not
                                     // NULL
                                     // or
                                     // THE OS GAINING CONSCIOUSNESS
        // ENTER
        if (scncd == ENTER){
            print_char_at('_', cursor_pos_x, cursor_pos_y, black);
            str_copy(input_buffer, input_buffer_history);
            command_process(input_buffer);
            input_buffer[0] = '\0';

        // BACKSPACE
        } else if (scncd == BACKSPACE){ 
            if (strlen(input_buffer) > 0) {
                sprint_remove_char(input_buffer);
                init_cmd_cursor();
                print_char_at('_', cursor_pos_x + 1, cursor_pos_y, black);
            }
        
        // UP
        } else if (scncd == UP){
            str_copy(input_buffer_history, input_buffer);
            sprint(input_buffer_history, white);
        
        // DEFAULT
        } else {
            append(input_buffer, ch);
            sprint_char(ch, white);
            init_cmd_cursor();
        }

        scncd = '\0';
        current_character = '\0';
    }
}

// COMMANDS

void command_process(char* the_string){
    // COMMANDS WITHOUT ARGUMENTS
    if (exec_flag != 1){
        sprint("\n", white);
    }
    
    if ( string_same(the_string, "rosefetch") ){
        rosefetch();
    }
    if ( string_same(the_string, "explode") ){
        while (1){
            playSound(1760);
            fill_screen(magenta);
            fill_screen(red);
            fill_screen(cyan);
            fill_screen(blue);
            fill_screen(green);
            fill_screen(yellow);
        }
    }
    if ( string_same(the_string, "start-menu") ){
        main_menu();
    }
    if ( string_same(the_string, "clear") ){
        clear_and_print();
    }
    if ( string_same(the_string, "about") ){
        sprint("\nRedRosesOS is a 64-bit O.S. made by theoriginalgrasshopper and with the help of AbdooOwd\n \n", nice_orange);
    }
    if ( string_same(the_string, "qemu-shutdown") ){
        outw(0x604, 0x2000);
    }
    if ( string_same(the_string, "reboot") ){
        reboot();
    }
    if ( string_same(the_string, "sound-stop") ){
        stopSound();
    }
    if ( string_same(the_string, "scroll") ){
        scroll_pixel_line();
    }
    if ( string_same(the_string, "mbr-read") ){
        read_mbr();
    }
    if ( string_same(the_string, "ls-root") ){
        Read_BPB_quiet(0);
        Read_root_dir();
    }
    if ( string_same(the_string, "cat") ){
        draw_rle_image(rle_image, rle_image_size, cursor_pos_x, cursor_pos_y);
    }
    if ( string_same(the_string, "date") ){
        read_rtc();
    }
    if ( string_same(the_string, "gui") ){
        stop_cmd_cursor();
        gui_init();
    }
    if ( string_same(the_string, "help") ){
        sprint("AVIABLE COMMANDS: \n\n", blue);
        sprint("GENERAL \n\n", green); 
        sprint("clear            | clears the screen \n", white);
        sprint("about            | shows information about RedRosesOS \n", white);
        sprint("help             | shows this message \n", white);
        sprint("qemu-shutdown    | shuts down QEMU ver. 2.0 and newer \n", white);
        sprint("reboot           | reboots the system \n\n", white);
        sprint("VISUAL \n\n", green);
        sprint("start-menu       | shows start menu \n", white);
        sprint("pixel            | accepts three arguments, draws a specified pixel at specified coordinates \n\n", white);
        sprint("SOUND \n\n", green);
        sprint("sound-play       | accepts one argument, plays a tone in specified frequency \n", white);
        sprint("sound-play-timed | accepts two arguments, plays a tone in specified frequency for a specified amount of ticks. 87 ticks is equal to a second \n", white);
        sprint("sound-stop       | stops the sound of the PC speaker \n\n", white);
        sprint("DISK & FS\n\n", green);
        sprint("diskw            | accepts three arguments, writes to the specified sector. The data written is provided by the user, as well as the amount of bytes to write \n", white);
        sprint("diskr            | accepts two arguments, reads the specified amount of bytes from the specified sector\n", white);    
        sprint("ls               | accepts one argument, lists the contents of the specified directory. '/' is considered both a separator and the root directory\n", white);
        sprint("ls-root          | alias for 'ls /' \n", white);
        sprint("read             | accepts two arguments, reads a file from the specified path with the specified extension \n", white);
        sprint("cat              | alias for 'read' \n", white);
        sprint("mkdir            | accepts one argument, creates a directory in the specified path \n", white);
        sprint("touch            | accepts two arguments, creates a file in the specified path with the specified extension  \n", white);
        sprint("write            | accepts four arguments: filepath, extension, data and amount of bytes to write. Writes to a file\n", white);
        sprint("mbr-read         | provides information about the MBR \n\n", white);
        sprint("MISC \n\n", green);
        sprint("explode          | originally made and implemented by pac-ac in osakaOS, flashes colours rapidly and produces a loud screech \n", white);
        sprint("cowsay           | accepts one argument, originally made by Tony Monroe, displays a cow saying a specified message \n", white);
        sprint("say              | accepts two arguments, prints a specified message the specified amount of times \n", white);
        sprint("math             | accepts 3 arguments, allows for simple operations with integers\n", white);
        sprint("rosefetch        | displays general information about the system \n", white);
        sprint("random           | accepts two arguments, prints out a pseudo-random value in specified range\n", white);
        sprint("cat              | cat\n\n", white);
    }

    // COMANDS WITH ARGUMENTS

    commands_with_argument_init(the_string);
    cowsay();
    say_times_what();
    sound();
    sound_timed();
    pixel();
    random();
    math();
    diskr();
    diskw();
    ls();
    read();
    touch();
    write();
    mkdir();
    execute();
    execute_bin();
    invoke_syscall();

    // SHELL CHARACTER
    if(mode == 1 && exec_flag != 1){
        sprint_char(TTY_CHAR, red);
    }
    reset_arguments();
}

// REMOVE ONE CHARACTER

void sprint_remove_char(char* string){
    int lenght_of_string = strlen(string);  
    
    if (lenght_of_string > 0) {
        string[lenght_of_string - 1] = '\0';  
    }

    int length_of_char = 1;
    for (int i = 0; i < length_of_char; i++) {
        print_char_at('!', cursor_pos_x - 1, cursor_pos_y, black); 
        cursor_pos_x = cursor_pos_x - 1;
    }

}
