/* -*- mode:c -*- */
#include "lcd.h"
#include <Wire.h>
#include <avr/pgmspace.h>

static const char p_test[] PROGMEM = "From Progmem!";

void setup()
{
  // put your setup code here, to run once:

  Wire.begin();

  lcd_init();

  lcd_set_power(1);
  lcd_home();
  lcd_puts("Hello World");

  lcd_gotoxy(1, 2);
  lcd_puts_p(p_test);
  
}

void loop() {
    uint8_t c = 0;
    char num[5];

    for(c = 0; c <255; c+=5)
    {
        lcd_set_contrast(c);
        delay(250);

        lcd_gotoxy(3,3);
        lcd_puts("   ");
        lcd_gotoxy(3, 3);
        itoa(c, num, 10);
        lcd_puts(num);
    }
  // put your main code here, to run repeatedly:

}
