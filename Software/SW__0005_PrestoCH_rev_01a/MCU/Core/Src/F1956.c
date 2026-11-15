/*
 * F1956.c
 *
 *  Created on: Dec 17, 2024
 *      Author: sp6gk
 *
 *  For SPI usage, define SPI and initialize with F1956_SetSPIHandle before calling F1956_S21_SPI
 *
 */

#include "stm32f4xx.h" //Tested with STM32F411
#include "gpio.h"
#include "F1956.h"
#include "spi.h"
#include <stdbool.h>
#include <math.h>

SPI_HandleTypeDef *F1956_SPI_Handle = NULL;

//Sets SPI handle, example usage for SPI2 after CubeMX code generation: F1956_SetSPIHandle(&hspi2);
void F1956_SetSPIHandle(SPI_HandleTypeDef *hspi) {
    F1956_SPI_Handle = hspi;
}


//Sets attenuator on a serial bus with address 'address'.
//Value of attenuation is in 7 bit binary not dB! Use the LUT function for encoding of dB.
//F1956 Vmode must be high or floating for serial mode to be active.
//Uses SPI hardware and HAL. Lower baud rate can be set in spi initialization code, change of gpio speed is also encouraged.
void F1956_S21_SPI(uint8_t att, uint8_t address){
	uint8_t cat_data[2]  = {att, address}; //concatenated data of attenuation and address.

	if(att > 128){
		att = 128;
	}

	if(address > 7){
		address = 7;
		//This is not the best handling of this exception, I know.
	}

	HAL_GPIO_WritePin(ATT_LE_GPIO_Port, ATT_LE_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(F1956_SPI_Handle, cat_data, sizeof(cat_data), 100);
	HAL_GPIO_WritePin(ATT_LE_GPIO_Port, ATT_LE_Pin, GPIO_PIN_SET);
}


//Returns value from 0 to 128 that represents the attenuation in dB (0 to 31.75 in 0.25 dB steps).
//Based on the look-up table 2 p. 7 of datasheet
uint8_t F1956_LUT(float att_dB) {
    uint8_t att_code = 0;
    uint8_t integer_part = 0;
    float fractional_part = 0;

    //Input "sanitization"
    if (att_dB > 31.75f) {
        att_dB = 31.75f;
    } else if (att_dB < 0.0f) {
        att_dB = 0.0f;
    }

    //Split the float into integer and fractional parts
    double temp_integer_part = 0.0;
    fractional_part = modf((double)att_dB, &temp_integer_part);
    integer_part = (uint8_t)temp_integer_part;

    if (integer_part > 31) {
        integer_part = 31;
    }

    //Handle the fractional part (rounding to 0.25 dB steps)
    if (fractional_part >= 0.875f) {
        fractional_part = 0.0f;    //Increment integer part if >= 0.875
        if (integer_part < 31) {
            integer_part++;
        }
    } else if (fractional_part >= 0.625f) {
        att_code |= 0x03;          //0.75 dB => set first two bits to `11` and so on...
    } else if (fractional_part >= 0.375f) {
        att_code |= 0x02;
    } else if (fractional_part >= 0.125f) {
        att_code |= 0x01;
    }

    //Extract the least significant 6 bits of the integer part and concatenate to create final code
    att_code |= (integer_part & 0x3F) << 2;

    return att_code;
}
