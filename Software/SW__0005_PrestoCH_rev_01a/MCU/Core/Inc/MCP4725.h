/*
 * MC4725.h
 *
 *  Created on: Apr 18, 2026
 *      Author: sp6gk
 */

#ifndef SRC_MCP4725_H_
#define SRC_MCP4725_H_

#define MCP4725_VCTCXO_DAC_ADDRESS 0x60 << 1

#include "stm32f4xx_hal.h"
#include "main.h"

typedef struct
{
	I2C_HandleTypeDef *hi2c;
	uint8_t 		  id;
	uint8_t			  i2c_address;
}MCP4725_HandleTypeDef;

void MCP4725_SetValue(MCP4725_HandleTypeDef *dev, uint16_t value);			//Set DAC using fast mode write command
void MCP4725_SetValueSetEEPROM(MCP4725_HandleTypeDef *dev, uint16_t value);	//Write DAC and EEPROM
void MCP4725_Read(MCP4725_HandleTypeDef *dev); 								//Read DAC register and EEPROM

#endif /* SRC_MCP4725_H_ */
