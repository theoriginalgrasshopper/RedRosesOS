# RedRosesOS 2.0

RedRosesOS is a monolithic hobbyist 64-bit operating system written in C.

![RedRosesOS Start Menu](https://github.com/theoriginalgrasshopper/RedRosesOS/blob/main/start_menu.png?raw=true)

## Table of Contents

1. [Features](#features)
2. [Commands](#commands)
3. [Installation](#installation)
4. [Building from Source](#building-from-source)
5. [Screenshots](#screenshots)
6. [Roadmap](#roadmap)
7. [Contributing](#contributing)
8. [License](#license)

## Features

- 64-bit architecture
- Monolithic kernel
- Basic shell interface
- Graphics capabilities (pixel drawing)
- Sound output through PC speaker
- Simple math operations with integers
- System information display

## Commands

RedRosesOS supports the following commands:

### GENERAL

| Command | Description |
|---------|-------------|
| `clear` | Clears the screen |
| `about` | Shows information about RedRosesOS |
| `help` | Displays a help message |
| `qemu-shutdown` | Shuts down QEMU ver. 2.0 and newer |
| `reboot` | Reboots the system |

### VISUAL

| Command | Description |
|---------|-------------|
| `start-menu` | Shows the start menu |
| `pixel` | Draws a specified pixel at given coordinates (accepts three arguments) |
| `gui` | Starts the gui mode of the OS (WIP) |


### SOUND

| Command | Description |
|---------|-------------|
| `sound-play` | Plays a tone at a specified frequency (accepts one argument) |
| `sound-play-timed` | Plays a tone for a specified duration (accepts two arguments; 87 ticks is equal to a second) |
| `sound-stop` | Stops the PC speaker sound |

### DISK & FS

| Command | Description |
|---------|-------------| 
| `diskw` | Writes specified data with the specified amount of bytes to the specified sector (accepts three arguments) |
| `diskr` | Reads the specified amount of bytes from the specified sector (accepts two arguments) |
| `ls` | Lists contents of the specified path. "/" is considered both a separator and the root directory (accepts one argumnet) |
| `ls-root` | Alias for "ls /" |
| `read` | Reads from the specified file with the specified extension. (accepts two arguments) |
| `cat` | Alias for "read" |
| `mkdir` | Creates a directory in the specidied path (accepts one argument) |
| `touch` | Creates a file in the specified path with the specified extension (accepts two arguments) | 
| `write` | Writes the specified data with the specified amount of bytes to the specified file with the specified extension (accepts four arguments) |
| `mbr` | Provides information about the MBR

### MISC

| Command | Description |
|---------|-------------|
| `explode` | Flashes colors rapidly and produces a loud screech (originally by pac-ac in osakaOS) |
| `cowsay` | Displays a cow saying a specified message (accepts one argument; originally by Tony Monroe) |
| `say` | Prints a specified message a givens number of times (accepts two arguments) |
| `math` | Performs simple operations with integers (accepts three arguments) |
| `rosefetch` | Displays general system information |
| `random` | Generates a pseudo-random value within a specified range (accepts two arguments) |
| `cat` | cat |

## Installation

1. Download the IMG file from the [releases page](https://github.com/theoriginalgrasshopper/RedRosesOS/releases).
2. Install QEMU if you haven't already.
3. Run the OS using QEMU with the following command:

```
qemu-system-x86_64 -M pc -m 2G -drive file=RedRosesOS.img,format=raw -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0 -monitor stdio
```

Note: When using other virtualization software, including virt-manager, use the VGA aviable card, as well as an IDE disk.

## Building from Source

To build RedRosesOS, you'll need the following dependencies:

- glibc_multi
- nasm
- xorriso
- gptfdisk
- parted

```bash
git clone https://github.com/theoriginalgrasshopper/RedRosesOS.git
cd RedRosesOS
make run-img
```

## Screenshots

![RedRosesOS Showcase](https://github.com/theoriginalgrasshopper/RedRosesOS/blob/main/showcaseh.png?raw=true)

## Roadmap

This is a list of implemented and not features that will come with future releases

- [x] GDT 
- [x] IDT 
- [x] Screen driver
- [x] Keyboard driver
- [X] ATA PIO driver
- [X] FAT12/16/32 filesystem driver
- [X] Memory Management
- [ ] ELF support
- [ ] Multitasking
- [ ] Windowing system
- [X] Mouse driver
- [ ] Networking 
- [ ] Web browser

## Contributing

Contributions to RedRosesOS are welcome! Please refer to the [Contributing Guidelines](CONTRIBUTING.md) for more information on how to get started (coming soon, for now, you can open PRs and issues freely).

---------

For more detailed documentation, please refer to the [Wiki page](https://github.com/theoriginalgrasshopper/RedRosesOS/wiki) (coming soon).
<br>
The README's updated look was made possible by Gurov.
