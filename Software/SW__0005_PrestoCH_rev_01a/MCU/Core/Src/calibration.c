/*
 * calibration.c
 *
 *  Created on: Sep 19, 2025
 *      Author: sp6gk
 */

#include "calibration.h"


//Default calibration values
uint16_t LO1_PLL_Cal = (2^12)/2;	//12 is the number of bits of tuning DAC
uint16_t LO2_PLL_Cal = (2^12)/2;

float RSSI_Calibration[3] = {1.0, 1.0, 1.0};
float RF_in_pwr_Calibration[3] = {1.0, 1.0, 1.0};
float RF_out_pwr_Calibration[3] = {1.0, 1.0, 1.0};

//Default calibration values in case of EEPROM failure on boot
float PSU_Vin_Calibration = 8.139;
float PSU_Vin_Calibration_offset = 0;
float PSU_5V5_Calibration = 2.085;
float PSU_3V6_Calibration = 2.085;
float LNB_V_Calibration = 6.8;
float LNB_V_Calibration_offset = 0;

float Imon_Calibration = 0.45;
float Imon_Calibration_offset = 0;

float RF_det_rx_calA = -4.586;
float RF_det_rx_calB = 62.155;
float RF_det_rx_calC = -96.418;

float RF_det_tx_calA = 2.558;
float RF_det_tx_calB = 43.030;
float RF_det_tx_calC = -88.475;


uint16_t DAC_VCTCXO = UINT16_MAX/2;

//RSSI and detector calibration

//Read only
float sampler_output_cal = 23.5;
