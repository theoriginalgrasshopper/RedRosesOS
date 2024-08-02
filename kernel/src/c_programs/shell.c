//#include <drivers/keyboard.h>
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





        // ALL THE COMMANDS
void command_init(){

    // COMMANDS WITHOUT ARGUMENTS
    
    sprint("\n", white);
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
    if ( string_same(input_buffer, "sound-stop") ){
        stopSound();
    }
    if ( string_same(input_buffer, "scroll") ){
        scroll_pixel_line();
    }
    if ( string_same(input_buffer, "cat") ){
        draw_rle_image(rle_image, rle_image_size, cursor_pos_x, cursor_pos_y);
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
        sprint("MISC \n\n", green);
        sprint("explode          | originally made and implemented by pac-ac in osakaOS, flashes colours rapidly and produces a loud screech \n", white);
        sprint("cowsay           | accepts one argument, originally made by Tony Monroe, displays a cow saying a specified message \n", white);
        sprint("say              | accepts two arguments, prints a specified message the specified amount of times \n", white);
        sprint("math             | accepts 3 arguments, allows for simple operations with integers\n", white);
        sprint("rosefetch        | displays general information about the system \n", white);
        sprint("random           | accepts two arguments, prints out a pseudo-random value in specified range\n", white);
        sprint("cat              | cat\n", white);
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

    // SHELL CHARACTER

    sprint_char('#', red);
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
        // print the cursor for debug
        //print_char_at('>', cursor_pos_x, cursor_pos_y, 0xffffff);
    }

}
