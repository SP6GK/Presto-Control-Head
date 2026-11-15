/*
 * MCP9800.h
 *
 *  Created on: Feb 21, 2026
 *      Author: sp6gk
 */

#ifndef INC_MCP9800_H_
#define INC_MCP9800_H_

#define MCP9800_amp_addr (0x48 << 1)
#define MCP9800_mb_addr  (0xFF << 1)

#include "stm32f4xx_hal.h"
#include "stdbool.h"

typedef struct{
	I2C_HandleTypeDef *hi2c;
	uint8_t address;
	uint8_t resolution;
	bool oneshot;
	uint8_t fault_queue;
	bool alert_polarity;
	bool comp_int;
	bool shutdown;
	float temp_hyst;
	float temp_limit;
}MCP9800_HandleTypeDef;


#define MCP9800_RES_9BIT  0
#define MCP9800_RES_10BIT 1
#define MCP9800_RES_11BIT 2
#define MCP9800_RES_12BIT 3

#define MCP9800_FAULT_1 0
#define MCP9800_FAULT_2 1
#define MCP9800_FAULT_4 2
#define MCP9800_FAULT_6 3


#define MCP9800_temp_reg_addr 0x0
#define MCP9800_conf_reg_addr 0x1
#define MCP9800_Thys_reg_addr 0x2
#define MCP9800_Tlim_reg_addr 0x3

void MCP9800_init(MCP9800_HandleTypeDef *dev);	//Write configuration register
uint16_t MCP9800_read_raw_temperature(MCP9800_HandleTypeDef *dev); //Read temperature register (2s complement code)

float MCP9800_read_temperature(MCP9800_HandleTypeDef *dev); //Convert temperature code
uint16_t convert_flotat_2sComplement(MCP9800_HandleTypeDef *dev, float temperature);



#endif /* INC_MCP9800_H_ */
