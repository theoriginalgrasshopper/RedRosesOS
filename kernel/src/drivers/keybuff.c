#include "keybuff.h"

volatile uint8_t  scancode_buffer[SCANCODE_BUFSIZE];
volatile uint32_t read_pos = 0;
volatile uint32_t write_pos = 0;

uint32_t sc_normalize(uint32_t val) {
	return val & (SCANCODE_BUFSIZE - 1);
}

void sc_saveelement(uint8_t scancode) {
	scancode_buffer[sc_normalize(write_pos++)] = scancode;
}

uint8_t sc_getelement(void) {
	return scancode_buffer[sc_normalize(read_pos++)];
}

uint32_t sc_numelements(void) {
	return write_pos - read_pos;
}

bool sc_isfull(void) {
	return sc_numelements() == SCANCODE_BUFSIZE;
}

bool sc_isempty(void) {
	return write_pos == read_pos;
}

void sc_flush(void) {
	write_pos = read_pos;
}
