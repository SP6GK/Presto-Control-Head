/*
 * frequency_functions.c
 *
 *  Created on: Dec 27, 2025
 *      Author: sp6gk
 */

#include "frequency_functions.h"
#include "main.h"
#include "spi.h"
#include "globals.h"
#include "LMX2572.h"

void init_synthesizers(LMX2572_HandleTypeDef *pll1, LMX2572_HandleTypeDef *pll2){
	HAL_GPIO_WritePin(LO1_CE_GPIO_Port, LO1_CE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LO2_CE_GPIO_Port, LO2_CE_Pin, GPIO_PIN_SET);
	osDelay(40);

	LMX2572_init(pll1);
	LMX2572_init(pll2);

	LMX2572_SetRFLvlB(pll1, 10); //Has additional LO amp to drive 17 dBm mixer, this is enough
	LMX2572_SetRFLvlB(pll2, 18); //Max output is 18, drives smaller 13 dBm LO mixer

	LMX2572_SetOutB(pll1, true);
	LMX2572_SetOutB(pll2, true);

	pll1->ld_flag = HAL_GPIO_ReadPin(LO1_LD_GPIO_Port, LO1_LD_Pin);
	pll2->ld_flag = HAL_GPIO_ReadPin(LO2_LD_GPIO_Port, LO2_LD_Pin);

	if(pll1->ld_flag != 1){
		//Error handling
	}
	if(pll2->ld_flag !=1){
		//Error handling
	}


}

void process_synthesizers(LMX2572_HandleTypeDef *pll1, LMX2572_HandleTypeDef *pll2){
	static uint64_t last_upLinkF = 0;
	static uint64_t last_DwLinkF = 0;
	static uint64_t last_TRXF = 0;
	static uint64_t last_LNB_LO = 0;

	static uint8_t last_tx_status = 0;

	uint64_t LO2_F = 0;
	uint64_t LO1_F = 0;

	if(tx_status == 1){
		if((last_upLinkF != UP_Link_F) || (last_TRXF != TRX_F) || (last_tx_status != tx_status)){
			LMX2572_SetOutB(pll2, true);
			LO2_F = UP_Link_F - IF;
			LMX2572_SetFrequency(pll2, LO2_F);


			last_upLinkF = UP_Link_F;

			LO1_F = IF - TRX_F; //Adjust 1st IF
			LMX2572_SetFrequency(pll1, LO1_F);
			last_TRXF = TRX_F;
			last_tx_status = tx_status;
		}



	}
	else if(tx_status == 0){	//Frequency settings in RX mode
		if((last_DwLinkF != DOWN_Link_F) || (last_TRXF != TRX_F) || (last_LNB_LO != LNB_LO) || (last_tx_status != tx_status)){
			LMX2572_SetOutB(pll2, false);
			LO1_F = (DOWN_Link_F - LNB_LO) + TRX_F; //Downconvert the LNB output to TRX F, note: in brackets the real input frequency is given, high side conversion is used
			LMX2572_SetFrequency(pll1, LO1_F);
			last_DwLinkF = DOWN_Link_F;
			last_TRXF = TRX_F;
			last_LNB_LO = LNB_LO;
			last_tx_status = tx_status;

		}
	}

	pll1->ld_flag = HAL_GPIO_ReadPin(LO1_LD_GPIO_Port, LO1_LD_Pin);
	pll2->ld_flag = HAL_GPIO_ReadPin(LO2_LD_GPIO_Port, LO2_LD_Pin);

	if(pll1->ld_flag != 1){
		//Error handling
	}
	if(pll2->ld_flag !=1){
		//Error handling
	}


}
