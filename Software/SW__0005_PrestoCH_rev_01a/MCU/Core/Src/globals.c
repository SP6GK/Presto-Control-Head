/*
 * globals.c
 *
 *  Created on: Sep 14, 2025
 *      Author: sp6gk
 */
#include "globals.h"


uint8_t SW_MAJ_VER = 1;
char    SW_MIN_Ver = 'A';

uint8_t uart_rxBUffer[UART_RX_BUFFER_SIZE] = {0};
uint8_t uart_txBuffer[UART_TX_BUFFER_SIZE] = {0};
bool UART_RX_semaphore = 1;

SemaphoreHandle_t i2cMutex;

// ADC DMA Buffer
uint16_t ADC_val[ADC_BUFFER_SIZE];
uint16_t ADC_avg_val[ADC_BUFFER_SIZE];

volatile float PSU_Vin = 0;
float PSU_5V5 = 0;
float PSU_3V6 = 0;
float LNB_V = 0;
float DET_rx_pwr = 0;	//Input RF detector
float DET_out_pwr = 0;	//Output RF detector
float RF_in_pwr = 0;
float RF_RSSI = -120;
float RF_out_pwr = 0;
float Imon_V = 0;
float Imon_I = 0;

float temp_amp = 0;
float temp_mb = 0;
float temp_mcu = 0;

int32_t encoder_counter_val = 0;

// PLL and LO
uint64_t LO1_F = 430e6;
uint64_t LO2_F = 1970e6;
uint64_t TRX_F = 29e6;
uint64_t UP_Link_F = 2400e6;
uint64_t DOWN_Link_F = 12000e6;
uint32_t f_step = 1;
const int32_t Freq_steps[] = {100, 1e3, 10e3, 100e3, 1e6};

uint64_t UP_link_high_f = 3e9;
uint64_t UP_link_low_f = 2e9;
uint64_t DOWN_link_high_f = 12750000000;
uint64_t DOWN_link_low_f = 10480000000;
uint64_t LNB_LO = 9750e6;
uint64_t IF = 433920000;

// Attenuators
float attenuation_dB_LNB = 0;
float attenuation_dB_TRX = 0;
uint8_t attenuation_code_LNB = 0;
uint8_t attenuation_code_TRX = 0;
uint8_t att_LNB_address = 0b000;
uint8_t att_TRX_address = 0b001;

uint8_t tx_pwr = 0; //tx power setting

uint8_t AGC_setting = 0;
uint8_t TRX_RX = 0;		//Use TRX as a second receiver

//LNB PSU
uint8_t LNB_setting = 0;

// Flags
uint16_t POST_Status = 0;
uint16_t POST_Steps = 19;
uint16_t display_scene = 0; //0 -> RX screen, 1 -> TX screen, 2 -> Boot (POST), 3, 4, 5-> Menu
bool PSU_FLT = false;
bool PSU_5V5_FLT = 0;
bool LNB_FLT = 0;
bool tx_status = false;
bool LNB_POL_Horizontal = false; //false -> vertical, true -> horizontal
bool LNB_EN = false;
bool RX_LNA_EN = false;
bool IF_LNA_EN = false;
uint8_t tmp_sens_disp = 0;

uint16_t lcd_brightness = 1;

bool GPIO_EXP_INT = false;

bool NV_Write_UP = 0;
bool NV_Write_Down = 0;
bool NV_Write_TRX = 0;

bool vfo_track = true;
bool vfo_lock = false;
uint8_t vfo_select = 1; // 1 -> Down, 2 -> UP, 3 -> TRX
bool PTT_Polarity = 0;

bool out1_status = 0;
bool out2_status = 0;

bool buttons_status[4] = {0};
uint16_t select_position_menu_global = 1;


bool Remote_Mode = 0;

//Service mode
uint8_t SRV_Mode = 0;


//BIT
bool CRC_CH_Read_fail  = true;
bool CRC_MB_Read_fail  = true;
bool CRC_LO1_Read_fail = true;
bool CRC_LO2_Read_fail = true;


uint16_t error_code = 0;
char error_string[18] = {0};

//Error code list

uint16_t error_codes[NERRORS];
uint16_t error_pointer = 0;

// Test variables
uint16_t test = 0;
float test_float = 0.0f;


