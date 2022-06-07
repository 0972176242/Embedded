/*
 * subFunction.c
 *
 *  Created on: Nov 30, 2021
 *      Author: Tuan Anh
 */

#include "subFunction.h"

//extern ADC_HandleTypeDef hadc1;
//extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
//extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim1;

void Display_LCD(char s[],int put_cur, float f){
	char str[16] = {0};
	lcd_put_cur(put_cur-1,0);
	sprintf(str,"%s: %.2f",s, f);
	lcd_send_string(str);
}

float mapping(float value, float in_min, float in_max, float out_min, float out_max){
	float percent = (value-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
	return percent;
}

void delay (uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim1))<time);
}

void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}


