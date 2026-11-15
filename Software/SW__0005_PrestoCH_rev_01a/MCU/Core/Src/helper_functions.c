/*
 * helper_functions.c
 *
 *  Created on: Sep 14, 2025
 *      Author: sp6gk
 */

#include "main.h"
//#include "cmsis_os.h"
//#include "dma.h"
#include "spi.h"
#include "gpio.h"

#include "F1956.h"

#include "stdbool.h"
#include "globals.h"
#include "NV_data.h"
#include "calibration.h"

#include "ili9341_font.h"
#include "ili9341_gfx.h"
#include "ili9341.h"
#include "helper_functions.h"
#include "24LC_EEPROM.h"

#include "math.h"
#include "stdlib.h"
#include <stdio.h>

#include "TCA9554.h"

#include "GPIO_expander.h"
#include "error_handler.h"


uint8_t HW_Init_POST(TCA9554_HandleTypeDef *GPIO_expander, uint8_t phase){
	//Steps 1-3 of POST
	osDelay(100);

	switch(phase){
		case 1:				//Initial power test
			osDelay(1000);
			PSU_FLT = HAL_GPIO_ReadPin(PSU_FLT_GPIO_Port, PSU_FLT_Pin);
			LNB_FLT = TCA9554_read_pin(GPIO_expander, LNB_FLT_PIN);
			PSU_5V5_FLT = HAL_GPIO_ReadPin(FLT_5V5_GPIO_Port, FLT_5V5_Pin);

			if(PSU_FLT == GPIO_PIN_RESET){
				//PSU fault
				error_code = 1;

			}
			if(LNB_FLT == GPIO_PIN_RESET){
				//LNB fault
				error_code = 2;
			}
			if(PSU_5V5_FLT == GPIO_PIN_RESET){
				//5V5 fault
				error_code = 3;
			}

			POST_Status ++;

			osDelay(500);
			if(PSU_Vin >= MAX_VIN){
				//Too high input voltage error handling
				error_code = 4;
			}
			if(PSU_Vin <= MIN_VIN){
				//Too low input voltage
				error_code = 5;
			}
			POST_Status ++;

			osDelay(500);
			if((PSU_3V6 < MIN_3V6) || (PSU_3V6 > MAX_3V6)){
				//Fail on 3V6 line
				error_code = 6;
			}
			if((PSU_5V5 < MIN_5V5) || (PSU_5V5 > MAX_5V5)){
				//Fail on 5V5 line
				error_code = 7;
			}
			if(Imon_I >= MAX_IIN){
				//Too high idle current
				error_code = 8;
			}
			POST_Status ++;
			break;


		case 2:
			osDelay(100);
			uint8_t i2c_devices = 0;
			uint8_t scan_addr = 0;
			  for(uint8_t i = 0; i < 128; i++){
				  scan_addr = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 3, 10000);
				  if(scan_addr != HAL_OK){
					  //No ACK received at the address
				  }
				  else if(scan_addr == HAL_OK){
					  i2c_devices++;
				  }
			  }

			if(i2c_devices <= 7){
				//Cannot communicate with I2C devices
				error_code = 24;
			}
			POST_Status ++;
			osDelay(100);

			if(CRC_CH_Read_fail == true){
				//Failed CRC for control head
				error_code = 9;
			}
			osDelay(100);
			POST_Status ++;
			break;

		case 3:
			osDelay(100);
			POST_Status ++;
			if(CRC_MB_Read_fail == true){
				//Failed CRC for mother board
				error_code = 10;
			}
			break;

		case 4:
			osDelay(100);
			POST_Status ++;
			if(CRC_LO1_Read_fail == true){
				//Failed CRC for local oscillator 1
				error_code = 11;
			}
			break;
			osDelay(100);
			POST_Status ++;

		case 5:
			osDelay(100);
			POST_Status ++;
			if(CRC_LO2_Read_fail == true){
				//Failed CRC for local oscillator 2
				error_code = 12;
			}
			break;
			osDelay(100);
			POST_Status ++;

		case 6:
			osDelay(100);
			POST_Status ++;

			if((HAL_GPIO_ReadPin(LO1_LD_GPIO_Port, LO1_LD_Pin)) != GPIO_PIN_SET){
				//LO1 not locked
				error_code = 13;
			}
			osDelay(100);
			POST_Status ++;
			if((HAL_GPIO_ReadPin(LO2_LD_GPIO_Port, LO2_LD_Pin)) != GPIO_PIN_SET){
				//LO2 not locked
				error_code = 14;
			}
			osDelay(100);
			POST_Status ++;
			break;

		case 7:
			//Enable /  disable TX check if current changes
			float Imon_idle = Imon_I;
			float Imon_tx = 0;

			Set_PA_EN(GPIO_expander, 1);
			Set_TX_Relay(GPIO_expander, 1);
			osDelay(1000);

			Imon_tx = Imon_I;
			if((Imon_tx) < (Imon_idle+0.025f)){
				//TX current too low
				error_code = 15;
			}
			osDelay(100);
			POST_Status ++;
			if((Imon_tx) > (Imon_idle + 0.5f)){
				//TX current too high
				error_code = 16;
			}
			osDelay(100);
			POST_Status ++;

			Set_PA_EN(GPIO_expander, 0);
			Set_TX_Relay(GPIO_expander, 0);
			POST_Status++;
			break;

		case 8:
 			Set_LNB_EN(GPIO_expander, 0);
			osDelay(100);
			float LNB_Volt_tmp = LNB_V;
			if(LNB_Volt_tmp > 5.0f){
				//LNB Does not turn off
				error_code = 17;
			}

			Set_LNB_EN(GPIO_expander, 1);
			Set_LNB_Pol(GPIO_expander, 0);
			osDelay(1000);
			LNB_Volt_tmp = LNB_V;
			if((LNB_Volt_tmp < LNBV_H_MIN) || (LNB_Volt_tmp > LNBV_H_MAX)){
				// Horizontal polarization voltage incorrect
				error_code = 18;
			}
			if(LNB_FLT != GPIO_PIN_SET){
				//LNB fault
				error_code = 19;
			}
			Set_LNB_EN(GPIO_expander, 1);
			Set_LNB_Pol(GPIO_expander, 1);
			POST_Status++;
			osDelay(1000);
			LNB_Volt_tmp = LNB_V;
			if((LNB_Volt_tmp < LNBV_V_MIN) || (LNB_Volt_tmp > LNBV_V_MAX)){
				// Vertical polarization voltage incorrect
				error_code = 20;
			}
			if(LNB_FLT != GPIO_PIN_SET){
				//LNB fault
				error_code = 19;
			}
			Set_LNB_EN(GPIO_expander, 0);
			Set_LNB_Pol(GPIO_expander, 0);

			POST_Status++;
			osDelay(1000);
			break;

		case 9:
			osDelay(100);
			if(temp_amp > MAX_TEMP_AMP){
				error_code = 21;
			}
			if(temp_mcu > MAX_TEMP_MCU){
				error_code = 22;
			}
			if(temp_mb > MAX_TEMP_MB){
				error_code = 23;
			}
			POST_Status++;

		case 10:

		    HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_SET);
		    HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_SET);
		    HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_SET);
		    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
		    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);	//Bottom button LED
		    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);	//Top button LED
		    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);	//2nd button LED
		    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);	//3rd button LED

		    osDelay(1000);

		    HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_RESET);
		    HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_RESET);
		    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
		    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);	//Bottom button LED
		    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);	//Top button LED
		    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);	//2nd button LED
		    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);	//3rd button LED
		    POST_Status++;
		    break;

	}


	return POST_Status;
}

float adcValueToVoltage(uint16_t adcValue) {
    //Assuming a 3.3V reference voltage and a 12-bit ADC
    float referenceVoltage = 3.3;
    uint16_t maxADCValue = 4095;

    float voltage = (float)adcValue / (float)maxADCValue * referenceVoltage;

    return voltage;
}

