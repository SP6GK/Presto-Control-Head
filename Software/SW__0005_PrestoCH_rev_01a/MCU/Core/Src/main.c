/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "F1956.h"

#include "stdbool.h"

#include "ili9341_font.h"
#include "ili9341_gfx.h"
#include "ili9341.h"
#include "splash.h"
#include "globals.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "BASIC.h"
#include "NV_data.h"
#include "24LC_EEPROM.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
volatile uint64_t Tim10_Counter;


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//SPI_HandleTypeDef hspi2;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI3_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_CRC_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, uart_rxBUffer, UART_RX_BUFFER_SIZE);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //Splash screen

/*
      testScreenFill(lcd);
      testScreenLines(lcd);
      HAL_Delay(2000);
      testScreenRects(lcd);
      HAL_Delay(2000);
      testScreenCircles(lcd);
      HAL_Delay(2000);
      testScreenText(lcd);
      HAL_Delay(2000);

*/



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	// External interrupts from buttons / input lines


//Define button pressed 1-4, interrupts set values to 1, other function in main task, checks on every run of state has changed, then serves if it did, then resets state
	if (GPIO_Pin == 1024 && (tx_status != 1)){
		buttons_status[0] = 1;
	}
	else if(GPIO_Pin == 512 && (tx_status != 1)){
    	buttons_status[1] = 1;
    }
	else if(GPIO_Pin == 256 && (tx_status != 1)){
		buttons_status[2] = 1;
	}
	else if((GPIO_Pin == 128) && (tx_status != 1)){
		buttons_status[3] = 1 - buttons_status[3];
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

		if(display_scene == 0){
			display_scene = 3;
		}
		else if(((display_scene >= 3) && (display_scene <= 6)) && buttons_status[3] == 0){
			display_scene = 0;
		}

	}

	else if((GPIO_Pin == 64) && (GPIO_EXP_INT != true)){	//TCA9554
		GPIO_EXP_INT = true;
	}

    else {
    	//Undefined interrupt number

    }


	//TODO redefine fault lines as interrupts.

}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	memset(uart_txBuffer, 0, sizeof(uart_txBuffer)); //Clear the buffer
}



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	//Semaphore could be replaced with FreeRTOS Mutex but instant diregard of message and signaling of NDAC was chosen as preferred method over waiting
	if(UART_RX_semaphore == 0){
		//Received command while system is free to respond
		UART_RX_semaphore = 1;


	}
	else if(UART_RX_semaphore != 0){
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)NDAC, strlen((char *)NDAC));
	}

	HAL_UARTEx_ReceiveToIdle_IT(&huart1, uart_rxBUffer, UART_RX_BUFFER_SIZE);
}



//Test functions

void testScreenFill(ili9341_t *lcd)
{
  ili9341_fill_screen(lcd, ILI9341_BLUE);
  HAL_Delay(500);
  ili9341_fill_screen(lcd, ILI9341_RED);
  HAL_Delay(500);
  ili9341_fill_screen(lcd, ILI9341_GREEN);
  HAL_Delay(500);
}

void testScreenLines(ili9341_t *lcd)
{
  ili9341_fill_screen(lcd, ILI9341_BLACK);

  ili9341_color_t color;
  uint8_t wheel = 0U;

  for (int32_t i = 0; i < lcd->screen_size.height; i += 10)
    { color = ili9341_color_wheel(&wheel); wheel += 3;
      ili9341_draw_line(lcd, color, 0, 0, lcd->screen_size.width, i); }
  for (int32_t i = lcd->screen_size.width; i >= 0; i -= 10)
    { color = ili9341_color_wheel(&wheel); wheel += 3;
      ili9341_draw_line(lcd, color, 0, 0, i, lcd->screen_size.height); }

  HAL_Delay(500);

  for (int32_t i = 0; i < lcd->screen_size.height; i += 10)
    { color = ili9341_color_wheel(&wheel); wheel += 3;
      ili9341_draw_line(lcd, color, lcd->screen_size.width, lcd->screen_size.height, 0, lcd->screen_size.height - i); }
  for (int32_t i = lcd->screen_size.width; i >= 0; i -= 10)
    { color = ili9341_color_wheel(&wheel); wheel += 3;
      ili9341_draw_line(lcd, color, lcd->screen_size.width, lcd->screen_size.height, lcd->screen_size.width - i, 0); }
}

void testScreenRects(ili9341_t *lcd)
{
  ili9341_fill_screen(lcd, ILI9341_BLACK);

  ili9341_color_t color;
  uint8_t wheel = 0U;

  for (uint16_t i = 0, in = 0; i < lcd->screen_size.width; i += 16, ++in) {
    for (uint16_t j = 0, jn = 0; j < lcd->screen_size.height; j += 16, ++jn) {
      color = ili9341_color_wheel(&wheel);
      if ((in & 1) == (jn & 1)) {
        ili9341_fill_rect(lcd, color, i, j, 16, 16);
      } else {
        ili9341_draw_rect(lcd, ~color, i+2, j+2, 12, 12);
      }
    }
  }
}

void testScreenCircles(ili9341_t *lcd)
{
  ili9341_fill_screen(lcd, ILI9341_BLACK);

  uint16_t x_mid = lcd->screen_size.width / 2;
  uint16_t y_mid = lcd->screen_size.height / 2;
  uint16_t radius = x_mid;
  if (radius < y_mid)
    { radius = y_mid; }

  ili9341_color_t color;
  uint8_t wheel = 0U;

  for (int16_t i = radius; i >= 0; i -= 6) {
    color = ili9341_color_wheel(&wheel);
    wheel += 9;
    ili9341_fill_circle(lcd, color, x_mid, y_mid, i);
  }

  HAL_Delay(500);

  for (int16_t i = 0, n = 0; i < lcd->screen_size.width; i += 24, ++n) {
    for (int16_t j = 0, m = 0; j < lcd->screen_size.height; j += 24, ++m) {

      color = ili9341_color_wheel(&wheel);
      wheel += 3;

      if ((n & 1) == (m & 1)) {
        ili9341_draw_circle(lcd, color, i + 12, j + 12, 16);
        ili9341_draw_circle(lcd, color, i + 12, j + 12, 10);
        ili9341_draw_circle(lcd, color, i + 12, j + 12, 4);
      }
      else {
        ili9341_draw_circle(lcd, color, i + 12, j + 12, 8);
      }
    }
  }
}

void testScreenText(ili9341_t *lcd)
{
  ili9341_fill_screen(lcd, ILI9341_BLACK);

  ili9341_text_attr_t textAttr = (ili9341_text_attr_t){
    .font = &ili9341_font_11x18,
    .fg_color = ILI9341_WHITE,
    .bg_color = ILI9341_RED,
    .origin_x = 10,
    .origin_y = 10
  };
  ili9341_draw_string(lcd, textAttr, "Hellord!");

}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM11 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
	if(htim->Instance==TIM10){
		Tim10_Counter = HAL_GetTick();

		if(TRX_F != CH_NV.TRX_F){
			NV_Write_TRX = WRITE_PENDING;
		}
		if(UP_Link_F != CH_NV.UP_Link_F){
			NV_Write_UP = WRITE_PENDING;
		}
		if(DOWN_Link_F != CH_NV.DOWN_Link_F){
			NV_Write_Down = WRITE_PENDING;
		}
  }
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM11) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
