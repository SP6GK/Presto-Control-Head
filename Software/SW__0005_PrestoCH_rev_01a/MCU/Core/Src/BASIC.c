/*
 * BASIC.c
 *
 *	Basic Automated SCPI Interpreter for Controllers
 *	Processes SCPI like commands
 *
 *  Created on: Oct 12, 2025
 *      Author: sp6gk
 */

#include "BASIC.h"
#include "usart.h"
#include "24LC_EEPROM.h"
#include "globals.h"
#include "calibration.h"

#include "helper_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inttypes.h>

//Boot up message
uint32_t BASIC_Init(void){
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)uart_splash, strlen((char *)uart_splash));
	static char msg[512];
	osDelay(100);
	int len = snprintf(msg, sizeof(msg),
	         "HF<->QO-100 transverter by SP6GK\r\n"
	         "Serial Number: %d\r\n"
	         "SW version: %d.%c\r\n"
	         "MB rev: %d.%c\r\n"
	         "CH rev: %d.%c\r\n"
	         "PLL1 rev: %d.%c\r\n"
	         "PLL2 rev: %d.%c\r\n"
	         "POST status: %d\r\n\n",
	         MB_NV.Serial_NoA,
	         SW_MAJ_VER, SW_MIN_Ver,
	         MB_NV.HW_major_rev, MB_NV.HW_minor_rev,
	         CH_NV.HW_major_rev, CH_NV.HW_minor_rev,
	         PLL1_NV.HW_major_rev, PLL1_NV.HW_minor_rev,
	         PLL2_NV.HW_major_rev, PLL2_NV.HW_minor_rev,
	         POST_Status);

	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)msg, len);
	osDelay(100);
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_UART_Transmit_DMA(&huart1, (uint8_t*)RDY, strlen((char *)RDY));

	if(status != HAL_OK){
		error_code = 49;
	}

	return status;
}

