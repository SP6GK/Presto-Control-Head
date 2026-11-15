/*
 * MCP9800.c
 *
 *  Created on: Feb 21, 2026
 *      Author: sp6gk
 */

#include "MCP9800.h"
#include "i2c.h"
#include <math.h>

void MCP9800_init(MCP9800_HandleTypeDef *dev){
	HAL_StatusTypeDef status;
	uint8_t readback;
	uint8_t buffer[2]; //Stores hysteresis and t limit in 2s form

	uint8_t config_reg =
			      ((dev->oneshot        & 0x01) << 7)
			    | ((dev->resolution     & 0x03) << 5)
			    | ((dev->fault_queue    & 0x03) << 3)
			    | ((dev->alert_polarity & 0x01) << 2)
			    | ((dev->comp_int       & 0x01) << 1)
			    |  (dev->shutdown       & 0x01);

	status = HAL_I2C_Mem_Write(dev->hi2c, dev->address, MCP9800_conf_reg_addr, I2C_MEMADD_SIZE_8BIT, &config_reg, 1, HAL_MAX_DELAY);

	status = HAL_I2C_Mem_Read(dev->hi2c, dev->address, MCP9800_conf_reg_addr, I2C_MEMADD_SIZE_8BIT, &readback, 1, HAL_MAX_DELAY);
	if((readback != config_reg) || (status != HAL_OK)){
		//Handle I2C exception
	}

	//Write hysteresis and limit temperature registers
	uint16_t temp_2s = convert_flotat_2sComplement(dev, dev->temp_hyst);
	buffer[0] = (temp_2s >> 8) & 0xFF;
	buffer[1] = temp_2s & 0xFF;
	status = HAL_I2C_Mem_Write(dev->hi2c, dev->address, MCP9800_Thys_reg_addr, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
	temp_2s = convert_flotat_2sComplement(dev, dev->temp_limit);
	buffer[0] = (temp_2s >> 8) & 0xFF;
	buffer[1] = temp_2s & 0xFF;
	status = HAL_I2C_Mem_Write(dev->hi2c, dev->address, MCP9800_Tlim_reg_addr, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);

	status = HAL_I2C_Mem_Read(dev->hi2c, dev->address, MCP9800_Thys_reg_addr, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
	uint16_t readback_16b;
	readback_16b = ((uint16_t)buffer[0] << 8) | buffer[1];
	if((readback_16b != temp_2s) || (status != HAL_OK)){
		//Handle exception, hysteresis was not set correctly
	}

	temp_2s = convert_flotat_2sComplement(dev, dev->temp_hyst);
	buffer[0] = (temp_2s >> 8) & 0xFF;
	buffer[1] = temp_2s & 0xFF;
	status = HAL_I2C_Mem_Read(dev->hi2c, dev->address, MCP9800_Tlim_reg_addr, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
	readback_16b = ((uint16_t)buffer[0] << 8) | buffer[1];
	if((readback_16b != temp_2s) || (status != HAL_OK)){
		//Handle exception, limit temperature was not set correctly
	}

}

uint16_t MCP9800_read_raw_temperature(MCP9800_HandleTypeDef *dev){
	uint8_t temp_raw[2];
	uint16_t temp_val = 0;
	HAL_StatusTypeDef tmp_read_status;

	tmp_read_status = HAL_I2C_Mem_Read(dev->hi2c, dev->address, MCP9800_temp_reg_addr, I2C_MEMADD_SIZE_8BIT, temp_raw, 2, HAL_MAX_DELAY);
	if(tmp_read_status == HAL_OK){
		temp_val = ((temp_raw[0] << 8) | temp_raw[1]);
		return temp_val;
	}
	else{
		//Handle exception, could not read temperature
	}
}

float MCP9800_read_temperature(MCP9800_HandleTypeDef *dev){
	uint16_t temp_code = MCP9800_read_raw_temperature(dev);
	int16_t signed_temp = (int16_t) temp_code;

	float res_f = 0;
	switch(dev->resolution){
		case 0:			//9 bits
			res_f = 0.5;
			signed_temp >>= 7;
			break;
		case 1:			//10 bits
			res_f = 0.25;
			signed_temp >>= 6;
			break;
		case 2:			//11 bits
			res_f = 0.125;
			signed_temp >>= 5;
			break;
		case 3:		//12 bits
			res_f = 0.0625;
			signed_temp >>= 4;
			break;
	}

	return signed_temp * res_f;
}

uint16_t convert_flotat_2sComplement(MCP9800_HandleTypeDef *dev, float temperature){
	uint8_t resolution_bits = 9 + dev->resolution;
	uint8_t shift = 16 - resolution_bits;
	float res_f = 0;

	if(temperature > 125){
		temperature = 125;
	}
	if(temperature < -55){
		temperature = -55;
	}

    switch (dev->resolution){
        case MCP9800_RES_9BIT:
        	res_f = 0.5f;
        	break;
        case MCP9800_RES_10BIT:
        	res_f = 0.25f;
        	break;
        case MCP9800_RES_11BIT:
        	res_f = 0.125f;
        	break;
        case MCP9800_RES_12BIT:
        	res_f = 0.0625f;
        	break;
        default:
        	return 0;
    }
    int16_t temp_code = (int16_t)roundf(temperature / res_f);
    temp_code <<= shift;

	return temp_code;
}
