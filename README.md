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

start-menu       | shows the start menu  <br>
pixel            | accepts three arguments, draws a specified pixel at specified coordinates       <br>

### SOUND--

sound-play       | accepts one argument, plays a tone in specified frequency     <br>
sound-play-timed | accepts two arguments, plays a tone in specified frequency for a specified amount of ticks. 87 ticks is equal to a second <br>
sound-stop       | stops the sound of the PC speaker   <br>

### MISC--

explode          | originally made and implemented by pac-ac in osakaOS, flashes colours rapidly and produces a loud screech   <br>
cowsay           | accepts one argument, originally made by Tony Monroe, displays a cow saying a specified message  <br>
<br>say              | accepts two arguments, prints a specified message the specified amount of times  <br>
math             | accepts 3 arguments, allows for simple operations with integers     <br>
rosefetch        | displays general information about the system   <br>
random           | accepts two arguments, prints out a pseudo-random value in specified range   <br>
cat              | cat 

![alt text](https://github.com/theoriginalgrasshopper/RedRosesOS/blob/main/showcase.png?raw=true)


## ---DOWNLOAD AND RUN---


The ISO file of the O.S. is located in the [releases](https://github.com/theoriginalgrasshopper/RedRosesOS/releases) page.
To run the O.S. with QEMU, use the following command: <br>```qemu-system-x86_64 -M q35 -m 2G -cdrom RedRosesOS.iso -boot d -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0``` <br>
The q35 graphics card is what works.

## ---BUILD---


To build the O.S, you will need a couple of dependencies.

**glibc_multi,
nasm,
xorriso**,

```git clone https://github.com/theoriginalgrasshopper/RedRosesOS.git``` <br>
```cd RedRosesOS``` <br>
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