//BIT, returns error codes when abnormal condition is detected
uint8_t Built_In_Test(void){
	PSU_FLT = HAL_GPIO_ReadPin(PSU_FLT_GPIO_Port, PSU_FLT_Pin);
	PSU_5V5_FLT = HAL_GPIO_ReadPin(FLT_5V5_GPIO_Port, FLT_5V5_Pin);

	static uint32_t internal_counter = 0;
	static uint8_t strike_counter[NERRORS_RUNTIME] = {0};
	uint32_t strike_max = 10;

	if((PSU_Vin < MIN_VIN)){
		strike_counter[0] ++;
		if(strike_counter[0] > strike_max){
			error_code = 25;
			strike_counter[0] = 0;
		}
	}
	else if (PSU_Vin > MAX_VIN) {
		strike_counter[1] ++;
		if(strike_counter[1] > strike_max){
			error_code = 26;
			strike_counter[1] = 0;
		}
	}
	else if(PSU_5V5 < MIN_5V5){
		strike_counter[2] ++;
		if(strike_counter[2] > strike_max){
			error_code = 27;
			strike_counter[2] = 0;
		}
	}
	else if(PSU_5V5 > MAX_5V5){
		strike_counter[3] ++;
		if(strike_counter[3] > strike_max){
			error_code = 28;
			strike_counter[3] = 0;
		}
	}
	else if (PSU_3V6 < MIN_3V6) {
		strike_counter[4] ++;
		if(strike_counter[4] > strike_max){
			error_code = 29;
			strike_counter[4] = 0;
		}
	}
	else if (PSU_3V6 > MAX_3V6) {
		strike_counter[5] ++;
		if(strike_counter[5] > strike_max){
			error_code = 30;
			strike_counter[5] = 0;
		}
	}

	if(LNB_setting == 1){	//Under and Over voltage conditions for vertical
		if(LNB_V < LNBV_V_MIN){
			strike_counter[6] ++;
			if(strike_counter[6] > strike_max){
				error_code = 31;
				strike_counter[6] = 0;
			}
		}
		else if (LNB_V > LNBV_V_MAX){
			strike_counter[7] ++;
			if(strike_counter[7] > strike_max){
				error_code = 32;
				strike_counter[7] = 0;
			}
		}
	}else if (LNB_setting == 2){
		if(LNB_V < LNBV_H_MIN){
			strike_counter[8] ++;
			if(strike_counter[8] > strike_max){
				error_code = 33;
				strike_counter[8] = 0;
			}
		}
		else if (LNB_V > LNBV_H_MAX){
			strike_counter[9] ++;
			if(strike_counter[9] > strike_max){
				error_code = 34;
				strike_counter[9] = 0;
			}
		}
	}
	else if(LNB_setting == 0){
		if(LNB_V > 1.0f){
			strike_counter[10] ++;
			if(strike_counter[10] > 5*strike_max){
				error_code = 35;
				strike_counter[10] = 0;
			}
		}
	}


	if(RF_in_pwr > MAX_IN_PWR){
		strike_counter[11] ++;
		if(strike_counter[11] > strike_max){
			error_code = 36;
			strike_counter[11] = 0;
		}
	}
	if (RF_out_pwr > MAX_OUT_PWR) {
		strike_counter[12] ++;
		if(strike_counter[12] > strike_max){
			error_code = 50;
			strike_counter[12] = 0;
		}
	}

	if(Imon_I > MAX_IIN){
		strike_counter[13] ++;
		if(strike_counter[13] > strike_max){
			error_code = 37;
			strike_counter[13] = 0;
		}
	}

	if(temp_amp > MAX_TEMP_AMP){
		strike_counter[14] ++;
		if(strike_counter[14] > strike_max){
			error_code = 38;
			strike_counter[14] = 0;
		}
	}
	else if (temp_mb > MAX_TEMP_MB) {
		strike_counter[15] ++;
		if(strike_counter[15] > strike_max){
			error_code = 39;
			strike_counter[15] = 0;
		}

	}
	else if (temp_mcu > MAX_TEMP_MCU) {
		strike_counter[16] ++;
		if(strike_counter[16] > strike_max){
			error_code = 40;
			strike_counter[16] = 0;
		}
	}

	if(PSU_FLT == 0){
		strike_counter[17] ++;
		if(strike_counter[17] > strike_max){
			error_code = 41;
			strike_counter[17] = 0;
		}
	}
	else if (PSU_5V5_FLT == 0) {
		strike_counter[18] ++;
		if(strike_counter[18] > strike_max){
			error_code = 52;
			strike_counter[18] = 0;
		}
	}
	else if (LNB_FLT == 0) {
		strike_counter[19] ++;
		if(strike_counter[19] > strike_max){
			error_code = 42;
			strike_counter[19] = 0;
		}
	}

	if((HAL_GPIO_ReadPin(LO1_LD_GPIO_Port, LO1_LD_Pin)) != GPIO_PIN_SET){
		//LO1 not locked
		strike_counter[20] ++;
		if(strike_counter[20] > strike_max){
			error_code = 43;
			strike_counter[20] = 0;
		}
	}
	if((HAL_GPIO_ReadPin(LO2_LD_GPIO_Port, LO2_LD_Pin)) != GPIO_PIN_SET){
		//LO2 not locked
		strike_counter[21] ++;
		if(strike_counter[21] > strike_max){
			error_code = 43;
			strike_counter[21] = 0;
		}
	}

	if(POST_Status > POST_STEPS_SUCESS){ //Priority of POST error codes, they are not cleared, passing 12 POST steps means correct POST
		internal_counter ++;
	}

	//Clearing of run time codes
	if(internal_counter > BIT_CLEAR_COUNTER){		//At least x cycles without error, clear error status
		error_code = 0;
		//Clear strike counter
		for(uint8_t i = 0; i < NERRORS_RUNTIME; i++){
			strike_counter[i] = 0;
		}

		internal_counter = 0;
	}

	return 0;
}


uint16_t rx_screen_display_scene(ili9341_t *lcd){
	  ili9341_fill_screen(lcd, ILI9341_BLACK);
	  ili9341_draw_border(lcd, ILI9341_ORANGE);
	  //border

	  Draw_meter_bar(lcd, 150, 26, 18, 95, tx_status);

	    ili9341_text_attr_t text_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_ORANGE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 15,
			.origin_y = 5
	    };
	    //ili9341_draw_string(lcd, text_attr, "PRESTO TRANSVERTER");
	    text_attr.origin_x = 222;
	    text_attr.bg_color = ILI9341_GREEN;
	    text_attr.fg_color = ILI9341_WHITE;
	    ili9341_draw_string(lcd, text_attr, "RX");

	    text_attr.origin_y = 30;
	    text_attr.origin_x = 8;
	    text_attr.bg_color = ILI9341_BLACK;
	    text_attr.font = &ili9341_font_7x10;
	    ili9341_draw_string(lcd, text_attr, "RSSI:");

	    text_attr.origin_y = 62;
	    text_attr.origin_x = 10;
	    text_attr.font = &ili9341_font_11x18;
	    ili9341_draw_string(lcd, text_attr, "DW ");

	    text_attr.origin_y = 102;
	    text_attr.origin_x = 8;
	    ili9341_draw_string(lcd, text_attr, "UP ");

	    text_attr.origin_y = 142;
	    ili9341_draw_string(lcd, text_attr, "TRX");

	    text_attr.origin_y = 170;
	    ili9341_draw_string(lcd, text_attr, "GPS:");

	    text_attr.origin_y = 190;
	    ili9341_draw_string(lcd, text_attr, "LNB:");

	    text_attr.origin_y = 210;
	    ili9341_draw_string(lcd, text_attr, "LNA:");

	    text_attr.origin_x = 145;
	    text_attr.origin_y = 190;
	    ili9341_draw_string(lcd, text_attr, "Vin:");

	    text_attr.origin_y = 210;
	    ili9341_draw_string(lcd, text_attr, "Tem:");

	return 0;
}

uint16_t tx_screen_display_scene(ili9341_t *lcd){
	  ili9341_fill_screen(lcd, ILI9341_BLACK);
	  ili9341_draw_border(lcd, ILI9341_ORANGE);

	  Draw_meter_bar(lcd, 150, 26, 18, 95, tx_status);

	    ili9341_text_attr_t text_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_ORANGE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 15,
			.origin_y = 5
	    };

	    //ili9341_draw_string(lcd, text_attr, "PRESTO TRANSVERTER");

	    text_attr.origin_x = 222;
	    text_attr.fg_color = ILI9341_WHITE;
	    text_attr.bg_color = ILI9341_RED;
	    ili9341_draw_string(lcd, text_attr, "TX");

	    text_attr.origin_y = 30;
	    text_attr.origin_x = 8;
	    text_attr.bg_color = ILI9341_BLACK;
	    ili9341_draw_string(lcd, text_attr, "PO");

	    text_attr.origin_x = 10;
	    text_attr.origin_y = 62;
	    ili9341_draw_string(lcd, text_attr, "DW ");

	    text_attr.origin_x = 8;
	    text_attr.origin_y = 102;
	    ili9341_draw_string(lcd, text_attr, "UP ");

	    text_attr.origin_y = 142;
	    ili9341_draw_string(lcd, text_attr, "TRX");

	    text_attr.origin_y = 170;
	    ili9341_draw_string(lcd, text_attr, "GPS:");

	    text_attr.origin_y = 190;
	    ili9341_draw_string(lcd, text_attr, "LNB:");

	    text_attr.origin_y = 210;
	    ili9341_draw_string(lcd, text_attr, "LNA:");

	    text_attr.origin_x = 145;
	    text_attr.origin_y = 190;
	    ili9341_draw_string(lcd, text_attr, "Vin:");

	    text_attr.origin_y = 210;
	    ili9341_draw_string(lcd, text_attr, "Tem:");

	    return 1;
}

