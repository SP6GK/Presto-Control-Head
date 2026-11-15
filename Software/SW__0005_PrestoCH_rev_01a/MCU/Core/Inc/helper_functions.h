/*
 * helper_functions.h
 *
 *  Created on: Sep 14, 2025
 *      Author: sp6gk
 */



#ifndef INC_HELPER_FUNCTIONS_H_
#define INC_HELPER_FUNCTIONS_H_

#define NERRORS_RUNTIME 21	//Number of run time errors monitored

#include "TCA9554.h"
#include "ili9341.h"

uint8_t HW_Init_POST(TCA9554_HandleTypeDef *GPIO_expander, uint8_t phase);
float adcValueToVoltage(uint16_t adcValue);
uint8_t Built_In_Test();


uint16_t rx_screen_display_scene(ili9341_t *lcd);
uint16_t tx_screen_display_scene(ili9341_t *lcd);
uint16_t menu1_display_scene(ili9341_t *lcd);
uint16_t menu2_display_scene(ili9341_t *lcd);
uint16_t menu3_display_scene(ili9341_t *lcd);
uint16_t menu4_display_scene(ili9341_t *lcd);
uint16_t draw_buttons(ili9341_t *lcd, uint16_t scene, uint8_t priority);

void menu1_display_settings(ili9341_t *lcd, uint8_t priority);
void menu2_display_settings(ili9341_t *lcd, uint8_t priority);
void menu3_display_settings(ili9341_t *lcd, uint8_t priority);
void menu4_display_diag(ili9341_t *lcd, uint8_t priority);

uint8_t Rx_LNA_Enable(void);
uint8_t Rx_LNA_Disable(void);

uint8_t LNB_Enable(void);
uint8_t LNB_Disable(void);

uint8_t LNB_Set_polarization(bool polarization);

uint8_t Draw_meter_bar(ili9341_t *lcd, uint16_t x, uint16_t y, uint16_t h, uint16_t w, bool mode);
uint8_t Update_meter_bar(ili9341_t *lcd, uint16_t value, uint16_t x, uint16_t y, uint16_t h, uint16_t w, bool mode);
void uart_printf(const char *fmt, ...);
void process_buttons(void);
uint16_t process_encoder(void);
uint8_t process_attenuators();
uint16_t draw_selection_rectangle(ili9341_t *lcd, uint16_t scene);
uint16_t display_menu(ili9341_t *lcd, uint16_t select);

uint16_t menu(ili9341_t *lcd);
uint8_t load_structs(void);
void process_nv_writes(void);
void save_settings(void);


float map_int_to_float(uint16_t inValue, uint16_t minIN, uint16_t maxIN, float minOUT, float maxOUT);

uint16_t map_float_to_int(float inValue, float minIN, float maxIN, uint16_t minOUT, uint16_t maxOUT);
uint32_t map_uint16(uint16_t inValue, uint16_t minIN, uint16_t maxIN, uint32_t minOUT, uint32_t maxOUT);

uint32_t Brightness_To_PWM(uint16_t brightness);

uint16_t draw_title_bar(ili9341_t *lcd);

void process_GPIO_mcu(void);

#endif /* INC_HELPER_FUNCTIONS_H_ */
