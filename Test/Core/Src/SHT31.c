/*
 * SHT31.c
 *
 *  Created on: Dec 2, 2021
 *      Author: Tuan Anh
 */

#include "SHT31.h"


extern I2C_HandleTypeDef hi2c2;
#define hi2c &hi2c2

t_h get_temp_humi(){
	uint8_t data[2];
	int temp;
	int humi;
	uint8_t readbuff[6];

	HAL_Delay(50);
	data[0] = 0x22; //command
	data[1] = 0x36;
	HAL_I2C_Master_Transmit(hi2c,SLAVE_ADDRESS_SHT31A<<1,data,2,50);
	HAL_Delay(50);
	HAL_I2C_Master_Receive(hi2c,SLAVE_ADDRESS_SHT31A<<1,readbuff,6,50);

	temp = (readbuff[0] * 256) + readbuff[1];
	SHT31.Temperature = -45.0 + (175.0 * temp / 65535.0);
	humi = (readbuff[3] * 256) + readbuff[4];
	SHT31.Humidity = (100.0 * humi / 65535.0);
	return SHT31;
}
