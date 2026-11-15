/*
 * globals.h
 *
 *  Created on: Sep 14, 2025
 *      Author: sp6gk
 */

#ifndef INC_GLOBALS_H_
#define INC_GLOBALS_H_

#include "main.h"
#include "cmsis_os.h"
#include "stdbool.h"
#include <semphr.h>


#define UART_RX_BUFFER_SIZE 256
#define UART_TX_BUFFER_SIZE 256

#define ATT_MAX 31.75f
#define RF_DET_RX_CAL_A_MIN -20
#define RF_DET_RX_CAL_A_MAX 20
#define RF_DET_RX_CAL_B_MIN -100
#define RF_DET_RX_CAL_B_MAX 100
#define RF_DET_RX_CAL_C_MIN -100
#define RF_DET_RX_CAL_C_MAX 100

#define RF_DET_TX_CAL_A_MIN -10
#define RF_DET_TX_CAL_A_MAX 10
#define RF_DET_TX_CAL_B_MIN -100
#define RF_DET_TX_CAL_B_MAX 100
#define RF_DET_TX_CAL_C_MIN -100
#define RF_DET_TX_CAL_C_MAX 100

#define SAMPLER_CAL_MIN 3
#define SAMPLER_CAL_MAX 50
#define LNB_V_CAL_MIN 5
#define LNB_V_CAL_MAX 10
#define LNB_V_CAL_OFFSET_MIN -1
#define LNB_V_CAL_OFFSET_MAX 1

#define PSU_VIN_CAL_MIN 5
#define PSU_VIN_CAL_MAX 10

#define PSU_VIN_CAL_OFFSET_MIN -1
#define PSU_VIN_CAL_OFFSET_MAX 1

#define P3V6_CALA_MIN 1
#define P3V6_CALA_MAX 10
#define P5V5_CALA_MIN 1
#define P5V5_CALA_MAX 10

#define IMON_CAL_MIN 0.1f
#define IMON_CAL_MAX 3
#define IMON_CAL_OFFSET_MIN -1
#define IMON_CAL_OFFSET_MAX 1

#define POST_STEPS_SUCESS 19
#define BIT_CLEAR_COUNTER 100

#define RX_DET_THR -26.0f

extern uint8_t SW_MAJ_VER;
extern char    SW_MIN_Ver;

//UART buffer and control
extern uint8_t uart_rxBUffer[UART_RX_BUFFER_SIZE];
extern uint8_t uart_txBuffer[UART_TX_BUFFER_SIZE];
extern bool UART_RX_semaphore;

extern SemaphoreHandle_t i2cMutex;

// ADC DMA Buffer
extern uint16_t ADC_val[ADC_BUFFER_SIZE];
extern uint16_t ADC_avg_val[ADC_BUFFER_SIZE];

extern volatile float PSU_Vin;
extern float PSU_5V5;
extern float PSU_3V6;
extern float LNB_V;
extern float DET_rx_pwr;
extern float DET_out_pwr;
extern float RF_in_pwr;
extern float RF_RSSI;
extern float RF_out_pwr;
extern float Imon_V;
extern float Imon_I;

extern float temp_amp;
extern float temp_mb;
extern float temp_mcu;

extern int32_t encoder_counter_val;

// PLL and LO
extern uint64_t LO1_F;
extern uint64_t LO2_F;
extern uint64_t TRX_F;
extern uint64_t UP_Link_F;
extern uint64_t DOWN_Link_F;

extern const int32_t Freq_steps[];
extern uint32_t f_step;

extern uint64_t UP_link_high_f;
extern uint64_t UP_link_low_f;
extern uint64_t DOWN_link_high_f;
extern uint64_t DOWN_link_low_f;
extern uint64_t LNB_LO;
extern uint64_t IF;

// Attenuators
extern float attenuation_dB_LNB;
extern float attenuation_dB_TRX;
extern uint8_t attenuation_code_LNB;
extern uint8_t attenuation_code_TRX;
extern uint8_t att_LNB_address;
extern uint8_t att_TRX_address;

extern uint8_t tx_pwr;

extern uint8_t AGC_setting;
extern uint8_t TRX_RX;
//LNB PSU
extern uint8_t LNB_setting;

// Flags
extern uint16_t POST_Status;
extern uint16_t POST_Steps;
extern uint16_t display_scene;
extern bool PSU_FLT;
extern bool PSU_5V5_FLT;
extern bool LNB_FLT;
extern bool tx_status;
extern bool LNB_POL_Horizontal;
extern bool LNB_EN;
extern bool RX_LNA_EN;
extern bool IF_LNA_EN;
extern uint8_t tmp_sens_disp;

extern uint16_t lcd_brightness;

extern bool GPIO_EXP_INT;

extern bool vfo_track;
extern bool vfo_lock;
extern uint8_t vfo_select;
extern bool PTT_Polarity;

extern bool out1_status;
extern bool out2_status;

extern bool buttons_status[4];
extern uint16_t select_position_menu_global;

extern bool NV_Write_UP;
extern bool NV_Write_Down;
extern bool NV_Write_TRX;

extern bool Remote_Mode;

//Service mode
extern uint8_t SRV_Mode;


// Test variables
extern uint16_t test;
extern float test_float;


//BIT
extern bool CRC_CH_Read_fail;
extern bool CRC_MB_Read_fail;
extern bool CRC_LO1_Read_fail;
extern bool CRC_LO2_Read_fail;


extern uint16_t error_code;
extern char error_string[18];

//Error code list
#define NERRORS 4
extern uint16_t error_codes[NERRORS];
extern uint16_t error_pointer;


#endif /* INC_GLOBALS_H_ */

