/*
 * i2c&lcd.h
 *
 *  Created on: Sep 28, 2021
 *      Author: Tuan Anh
 */

#include "supFunction.h"

void lcd_init (void);   // initialize lcd

void lcd_send_command (char);  // send command to the lcd

void lcd_send_data (char);  // send data to the lcd

void lcd_send_string (char*);  // send string to the lcd

void lcd_put_cur(int, int);  // put cursor - position row (0 or 1), col (0-15)

void lcd_clear (void);

