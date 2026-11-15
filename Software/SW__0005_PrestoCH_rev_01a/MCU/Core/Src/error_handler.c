/*
 * error_handler.c
 *
 *  Created on: May 12, 2026
 *      Author: sp6gk
 */

#include "main.h"
#include "error_handler.h"
#include "stdbool.h"
#include "globals.h"
#include "ili9341.h"

extern bool PSU_error_latch;
extern bool LNB_error_latch;
extern bool TX_error_latch;
extern bool RF_pwr_error_latch;


void error_handler(uint16_t error_code){


	switch(error_code){
	case 1:

		break;

	}

}


const char * const error_strings[] = {
		"                  ",
		"E01 PSU FLT BOOT  ",
		"E02 LNB FLT BOOT  ",
		"E03 5V5 FLT BOOT  ",
		"E04 Vin > Vmax    ",
		"E05 Vin < Vmin    ",
		"E06 3V6 VOLT FLT  ",
		"E07 5V5 VOLT FLT  ",
		"E08 IDLE CURRENT  ",
		"E09 CH MEMORY     ",
		"E10 MB MEMORY     ",
		"E11 LO1 MEMORY    ",
		"E12 LO2 MEMORY    ",
		"E13 LO1 LOCK      ",
		"E14 LO2 LOCK      ",
		"E15 TX CURRENT<   ",
		"E16 TX CURRENT>   ",
		"E17 LNB SWITCH    ",
		"E18 LNB H POL FLT ",
		"E19 LNB PSU FLT   ",
		"E20 LNB V POL FLT ",
		"E21 T AMP > TMAX  ",
		"E21 T TX > TMAX   ",
		"E21 T MB > TMAX   ",
		"E24 I2C BUS       ",	//Last code related to POST, waht follows are run time errors either reported by SW or detected by BIT
		"E25 VIN LOW       ",
		"E26 VIN HIGH      ",
		"E27 5V5 LOW       ",
		"E28 5V5 HIGH      ",
		"E29 3V6 LOW       ",
		"E30 3V6 HIGH      ",
		"E31 LNB H POL LO  ",
		"E32 LNB H POL HI  ",
		"E33 LNB V POL LO  ",
		"E34 LNB V POL HI  ",
		"E35 LNB SWITCH    ",
		"E36 RF IN HIGH    ",
		"E37 CURRENT LIM   ",
		"E38 MAX TEMP TX   ",
		"E39 MAX TEMP MB   ",
		"E40 MAX TEMP CH   ",
		"E41 PSU FAULT     ",
		"E42 LNB FAULT     ",
		"E43 LO1 LOCK      ",
		"E44 LO2 LOCK      ",
		"E45 SAVE FREQ     ",
		"E46 SAVE NV       ",
		"E47 CRC WRITE     ",
		"E48 CRC POKE      ",
		"E49 BASIC INIT    ",
		"E50 RF OUT LIM    ",
		"E52 5V5 FLT       ",

};