uint16_t menu(ili9341_t *lcd){
	uint16_t menu_number = 0;
	uint16_t drawn_menu = 0;


	if(select_position_menu_global <= 4){		//Menu 1
		menu_number = 1;
		if(menu_number != drawn_menu){
			drawn_menu = menu1_display_scene(lcd); 	//Static elements (text)
		}
	}
	else if((select_position_menu_global > 4) && (select_position_menu_global <= 8)){	 //Menu 2
		menu_number = 2;
		if(menu_number != drawn_menu){
			drawn_menu = menu2_display_scene(lcd); 	//Static elements (text)
		}
	}
	else if((select_position_menu_global > 8) && (select_position_menu_global <= 12)){	 //Menu 3
		menu_number = 3;
		if(menu_number != drawn_menu){
			drawn_menu = menu3_display_scene(lcd);	//Static elements (text)
		}
	}
	else if((select_position_menu_global > 12) && (select_position_menu_global <= 16)){	//Menu 3
		menu_number = 4;
		if(menu_number != drawn_menu){
			drawn_menu = menu4_display_scene(lcd);	//Static elements (text)
		}
	}

	return 3;
}


uint16_t menu1_display_scene(ili9341_t *lcd){
	const uint8_t line_height = 40;
	ili9341_fill_screen(lcd, ILI9341_BLACK);
	ili9341_draw_border(lcd, ILI9341_ORANGE);


	ili9341_text_attr_t setting_line_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 222,
			.origin_y = 5
	    };

	ili9341_draw_string(lcd, setting_line_attr, "1/4");
	setting_line_attr.origin_x = 15;
	setting_line_attr.origin_y = 40;
	ili9341_draw_string(lcd, setting_line_attr, "LNB Setting");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "RX LNA");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "Freq step");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "RX ATTN");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "Brightness");

	return 3;
}

void menu1_display_settings(ili9341_t *lcd, uint8_t priority){
	const uint8_t line_height = 40;
	static uint32_t last_encoder_value = 0;
	const char *LNB_Strings[] = {"OFF ", "VERT", "HORI"};
	const char *LNA_Strings[] = {"OFF", "ON "};
	const char *Freq_steps_Strings[] = {"100  Hz", "  1 KHz", " 10 KHz", "100 KHz", "  1 MHz"};

	ili9341_text_attr_t setting_line_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 160,
			.origin_y = 40
	    };

	if((last_encoder_value != encoder_counter_val) || (priority == 1)){
		//Print LNB settings
		ili9341_draw_string(lcd, setting_line_attr, LNB_Strings[LNB_setting]);
		setting_line_attr.origin_y += line_height;
		ili9341_draw_string(lcd, setting_line_attr, LNA_Strings[RX_LNA_EN]);
		setting_line_attr.origin_y += line_height;

		char float_str[16] = {'\0'};
		if(attenuation_dB_LNB >= 10){
			snprintf(float_str, sizeof(float_str), " %.2fdB", attenuation_dB_LNB);
		}
		else if(attenuation_dB_LNB < 10){
			snprintf(float_str, sizeof(float_str), " %.2f dB", attenuation_dB_LNB);
		}

		ili9341_draw_string(lcd, setting_line_attr, Freq_steps_Strings[f_step]);
		setting_line_attr.origin_y += line_height;
		ili9341_draw_string(lcd, setting_line_attr, float_str);
		setting_line_attr.origin_y += line_height;

		sprintf(float_str, " %d ", lcd_brightness*10);
		ili9341_draw_string(lcd, setting_line_attr, float_str);

	}

}

uint16_t menu2_display_scene(ili9341_t *lcd){
	const uint8_t line_height = 40;
	ili9341_fill_screen(lcd, ILI9341_BLACK);
	ili9341_draw_border(lcd, ILI9341_ORANGE);

	ili9341_text_attr_t setting_line_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 222,
			.origin_y = 5
	    };

	ili9341_draw_string(lcd, setting_line_attr, "2/4");
	setting_line_attr.origin_x = 15;
	setting_line_attr.origin_y = 40;
	ili9341_draw_string(lcd, setting_line_attr, "LNB fmin");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "LNB fmax");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "AGC");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "LNB LO f");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "2nd RX");


	return 4;
}


void menu2_display_settings(ili9341_t *lcd, uint8_t priority){
	const uint8_t line_height = 40;
	static uint32_t last_encoder_value = 0;
	const char *AGC_Strings[] = {"OFF ", "ON "};
	const char *TRX_RX_Strings[] = {"OFF", "ON "};
	char float_str[20] = {'\0'};

	ili9341_text_attr_t setting_line_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 110,
			.origin_y = 40
	    };

	if((last_encoder_value != encoder_counter_val) || (priority == 1)){
		//Downlink low and high frequencies
		sprintf(float_str, "%.3f MHz", DOWN_link_low_f/1e6);
		ili9341_draw_string(lcd, setting_line_attr, float_str);
		setting_line_attr.origin_y += line_height;
		sprintf(float_str, "%.3f MHz", DOWN_link_high_f/1e6);
		ili9341_draw_string(lcd, setting_line_attr, float_str);
		setting_line_attr.origin_y += line_height;
		ili9341_draw_string(lcd, setting_line_attr, AGC_Strings[AGC_setting]);
		setting_line_attr.origin_y += line_height;
		sprintf(float_str, "%.3f MHz", LNB_LO/1e6);
		ili9341_draw_string(lcd, setting_line_attr, float_str);
		setting_line_attr.origin_y += line_height;
		ili9341_draw_string(lcd, setting_line_attr, TRX_RX_Strings[TRX_RX]);
		setting_line_attr.origin_y += line_height;
		last_encoder_value = encoder_counter_val;
	}

}


uint16_t menu3_display_scene(ili9341_t *lcd){
	const uint8_t line_height = 40;
	ili9341_fill_screen(lcd, ILI9341_BLACK);
	ili9341_draw_border(lcd, ILI9341_ORANGE);

	ili9341_text_attr_t setting_line_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 222,
			.origin_y = 5
	    };

	ili9341_draw_string(lcd, setting_line_attr, "3/4");
	setting_line_attr.origin_x = 15;
	setting_line_attr.origin_y = 40;
	ili9341_draw_string(lcd, setting_line_attr, "TX low f");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "TX max f");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "TX power");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "PTT pol");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "Sampler");


	return 5;
}


void menu3_display_settings(ili9341_t *lcd, uint8_t priority){
	const uint8_t line_height = 40;
	static uint32_t last_encoder_value = 0;
	const char *PTT_Pol_strings[] = {"ACTIVE LOW ", "ACTIVE HIGH"};
	char float_str[20] = {'\0'};

	ili9341_text_attr_t setting_line_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 110,
			.origin_y = 40
	    };

	if((last_encoder_value != encoder_counter_val) || (priority == 1)){
		//Downlink low and high frequencies
		sprintf(float_str, "%.3f MHz", UP_link_low_f/1e6);
		ili9341_draw_string(lcd, setting_line_attr, float_str);
		setting_line_attr.origin_y += line_height;
		sprintf(float_str, "%.3f MHz", UP_link_high_f/1e6);
		ili9341_draw_string(lcd, setting_line_attr, float_str);
		setting_line_attr.origin_y += line_height;
		if(tx_pwr < 10){
			sprintf(float_str, " %d ", tx_pwr);
		}
		else if(tx_pwr >= 10){
			sprintf(float_str, "%d ", tx_pwr);
		}
		ili9341_draw_string(lcd, setting_line_attr, float_str);
		setting_line_attr.origin_y += line_height;
		ili9341_draw_string(lcd, setting_line_attr, PTT_Pol_strings[PTT_Polarity]);
		setting_line_attr.origin_y += line_height;
		setting_line_attr.fg_color = ILI9341_DARKGREY;
		sprintf(float_str, "%.1f dB", sampler_output_cal);
		ili9341_draw_string(lcd, setting_line_attr, float_str);
		last_encoder_value = encoder_counter_val;
	}

}

uint16_t menu4_display_scene(ili9341_t *lcd){
	const uint8_t line_height = 20;
	ili9341_fill_screen(lcd, ILI9341_BLACK);
	ili9341_draw_border(lcd, ILI9341_ORANGE);

	ili9341_text_attr_t setting_line_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_7x10,
	    	.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 222,
			.origin_y = 5
	    };

	ili9341_draw_string(lcd, setting_line_attr, "4/4");
	setting_line_attr.origin_x = 10;
	setting_line_attr.origin_y += 2*line_height;
	ili9341_draw_string(lcd, setting_line_attr, "Diagnostics:");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "Vin:             Imon:");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "5V5 rail:        3V6 rail:");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "T MB     MCU      AMP     *C");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "CH SW         MB SN");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "MB rev        CH rev");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "LO1 rev       LO2 rev");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "POST:         Pwr on:");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "Last error code:");
	setting_line_attr.origin_y += line_height;
	ili9341_draw_string(lcd, setting_line_attr, "by SP6GK, Licence: CERN-OHL-S");

	return 6;
}

