/*
 * supFunction.h
 *
 *  Created on: Nov 30, 2021
 *      Author: Tuan Anh
 */

#ifndef INC_SUBFUNCTION_H_
#define INC_SUBFUNCTION_H_

#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "i2c&lcd.h"

// supFunction
void Display_LCD(char s[],int put_cur, float f);
// display to LCD
// s[]: name of value
// f  : value (%.2f)

float mapping(float value, float in_min, float in_max, float out_min, float out_max);
// convert analog value to %

void delay(uint16_t);
// delay in us

void Set_Pin_Output(GPIO_TypeDef*, uint16_t);
// set pin as GIPO_OutPut

void Set_Pin_Input(GPIO_TypeDef*, uint16_t);
// set pin as GPIO_InPut

#endif /* INC_SUBFUNCTION_H_ */
