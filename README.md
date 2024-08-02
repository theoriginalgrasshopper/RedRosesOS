# RedRosesOS 1.0

RedRosesOS is a monolithic hobbyist 64 bit operating system written in C.
![alt text](https://github.com/theoriginalgrasshopper/RedRosesOS/blob/main/start_menu.png?raw=true)

Documentation will soon be in the wiki page. 

## ---USE---

The O.S. supports a couple of commands. Full list is below.

### GENERAL--

clear            | clears the screen  
about            | shows information about RedRosesOS  
help             | shows a help message  
qemu-shutdown    | shuts down QEMU ver. 2.0 and newer  
reboot           | reboots the system  

### VISUAL--

start-menu       | shows the start menu  
pixel            | accepts three arguments, draws a specified pixel at specified coordinates     |     pixel <x> <y> <colour in hex>  

### SOUND--

sound-play       | accepts one argument, plays a tone in specified frequency    |    sound-play <frequency>  
sound-play-timed | accepts two arguments, plays a tone in specified frequency for a specified amount of ticks. 87 ticks is equal to a second    |    sound-play-timed <frequency> <ticks>  
sound-stop       | stops the sound of the PC speaker  

### MISC--

explode          | originally made and implemented by pac-ac in osakaOS, flashes colours rapidly and produces a loud screech  
cowsay           | accepts one argument, originally made by Tony Monroe, displays a cow saying a specified message    |    cowsay <"message">  
say              | accepts two arguments, prints a specified message the specified amount of times    |    say <"message"> <"amount">  
math             | accepts 3 arguments, allows for simple operations with integers    |    math <num one> <operator> <num two>
rosefetch        | displays general information about the system  
random           | accepts two arguments, prints out a pseudo-random value in specified range    |    random <num one> <num two>
cat              | cat 

![alt text](https://github.com/theoriginalgrasshopper/RedRosesOS/blob/main/showcase.png?raw=true)


## ---DOWNLOAD---


The ISO file of the O.S. is located in the releases page


## ---BUILD---


To build the O.S, you will need a couple of dependencies.

**glibc_multi,
nasm,
xorriso**,

```git clone https://github.com/theoriginalgrasshopper/RedRosesOS.git``` <br \>
```cd RedRosesOS``` <br \>
```make run```


## ---TO-DO---


This is a list of implemented and not features that will come with future releases

* ~~GDT~~
* ~~IDT~~
* ~~Screen text driver~~
* ~~Keyboard driver~~
* FAT12/16/32 filesystem driver
* Custom compiler for programs
* Desktop
* Mouse driver
* Networking
* Web browser