void menu4_display_diag(ili9341_t *lcd, uint8_t priority){
	const uint8_t line_height = 20;
	char float_str[20] = {'\0'};

	ili9341_text_attr_t diag_line_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_7x10,
	    	.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 10,
			.origin_y = 5
	    };
		diag_line_attr.origin_y += 3*line_height;
		diag_line_attr.origin_x += 35;
		if(PSU_Vin < 10){
			diag_line_attr.fg_color = ILI9341_RED;
			sprintf(float_str, "%.3f V", PSU_Vin);
		}
		else if(PSU_Vin >= 10){
			if(PSU_Vin < 12)
				diag_line_attr.fg_color = ILI9341_YELLOW;
			else if((PSU_Vin > 12) && (PSU_Vin < 16))
				diag_line_attr.fg_color = ILI9341_GREEN;
			else if((PSU_Vin >= 16))
				diag_line_attr.fg_color = ILI9341_RED;
			sprintf(float_str, "%.2f V", PSU_Vin);
		}
		ili9341_draw_string(lcd, diag_line_attr, float_str);
		diag_line_attr.fg_color = ILI9341_WHITE;
		diag_line_attr.origin_x += 120;
		sprintf(float_str, "%.3f A ", Imon_I);
		ili9341_draw_string(lcd, diag_line_attr, float_str);
		diag_line_attr.origin_y += line_height;
		diag_line_attr.origin_x = 80;
		sprintf(float_str, "%.2f V", PSU_5V5);
		ili9341_draw_string(lcd, diag_line_attr, float_str);
		diag_line_attr.origin_x +=120;
		sprintf(float_str, "%.2f V", PSU_3V6);
		ili9341_draw_string(lcd, diag_line_attr, float_str);
		diag_line_attr.origin_y += line_height;
		diag_line_attr.origin_x = 45;
		sprintf(float_str, "%.0f", temp_mb);
		ili9341_draw_string(lcd, diag_line_attr, float_str);
		diag_line_attr.origin_x += 55;
		sprintf(float_str, "%.0f", temp_mcu);
		ili9341_draw_string(lcd, diag_line_attr, float_str);
		diag_line_attr.origin_x += 75;
		sprintf(float_str, "%.0f", temp_amp);
		ili9341_draw_string(lcd, diag_line_attr, float_str);
		diag_line_attr.origin_y += line_height;
		diag_line_attr.origin_x = 70;
		sprintf(float_str, "%d%c", SW_MAJ_VER, SW_MIN_Ver);
		ili9341_draw_string(lcd, diag_line_attr, float_str); //CH SW
		diag_line_attr.origin_x += 120;
		sprintf(float_str, "%d", MB_NV.Serial_NoA);
		ili9341_draw_string(lcd, diag_line_attr, float_str); //MB SN
		diag_line_attr.origin_y += line_height;
		diag_line_attr.origin_x = 70;
		sprintf(float_str, "%d%c", MB_NV.HW_major_rev, MB_NV.HW_minor_rev);
		ili9341_draw_string(lcd, diag_line_attr, float_str); //MB rev
		diag_line_attr.origin_x += 120;
		sprintf(float_str, "%d%c", CH_NV.HW_major_rev, CH_NV.HW_minor_rev);
		ili9341_draw_string(lcd, diag_line_attr, float_str); //Ch rev
		diag_line_attr.origin_y += line_height;
		diag_line_attr.origin_x = 70;
		sprintf(float_str, "%d%c", PLL1_NV.HW_major_rev, PLL1_NV.HW_minor_rev); //LO1 rev
		ili9341_draw_string(lcd, diag_line_attr, float_str);
		diag_line_attr.origin_x += 120;
		sprintf(float_str, "%d%c", PLL2_NV.HW_major_rev, PLL2_NV.HW_minor_rev); //LO2 rev
		ili9341_draw_string(lcd, diag_line_attr, float_str);
		diag_line_attr.origin_y += line_height;
		diag_line_attr.origin_x = 45;
		sprintf(float_str, "%d", POST_Status);
		ili9341_draw_string(lcd, diag_line_attr, float_str); //POST
		diag_line_attr.origin_x += 120;
		sprintf(float_str, "%d", (int)PowerOn);
		ili9341_draw_string(lcd, diag_line_attr, float_str); //Power on times
		diag_line_attr.origin_y += line_height;
		diag_line_attr.origin_x += 15;
		sprintf(float_str, "%d", error_codes[error_pointer-CH_NV_Error_Code1_addr]);
		ili9341_draw_string(lcd, diag_line_attr, float_str); //Power on times


}

uint8_t Draw_meter_bar(ili9341_t *lcd, uint16_t x, uint16_t y, uint16_t h, uint16_t w, bool mode){
	ili9341_draw_rect(lcd, ILI9341_WHITE, x, y, w, h);
    ili9341_text_attr_t meter_txt_attr = (ili9341_text_attr_t){
    	.font = &ili9341_font_7x10,
    	.fg_color = ILI9341_WHITE,
		.bg_color = ILI9341_BLACK,
		.origin_x = x-8,
		.origin_y = y+h+2
    };

	if(mode == 0){
		ili9341_draw_string(lcd, meter_txt_attr, "0  3  6  9");
		meter_txt_attr.origin_x = x-20;
		ili9341_draw_string(lcd, meter_txt_attr, "S");
		meter_txt_attr.origin_x = x+70;
		meter_txt_attr.fg_color = ILI9341_RED;
		ili9341_draw_string(lcd, meter_txt_attr, "20 40");
	}
	else if(mode == 1){
		ili9341_draw_string(lcd, meter_txt_attr, "0  6  12  18 ");
		meter_txt_attr.origin_x = x-20;
		ili9341_draw_string(lcd, meter_txt_attr, " ");
		meter_txt_attr.origin_x = x+90;
		meter_txt_attr.fg_color = ILI9341_RED;
		ili9341_draw_string(lcd, meter_txt_attr, "24");
	}

	return 0;
}


uint8_t Update_meter_bar(ili9341_t *lcd, uint16_t value, uint16_t x, uint16_t y, uint16_t h, uint16_t w, bool mode){
    static int16_t prev_value = 0;
    int16_t threshold = (int16_t)(w * 0.75);

    if(value >= w-1){
        value = w-2;
    }

    if(mode == 0){
        if(value < prev_value){
            ili9341_fill_rect(lcd, ILI9341_BLACK, x+1+value, y+1, (prev_value - value), h-2);
        }

        if(value <= threshold){
            ili9341_fill_rect(lcd, ILI9341_GREEN, x+1, y+1, value, h-2);
        }
        else {
            ili9341_fill_rect(lcd, ILI9341_GREEN, x+1, y+1, threshold, h-2);
            ili9341_fill_rect(lcd, ILI9341_RED, x+1+threshold, y+1, (value - threshold), h-2);
        }
    }
    else if(mode == 1){
        // ili9341_draw_rect(lcd, ILI9341_WHITE, x, y, w, h);
    }

    prev_value = value;
    return 0;
}

