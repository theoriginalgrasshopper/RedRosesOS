#include <stdint.h>

// Function prototypes
void mouse_wait(uint8_t type);
void mouse_write(uint8_t data);
uint8_t mouse_read();
void mouse_init();