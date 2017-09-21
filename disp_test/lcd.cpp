
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
 *  lcd.c
 *
 *  Created by Michael Köhler on 22.12.16.
 *  Copyright 2016 Skie-Systems. All rights reserved.
 *
 */
/* Standard ASCII 6x8 font */

#include "lcd.h"
#include "font6x8.h"
#include <Wire.h>

#define LCD_CMD 0x00
#define LCD_DATA 0x40

static void lcd_send(uint8_t* buf, uint8_t len);


/* Initialization Sequence */
const uint8_t ssd1306_init_sequence [] PROGMEM =
{
    LCD_DISP_OFF,			// Display OFF (sleep mode)

    0x20, 0b00,		// Set Memory Addressing Mode
    // 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
    // 10=Page Addressing Mode (RESET); 11=Invalid

    0xB0,			// Set Page Start Address for Page Addressing Mode, 0-7

    0xC8,			// Set COM Output Scan Direction

    0x00,			// --set low column address
    0x10,			// --set high column address
    0x40,			// --set start line address
    0x81, 0x3F,		// Set contrast control register
    0xA1,			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
    0xA6,			// Set display mode. A6=Normal; A7=Inverse
    0xA8, 0x3F,		// Set multiplex ratio(1 to 64)
    0xA4,			// Output RAM to Display
				// 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
    0xD3, 0x00,		// Set display offset. 00 = no offset
    0xD5,			// --set display clock divide ratio/oscillator frequency
    0xF0,			// --set divide ratio
    0xD9, 0x22,		// Set pre-charge period
    0xDA, 0x12,		// Set com pins hardware configuration
    0xDB,			// --set vcomh
    0x20,			// 0x20,0.77xVcc
    0x8D, 0x14,		// Set DC-DC enable
};

//#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega48P__)
//
//	#if PSC_I2C != 1 && PSC_I2C != 4 && PSC_I2C != 16 && PSC_I2C != 64
//		#error "Wrong prescaler for TWI !"
//	#elif SET_TWBR < 0 || SET_TWBR > 255
//		#error "TWBR out of range, change PSC_I2C or F_I2C !"
//	#endif
//
//void i2c_init(void)
//{
//    LCD_PORT |= (1 << SDA_Pin)|(1 << SDC_Pin);        // experimental, pullups for 12c_bus
//    LCD_PORT_DDR |= (1 << SDA_Pin)|(1 << SDC_Pin);
//    // set clock
//    switch (PSC_I2C) {
//        case 4:
//            TWSR = 0x1;
//            break;
//        case 16:
//            TWSR = 0x2;
//            break;
//        case 64:
//            TWSR = 0x3;
//            break;
//        default:
//            TWSR = 0x00;
//            break;
//    }
//    TWBR = (uint8_t)SET_TWBR;
//    // enable
//    TWCR = (1 << TWEN);
//}
//void lcd_send_i2c_start(void){
//    // i2c start
//    TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
//    while((TWCR & (1 << TWINT)) == 0);
//    // send adress
//    TWDR = LCD_I2C_ADDR;
//    TWCR = (1 << TWINT)|( 1 << TWEN);
//    while((TWCR & (1 << TWINT)) == 0);
//}
//void lcd_send_i2c_stop(void){
//    // i2c stop
//    TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
//}
//void lcd_send_i2c_byte(uint8_t byte){
//    TWDR = byte;
//    TWCR = (1 << TWINT)|( 1 << TWEN);
//    while((TWCR & (1 << TWINT)) == 0);
//}

void lcd_init(uint8_t dispAttr) {
    uint8_t buf[2] = {LCD_CMD};
    
    if(LCD_INIT_I2C == YES)
    {
        i2c_init();
    }
    
    for(uint8_t i = 0; i < sizeof(ssd1306_init_sequence); i++)
    {
        buf[1] = pgm_read_byte(&ssd1306_init_sequence[i]);
        lcd_send(buf, sizeof(buf));
        //lcd_command();
    }

    buf[1] = dispAttr;
    lcd_send(buf, sizeof(buf));
    //lcd_command(dispAttr);
    
    lcd_clrscr();
}

