/*
 * F1956.h
 *
 *  Created on: Dec 17, 2024
 *      Author: sp6gk
 */

#ifndef SRC_F1956_H_
#define SRC_F1956_H_

#define F1956_MAX_CODE 127

extern SPI_HandleTypeDef *F1956_SPI_Handle;

#endif /* SRC_F1956_H_ */

void F1956_SetSPIHandle(SPI_HandleTypeDef *hspi);

void F1956_S21_set(uint8_t att, uint8_t address);

void F1956_S21_SPI(uint8_t att, uint8_t address);

uint8_t F1956_LUT(float att_dB);
