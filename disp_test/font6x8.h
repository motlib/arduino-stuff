#ifndef FONT6X8_H
#define FONT6X8_H

#include <stdint.h>
#include <avr/pgmspace.h>

/**
 * Character pixel patterns (glyphs) for 6x8 font.
 */
extern const uint8_t font6x8_glyphs[] PROGMEM;

/**
 * Convert a character to it's index in the font table.
 */
uint8_t font6x8_get_charcode(char c);

#endif /* FONT6X8_H */