uint16_t draw_buttons(ili9341_t *lcd, uint16_t scene, uint8_t priority){
	static bool vfo_track_last = 0;
	static bool vfo_lock_last = 0;
	static uint8_t vfo_select_last = 0;
	static uint8_t display_scene_last = 0;


	if((vfo_track_last == vfo_track) && (vfo_lock_last == vfo_lock) && (vfo_select_last == vfo_select) && (display_scene_last == display_scene) && (priority != 1)){
		//buttons_status[3] = 0;
		return 0;	//Terminate here is state of buttons did not change
	}

	if(scene == 0){		//RX
		ili9341_text_attr_t text_attr = (ili9341_text_attr_t){
			    	.font = &ili9341_font_11x18,
			    	.fg_color = ILI9341_WHITE,
					.bg_color = ILI9341_BLACK,
					.origin_x = 262,
					.origin_y = 12
			    };
		if(vfo_track == 1){
			ili9341_fill_rect(lcd, ILI9341_ORANGE, 260, 1, 60, 50);
				ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 1, 60, 50);
				text_attr.bg_color = ILI9341_ORANGE;
			    ili9341_draw_string(lcd, text_attr, "TRACK");
		}
		else if(vfo_track == 0){
				ili9341_fill_rect(lcd, ILI9341_BLACK, 260, 1, 60, 50);
				text_attr.bg_color = ILI9341_BLACK;
				ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 1, 60, 50);
				ili9341_draw_string(lcd, text_attr, "INDEP");
		}

	    text_attr.origin_x = 265;
	    text_attr.origin_y = 80;

	    if(Remote_Mode == 1){
			ili9341_fill_rect(lcd, ILI9341_ORANGE, 260, 63, 60, 50);

		    text_attr.bg_color = ILI9341_ORANGE;
			ili9341_draw_string(lcd, text_attr, "REMO");
		}
	    else if(Remote_Mode == 0){


			if(vfo_lock == 1){
				ili9341_fill_rect(lcd, ILI9341_ORANGE, 260, 63, 60, 50);

				text_attr.bg_color = ILI9341_ORANGE;
				ili9341_draw_string(lcd, text_attr, "LOCK");
			}
			else if(vfo_lock == 0){
				ili9341_fill_rect(lcd, ILI9341_BLACK, 260, 63, 60, 48);

				text_attr.bg_color = ILI9341_BLACK;
				ili9341_draw_string(lcd, text_attr, "LOCK");
			}
	    }

		ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 63, 60, 50);

	    text_attr.origin_x = 265;
	    text_attr.origin_y = 160;

		if(vfo_track == 0){
			text_attr.fg_color = ILI9341_WHITE;
			text_attr.bg_color = ILI9341_BLACK;

		}
		else if(vfo_track == 1){
			text_attr.fg_color = ILI9341_WHITE;
			text_attr.bg_color = ILI9341_ORANGE;

		}

		ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 125, 60, 50);
		text_attr.origin_y = 143;
		text_attr.bg_color = ILI9341_BLACK;

			    if(vfo_select == 1){
			    	ili9341_draw_string(lcd, text_attr, "DW");
			    }
			    else if(vfo_select == 2){
			    	ili9341_draw_string(lcd, text_attr, "UP");
			    }
			    else if(vfo_select == 3){
			    	ili9341_draw_string(lcd, text_attr, "TRX ");
			    }

				ili9341_fill_rect(lcd, ILI9341_BLACK, 260, 189, 58, 48);

			    text_attr.origin_x  = 265;
			    text_attr.origin_y = 205;

			    ili9341_draw_string(lcd, text_attr, "MENU");
			    ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 189, 60, 50);

	}
	else if(scene == 1){		//TX
		if(vfo_track == 1){
			ili9341_fill_rect(lcd, ILI9341_ORANGE, 260, 1, 60, 50);
			ili9341_text_attr_t up_txt_attr = (ili9341_text_attr_t){
				    	.font = &ili9341_font_11x18,
				    	.fg_color = ILI9341_DARKGREY,
						.bg_color = ILI9341_ORANGE,
						.origin_x = 262,
						.origin_y = 12
				    };
				ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 1, 60, 50);
			    ili9341_draw_string(lcd, up_txt_attr, "TRACK");
		}
		else if(vfo_track == 0){
			ili9341_fill_rect(lcd, ILI9341_BLACK, 260, 1, 60, 50);
			ili9341_text_attr_t up_txt_attr = (ili9341_text_attr_t){
				    	.font = &ili9341_font_11x18,
				    	.fg_color = ILI9341_DARKGREY,
						.bg_color = ILI9341_BLACK,
						.origin_x = 262,
						.origin_y = 12
				    };
				ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 1, 60, 50);
				ili9341_draw_string(lcd, up_txt_attr, "INDEP");
		}

	    ili9341_text_attr_t lock_txt_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_DARKGREY,
			.bg_color = ILI9341_ORANGE,
			.origin_x = 265,
			.origin_y = 80
	    };

	    if(Remote_Mode == 1){
			ili9341_fill_rect(lcd, ILI9341_ORANGE, 260, 63, 60, 50);

			lock_txt_attr.bg_color = ILI9341_ORANGE;
			ili9341_draw_string(lcd, lock_txt_attr, "REMO");
		}
	    else{
		if(vfo_lock == 1){
			ili9341_fill_rect(lcd, ILI9341_ORANGE, 260, 63, 60, 50);
			ili9341_draw_string(lcd, lock_txt_attr, "LOCK");
		}
		else if(vfo_lock == 0){
			ili9341_fill_rect(lcd, ILI9341_BLACK, 260, 63, 60, 48);
		    ili9341_text_attr_t lock_txt_attr = (ili9341_text_attr_t){
		    	.font = &ili9341_font_11x18,
		    	.fg_color = ILI9341_DARKGREY,
				.bg_color = ILI9341_BLACK,
				.origin_x = 265,
				.origin_y = 80
		    };
		    ili9341_draw_string(lcd, lock_txt_attr, "LOCK");
		}
	    }
		ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 63, 60, 50);

	    ili9341_text_attr_t vfo_select_txt_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_DARKGREY,
			.bg_color = ILI9341_BLACK,
			.origin_x = 265,
			.origin_y = 160
	    };

		if(vfo_track == 0){
			     vfo_select_txt_attr.fg_color = ILI9341_DARKGREY;
			     vfo_select_txt_attr.bg_color = ILI9341_BLACK;

		}
		else if(vfo_track == 1){
			     vfo_select_txt_attr.fg_color = ILI9341_DARKGREY;
			     vfo_select_txt_attr.bg_color = ILI9341_BLACK;

		}

		ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 125, 60, 50);
		vfo_select_txt_attr.origin_y = 143;
		vfo_select_txt_attr.bg_color = ILI9341_BLACK;

			    if(vfo_select == 1){
			    	ili9341_draw_string(lcd, vfo_select_txt_attr, "DW");
			    }
			    else if(vfo_select == 2){
			    	ili9341_draw_string(lcd, vfo_select_txt_attr, "UP");
			    }
			    else if(vfo_select == 3){
			    	ili9341_draw_string(lcd, vfo_select_txt_attr, "TRX ");
			    }

				ili9341_fill_rect(lcd, ILI9341_BLACK, 260, 189, 58, 48);
			    ili9341_text_attr_t menu_txt_attr = (ili9341_text_attr_t){
			    	.font = &ili9341_font_11x18,
			    	.fg_color = ILI9341_DARKGREY,
					.bg_color = ILI9341_BLACK,
					.origin_x = 265,
					.origin_y = 205
			    };
			    ili9341_draw_string(lcd, menu_txt_attr, "MENU");
			    ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 189, 60, 50);

	}
	else if((scene == 3) || (scene == 4) || (scene == 5) || (scene == 6)){ //Menu
		ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 1, 60, 50);
		ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 63, 60, 50);
		ili9341_draw_rect(lcd, ILI9341_ORANGE, 260, 125, 60, 50);

	    ili9341_text_attr_t text_attr = (ili9341_text_attr_t){
	    	.font = &ili9341_font_11x18,
	    	.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 273,
			.origin_y = 12
	    };
	    ili9341_draw_string(lcd, text_attr, "/\\");

	    text_attr.origin_y = 145;
	    ili9341_draw_string(lcd, text_attr, "\\/");

		ili9341_fill_rect(lcd, ILI9341_ORANGE, 260, 189, 60, 50);

	    text_attr.origin_x = 265;
	    text_attr.origin_y = 80;
	    ili9341_draw_string(lcd, text_attr, "SAVE");

	    text_attr.origin_y = 205;
	    text_attr.bg_color = ILI9341_ORANGE;
	    ili9341_draw_string(lcd, text_attr, "MENU");

	}

	//Update values of previous state
	vfo_track_last = vfo_track;
	vfo_lock_last = vfo_lock;
	vfo_select_last = vfo_select;
	display_scene_last = display_scene;

	return 1;
}

void process_buttons(void){
	//Serve the buttons unless device is transmitting
	if(tx_status != 1){
		if((buttons_status[0] == 1) && (display_scene == 0)){			//Buttons in RX mode
			vfo_track =  1- vfo_track;
			if(vfo_track == 1){
				HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
			}
			buttons_status[0] = 0;
		}
		if((buttons_status[1] == 1) && (display_scene == 0)){
			if(Remote_Mode == 1){
				Remote_Mode = 0;
			}
			else{
			vfo_lock = 1 - vfo_lock;
			if(vfo_lock == 1){
				HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
			 }
			}
			buttons_status[1] = 0;
		}
		if((buttons_status[2] == 1) && (display_scene == 0)){
			vfo_select ++;
			if(vfo_select > 3){
				vfo_select = 1;
			}
			buttons_status[2] = 0;
		}

		if((buttons_status[0] == 1) && (display_scene >= 3) && (display_scene != 1)){ 		//Buttons in menu
			if(select_position_menu_global > 1){ 						//Go up in menu options
				select_position_menu_global --;
			}
			buttons_status[0] = 0;

		}
		if((buttons_status[1] == 1) && (display_scene >= 3)){			//Change setting
			save_settings();
			buttons_status[1] = 0;

		}
		if((buttons_status[2] == 1) && (display_scene >= 3)){
			if(select_position_menu_global >= 0){ 						//Go down in menu options
				select_position_menu_global ++;
			}
			if(select_position_menu_global > 20){
				select_position_menu_global = 1;
			}

			buttons_status[2] = 0;
		}




	}

}


