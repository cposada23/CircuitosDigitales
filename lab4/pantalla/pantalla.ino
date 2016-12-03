/*
  Hello World.ino
  2013 Copyright (c) Seeed Technology Inc.  All right reserved.
  Author:Loovee
  2013-9-18
  Grove - Serial LCD RGB Backlight demo.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

int colorR = 200;
int colorG = 100;
int colorB = 255;
int x = 0;
void setup() 
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    
    lcd.setRGB(colorR, colorG, colorB);
    
    // Print a message to the LCD.
    lcd.print("hello, world!");

    delay(1000);
}

void loop() 
{
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    
    lcd.setCursor(x, 1);
    x= (x + 1) % 17;
    // print the number of seconds since reset:
    lcd.print(millis()/1000);
    colorR = (colorR + 1) % 255;
    colorG = (colorG + 1) % 255;
    colorB = (colorB + 1) % 255;
    lcd.setRGB(colorR, colorG, colorB);
    delay(100);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
