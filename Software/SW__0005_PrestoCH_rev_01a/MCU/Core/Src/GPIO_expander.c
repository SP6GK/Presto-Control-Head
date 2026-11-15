/*
 * GPIO_expander.c
 *
 *  Created on: Dec 16, 2025
 *      Author: sp6gk
 *
 *      Abstraction layer for control of GPIO expander
 */

#include "GPIO_expander.h"
#include "globals.h"



//Set state of expander's outputs
void Set_TX_Relay(TCA9554_HandleTypeDef *dev, bool status){
	TCA9554_set_pin(dev, TX_EN_PIN, status);
}
void Set_PA_EN(TCA9554_HandleTypeDef *dev, bool status){
	TCA9554_set_pin(dev, PA_EN_PIN, status);
}
void Set_LNB_Pol(TCA9554_HandleTypeDef *dev, bool status){
	TCA9554_set_pin(dev, LNB_POL_PIN, status);
}
void Set_LNB_EN(TCA9554_HandleTypeDef *dev, bool status){
	TCA9554_set_pin(dev, LNB_EN_PIN, status);
}
void Set_OUT1(TCA9554_HandleTypeDef *dev, bool status){
	TCA9554_set_pin(dev, OUT1_PIN, status);
}

void Set_OUT2(TCA9554_HandleTypeDef *dev, bool status){
	TCA9554_set_pin(dev, OUT2_PIN, status);
}

//Read state of expander's inputs
bool Get_LNB_FLT(TCA9554_HandleTypeDef *dev){
 return TCA9554_read_pin(dev, LNB_FLT_PIN);
}

bool Get_PTT_State(TCA9554_HandleTypeDef *dev){
	return TCA9554_read_pin(dev, PTT_PIN);
}


void process_GPIO_expander(TCA9554_HandleTypeDef *dev){
	static bool last_tx_status = 0;
	static uint8_t last_LNB_setting = 0;
	static bool last_out1_status = 0;
	static bool last_out2_status = 0;


	if((last_tx_status != tx_status)){
			Set_TX_Relay(dev, tx_status);
			osDelay(5);
			Set_PA_EN(dev, tx_status);
		last_tx_status = tx_status;
	}

	if(last_out1_status != out1_status){
		TCA9554_set_pin(dev, OUT1_PIN, out1_status);
		last_out1_status = out1_status;
	}

	if(last_out2_status != out2_status){
		TCA9554_set_pin(dev, OUT2_PIN, out2_status);
		last_out2_status = out2_status;
	}


	if((last_LNB_setting != LNB_setting)){
		if(LNB_setting > 0){
			Set_LNB_EN(dev, GPIO_PIN_SET);
			if(LNB_setting == 1){
				Set_LNB_Pol(dev, GPIO_PIN_SET);
			}
			else{
				Set_LNB_Pol(dev, GPIO_PIN_RESET);
			}
		}
		else{
			Set_LNB_EN(dev, GPIO_PIN_RESET);
		}

		last_LNB_setting = LNB_setting;
	}
}