uint16_t process_encoder(void) {
    static int32_t last_encoder_counter_val = 0;
    int32_t encoder_delta = 0;
    int64_t f_increment = 0;
    uint64_t new_f = 0;

    encoder_counter_val = ((int32_t)TIM2->CNT - 32767) >> 2;
    encoder_delta = encoder_counter_val - last_encoder_counter_val;

    if (encoder_delta != 0) {
        if ((display_scene == 0) && (vfo_lock != 1) && (tx_status != 1)) {		 //Process frequency adjustment (VFO in RX mode)
        	if((vfo_track == 1) && (vfo_select <= 2)){
        		f_increment = encoder_delta * (Freq_steps[f_step]);
        		//TRX_F += f_increment;
        		UP_Link_F += f_increment;
        		DOWN_Link_F += f_increment;
        	}
        	else{
        		new_f = (encoder_delta * (Freq_steps[f_step]));
				switch(vfo_select){
					case 1: //Down
						DOWN_Link_F += new_f;

						break;
					case 2: //UP
						UP_Link_F += new_f;

						break;
					case 3:	//TRX
						TRX_F += new_f;

						break;
					}
        	}


			if(DOWN_Link_F < DOWN_link_low_f){
				DOWN_Link_F = DOWN_link_low_f;
			}
			else if(DOWN_Link_F > DOWN_link_high_f){
				DOWN_Link_F = DOWN_link_high_f;
			}

			if(UP_Link_F < UP_link_low_f){
				UP_Link_F = UP_link_low_f;
			}
			else if(UP_Link_F > UP_link_high_f){
				UP_Link_F = UP_link_high_f;
			}

			if(TRX_F < 10e6){
				TRX_F = 10e6;
			}
			else if(TRX_F > 55e6){
				TRX_F = 55e6;
			}



        }
        else if(display_scene >= 3){ 														// Settings menu options
            switch (select_position_menu_global) {
                case 1: // LNB Setting
                    LNB_setting += encoder_delta;
                    if (LNB_setting < 0){
                        LNB_setting = 0;
                    }
                    else if(LNB_setting > 2)
                        LNB_setting = 2;

                    break;

                case 2: //RX LNA
                	RX_LNA_EN = RX_LNA_EN - 1;
                    break;

                case 3: //Freq Step
                	f_step += encoder_delta;
                	if(f_step < 0){
                		f_step = 0;
                	}
                	else if(f_step > 4){
                		f_step = 4;
                	}
                	break;

                case 4: //RX attenuator
                	attenuation_dB_LNB += encoder_delta*0.25;
                	if(attenuation_dB_LNB < 0){
                		attenuation_dB_LNB = 0;
                	}
                	else if(attenuation_dB_LNB > ATT_MAX){
                		attenuation_dB_LNB = ATT_MAX;
                	}
                	attenuation_code_LNB = map_float_to_int(attenuation_dB_LNB, 0, ATT_MAX, 0, UINT8_MAX);
                	//Setting attenuators is handled by process_attenuators in main task
                    break;
                case 5: // Temperature sensor on display select
                	lcd_brightness += encoder_delta;
                	if(lcd_brightness < 0){
                		lcd_brightness = 0;
                	}
                	else if(lcd_brightness > 10){
                		lcd_brightness = 10;
                	}
                	break;
                case 6:		//Downlink low frequency
                	DOWN_link_low_f += (encoder_delta*Freq_steps[f_step]*100);

                	if(DOWN_link_low_f < 1e10){
                		DOWN_link_low_f = 1e10;
                	}
                	else if(DOWN_link_low_f > 14e9){
                		DOWN_link_low_f = 14e9;
                	}

                	if(DOWN_link_low_f > DOWN_link_high_f){		//Ensure that low f is smaller than high
                		DOWN_link_low_f = DOWN_link_high_f-1e6;
                	}

                	break;
                case 7:		//Downlink high frequency
                	DOWN_link_high_f += (encoder_delta*Freq_steps[f_step]*100);

                	if(DOWN_link_high_f < 1e10){
                		DOWN_link_high_f = 1e10;
                	}
                	else if(DOWN_link_high_f > 14e9){
                		DOWN_link_high_f = 14e9;
                	}

                	if(DOWN_link_high_f <= DOWN_link_low_f){		//Ensure that low f is smaller than high
                		DOWN_link_high_f = DOWN_link_low_f+1e6;
                	}

                	break;
                case 8:		//AGC Setting
                	AGC_setting += encoder_delta;
                	if(AGC_setting < 0){
                		AGC_setting = 0;
                	}
                	else if(AGC_setting > 1){
                		AGC_setting = 1;
                	}
                	break;
                case 9:		//LNB LO frequency
                	LNB_LO += (encoder_delta*Freq_steps[f_step]);
                	if(LNB_LO < 1e9){
                		LNB_LO = 9e9;
                	}
                	if(LNB_LO > 11e9){
                		LNB_LO = 11e9;
                	}
                	break;
                case 10:		//TRX serves as a 2nd RX while not transmitting
                	TRX_RX += encoder_delta;
                	if(TRX_RX < 0){
                		TRX_RX = 0;
                	}
                	else if(TRX_RX > 1){
                		TRX_RX = 1;
                	}
                	break;
                case 11:		//Up link low frequency
                	UP_link_low_f += (encoder_delta*Freq_steps[f_step]*100);

                	if(UP_link_low_f < 2e9){
                		UP_link_low_f = 2e9;
                	}
                	else if(UP_link_low_f > 3e9){
                		UP_link_low_f = 3e9;
                	}

                	if(UP_link_low_f > UP_link_high_f){		//Ensure that low f is smaller than high
                		UP_link_low_f = UP_link_high_f-1e6;
                	}

                	break;
                case 12:		//Up link high frequency
                	UP_link_high_f += (encoder_delta*Freq_steps[f_step]*100);

                	if(UP_link_high_f < 2e9){
                		UP_link_high_f = 2e9;
                	}
                	else if(UP_link_high_f > 3e9){
                		UP_link_high_f = 3e9;
                	}

                	if(UP_link_high_f <= UP_link_low_f){		//Ensure that low f is smaller than high
                		UP_link_high_f = UP_link_low_f+1e6;
                	}

                	break;
                case 13:
                	tx_pwr += encoder_delta;

                	if(tx_pwr < 0){
                		tx_pwr = 0;
                	}
                	else if(tx_pwr > 100){
                		tx_pwr = 0;
                	}

                	break;

                case 14:
                	PTT_Polarity += encoder_delta;

                	if(PTT_Polarity == false){
                		PTT_Polarity = 0;
                	}
                	else if(PTT_Polarity == true){
                		PTT_Polarity = 1;
                	}

                default:
                    break;
            }
        }

        //TIM2->CNT = 32767; //Reset value, in other functions check whether encoder_counter_calue has changed and determine whether to add or subtract from modified variable
        last_encoder_counter_val = encoder_counter_val;
    }

    return 0;
}

uint16_t draw_selection_rectangle(ili9341_t *lcd, uint16_t scene){
	static uint16_t last_scene;
	uint16_t f_rect_h = 36;
	uint16_t s_rect_h = 40;

	static uint16_t last_select_f = 0xfff;
	static uint16_t last_select_s = 0xfff;

	static bool last_REM_Mode = 0;

	uint16_t local_rect_pos_s = 0;

	if((scene == 0) && ((scene != last_scene) || (last_select_f != vfo_select) || (last_REM_Mode != Remote_Mode))){ 		//Draw frequency selection rectangle
			ili9341_draw_rect(lcd, ILI9341_BLACK, 5, 58+(f_rect_h)*last_select_f-f_rect_h, 250, f_rect_h);
			ili9341_draw_rect(lcd, ILI9341_ORANGE, 5, 58+(f_rect_h)*vfo_select-f_rect_h, 250, f_rect_h);
			//last_select_f = vfo_select;
			last_REM_Mode = Remote_Mode;
	}
	else if((scene >= 3) && ((scene != last_scene) || (last_select_s != select_position_menu_global))){	//Draw setting selection rectangle
		for(uint8_t i = 0; i < 5; i++){
			ili9341_draw_rect(lcd, ILI9341_BLACK, 5, 36+(i*s_rect_h), 250, s_rect_h);
		}
		local_rect_pos_s = (select_position_menu_global - 1) % 5;
		ili9341_draw_rect(lcd, ILI9341_ORANGE, 5, 36+(local_rect_pos_s*s_rect_h), 250, s_rect_h);
	}

	last_select_s = select_position_menu_global;
	last_select_f = vfo_select;
	last_scene = scene;
	return 0;

}

uint16_t draw_title_bar(ili9341_t *lcd){
	static bool title_drawn = 0;

	if((error_code == 0) && (title_drawn != 1)){
		ili9341_text_attr_t text_attr = (ili9341_text_attr_t){
			.font = &ili9341_font_11x18,
			.fg_color = ILI9341_ORANGE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 15,
			.origin_y = 5
		};
		ili9341_draw_string(lcd, text_attr, "PRESTO TRANSVERTER");
	}
	else if(error_code != 0){
		ili9341_text_attr_t text_attr = (ili9341_text_attr_t){
			.font = &ili9341_font_11x18,
			.fg_color = ILI9341_RED,
			.bg_color = ILI9341_WHITE,
			.origin_x = 15,
			.origin_y = 5
		};
		ili9341_draw_string(lcd, text_attr, error_strings[error_code]);
	}

	return 0;
}

