/* -*- mode:c -*- */
#include "lcd.h"
#include <Wire.h>

void setup() {
  // put your setup code here, to run once:

  Wire.begin();

  lcd_init();

  lcd_set_power(1);
  lcd_home();
  lcd_puts("Hello World");
}

void loop() {
  // put your main code here, to run repeatedly:

}
