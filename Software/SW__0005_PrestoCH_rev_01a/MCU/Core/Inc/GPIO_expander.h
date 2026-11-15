/*
 * GPIO_expander.h
 *
 *  Created on: Dec 16, 2025
 *      Author: sp6gk
 */

#ifndef INC_GPIO_EXPANDER_H_
#define INC_GPIO_EXPANDER_H_

//Pin names definitions
#define TX_EN_PIN   0
#define LNB_FLT_PIN 1
#define LNB_POL_PIN 2
#define LNB_EN_PIN  3
#define PA_EN_PIN   4
#define PTT_PIN		5
#define OUT1_PIN    6
#define OUT2_PIN    7



#include "stdbool.h"
#include "TCA9554.h"

void Set_TX_Relay(TCA9554_HandleTypeDef *dev, bool status);
void Set_PA_EN(TCA9554_HandleTypeDef *dev, bool status);
void Set_LNB_Pol(TCA9554_HandleTypeDef *dev, bool status);
void Set_LNB_EN(TCA9554_HandleTypeDef *dev, bool status);
void Set_OUT1(TCA9554_HandleTypeDef *dev, bool status);
void Set_OUT2(TCA9554_HandleTypeDef *dev, bool status);

bool Get_LNB_FLT(TCA9554_HandleTypeDef *dev);
bool Get_PTT_State(TCA9554_HandleTypeDef *dev);


void process_GPIO_expander(TCA9554_HandleTypeDef *dev);

#endif /* INC_GPIO_EXPANDER_H_ */
