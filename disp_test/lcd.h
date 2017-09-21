/*
 * This file is part of lcd library for ssd1306 oled-display.
 *
 * lcd library for ssd1306 oled-display is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any later version.
 *
 * lcd library for ssd1306 oled-display is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von lcd library for ssd1306 oled-display.
 *
 * lcd library for ssd1306 oled-display ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * lcd library for ssd1306 oled-display wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 *  lcd.h
 *
 *  Created by Michael Köhler on 22.12.16.
 *  Copyright 2016 Skie-Systems. All rights reserved.
 *
 *  lib for OLED-Display with SSD1306-Controller
 *  first dev-version only for I2C-Connection
 *  at ATMega328P like Arduino Uno or ATMega168PA/88PA/48PA
 *
 */

#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/* 0x78 for 8-bit-adressmode, 0x3c for 7-bit-adressmode */
#define LCD_I2C_ADDR	0x3C	
#define F_I2C			400000UL // clock i2c


void lcd_init(uint8_t dispAttr);

void lcd_home(void);

/**
 * set curser at pos x, y. x means character, y means line (page, refer lcd manual).
 */
void lcd_gotoxy(uint8_t x, uint8_t y);

/**
 * Clear screen.
 */
void lcd_clrscr(void);

/**
 * Print single character at current cursor position.
 */
void lcd_putc(char c);

/**
 * Print zero-terminated string.
 */
void lcd_puts(const char* s);

/**
 * Print zero-terminated string from program memory.
 */
void lcd_puts_p(const char* progmem_s);

/**
 * Set lcd to normal or inverted mode.
 */
void lcd_invert(uint8_t invert);

/**
 * Configure LCD contrast setting.
 */
void lcd_set_contrast(uint8_t contrast);

/**
 * Switch on or off the display.
 */
void lcd_set_power(uint8_t state);

#endif /*  LCD_H  */