uint32_t BASIC_cmd(uint8_t *cmd, uint16_t length){
	//Syntax of command root:key1:key2:value
	//root - required. key1 - required unless root is a special command (ex. *IDN?), key2 - optional (value = key2 if key2 is None)
	HAL_UART_Transmit(&huart1, cmd, length, 100); //Echo back the command debug
	char local_buffer[UART_RX_BUFFER_SIZE] = {0};
	char tx_msg[64] = {0};
	//char root[4] = {0};
	//char key1[4] = {0};
	//char key2[4] = {0};
	char current_char;
	uint8_t root_len = 0;
	uint8_t key1_len = 0;
	uint8_t key2_len = 0;
	uint8_t value_len = 0;
	uint16_t words_count = 0;

	if(length > sizeof(local_buffer)){
		length = sizeof(local_buffer);
	}

	memcpy(local_buffer, cmd, length);

	//Get length of root
	for(uint8_t i = 0; (i < length) && (root_len == 0); i++){
		current_char = local_buffer[i];
		if((current_char != ':') && (current_char != '?')){		//Build the root
			//root[i] = current_char;
		}
		else if((current_char == ':') || (current_char == '?')){	//End of root
			root_len = i;
			words_count++;
			if(current_char == '?'){
				root_len ++;
			}
		}
	}

	//Get length of key1
	for(uint8_t i = root_len+1; (i < length) && (key1_len == 0); i++){
		current_char = local_buffer[i];
		if((current_char != ':') && (current_char != '?')){

		}
		else if((current_char == ':') || (current_char == '?')){
			key1_len = (i - root_len) -1;
			words_count++;
			if(current_char == '?'){
				key1_len ++;
			}
		}
	}

	//Get length of key2 (key 2 is optional and might return 0)
	for(uint8_t i = (root_len + key1_len)+2; (i < length) && (key2_len == 0); i++){
		current_char = local_buffer[i];
		if((current_char != ':') && (current_char != '?')){		//Not sure why I put && here, in tests check statements with '?' at the end

		}
		else if((current_char == ':') || (current_char == '?')){
			key2_len = i - (root_len + key1_len) -2;
			words_count++;
			if(current_char == '?'){
				key1_len ++;
			}
		}
	}

	//Get value_len if key2 is empty (value follows key1)	TODO, this might be unnecessary since if key2_len is 0 then evaluation of this statement is the same as the next  one?
	if(key2_len == 0){
		for(uint8_t i = (root_len + key1_len)+2; ((i < length) && (value_len == 0)); i++){
			current_char = local_buffer[i];
			if((current_char != '\n')){

			}
			else if(current_char == '\n'){
				value_len = i - (root_len + key1_len) -2;
				words_count++;
			}
		}
	}
	else if(key2_len !=0){
		//Get value_len if key2 exists
		for(uint8_t i = (root_len + key1_len +key2_len)+2; ((i < length) && (value_len == 0)); i++){
			current_char = local_buffer[i];
			if((current_char != '\n')){

			}
			else if(current_char == '\n'){
				value_len = i - (root_len + key1_len + key2_len) -2;
				words_count++;
			}
		}
	}


	//Allocate on run time arrays for parts of command
	//TODO, allocate key1 and key2 only if they are present in command (*special commands or some subsystems might not require them)
	char *root = malloc(root_len * sizeof(char) +1); //+1 to size for null terminator
	if((root == NULL)){
		//Malloc failed, root is always present in command syntax
		return 1;
	}
	memcpy(root, local_buffer, root_len);
	root[root_len] = '\0';

	char *key1 = malloc(key1_len * sizeof(char) +1);
	if(key1_len != 0){
		if(key1 == NULL){
			return 1;
		}
		memcpy(key1, local_buffer+root_len+1, key1_len);
		key1[key1_len] = '\0';
	}

	char *key2 = malloc(key2_len * sizeof(char) +1);
	if(key2_len != 0){
		if(key2 == NULL){
			return 1;
		}
		memcpy(key2, local_buffer+root_len+key1_len+2, key2_len);
		key2[key2_len] = '\0';
	}

	char *value = malloc(value_len * sizeof(char) +1);
	if(value_len != 0){
		if(value == NULL){
			return 1;
		}
		memcpy(value, local_buffer+root_len+key1_len+key2_len+words_count-1, value_len);
		value[value_len] = '\0';
	}


	char *endptr;
	//Conditionals to select action based on command			//TODO, small fix to commands ending with ? for key 1 and key2, might require if key1=='?' key1_len++ (see root).
	if(strcmp(root, "*IDN?") == 0){
		snprintf(tx_msg, sizeof(tx_msg), "PRESTO TRANSVERTER SN: %d, SW: %d.%c\r\n", MB_NV.Serial_NoA, SW_MAJ_VER, SW_MIN_Ver);
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
	}
	else if(strcmp(root, "*RST?") == 0){
		snprintf(tx_msg, sizeof(tx_msg), "RST\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg), 100);
		HAL_NVIC_SystemReset();
	}


	else if(strcmp(root, "LNB") == 0){
		if(strcmp(key1, "SET?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "LNB:SET=%d \n", LNB_setting);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		if(strcmp(key1, "SET") == 0){
			if(strcmp(value, "0") == 0){
				LNB_setting = 0;
			}
			else if(strcmp(value, "1") == 0){
				LNB_setting = 1;
			}
			else if(strcmp(value, "2") == 0){
				LNB_setting = 2;
			}

			snprintf(tx_msg, sizeof(tx_msg), "LNB:SET=%d\n", LNB_setting);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "VOLT?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "LNB:VOLT=%f\n", LNB_V);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "FLT?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "LNB:FLT=%d\n", LNB_FLT);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "LNA?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "LNB:LNA=%d\n", RX_LNA_EN);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "LNA") == 0){
			if(strcmp(value, "0") == 0){
				RX_LNA_EN = 0;
			}
			else if(strcmp(value, "1") == 0){
				RX_LNA_EN = 1;
			}
			snprintf(tx_msg, sizeof(tx_msg), "LNB:LNA=%d\n", RX_LNA_EN);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
	}
	else if(strcmp(root, "PSU") == 0){
		if(strcmp(key1, "5V5?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "PSU:5V5=%f\n", PSU_5V5);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "3V6?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "PSU:3V6=%f\n", PSU_3V6);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "VIN?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "PSU:VIN=%f\n", PSU_Vin);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "IMON?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "PSU:IMON=%f\n", Imon_I);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
	}

	else if(strcmp(root, "AMP") == 0){
		if(strcmp(key1, "TX?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "AMP:TX=%d\n", tx_status);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}

		else if(strcmp(key1, "IF?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "AMP:IF=%d\n", IF_LNA_EN);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}

		else if(strcmp(key1, "TX") == 0){
			Remote_Mode = 1;
			if(strcmp(value, "0") == 0){
				tx_status = 0;
				GPIO_EXP_INT = true;
			}
			else if(strcmp(value, "1") == 0){
				tx_status = 1;
			}

			snprintf(tx_msg, sizeof(tx_msg), "AMP:TX=%d\n", tx_status);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}

		else if(strcmp(key1, "IF") == 0){
			Remote_Mode = 1;
			if(strcmp(value, "0") == 0){
				IF_LNA_EN = 0;
			}
			else if(strcmp(value, "1") == 0){
				IF_LNA_EN = 1;
			}

			snprintf(tx_msg, sizeof(tx_msg), "AMP:IF=%d\n", IF_LNA_EN);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}

	}

	else if(strcmp(root, "ATT") == 0){
		if(strcmp(key1, "LNB?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "ATT:LNB=%f\n", attenuation_dB_LNB);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "TRX?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "ATT:TRX=%f\n", attenuation_dB_TRX);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}

		else if(strcmp(key1, "LNB") == 0){
			attenuation_dB_LNB = strtoul(value, &endptr, 10);
		}
		else if(strcmp(key1, "TRX") == 0){
			attenuation_dB_LNB = strtoul(value, &endptr, 10);
		}


	}

	else if(strcmp(root, "IO") == 0){
		if(strcmp(key1, "PTT?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "IO:PTT=%d\n", tx_status);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "OUT1?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "IO:OUT1=%d\n", out1_status);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "OUT2?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "IO:OUT2=%d\n", out2_status);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}

		else if(strcmp(key1, "OUT1") == 0){
			out1_status = strtoul(value, &endptr, 10);
			snprintf(tx_msg, sizeof(tx_msg), "IO:OUT2=%d\n", out1_status);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		else if(strcmp(key1, "OUT2") == 0){
			out2_status = strtoul(value, &endptr, 10);
			snprintf(tx_msg, sizeof(tx_msg), "IO:OUT2=%d\n", out2_status);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}


	}


	else if(strcmp(root, "TEMP") == 0){
		if(strcmp(key1, "TX?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "TEMP:TX=%f\n", temp_amp);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		if(strcmp(key1, "MB?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "TEMP:MB=%f\n", temp_mb);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}
		if(strcmp(key1, "MCU?") == 0){
			snprintf(tx_msg, sizeof(tx_msg), "TEMP:MCU=%f\n", temp_mcu);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg));
		}

	}

	else if(strcmp(root, "VFO") == 0){
		 if(strcmp(key1, "UP?") == 0){
			uint32_t high = (uint32_t)(UP_Link_F / 1000000000ULL);
			uint32_t low  = (uint32_t)(UP_Link_F % 1000000000ULL);

			if (high > 0) {
			    snprintf(tx_msg, sizeof(tx_msg), "VFO:UP=%lu%09lu\n", high, low);
			} else {
			    snprintf(tx_msg, sizeof(tx_msg), "VFO:UP=%lu\n", low);
			}
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen(tx_msg));
		}
		else if(strcmp(key1, "DOWN?") == 0){
			uint32_t high = (uint32_t)(DOWN_Link_F / 1000000000ULL);
			uint32_t low  = (uint32_t)(DOWN_Link_F % 1000000000ULL);

			if (high > 0) {
			    snprintf(tx_msg, sizeof(tx_msg), "VFO:DOWN=%lu%09lu\n", high, low);
			} else {
			    snprintf(tx_msg, sizeof(tx_msg), "VFO:DOWN=%lu\n", low);
			}
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen(tx_msg));
		}
		else if(strcmp(key1, "TRX?") == 0){
			uint32_t high = (uint32_t)(TRX_F / 1000000000ULL);
			uint32_t low  = (uint32_t)(TRX_F % 1000000000ULL);

			if (high > 0) {
			    snprintf(tx_msg, sizeof(tx_msg), "VFO:TRX=%lu%09lu\n", high, low);
			} else {
			    snprintf(tx_msg, sizeof(tx_msg), "VFO:TRX=%lu\n", low);
			}

			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_msg, strlen(tx_msg));
		}
		 //Input sanitation might be beneficial here
		else if(strcmp(key1, "UP") == 0){
			UP_Link_F = strtoul(value, &endptr, 10);
		}
		else if(strcmp(key1, "DOWN") == 0){
			DOWN_Link_F = strtoul(value, &endptr, 10);
		}
		else if(strcmp(key1, "TRX") == 0){
			TRX_F = strtoul(value, &endptr, 10);
		}

	}

	//Enable service mode
	else if(strcmp(root, "SRV") == 0){
		if(strcmp(key1, "ACT") == 0){
			if(strcmp(value, "123") == 0){
				SRV_Mode = 1;
				snprintf(tx_msg, sizeof(tx_msg), "SRV=1\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg), 100);
			}
		}
		else if(strcmp(key1, "DIS") == 0){
			SRV_Mode = 0;
			snprintf(tx_msg, sizeof(tx_msg), "SRV=0\r\n");
			HAL_UART_Transmit(&huart1, (uint8_t *)tx_msg, strlen((char *)tx_msg), 100);
		}
	}

	else if((SRV_Mode == 1) && (strcmp(root, "PEEK")) == 0){	//Memory subsystem
		if(strcmp(key1, "MB") == 0){
			PEEK(MB_EEPROM_ADDR, strtoul(value, &endptr, 10));
				//PEEK_MB(strtoul(value, &endptr, 10));	//endptr == str -> No numbers found
		}
		else if(strcmp(key1, "CH") == 0){
			PEEK(CH_EEPROM_ADDR, strtoul(value, &endptr, 10));
		}
		else if(strcmp(key1, "LO1") == 0){
			PEEK(PLL1_EEPROM_ADDR, strtoul(value, &endptr, 10));
		}
		else if(strcmp(key1, "LO2") == 0){
			PEEK(PLL2_EEPROM_ADDR, strtoul(value, &endptr, 10));
		}

	}

	else if((SRV_Mode == 1) && (strcmp(root, "POKE") == 0)){
		if(strcmp(key1, "MB") == 0){
			POKE(MB_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoul(value, &endptr, 10));
		}
		else if(strcmp(key1, "CH") == 0){
			POKE(CH_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoul(value, &endptr, 10));
		}
		else if(strcmp(key1, "LO1") == 0){
			POKE(PLL1_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoul(value, &endptr, 10));
		}
		else if(strcmp(key1, "LO2") == 0){
			POKE(PLL2_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoul(value, &endptr, 10));
		}
	}
	else if((SRV_Mode == 1) && (strcmp(root, "POKE32") == 0)){
		if(strcmp(key1, "MB") == 0){
			POKE32(MB_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoul(value, &endptr, 10));
		}
		else if(strcmp(key1, "CH") == 0){
			POKE32(CH_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoul(value, &endptr, 10));
		}
		else if(strcmp(key1, "LO1") == 0){
			POKE32(PLL1_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoul(value, &endptr, 10));
		}
		else if(strcmp(key1, "LO2") == 0){
			POKE32(PLL2_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoul(value, &endptr, 10));
		}
	}

	else if((SRV_Mode == 1) && (strcmp(root, "POKE64")) == 0){
		if(strcmp(key1, "MB") == 0){
			POKE64(MB_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoull(value, &endptr, 10));
		}
		else if(strcmp(key1, "CH") == 0){
			POKE64(CH_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoull(value, &endptr, 10));
		}
		else if(strcmp(key1, "LO1") == 0){
			POKE64(PLL1_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoull(value, &endptr, 10));
		}
		else if(strcmp(key1, "LO2") == 0){
			POKE64(PLL2_EEPROM_ADDR, strtoul(key2, &endptr, 10), strtoull(value, &endptr, 10));
		}
	}
	else if((SRV_Mode == 1) && (strcmp(root, "MEM") == 0)){	//MEM:SCAN:?
		if(strcmp(key1, "SCAN?") == 0){
			I2C_Scan();
		}
	}

	//Calibration
	else if((SRV_Mode == 1) && (strcmp(root, "CAL") == 0)){
		uint16_t data = 0;
		uint32_t crc = 0;
		HAL_StatusTypeDef status = HAL_OK;
		if(strcmp(key1, "LNB") == 0){
			if(strcmp(key2, "A") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				LNB_V_Calibration = map_int_to_float(data, 0, UINT16_MAX, LNB_V_CAL_MIN, LNB_V_CAL_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_LNBV_sense_calA_addr, data);
				MB_NV.LNBV_sense_calA = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);


			}
			if(strcmp(key2, "B") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				LNB_V_Calibration_offset = map_int_to_float(data, 0, UINT16_MAX, LNB_V_CAL_OFFSET_MIN, LNB_V_CAL_OFFSET_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_LNBV_sense_calB_addr, data);
				MB_NV.LNBV_sense_calB = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}
		}
		if(strcmp(key1, "PSU") == 0){
			if(strcmp(key2, "IN") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				PSU_Vin_Calibration = map_int_to_float(data, 0, UINT16_MAX, PSU_VIN_CAL_MIN, PSU_VIN_CAL_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_Vin_sense_calA_addr, data);
				MB_NV.Vin_sense_calA = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}
			if(strcmp(key2, "INOFFSET") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				PSU_Vin_Calibration_offset = map_int_to_float(data, 0, UINT16_MAX, PSU_VIN_CAL_OFFSET_MIN, PSU_VIN_CAL_OFFSET_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_Vin_sense_calB_addr, data);
				MB_NV.Vin_sense_calB = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}
			if(strcmp(key2, "3V6") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				PSU_3V6_Calibration = map_int_to_float(data, 0, UINT16_MAX, P3V6_CALA_MIN, P3V6_CALA_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_P3V6_sense_calA_addr, data);
				MB_NV.P3V6_sense_calA = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}
			if(strcmp(key2, "5V5") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				PSU_5V5_Calibration = map_int_to_float(data, 0, UINT16_MAX, P5V5_CALA_MIN, P5V5_CALA_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_P5v5_sense_calA_addr, data);
				MB_NV.P5v5_sense_calA = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}
			if(strcmp(key2, "IMON") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				Imon_Calibration = map_int_to_float(data, 0, UINT16_MAX, IMON_CAL_MIN, IMON_CAL_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_Imon_calA_addr, data);
				MB_NV.Imon_calA = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}
			if(strcmp(key2, "IMONOFFSET") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				Imon_Calibration_offset = map_int_to_float(data, 0, UINT16_MAX, IMON_CAL_OFFSET_MIN, IMON_CAL_OFFSET_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_Imon_calB_addr, data);
				MB_NV.Imon_calB = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}
		}
		if(strcmp(key1, "RF") == 0){
			if(strcmp(key2, "LO") == 0){
				DAC_VCTCXO = (uint16_t)strtoul(value, &endptr, 10);
				POKE(PLL1_EEPROM_ADDR, PLL_NV_VCTCXO_cal_addr, DAC_VCTCXO);
				PLL1_NV.VCTCXO_cal = data;
				crc = EEPROM_Calculate_CRC(PLL1_EEPROM_ADDR);
				status = POKE32(PLL1_EEPROM_ADDR, PLL_NV_CRC_addr, crc);
			}
			if(strcmp(key2, "COUPLER") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				sampler_output_cal = map_int_to_float(data, 0, UINT16_MAX, SAMPLER_CAL_MIN, SAMPLER_CAL_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_Sampler_Cal_addr, data);
				MB_NV.Sampler_Cal = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}

			//Detector calibration commands
			if(strcmp(key2, "DETRXA") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				RF_det_rx_calA = map_int_to_float(data, 0, UINT16_MAX, RF_DET_RX_CAL_A_MIN, RF_DET_RX_CAL_A_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_RF_Det_RX_calA_addr, data);
				MB_NV.RF_Det_RX_calA = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}

			if(strcmp(key2, "DETRXB") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				RF_det_rx_calB = map_int_to_float(data, 0, UINT16_MAX, RF_DET_RX_CAL_B_MIN, RF_DET_RX_CAL_B_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_RF_Det_RX_calB_addr, data);
				MB_NV.RF_Det_RX_calB = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}

			if(strcmp(key2, "DETRXC") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				RF_det_rx_calC = map_int_to_float(data, 0, UINT16_MAX, RF_DET_RX_CAL_C_MIN, RF_DET_RX_CAL_C_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_RF_Det_RX_calC_addr, data);
				MB_NV.RF_Det_RX_calC = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}

			if(strcmp(key2, "DETTXA") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				RF_det_tx_calA = map_int_to_float(data, 0, UINT16_MAX, RF_DET_TX_CAL_A_MIN, RF_DET_TX_CAL_A_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_RF_Det_TX_calA_addr, data);
				MB_NV.RF_Det_TX_calA = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}

			if(strcmp(key2, "DETTXB") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				RF_det_tx_calB = map_int_to_float(data, 0, UINT16_MAX, RF_DET_TX_CAL_B_MIN, RF_DET_TX_CAL_B_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_RF_Det_TX_calB_addr, data);
				MB_NV.RF_Det_TX_calB = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}

			if(strcmp(key2, "DETTXC") == 0){
				data = (uint16_t)strtoul(value, &endptr, 10);
				RF_det_tx_calC = map_int_to_float(data, 0, UINT16_MAX, RF_DET_TX_CAL_C_MIN, RF_DET_TX_CAL_C_MAX);
				POKE(MB_EEPROM_ADDR, MB_NV_RF_Det_TX_calC_addr, data);
				MB_NV.RF_Det_TX_calC = data;
				crc = EEPROM_Calculate_CRC(MB_EEPROM_ADDR);
				status = POKE32(MB_EEPROM_ADDR, MB_NV_CRC_addr, crc);
			}



		}


		if(status != HAL_OK){
			//POKE has failed while writing CRC
			error_code = 48;
		}
	}

	memset(local_buffer, 0, sizeof(local_buffer));
	//Remember to free only the elements that were allocated!
	if(root != NULL){
		free(root);
	}
	if(key1 != NULL){
		free(key1);
	}
	if(key2 != NULL){
		free(key2);
	}
	if(value != NULL){
		free(value);
	}


	return 0;
}
