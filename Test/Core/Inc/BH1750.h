/*
 * BH1750.h
 *
 *  Created on: Sep 28, 2021
 *      Author: Tuan Anh
 */

#ifndef INC_BH1750_H_
#define INC_BH1750_H_

#include "supFunction.h"

#define BH1750_ADDR_LOW_W 0x46
#define BH1750_ADDR_LOW_R 0x47

typedef enum{
	POWER_ON = 0x01,
	POWER_DOWN = 0x00,
	RESET_REGISTER = 0x07,
	CONT_H_MODE = 0x10,
	CONT_H_MODE2 = 0x11,
	CONT_L_MODE = 0x13,
	ONCE_H_MODE = 0x20,
	ONCE_H_mODE2 = 0x21,
	ONCE_L_MODE = 0x23
}BH1750_cmd;

void BH1750_reset();
void PowerOn();
void PowerDown();
void setMeasurement(BH1750_cmd);
float BH1750_getResult();

#endif /* INC_BH1750_ADDRESS_H_ */