//Assign values from EEPROM NV structs to global variables at startup, process if necessary
uint8_t load_structs(void){
	if(CRC_CH_Read_fail == false){
	TRX_F = CH_NV.TRX_F;
	UP_Link_F = CH_NV.UP_Link_F;
	DOWN_Link_F = CH_NV.DOWN_Link_F;

	RX_LNA_EN = CH_NV.RX_LNA;
	f_step = CH_NV.f_step;
	LNB_setting = CH_NV.LNB_Setting;

	attenuation_code_LNB = CH_NV.attenuation_code_LNB;
	attenuation_dB_LNB = map_int_to_float(attenuation_code_LNB, 0, UINT8_MAX, 0, ATT_MAX);

	attenuation_code_TRX = CH_NV.attenuation_code_TRX;
	attenuation_dB_TRX = map_int_to_float(attenuation_code_TRX, 0, UINT8_MAX, 0, ATT_MAX);

	tmp_sens_disp = CH_NV.temp_sens_disp;

	DOWN_link_low_f = CH_NV.DOWN_link_low_f;
	DOWN_link_high_f = CH_NV.DOWN_link_high_f;
	DOWN_Link_F = CH_NV.DOWN_Link_F;
	LNB_LO = CH_NV.LNB_LO;

	AGC_setting = CH_NV.AGC_setting;
	TRX_RX = CH_NV.TRX_RX;

	UP_link_low_f = CH_NV.Up_Link_low_f;
	UP_link_high_f = CH_NV.Up_Link_High_f;
	tx_pwr = CH_NV.tx_pwr;
	PTT_Polarity = CH_NV.Pin_Settings; //TODO in the future from these bits more data can be extracted
	lcd_brightness = CH_NV.Brightness;
	TIM4->CCR4 = map_uint16(CH_NV.Brightness, 0, 10, 1e3, UINT32_MAX);
	}


	else{
		TIM4->CCR4 = UINT32_MAX;
	}


	if(CRC_MB_Read_fail == false){
		sampler_output_cal = map_int_to_float(MB_NV.Sampler_Cal, 0, UINT16_MAX, SAMPLER_CAL_MIN, SAMPLER_CAL_MAX);

		LNB_V_Calibration = map_int_to_float(MB_NV.LNBV_sense_calA, 0, UINT16_MAX, LNB_V_CAL_MIN, LNB_V_CAL_MAX);
		LNB_V_Calibration_offset = map_int_to_float(MB_NV.LNBV_sense_calB, 0, UINT16_MAX, LNB_V_CAL_OFFSET_MIN, LNB_V_CAL_OFFSET_MAX);

		PSU_Vin_Calibration = map_int_to_float(MB_NV.Vin_sense_calA, 0, UINT16_MAX, PSU_VIN_CAL_MIN, PSU_VIN_CAL_MAX);
		PSU_Vin_Calibration_offset = map_int_to_float(MB_NV.Vin_sense_calB, 0, UINT16_MAX, PSU_VIN_CAL_OFFSET_MIN, PSU_VIN_CAL_OFFSET_MAX);

		PSU_3V6_Calibration = map_int_to_float(MB_NV.P3V6_sense_calA, 0, UINT16_MAX, P3V6_CALA_MIN, P3V6_CALA_MAX);
		PSU_5V5_Calibration = map_int_to_float(MB_NV.P5v5_sense_calA, 0, UINT16_MAX, P5V5_CALA_MIN, P5V5_CALA_MAX);

		Imon_Calibration = map_int_to_float(MB_NV.Imon_calA, 0, UINT16_MAX, 0.1, 3);
		Imon_Calibration_offset = map_int_to_float(MB_NV.Imon_calB, 0, UINT16_MAX, -1, 1);

		RF_det_rx_calA = map_int_to_float(MB_NV.RF_Det_RX_calA, 0, UINT16_MAX, RF_DET_RX_CAL_A_MIN, RF_DET_RX_CAL_A_MAX);
		RF_det_rx_calB = map_int_to_float(MB_NV.RF_Det_RX_calB, 0, UINT16_MAX, RF_DET_RX_CAL_B_MIN, RF_DET_RX_CAL_B_MAX);
		RF_det_rx_calC = map_int_to_float(MB_NV.RF_Det_RX_calC, 0, UINT16_MAX, RF_DET_RX_CAL_C_MIN, RF_DET_RX_CAL_C_MAX);

		RF_det_tx_calA = map_int_to_float(MB_NV.RF_Det_TX_calA, 0, UINT16_MAX, RF_DET_TX_CAL_A_MIN, RF_DET_TX_CAL_A_MAX);
		RF_det_tx_calB = map_int_to_float(MB_NV.RF_Det_TX_calB, 0, UINT16_MAX, RF_DET_TX_CAL_B_MIN, RF_DET_TX_CAL_B_MAX);
		RF_det_tx_calC = map_int_to_float(MB_NV.RF_Det_TX_calC, 0, UINT16_MAX, RF_DET_TX_CAL_C_MIN, RF_DET_TX_CAL_C_MAX);


	}


	if(CRC_LO1_Read_fail == false){
		DAC_VCTCXO = PLL1_NV.VCTCXO_cal;
	}



	return 0;
}

//Checks flags set in TIM IRQ
void process_nv_writes(void){
	HAL_StatusTypeDef status = HAL_ERROR;

	HAL_StatusTypeDef crc_status = HAL_OK;
	uint32_t crc = 0;

	if(NV_Write_TRX){
		status = POKE64(CH_EEPROM_ADDR, CH_NV_TRX_F_addr, TRX_F);
		if(status == HAL_OK){
			CH_NV.TRX_F = TRX_F;
			NV_Write_TRX &= ~WRITE_PENDING;
		}
		else{
			error_code = 45;
		}
	}

	if(NV_Write_UP){
		status = POKE64(CH_EEPROM_ADDR, CH_NV_UP_Link_F_addr, UP_Link_F);
		osDelay(50);
		if(status == HAL_OK){
			CH_NV.UP_Link_F = UP_Link_F;
			NV_Write_UP &= ~WRITE_PENDING;
		}
		else{
			error_code = 45;
		}
	}

	if(NV_Write_Down){
		status = POKE64(CH_EEPROM_ADDR, CH_NV_DOWN_Link_F_addr, DOWN_Link_F);
		osDelay(50);
		if(status == HAL_OK){
			CH_NV.DOWN_Link_F = DOWN_Link_F;
			NV_Write_Down &= ~WRITE_PENDING;
		}
		else{
			error_code = 45;
		}
	}

	//Update CRC
	if(status == HAL_OK){
		crc = EEPROM_Calculate_CRC(CH_EEPROM_ADDR);
		crc_status = POKE32(CH_EEPROM_ADDR, CH_NV_CRC32_addr, crc);
	}
	else{
		//Saving setting failed
		//error_code = 45;
	}


	if(crc_status != HAL_OK){
		//CRC update failed
		error_code = 47;
	}



}

