/*
 * calibration.h
 *
 *  Created on: Sep 19, 2025
 *      Author: sp6gk
 */

#ifndef INC_CALIBRATION_H_
#define INC_CALIBRATION_H_

#define RX_LNA_GAIN 14
#define RX_CPL 3

#include "main.h"
#include "cmsis_os.h"
#include "stdbool.h"
#include <semphr.h>

extern uint16_t LO1_PLL_Cal;
extern uint16_t LO2_PLL_Cal;

extern float RSSI_Calibration[3];
extern float RF_in_pwr_Calibration[3];
extern float RF_out_pwr_Calibration[3];

extern float PSU_Vin_Calibration;
extern float PSU_Vin_Calibration_offset;
extern float PSU_5V5_Calibration;
extern float PSU_3V6_Calibration;
extern float LNB_V_Calibration;
extern float LNB_V_Calibration_offset;
extern float Imon_Calibration;
extern float Imon_Calibration_offset;

extern float RF_det_tx_calA;
extern float RF_det_tx_calB;
extern float RF_det_tx_calC;

extern float RF_det_rx_calA;
extern float RF_det_rx_calB;
extern float RF_det_rx_calC;

extern uint16_t DAC_VCTCXO;

//Read only
extern float sampler_output_cal;

#endif /* INC_CALIBRATION_H_ */
