/*
 * error_handler.h
 *
 *  Created on: May 12, 2026
 *      Author: sp6gk
 */

#ifndef INC_ERROR_HANDLER_H_
#define INC_ERROR_HANDLER_H_

#include "main.h"
#include "stdbool.h"
#include "ili9341.h"

//Error latches, critical latches that block certain functionality of device or put it in special configuration to prevent damage
extern bool PSU_error_latch;
extern bool LNB_error_latch;
extern bool TX_error_latch;
extern bool RF_pwr_error_latch;

extern const char * const error_strings[];

void error_handler(uint16_t error_code);



#endif /* INC_ERROR_HANDLER_H_ */