//Saves menu settings to NV EEPROM after button press in menu
void save_settings(void){
	HAL_StatusTypeDef status = HAL_OK;
	HAL_StatusTypeDef crc_status = HAL_OK;
	uint32_t crc = 0;

	if(LNB_setting != CH_NV.LNB_Setting){
		status = POKE(CH_EEPROM_ADDR, CH_NV_LNB_Setting_addr, (uint16_t)LNB_setting);
		if(status == HAL_OK){
			CH_NV.LNB_Setting = LNB_setting;
		}
	}

	if(RX_LNA_EN != CH_NV.RX_LNA){
		status = POKE(CH_EEPROM_ADDR, CH_NV_RX_LNA_addr, (uint16_t)RX_LNA_EN);
		if(status == HAL_OK){
			CH_NV.RX_LNA = RX_LNA_EN;
		}
	}

	if(f_step != CH_NV.f_step){
		status = POKE(CH_EEPROM_ADDR, CH_NV_f_step_addr, (uint16_t)f_step);
		if(status == HAL_OK){
			CH_NV.f_step = f_step;
		}
	}

	if(attenuation_code_LNB != CH_NV.attenuation_code_LNB){
		status = POKE(CH_EEPROM_ADDR, CH_NV_attenuation_code_LNB_addr, (uint16_t)attenuation_code_LNB);
		if(status == HAL_OK){
			CH_NV.f_step = attenuation_code_LNB;
		}
	}

	if(tmp_sens_disp != CH_NV.temp_sens_disp){
		status = POKE(CH_EEPROM_ADDR, CH_NV_temp_sens_disp_addr, (uint16_t)tmp_sens_disp); //depreciated feature in favour of brightness
		if(status == HAL_OK){
			CH_NV.temp_sens_disp = tmp_sens_disp;
		}
	}

	if(DOWN_link_low_f != CH_NV.DOWN_link_low_f){
		status = POKE64(CH_EEPROM_ADDR, CH_NV_DOWN_link_low_f_addr, (uint64_t)DOWN_link_low_f);
		if(status == HAL_OK){
			CH_NV.DOWN_link_low_f = DOWN_link_low_f;
		}
	}

	if(DOWN_link_high_f != CH_NV.DOWN_link_high_f){
		status = POKE64(CH_EEPROM_ADDR, CH_NV_DOWN_link_high_f_addr, (uint64_t)DOWN_link_high_f);
		if(status == HAL_OK){
			CH_NV.DOWN_link_high_f = DOWN_link_high_f;
		}
	}

	if(DOWN_Link_F != CH_NV.DOWN_Link_F){
		status = POKE64(CH_EEPROM_ADDR, CH_NV_DOWN_Link_F_addr, (uint64_t)DOWN_Link_F);
		if(status == HAL_OK){
			CH_NV.DOWN_Link_F = DOWN_Link_F;
		}
	}

	if(AGC_setting != CH_NV.AGC_setting){
		status = POKE(CH_EEPROM_ADDR, CH_NV_AGC_setting_addr, (uint64_t)AGC_setting);
		if(status == HAL_OK){
			CH_NV.AGC_setting = AGC_setting;
		}
	}

	if(TRX_RX != CH_NV.TRX_RX){
		status = POKE(CH_EEPROM_ADDR, CH_NV_TRX_RX_addr, (uint64_t)TRX_RX);
		if(status == HAL_OK){
			CH_NV.TRX_RX = TRX_RX;
		}
	}

	//Settings screen 3/4
	if(DOWN_link_low_f != CH_NV.DOWN_link_low_f){
		status = POKE64(CH_EEPROM_ADDR, CH_NV_DOWN_link_low_f_addr, (uint64_t)DOWN_link_low_f);
		if(status == HAL_OK){
			CH_NV.DOWN_link_low_f = DOWN_link_low_f;
		}
	}

	if(DOWN_link_high_f != CH_NV.DOWN_link_high_f){
		status = POKE64(CH_EEPROM_ADDR, CH_NV_DOWN_link_high_f_addr, (uint64_t)DOWN_link_high_f);
		if(status == HAL_OK){
			CH_NV.DOWN_link_high_f = DOWN_link_high_f;
		}
	}

	if(tx_pwr != CH_NV.tx_pwr){
		status = POKE(CH_EEPROM_ADDR, CH_NV_tx_pwr_addr, (uint64_t)tx_pwr);
		if(status == HAL_OK){
			CH_NV.tx_pwr = tx_pwr;
		}
	}

	if( PTT_Polarity != CH_NV.Pin_Settings){
		status = POKE(CH_EEPROM_ADDR, CH_NV_Pin_Settings_addr, (uint64_t)PTT_Polarity);
		if(status == HAL_OK){
			CH_NV.Pin_Settings = PTT_Polarity;
		}
	}


	if(lcd_brightness != CH_NV.Brightness){
	    status = POKE(CH_EEPROM_ADDR, CH_NV_Brightness_addr, lcd_brightness);
        CH_NV.Brightness = lcd_brightness;
        TIM4->CCR4 = Brightness_To_PWM(CH_NV.Brightness);

	    if (status == HAL_OK) {

	    }
	}

	//Update CRC
	if(status == HAL_OK){
		crc = EEPROM_Calculate_CRC(CH_EEPROM_ADDR);
		crc_status = POKE32(CH_EEPROM_ADDR, CH_NV_CRC32_addr, crc);
	}
	else{
		//Saving setting failed
		error_code = 46;
	}


	if(crc_status != HAL_OK){
		//CRC update failed
		error_code = 47;
	}

}


float map_int_to_float(uint16_t inValue, uint16_t minIN, uint16_t maxIN, float minOUT, float maxOUT){
    if (maxIN == minIN) {
        return minOUT;
    }
    float ratio = (float)(inValue - minIN) / (float)(maxIN - minIN);
    return minOUT + ratio * (maxOUT - minOUT);
}


uint16_t map_float_to_int(float inValue, float minIN, float maxIN, uint16_t minOUT, uint16_t maxOUT){
    if (maxIN == minIN) {
        return minOUT;
    }

    float ratio = (inValue - minIN) / (maxIN - minIN);

    float scaled = minOUT + ratio * (float)(maxOUT - minOUT);

    if (scaled < minOUT) scaled = minOUT;
    if (scaled > maxOUT) scaled = maxOUT;

    return (uint16_t)scaled;
}

uint8_t map_uint8(uint8_t inValue, uint8_t minIN, uint8_t maxIN, uint8_t minOUT, uint8_t maxOUT){
    return (inValue - minIN) * (maxOUT - minOUT) / (maxIN - minIN) + minOUT;
}

uint32_t map_uint16(uint16_t inValue, uint16_t minIN, uint16_t maxIN, uint32_t minOUT, uint32_t maxOUT){
    return (inValue - minIN) * (maxOUT - minOUT) / (maxIN - minIN) + minOUT;
}

//Handles AGC, manual attenuation (AGC off), and tx power control
uint8_t process_attenuators(void){
	static uint8_t tmp_LNB_attenuator_code = 0; //LNB attenuator, constant in TX and RX mode, set by memory
	static uint8_t tmp_RX_attenuator_code = 0;  //TRX path attenuator code in RX mode, set by AGC
	//static uint8_t tmp_TX_attenuator_code = 0;  //TRX path attenuator code in TX mode, set by memory

	float RX_lvl_threshold = -63; //RX_DET_THR; //Maximal level to be maintained at the connected TRX (S9+10 dB)

	//Program the TRX side attenuator (changes in TX and RX)
	if((tx_status != true) && (AGC_setting == 1)){	//Process RX RSSI information for RF attenuation in AGC mode ON
		if(DET_rx_pwr > RX_lvl_threshold){
			attenuation_dB_TRX = abs(DET_rx_pwr - RX_lvl_threshold);
			if(attenuation_dB_TRX >= ATT_MAX){
				//Note: this is protection against really strong signal from LNB connector.
				attenuation_dB_LNB = attenuation_dB_TRX - ATT_MAX;
				if(attenuation_dB_LNB >= ATT_MAX){
					attenuation_dB_LNB = ATT_MAX;
				}
				attenuation_code_LNB = F1956_LUT(attenuation_dB_LNB);

				attenuation_dB_TRX = ATT_MAX;
			}
			attenuation_code_TRX = F1956_LUT(attenuation_dB_TRX);
		}
		else{
			attenuation_dB_TRX = 0;
			attenuation_code_TRX = 0;
		}

		//Update attenuators if status has changed
		if(tmp_RX_attenuator_code != attenuation_code_TRX){
			F1956_S21_SPI(attenuation_code_TRX, att_TRX_address);
			tmp_RX_attenuator_code = attenuation_code_TRX;
		}
		if(tmp_LNB_attenuator_code != attenuation_code_LNB){
			F1956_S21_SPI(attenuation_code_LNB, att_LNB_address);
			tmp_RX_attenuator_code = attenuation_code_LNB;
		}
	}
	else if((tx_status != true) && (AGC_setting == 0)){		//Manual RF attenuation controlt, AGC off. Controls only LNB input attenuator
		//Program LNB attenuator (user controlled)
		attenuation_code_LNB = F1956_LUT(attenuation_dB_LNB);
		attenuation_code_TRX = 0;

		if(tmp_LNB_attenuator_code != attenuation_code_LNB){
			F1956_S21_SPI(attenuation_code_LNB, att_LNB_address);
			tmp_LNB_attenuator_code = attenuation_code_LNB;
		}

		if(tmp_RX_attenuator_code != attenuation_code_TRX){
			F1956_S21_SPI(attenuation_code_TRX, att_TRX_address);
			tmp_RX_attenuator_code = attenuation_code_TRX;
		}

	}

	//TX power control
	if(tx_status == true){
	 	attenuation_code_TRX = UINT8_MAX - map_uint8(tx_pwr, 0, 100, 0, F1956_MAX_CODE);

		if(tmp_RX_attenuator_code != attenuation_code_TRX){
			F1956_S21_SPI(attenuation_code_TRX, att_TRX_address);
			tmp_RX_attenuator_code = attenuation_code_TRX;
		}

	}

	return 0;
}

void process_GPIO_mcu(void){
	static uint8_t RX_LNA_last = 0;
	static uint8_t IF_LNA_last = 0;

	if(IF_LNA_EN != IF_LNA_last){
		HAL_GPIO_WritePin(IF_LNA_EN_GPIO_Port, IF_LNA_EN_Pin, IF_LNA_EN);
		IF_LNA_last = IF_LNA_EN;
	}

	if(RX_LNA_EN != RX_LNA_last){
		HAL_GPIO_WritePin(RX_LNA_EN_GPIO_Port, RX_LNA_EN_Pin, RX_LNA_EN);
		RX_LNA_last = RX_LNA_EN;
	}



}

uint32_t Brightness_To_PWM(uint16_t brightness){
    if (brightness > BRIGHTNESS_MAX) brightness = BRIGHTNESS_MAX;

    return PWM_MIN + ((uint64_t)brightness * (PWM_MAX - PWM_MIN) / BRIGHTNESS_MAX);
}


