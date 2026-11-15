/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "dma.h"
#include "spi.h"
#include "adc.h"
#include "gpio.h"
#include "math.h"

#include "F1956.h"
#include "LMX2572.h"
#include "frequency_functions.h"
#include "TCA9554.h"

#include "GPIO_expander.h"
#include "MCP9800.h"
#include "MCP4725.h"

#include "stdbool.h"

#include "ili9341_font.h"
#include "ili9341_gfx.h"
#include "ili9341.h"
#include "splash.h"
#include "globals.h"
#include "helper_functions.h"
#include "calibration.h"
#include "24LC_EEPROM.h"
#include "NV_data.h"
#include "error_handler.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "usart.h"
#include <semphr.h>

#include "tim.h"

#include "BASIC.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */


/* USER CODE END Variables */
/* Definitions for Main_task */
osThreadId_t Main_taskHandle;
const osThreadAttr_t Main_task_attributes = {
  .name = "Main_task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for ADC_task */
osThreadId_t ADC_taskHandle;
const osThreadAttr_t ADC_task_attributes = {
  .name = "ADC_task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh1,
};
/* Definitions for Display_task */
osThreadId_t Display_taskHandle;
const osThreadAttr_t Display_task_attributes = {
  .name = "Display_task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for Communication_t */
osThreadId_t Communication_tHandle;
const osThreadAttr_t Communication_t_attributes = {
  .name = "Communication_t",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Main_task_start(void *argument);
void ADC_task_start(void *argument);
void Display_task_start(void *argument);
void Communication_task_start(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
	i2cMutex = xSemaphoreCreateMutex();
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Main_task */
  Main_taskHandle = osThreadNew(Main_task_start, NULL, &Main_task_attributes);

  /* creation of ADC_task */
  ADC_taskHandle = osThreadNew(ADC_task_start, NULL, &ADC_task_attributes);

  /* creation of Display_task */
  Display_taskHandle = osThreadNew(Display_task_start, NULL, &Display_task_attributes);

  /* creation of Communication_t */
  Communication_tHandle = osThreadNew(Communication_task_start, NULL, &Communication_t_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_Main_task_start */
/**
  * @brief  Function implementing the Main_task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Main_task_start */
void Main_task_start(void *argument)
{
  /* USER CODE BEGIN Main_task_start */
	TIM4->CCR4 = Brightness_To_PWM(lcd_brightness);

	 TCA9554_HandleTypeDef GPIO_expander =
	 {
			  .hi2c = &hi2c1,
			  .address = TCA9554_ADDRESS,
			  .io_configuration = 0b00100010,		//OUT2 OUT1 PTT PA_EN LNB_POL LNB_SHDN LNB_FLT TX_EN
			  .io_states = 0b00000000,
			  .pol_inversion = 0x0
	 };

	TCA9554_init(&GPIO_expander);


	error_handler(HW_Init_POST(&GPIO_expander, 1));


	static MCP4725_HandleTypeDef VCTCXO_DAC =
	{
			.hi2c = &hi2c1,
			.id = 1,
			.i2c_address = MCP4725_VCTCXO_DAC_ADDRESS,
	};


	static LMX2572_HandleTypeDef pll1 =
	  {
	      .hspi    = &hspi2,
	      .cs_port = LO1_CS_GPIO_Port,
	      .cs_pin  = LO1_CS_Pin,
		  .ref_freq_hz = 48000000UL,
		  .ref_div = 1,
		  .id = 1
	  };

	 static LMX2572_HandleTypeDef pll2 =
	  {
	      .hspi    = &hspi2,
	      .cs_port = LO2_CS_GPIO_Port,
	      .cs_pin  = LO2_CS_Pin,
		  .ref_freq_hz = 48000000UL,
		  .ref_div = 48*5,
		  .id = 2
	  };

	 static uint8_t last_Remote_mode = 0;

	HAL_GPIO_WritePin(LO1_CS_GPIO_Port, LO1_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LO2_CS_GPIO_Port, LO2_CS_Pin, GPIO_PIN_SET);

	osDelay (500);

	TIM2->CNT = 32767;

	//Read NV structures form EEPROM
	//I2C_Scan();  //Scan I2C addresses
	xSemaphoreTake(i2cMutex, portMAX_DELAY);
	CRC_CH_Read_fail = EEPROM_Read_CH();
	HW_Init_POST(&GPIO_expander, 2);

	CRC_MB_Read_fail = EEPROM_Read_MB();
	HW_Init_POST(&GPIO_expander, 3);

	CRC_LO1_Read_fail = EEPROM_Read_LO1();
	HW_Init_POST(&GPIO_expander, 4);

	CRC_LO2_Read_fail = EEPROM_Read_LO2();
	HW_Init_POST(&GPIO_expander, 5);

	xSemaphoreGive(i2cMutex);
	load_structs();

	MCP4725_SetValue(&VCTCXO_DAC, PLL1_NV.VCTCXO_cal);

	init_synthesizers(&pll1, &pll2);

	HW_Init_POST(&GPIO_expander, 6);

	HAL_TIM_Base_Start_IT(&htim10); //Timer for periodic save functionality
	UART_RX_semaphore = 0;

	HW_Init_POST(&GPIO_expander, 7);
	HW_Init_POST(&GPIO_expander, 8);
	HW_Init_POST(&GPIO_expander, 9);
	HW_Init_POST(&GPIO_expander, 10);

	F1956_SetSPIHandle(&hspi3);
	F1956_S21_SPI(F1956_LUT(attenuation_dB_LNB), att_LNB_address);
	F1956_S21_SPI(F1956_LUT(attenuation_dB_TRX), att_TRX_address);

	//Basic Automated SCPI Interpreter for Controllers
	BASIC_Init();

	uint32_t main_loop_counter1 = 0;

  /* Infinite loop */
  for(;;){
	process_encoder();
	process_buttons();
	xSemaphoreTake(i2cMutex, portMAX_DELAY);
	process_nv_writes();
	xSemaphoreGive(i2cMutex);
	process_attenuators();	//Rewrite the F1956 driver to use handletypedef
	process_synthesizers(&pll1, &pll2);


	process_GPIO_mcu();
	process_GPIO_expander(&GPIO_expander);
	if((GPIO_EXP_INT == true) || (last_Remote_mode != Remote_Mode)){
		osDelay(5);
		tx_status = Get_PTT_State(&GPIO_expander);


		process_synthesizers(&pll1, &pll2);
		if(tx_status == 1){
			HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_SET);
			display_scene = 1;
		}
		else if(tx_status == 0){
			HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_RESET);
			display_scene = 0;
		}

		LNB_FLT = Get_LNB_FLT(&GPIO_expander);

		GPIO_EXP_INT = false;
		last_Remote_mode = Remote_Mode;
	}


	//Process less frequent tasks
	main_loop_counter1++;
	if(main_loop_counter1 > 1e2){

		if(main_loop_counter1 >=1e2){
			main_loop_counter1 = 0;
		}
	}

    osDelay(5);
    //HAL_UART_Transmit_DMA(&huart1, self_test_Str, strlen((char *)self_test_Str));


  }
  /* USER CODE END Main_task_start */
}

/* USER CODE BEGIN Header_ADC_task_start */
/**
* @brief Function implementing the ADC_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ADC_task_start */
void ADC_task_start(void *argument)
{
  /* USER CODE BEGIN ADC_task_start */
	uint32_t ADC_val_sum[ADC_BUFFER_SIZE];
	uint8_t counter = 0;

	float RF_det_tx_U = 0;
	float RF_det_in_U = 0;

	//uint8_t temp_raw[2];
	//uint16_t temp_val = 0;
	//int16_t signed_temp = 0;
	//HAL_StatusTypeDef tmp_read_status;

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_val, ADC_BUFFER_SIZE);

	MCP9800_HandleTypeDef tx_amp_temperature_sensor =
	{
			.address = MCP9800_amp_addr,
			.hi2c    = &hi2c1,
			.resolution = MCP9800_RES_12BIT,
			.oneshot = false,
			.fault_queue = MCP9800_FAULT_1,
			.alert_polarity = 0,
			.comp_int = 0,
			.shutdown = 0,
			.temp_hyst = 80,
			.temp_limit = 85
	};

	MCP9800_init(&tx_amp_temperature_sensor);



  /* Infinite loop */
  for(;;)
  {
	  for(uint8_t i = 0; i < ADC_BUFFER_SIZE; i++){
		  ADC_val_sum[i] += ADC_val[i];
	  }
	  counter++;

	  if(counter == AVG_TIMES){
		  for(uint8_t i = 0; i < ADC_BUFFER_SIZE; i++){
			  ADC_avg_val[i] = ADC_val_sum[i]/AVG_TIMES;
			  ADC_val_sum[i] = 0;
		  }
		  counter = 0;

			PSU_Vin  = (PSU_Vin_Calibration * adcValueToVoltage(ADC_avg_val[0])) - PSU_Vin_Calibration_offset;
			PSU_5V5  = PSU_5V5_Calibration * adcValueToVoltage(ADC_avg_val[5]);
			PSU_3V6  = PSU_3V6_Calibration * adcValueToVoltage(ADC_avg_val[6]);
			LNB_V    = (LNB_V_Calibration * adcValueToVoltage(ADC_avg_val[1])) - LNB_V_Calibration_offset;
			Imon_I   = (Imon_Calibration * adcValueToVoltage(ADC_avg_val[2])) - Imon_Calibration_offset;

			RF_det_in_U = adcValueToVoltage(ADC_avg_val[3]);
			if(tx_status == 1){
				RF_det_tx_U = adcValueToVoltage(ADC_avg_val[4]);

				DET_out_pwr = RF_det_tx_calA * powf(RF_det_tx_U, 2) + RF_det_tx_calB * (RF_det_tx_U) + RF_det_tx_calC + sampler_output_cal;
				//DET_out_pwr = DET_out_pwr + sampler_output_cal;	//Output in dBm

				RF_in_pwr = RF_det_rx_calA * powf(RF_det_in_U, 2) + RF_det_rx_calB * (RF_det_in_U) + RF_det_rx_calC + 37; //Input detector + attenuator
			}
			else{

				DET_rx_pwr = (RF_det_rx_calA * powf(RF_det_in_U, 2) + RF_det_rx_calB * (RF_det_in_U) + RF_det_rx_calC); //Level at the detector during RX
				if(RX_LNA_EN == 0){
					RF_RSSI = DET_rx_pwr + RX_CPL + attenuation_dB_LNB;	//Signal strength from LNB
				}
				else{
					RF_RSSI = DET_rx_pwr + RX_CPL + attenuation_dB_LNB + RX_LNA_GAIN;
				}
			}

			//Readout temperature from sensors
			temp_amp = MCP9800_read_temperature(&tx_amp_temperature_sensor);

			if(display_scene != 2){ //Ensure we are past BOOT screen (POST is not runing)
				Built_In_Test();
			}
	  }


    osDelay(10);
  }
  /* USER CODE END ADC_task_start */
}

/* USER CODE BEGIN Header_Display_task_start */
/**
* @brief Function implementing the Display_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Display_task_start */
void Display_task_start(void *argument)
{
  /* USER CODE BEGIN Display_task_start */
	  //Splash screen
	display_scene = 2;
	  ili9341_t *_screen;

	  ili9341_t *screen(void){
		  return _screen;
	  }


	  _screen = ili9341_new(
		        &hspi1,
		        TFT_RESET_GPIO_Port, TFT_RESET_Pin,
		        TFT_CS_GPIO_Port,    TFT_CS_Pin,
		        TFT_DC_GPIO_Port,    TFT_DC_Pin,
		        isoLandscape,
		        TOUCH_CS_GPIO_Port,  TOUCH_CS_Pin,
		        TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin,
		        itsSupported,
				itnNormalized);

	  ili9341_t *lcd = screen();
	  	ili9341_fill_screen(lcd, ILI9341_BLACK);
	  	ili9341_draw_border(lcd, ILI9341_ORANGE);

	      ILI9341_DrawImage((ILI9341_WIDTH - 160)/2,(ILI9341_HEIGHT-180)/2, 160, 160,
	      (const uint16_t*)splash_160x160);

	  	uint16_t LCD_bright_pwm = 32000;
	  	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
	  	TIM4->CCR4 = LCD_bright_pwm;

	      ili9341_text_attr_t splash_tittle_attr = (ili9341_text_attr_t){
	      	.font = &ili9341_font_16x26,
	      	.fg_color = ILI9341_ORANGE,
	  		.bg_color = ILI9341_BLACK,
	  		.origin_x = 15,
	  		.origin_y = 5
	      };

	      ili9341_text_attr_t error_txt_attr = (ili9341_text_attr_t){
	      	.font = &ili9341_font_16x26,
	      	.fg_color = ILI9341_WHITE,
	  		.bg_color = ILI9341_RED,
	  		.origin_x = 15,
	  		.origin_y = 186
	      };

	      ili9341_draw_string(lcd, splash_tittle_attr, "PRESTO TRANSVERTER");
	      splash_tittle_attr.origin_y = 190;
	      splash_tittle_attr.font = &ili9341_font_11x18;
	      ili9341_draw_string(lcd, splash_tittle_attr, "by SP6GK         sw ver 1.0"); //TODO change to read from ZCH NV


	  //Live update of progress bar
	  while(POST_Status < POST_Steps){

		  if(error_code != 0){
			  ili9341_draw_string(lcd, error_txt_attr, error_strings[error_code]);
		  }

		    ili9341_fill_rect(lcd, ILI9341_GREEN, 10, 210, ((POST_Status)*300 / POST_Steps), 18);
		    if(POST_Status < POST_Steps/4){
		    	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
		    }
		    else if((POST_Status < POST_Steps/3) && (POST_Status > POST_Steps/4)){
		    	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		    	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
		    }
		    else if((POST_Status < POST_Steps/2) && (POST_Status > POST_Steps/3)){
		    	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		    	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
		    }
		    else if(((POST_Status < POST_Steps) && (POST_Status > POST_Steps/2)) && (POST_Status < POST_Steps-2)){ //Last 2 steps take control over LEDs
		    	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
		    	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
		    }
		    else if(POST_Status >= POST_Steps){
		    	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
		    }
	  }

	  //Initialize buttons LED
	  if(vfo_track == 1){
		  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
	  }
	  if(vfo_lock == 1){
		  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
	  }
	  //osDelay(1000);

	  display_scene = 0;
	  uint16_t screen_drawn = 2;
	  //uint16_t drawn_buttons = 0;
	  float disp_temperature = 0;
	  //static uint16_t last_select_menu = 0;

	  static bool last_Remote_mode = 0;

  /* Infinite loop */
  for(;;)
  {
	 draw_title_bar(lcd); //Prints presto transverter at the top if everything is ok, otherwise displays error code

	if(((display_scene == 0) && (screen_drawn != 0)) || (last_Remote_mode != Remote_Mode)){ //display_scene is a desired static elements of lcd, screen_drawin is a status ie. what is actually drawn
		//Default RX screen
		screen_drawn = rx_screen_display_scene(lcd);
		draw_selection_rectangle(lcd, screen_drawn);
		draw_buttons(lcd, screen_drawn, 1);
		draw_selection_rectangle(lcd, display_scene);
		last_Remote_mode = Remote_Mode;
	}
	else if(((display_scene == 1) && (screen_drawn != 1)) || (last_Remote_mode != Remote_Mode)){
		//TX screen
		screen_drawn = tx_screen_display_scene(lcd);
		draw_selection_rectangle(lcd, screen_drawn);
		draw_buttons(lcd, screen_drawn, 1);
		draw_selection_rectangle(lcd, display_scene);
		last_Remote_mode = Remote_Mode;
	}
	else if(buttons_status[3] == 1){
		//Entry point for menu mode

		if(select_position_menu_global <= 5){		//Menu 1
			display_scene = 3;
			if(display_scene != screen_drawn){
				screen_drawn = menu1_display_scene(lcd); 	//Static elements (text)
				menu1_display_settings(lcd, 1);
			}
			menu1_display_settings(lcd, 0); //Update settings
		}
		else if((select_position_menu_global > 5) && (select_position_menu_global <= 10)){	 //Menu 2
			display_scene = 4;
			if(display_scene != screen_drawn){
				screen_drawn = menu2_display_scene(lcd); 	//Static elements (text)
				menu2_display_settings(lcd, 1);
			}
			menu2_display_settings(lcd, 0);
		}
		else if((select_position_menu_global > 10) && (select_position_menu_global <= 15)){	 //Menu 3
			display_scene = 5;
			if(display_scene != screen_drawn){
				screen_drawn = menu3_display_scene(lcd);	//Static elements (text)
				menu3_display_settings(lcd, 1);
			}
			menu3_display_settings(lcd, 0);
		}
		else if((select_position_menu_global > 15) && (select_position_menu_global <= 20)){	//Menu 3
			display_scene = 6;
			if(display_scene != screen_drawn){
				screen_drawn = menu4_display_scene(lcd);	//Static elements (text)
				menu4_display_diag(lcd, 1);
			}
			menu4_display_diag(lcd, 0);
		}

	}
	draw_selection_rectangle(lcd, screen_drawn);
	draw_buttons(lcd, screen_drawn, 0);


    //Print frequencies and RSSI
	if((display_scene == 0) || (display_scene == 1)){
		ili9341_text_attr_t float_txt_attr = (ili9341_text_attr_t){
			.font = &ili9341_font_11x18,
			.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 50,
			.origin_y = 26
		};
		char test_float_Str[32] = {'\0'};
		double freq_mhz = 0;

		if(tx_status == 0){
			if(RF_RSSI > -26){
				sprintf(test_float_Str, "%.1f dBm", RF_RSSI);
			}
			else if(RF_RSSI <= -26){
				sprintf(test_float_Str, "<%.0f dBm", -26.0f);
			}
		}
		else{
			if(DET_out_pwr <= -100){
				sprintf(test_float_Str, "%.0f dBm", DET_out_pwr);
			}
			else if(DET_out_pwr > -100){
				sprintf(test_float_Str, "%.1f dBm", DET_out_pwr);
			}
		}

		ili9341_draw_string(lcd, float_txt_attr, test_float_Str);


		ili9341_text_attr_t Freq_text_attr = (ili9341_text_attr_t){
			.font = &ili9341_font_16x26,
			.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 41,
			.origin_y = 62
		};

		freq_mhz = (double)DOWN_Link_F / 1e6;
		sprintf(test_float_Str, "%.3f MHz", freq_mhz);
		ili9341_draw_string(lcd, Freq_text_attr, test_float_Str);

		Freq_text_attr.origin_y = 100;
		Freq_text_attr.origin_x = 55;

		freq_mhz = (double)UP_Link_F / 1e6;
		sprintf(test_float_Str, "%.3f MHz", freq_mhz);
		ili9341_draw_string(lcd, Freq_text_attr, test_float_Str);

		Freq_text_attr.origin_y = 138;
		Freq_text_attr.origin_x = 71;

		freq_mhz = (double)TRX_F / 1e6;
		sprintf(test_float_Str, "%.4f MHz", freq_mhz);
		ili9341_draw_string(lcd, Freq_text_attr, test_float_Str);

		Update_meter_bar(lcd, test_float, 150, 26, 18, 95, tx_status);

		//Debug
/*
		for(uint8_t i = 0; i < 100; i++){
			test_float++;
			Update_meter_bar(lcd, test_float, 150, 26, 18, 95, tx_status);
			osDelay(5);
		}
		for(uint8_t i = 100; i > 0; i--){
			test_float--;
			Update_meter_bar(lcd, test_float, 150, 26, 18, 95, tx_status);
			osDelay(5);
		}
*/

		ili9341_text_attr_t bottom_txt_attr = (ili9341_text_attr_t){
			.font = &ili9341_font_11x18,
			.fg_color = ILI9341_WHITE,
			.bg_color = ILI9341_BLACK,
			.origin_x = 50,
			.origin_y = 170
		};
		ili9341_draw_string(lcd, bottom_txt_attr, "NOT FITTED");

		//LNB status
		bottom_txt_attr.origin_x = 50;
		bottom_txt_attr.origin_y = 190;
		if(LNB_setting != 0){
			if(LNB_setting == 1){
				sprintf(test_float_Str, "V %.1f V", LNB_V);
				ili9341_draw_string(lcd, bottom_txt_attr, test_float_Str);
			}
			else if(LNB_setting == 2){
				sprintf(test_float_Str, "H %.1f V", LNB_V);
				ili9341_draw_string(lcd, bottom_txt_attr, test_float_Str);
			}
		}
		else if(LNB_setting == 0){
			ili9341_draw_string(lcd, bottom_txt_attr, "OFF     ");
		}

		bottom_txt_attr.origin_x = 185;
		bottom_txt_attr.origin_y = 190;

		if(PSU_Vin < 10){
			sprintf(test_float_Str, "%.2f V", PSU_Vin);
		}
		else if((PSU_Vin >= 10) && (PSU_Vin < 100)){
			sprintf(test_float_Str, "%.1f V", PSU_Vin);
		}
		else if(PSU_Vin >= 100){
			sprintf(test_float_Str, "%.0f V", PSU_Vin);
		}
		ili9341_draw_string(lcd, bottom_txt_attr, test_float_Str);

		bottom_txt_attr.origin_y = 210;

		disp_temperature = temp_amp;
		if(disp_temperature < 99){
			sprintf(test_float_Str, "%.1f", disp_temperature);
			ili9341_draw_string(lcd, bottom_txt_attr, test_float_Str);
		}
		else{
			ili9341_draw_string(lcd, bottom_txt_attr, ">99");
		}
		ili9341_draw_circle(lcd, ILI9341_WHITE, 235, 212, 2);
		bottom_txt_attr.origin_x = 240;
		ili9341_draw_string(lcd, bottom_txt_attr, "C");

		bottom_txt_attr.origin_x = 50;
		bottom_txt_attr.origin_y = 210;

		if(RX_LNA_EN == 0){
			ili9341_draw_string(lcd, bottom_txt_attr, "OFF");
		}
		else if(RX_LNA_EN == 1){
			ili9341_draw_string(lcd, bottom_txt_attr, "ON ");
		}

	}


    osDelay(10);
  }
  /* USER CODE END Display_task_start */
}

/* USER CODE BEGIN Header_Communication_task_start */
/**
* @brief Function implementing the Communication_t thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Communication_task_start */
void Communication_task_start(void *argument)
{
  /* USER CODE BEGIN Communication_task_start */
	//Note: in future projects it would be better to handle BASIC commands here instead of ISR
	uint16_t heartbeat_led_delay = 100;
  /* Infinite loop */
  for(;;)
  {
	  if(SRV_Mode == 0){
		  osDelay(5*heartbeat_led_delay);
	  }
	  else if(SRV_Mode == 1){
		  osDelay(heartbeat_led_delay);
	  }
		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);


		if((UART_RX_semaphore == 1) && (display_scene != 2)){
			BASIC_cmd(uart_rxBUffer, strlen((char *)uart_rxBUffer));
			memset(uart_rxBUffer, 0, sizeof(uart_rxBUffer));

			UART_RX_semaphore = 0;

		}


  }
  /* USER CODE END Communication_task_start */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

