/*
 * SHT31.h
 *
 *  Created on: Dec 2, 2021
 *      Author: Tuan Anh
 */

#ifndef INC_SHT31_H_
#define INC_SHT31_H_

#include "supFunction.h"

#define SLAVE_ADDRESS_SHT31A 0x44 // ADDR (pin 2) connect to VSS(GND)
#define SLAVE_ADDRESS_SHT31B 0x45 // ADDR (pin 2) connect to VDD

typedef struct temp_humi{
	float Temperature,Humidity;
}t_h;

t_h SHT31;

t_h get_temp_humi();


#endif /* INC_SHT31_H_ */
