/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED5_Pin GPIO_PIN_14
#define LED5_GPIO_Port GPIOC
#define ATT_LE_Pin GPIO_PIN_7
#define ATT_LE_GPIO_Port GPIOA
#define LO2_LD_Pin GPIO_PIN_4
#define LO2_LD_GPIO_Port GPIOC
#define IF_LNA_EN_Pin GPIO_PIN_5
#define IF_LNA_EN_GPIO_Port GPIOC
#define PSU_FLT_Pin GPIO_PIN_0
#define PSU_FLT_GPIO_Port GPIOB
#define LO2_Buf_EN_Pin GPIO_PIN_15
#define LO2_Buf_EN_GPIO_Port GPIOE
#define RX_LNA_EN_Pin GPIO_PIN_10
#define RX_LNA_EN_GPIO_Port GPIOB
#define LO2_CS_Pin GPIO_PIN_12
#define LO2_CS_GPIO_Port GPIOB
#define LO1_CS_Pin GPIO_PIN_14
#define LO1_CS_GPIO_Port GPIOB
#define LO1_CE_Pin GPIO_PIN_8
#define LO1_CE_GPIO_Port GPIOD
#define RX_EN_Pin GPIO_PIN_10
#define RX_EN_GPIO_Port GPIOD
#define LED_RX_Pin GPIO_PIN_11
#define LED_RX_GPIO_Port GPIOD
#define LED_TX_Pin GPIO_PIN_12
#define LED_TX_GPIO_Port GPIOD
#define LED4_Pin GPIO_PIN_13
#define LED4_GPIO_Port GPIOD
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_15
#define LED2_GPIO_Port GPIOD
#define PCF_INT_Pin GPIO_PIN_6
#define PCF_INT_GPIO_Port GPIOC
#define PCF_INT_EXTI_IRQn EXTI9_5_IRQn
#define FLT_5V5_Pin GPIO_PIN_7
#define FLT_5V5_GPIO_Port GPIOC
#define LO2_CE_Pin GPIO_PIN_8
#define LO2_CE_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_9
#define LED1_GPIO_Port GPIOC
#define LED0_Pin GPIO_PIN_8
#define LED0_GPIO_Port GPIOA
#define LO1_LD_Pin GPIO_PIN_11
#define LO1_LD_GPIO_Port GPIOA
#define Buzzer_Pin GPIO_PIN_15
#define Buzzer_GPIO_Port GPIOA
#define TOUCH_IRQ_Pin GPIO_PIN_3
#define TOUCH_IRQ_GPIO_Port GPIOD
#define TOUCH_CS_Pin GPIO_PIN_4
#define TOUCH_CS_GPIO_Port GPIOD
#define TFT_DC_Pin GPIO_PIN_5
#define TFT_DC_GPIO_Port GPIOD
#define TFT_CS_Pin GPIO_PIN_6
#define TFT_CS_GPIO_Port GPIOD
#define TFT_RESET_Pin GPIO_PIN_7
#define TFT_RESET_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define MAX_VIN 16
#define MIN_VIN 10
#define MAX_IIN 1

#define MAX_IN_PWR 39
#define MAX_OUT_PWR 0.25f

#define MAX_5V5 5.58f
#define MIN_5V5 5.4f
#define MIN_3V6 3.5f
#define MAX_3V6 3.7f

#define LNBV_H_MAX 19
#define LNBV_H_MIN 17
#define LNBV_V_MAX 14
#define LNBV_V_MIN 13

#define MAX_TEMP_AMP 80
#define MAX_TEMP_MB 70
#define MAX_TEMP_MCU 70

#define ADC_BUFFER_SIZE 7
#define AVG_TIMES 9

#define BRIGHTNESS_MIN      0
#define BRIGHTNESS_MAX      10

#define PWM_MIN             4000
#define PWM_MAX             60000   // or your actual timer ARR/CCR max
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
