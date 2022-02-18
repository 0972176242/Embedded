#include "BH1750.h"
extern I2C_HandleTypeDef hi2c2;
#define hi2c &hi2c2

void BH1750_reset(){
	BH1750_cmd cmd = RESET_REGISTER;
	HAL_I2C_Master_Transmit(hi2c, BH1750_ADDR_LOW_W, (uint8_t *)&cmd, 1, 50);
	HAL_Delay(1);
}

void PowerOn(){
	BH1750_cmd cmd = POWER_ON;
	HAL_I2C_Master_Transmit(hi2c, BH1750_ADDR_LOW_W, (uint8_t *)&cmd, 1, 50);
	HAL_Delay(1);
}

void PowerDown(){
	BH1750_cmd cmd = POWER_DOWN;
	HAL_I2C_Master_Transmit(hi2c, BH1750_ADDR_LOW_W, (uint8_t *)&cmd, 1, 50);
	HAL_Delay(1);
}

void setMeasurement(BH1750_cmd cmd){
	if(cmd == CONT_L_MODE||ONCE_L_MODE) {
		HAL_I2C_Master_Transmit(hi2c, BH1750_ADDR_LOW_W, (uint8_t *)&cmd, 1, 50);
		HAL_Delay(16);
	}
	else {
		HAL_I2C_Master_Transmit(hi2c, BH1750_ADDR_LOW_W, (uint8_t *)&cmd, 1, 50);
		HAL_Delay(120);
	}
}

float BH1750_getResult(){
	uint8_t byte[2];
	uint16_t result;
	float light = 0;

	HAL_I2C_Master_Receive(hi2c, BH1750_ADDR_LOW_R, (uint8_t *)&byte, 2, 100);
	result = (byte[0]<<8)|byte[1];
	light = (float)result/1.2;
	return light;
}