void lcd_home(void)
{
    lcd_gotoxy(0, 0);
}

static void lcd_send(uint8_t* buf, uint8_t len)
{
    Wire.beginTransmission(LCD_I2C_ADDR);
    
    while(len)
    {
        Wire.write(*buf);
        ++buf;
        --len;
    }
    
    Wire.endTransmission();
}


//void lcd_command(uint8_t cmd)
//{
//    lcd_send_i2c_start();
//    lcd_send_i2c_byte(0x00);    // 0x00 for command, 0x40 for data
//    lcd_send_i2c_byte(cmd);
//    lcd_send_i2c_stop();
//}
//
//void lcd_data(uint8_t data)
//{
//    lcd_send_i2c_start();
//    lcd_send_i2c_byte(0x40);    // 0x00 for command, 0x40 for data
//    lcd_send_i2c_byte(data);
//    lcd_send_i2c_stop();
//}


void lcd_gotoxy(uint8_t x, uint8_t y)
{
    uint8_t buf[] = {
        LCD_CMD,
        (uint8_t)(0xb0 + y), // set page start to y
        0x21,// set column start
        (uint8_t)(x * 6u),
        0x7f // set column end to 127
    };

    lcd_send(buf, sizeof(buf));
}

void lcd_clrscr(void){
    lcd_home();

    Wire.beginTransmission(LCD_I2C_ADDR);
    Wire.write(LCD_DATA);
    
    //lcd_send_i2c_start();
    //lcd_send_i2c_byte(0x40);    // 0x00 for command, 0x40 for data

    //FIXME:This will fail because of write buffer overflow in wire lib. 
    for(uint16_t i = 0; i < 128 * 8 ; i++)
    {
        Wire.write(0x00);
        //lcd_send_i2c_byte(0);    // clear display while printing space
    }
    Wire.endTransmission();
    //lcd_send_i2c_stop();
    
    lcd_home();
}


/**
 * Write a single character to the current cursor position.
 */
void lcd_putc(char c)
{
    uint8_t idx;
    uint8_t buf[7] = {LCD_DATA};

    idx = font6x8_get_charcode(c);

//    lcd_send_i2c_start();
//    lcd_send_i2c_byte(0x40);    // 0x00 for command, 0x40 for data

    
    // print font to ram, print 6 columns
    for (uint8_t i = 0; i < 6; i++)
    {
        buf[i + 1] = pgm_read_byte(&font6x8_glyphs[idx * 6 + i]);
//        lcd_send_i2c_byte();
    }

    lcd_send(buf, sizeof(buf));
    
//    lcd_send_i2c_stop();
}

void lcd_puts(const char* s)
{
    while (*s) {
        lcd_putc(*s++);
    }
}

void lcd_puts_p(const char* progmem_s)
{
    uint8_t c;

    while ((c = pgm_read_byte(progmem_s++)))
    {
        lcd_putc(c);
    }
}

void lcd_invert(uint8_t invert)
{
    uint8_t buf[2] = {LCD_CMD, 0xA6};

    if(invert)
    {
        buf[1] = 0xA7;
    }

    lcd_send(buf, sizeof(buf));

    
    //lcd_send_i2c_start();
    //lcd_send_i2c_byte(0x00);    // 0x00 for command, 0x40 for data
    //if(invert) {
    //{
    //    lcd_send_i2c_byte(0xA7);// set display inverted mode
    //}
    //else
    //{
    //    lcd_send_i2c_byte(0xA6);// set display normal mode
    //}
    //
    //lcd_send_i2c_stop();
}

void lcd_set_contrast(uint8_t contrast)
{
    uint8_t buf[3] = {LCD_CMD, 0x81, contrast };

    lcd_send(buf, sizeof(buf));
    
    //lcd_send_i2c_start();
    //lcd_send_i2c_byte(0x00);    // 0x00 for command, 0x40 for data
    //lcd_send_i2c_byte(0x81);    // set display contrast
    //lcd_send_i2c_byte(contrast);// to contrast
    //lcd_send_i2c_stop();
}
