/*
 * MCP4725.c
 *
 *  Created on: Apr 18, 2026
 *      Author: sp6gk
 */


#include <MCP4725.h>
#include "i2c.h"
#include "globals.h"


void MCP4725_SetValue(MCP4725_HandleTypeDef *dev, uint16_t value){
    HAL_StatusTypeDef status;
    uint8_t buffer[2];

    value &= 0x0FFF; // 12-bit mask

    //First byte: 0 0 PD1 PD0 D11 D10 D9 D8
    buffer[0] = (uint8_t)((value >> 8) & 0x0F);
    //Second byte: D7 D6 D5 D4 D3 D2 D1 D0
    buffer[1] = (uint8_t)(value & 0xFF);
    xSemaphoreTake(i2cMutex, portMAX_DELAY);
    status = HAL_I2C_Master_Transmit(dev->hi2c, dev->i2c_address, buffer, 2, HAL_MAX_DELAY);
    xSemaphoreGive(i2cMutex);

    if(status != HAL_OK){
    	//Handle error
    }

}

void MCP4725_SetValueSetEEPROM(MCP4725_HandleTypeDef *dev, uint16_t value){

}

void MCP4725_Read(MCP4725_HandleTypeDef *dev){

}
