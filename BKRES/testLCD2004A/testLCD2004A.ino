
  
/*
 *  Download library: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
 *  
 *  Updated by Ahmad Shamshiri on July 08, 2018 at 19:14 in Ajax, Ontario, Canada
 * for Robojax.com
 * Get this code from Robojax.com
 * Watch video instruction for this code at:https://youtu.be/DKmNSCMPDjE
 * 
 */
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
	// initialize the LCD
  lcd.begin();

	// Turn on the blacklight and print a message.
	lcd.backlight();
	lcd.print("Hello, world!");
}

void loop()
{
	// Do nothing here...
}
 