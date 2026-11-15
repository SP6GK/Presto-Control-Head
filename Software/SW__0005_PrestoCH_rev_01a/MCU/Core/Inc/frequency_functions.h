/*
 * frequency_functions.h
 *
 *  Created on: Dec 27, 2025
 *      Author: sp6gk
 */

#ifndef INC_FREQUENCY_FUNCTIONS_H_
#define INC_FREQUENCY_FUNCTIONS_H_

#include "stm32f4xx_hal.h"
#include "LMX2572.h"

void init_synthesizers(LMX2572_HandleTypeDef *pll1, LMX2572_HandleTypeDef *pll2); //TODO most likely pass handles to pll1 and pll2
void process_synthesizers(LMX2572_HandleTypeDef *pll1, LMX2572_HandleTypeDef *pll2);

#endif /* INC_FREQUENCY_FUNCTIONS_H_ */
