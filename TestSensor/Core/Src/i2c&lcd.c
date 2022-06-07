/*
 * i2c&lcd.c
 *
 *  Created on: Sep 28, 2021
 *      Author: Tuan Anh
 */

#include "i2c&lcd.h"

extern I2C_HandleTypeDef hi2c2;
#define hi2c &hi2c2

#define SLAVE_ADDRESS_LCD 0x4E

void lcd_init (void)
{
	// 4 bit set
	HAL_Delay(20);  // wait for >15ms
	lcd_send_command (0x30);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_command (0x30);
	HAL_Delay(1);  // wait for >100us
	lcd_send_command (0x30);
	HAL_Delay(10);
	lcd_send_command (0x20);  // 4bit mode
	HAL_Delay(10);
	// set display
	lcd_send_command (0x28); // Function set: DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_command (0x08); //Display on/off control: D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_command (0x01);  // clear display
	HAL_Delay(2);
	lcd_send_command (0x06); //Entry mode set: I/D = 1 (increment cursor), SH = 0 (no shift)
	HAL_Delay(1);
	lcd_send_command (0x0C); //Display on/off control: D = 1, C, B = 0
}

void lcd_send_command (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  // E =1, RS =0, R/W =0
	data_t[1] = data_u|0x08;  // E =0, RS =0
	data_t[2] = data_l|0x0C;  // E =1, RS =0
	data_t[3] = data_l|0x08;  // E =0, RS =0
	HAL_I2C_Master_Transmit (hi2c, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  // E =1, RS =1, R/W =0
	data_t[1] = data_u|0x09;  // E =0, RS =1
	data_t[2] = data_l|0x0D;  // E =1, RS =1
	data_t[3] = data_l|0x09;  // E =0, RS =1
	HAL_I2C_Master_Transmit (hi2c, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_clear (void)
{
	lcd_send_command(0x01);
	HAL_Delay(2);
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80; // DDRAM address row 1
            break;
        case 1:
            col |= 0xC0; // DDRAM address row 2
            break;
    }

    lcd_send_command (col);
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}
