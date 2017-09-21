#ifndef FONT6X8_H
#define FONT6X8_H

#include <stdint.h>
#include <avr/pgmspace.h>

extern const uint8_t font6x8_glyphs[] PROGMEM;
uint8_t font6x8_get_charcode(char c);

#endif /* FONT6X8_H */
