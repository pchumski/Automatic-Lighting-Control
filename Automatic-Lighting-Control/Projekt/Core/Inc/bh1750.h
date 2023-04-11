/*
 * bh1750.h
 *
 *  Created on: Nov 13, 2021
 *      Author: konst
 */

#ifndef BH1750_H_
#define BH1750_H_

// Includes
#include "stm32f7xx_hal.h"
#include "i2c.h"

#define BH1750_I2CType I2C_HandleTypeDef*

typedef struct{
	BH1750_I2CType I2C;
	uint8_t Address;
	uint32_t Timeout;
}BH1750_HandleTypeDef;

// Definicje
#define BH1750_ADDRESS_L (0x23 << 1)
#define BH1750_ADDRESS_H (0x5C << 1)
#define BH1750_POWER_DOWN 0x00
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07
#define BH1750_CONTINOUS_H_RES_MODE 0X10
#define BH1750_CONTINOUS_H_RES_MODE2 0x11
#define BH1750_CONTINOUS_L_RES_MODE 0x13
#define BH1750_ONE_TIME_H_RES_MODE 0x20
#define BH1750_ONE_TIME_H_RES_MODE2 0x21
#define BH1750_ONE_TIME_L_RES_MODE 0x23

// Funkcje
void BH1750_Init(BH1750_HandleTypeDef* hbh1750,uint8_t command);
float BH1750_ReadLux(BH1750_HandleTypeDef* hbh1750);


#endif /* BH1750_H_ */
