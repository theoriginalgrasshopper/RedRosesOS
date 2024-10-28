#include "shell.h"
#include <sprint.h>
#include <include/constants.h>
#include "clear_and_print.h"
#include <screen.h>
#include "reboot.h"
#include "cowsay.h"
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
#include <gui/gui_draw.h>
#include <gui/mode.h>
#include <drivers/disk/mbr.h>
#include <drivers/disk/fat.h>
#include <interrupts/io.h>
#include <drivers/keyboard.h>
#include <software/petals.h>
#include <a_tools/clock.h>
#include <multitasking/multitasking.h>
#include <gui/mode.h>

// FLAGS
extern bool exec_flag;

void command_init(){

    // COMMANDS WITHOUT ARGUMENTS
    if (exec_flag != 1){
        sprint("\n", white);
    }
    if ( string_same(input_buffer, "rosefetch") ){
        rosefetch();
    }
    if ( string_same(input_buffer, "explode") ){
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
    if ( string_same(input_buffer, "start-menu") ){
        main_menu();
    }
    if ( string_same(input_buffer, "clear") ){
        clear_and_print();
    }
    if ( string_same(input_buffer, "about") ){
        sprint("\nRedRosesOS is a 64-bit O.S. made by theoriginalgrasshopper and with the help of AbdooOwd\n \n", nice_orange);
    }
    if ( string_same(input_buffer, "qemu-shutdown") ){
        outw(0x604, 0x2000);
    }
    if ( string_same(input_buffer, "reboot") ){
        reboot();
    }
    if ( string_same(input_buffer, "multitasking-test") ){
        yield();
    }
    if ( string_same(input_buffer, "sound-stop") ){
        stopSound();
    }
    if ( string_same(input_buffer, "scroll") ){
        scroll_pixel_line();
    }
    if ( string_same(input_buffer, "mbr-read") ){
        read_mbr();
    }
    if ( string_same(input_buffer, "ls-root") ){
        Read_BPB_quiet(0);
        Read_root_dir();
    }
    if ( string_same(input_buffer, "cat") ){
        draw_rle_image(rle_image, rle_image_size, cursor_pos_x, cursor_pos_y);
    }
    if ( string_same(input_buffer, "date") ){
        read_rtc();
    }
    if ( string_same(input_buffer, "gui") ){
        stop_cmd_cursor();
        gui_init();
    }
    if ( string_same(input_buffer, "help") ){
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

    commands_with_argument_init(input_buffer);
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

    // SHELL CHARACTER
    if(mode == 1 && exec_flag != 1){
        sprint_char(TTY_CHAR, red);
    }
    reset_arguments();
}

void shift_button_pressed(){
    if ( shift_pressed == true ){
    }
    if ( shift_pressed == false ){
    }
}
void backspace_button_pressed(){
    if ( backspace_pressed == true ){
        sprint_remove_char();
    }
}
void enter_button_pressed(){
    if ( enter_pressed == true ){
        sprint("AAAAA ENTER", red);
        sprint("\n", white);
    }
}

// REMOVE ONE CHARACTER

void sprint_remove_char(){
    int lenght_of_string = strlen(input_buffer);  
    
    if (lenght_of_string > 0) {
        input_buffer[lenght_of_string - 1] = '\0';  
    }

    int length_of_char = 1;
    for (int i = 0; i < length_of_char; i++) {
        print_char_at('!', cursor_pos_x - 1, cursor_pos_y, black); 
        cursor_pos_x = cursor_pos_x - 1;
    }

}
