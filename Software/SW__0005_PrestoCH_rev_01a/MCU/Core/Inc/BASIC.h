/*
 * BASIC.h
 *
 *  Created on: Oct 12, 2025
 *      Author: sp6gk
 */

#ifndef INC_BASIC_H_
#define INC_BASIC_H_

#include "BASIC.h"
#include "main.h"
#include "globals.h"
#include "usart.h"
#include "NV_data.h"

uint32_t BASIC_Init(void);
uint32_t BASIC_cmd(uint8_t *cmd, uint16_t length);


static const char uart_splash[] =
	"\n"
	"	██████  ██████  ███████ ███████ ████████  ██████   \n"
	"	██   ██ ██   ██ ██      ██         ██    ██    ██  \n"
	"	██████  ██████  █████   ███████    ██    ██    ██  \n"
	"	██      ██   ██ ██           ██    ██    ██    ██  \n"
	"	██      ██   ██ ███████ ███████    ██     ██████   \n"
	"\n";

static const char RDY[] = "READY\n";
static const char NDAC[] = "NDAC\n";

#endif /* INC_BASIC_H_ */
