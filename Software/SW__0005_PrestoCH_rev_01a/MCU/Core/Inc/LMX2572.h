/*
 * LMX2572.h
 *
 *  Created on: Dec 26, 2025
 *      Author: sp6gk
 */

#ifndef INC_LMX2572_H_
#define INC_LMX2572_H_


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#include "stm32f4xx_hal.h"
#include "main.h"
#include "stdbool.h"


typedef struct
{
    /* Hardware interface */
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef      *cs_port;
    uint16_t           cs_pin;

    /* PLL Id */
    uint8_t			   id;

    /* Reference configuration */
    uint32_t           ref_freq_hz;    //48 MHz
    uint16_t           ref_div;        //R divider

    /* Output configuration */
    uint32_t           fout_hz;        //requested output frequency
    uint32_t           step_hz;        //channel step

    /* PLL solution (computed) */
    uint32_t           n_div;           //integer N
    uint32_t           numerator;       //NUM
    uint32_t           denominator;     //DEN
    uint16_t            chdiv;          //channel divider

    /* SMASH (noise shaping sigma delta modulator) */
    uint8_t 		   mash_order; 	//0 - integer, 1, 2, 3, 4
    uint16_t           mash_seed;
    uint8_t            pfd_dly;

    /* Diagnostic */
    bool ld_flag;


    /* Shadow registers */
    uint16_t LMX2572_R00;
    uint16_t LMX2572_R2C;
    uint16_t LMX2572_R2D;
    uint16_t LMX2572_R25;

} LMX2572_HandleTypeDef;

#define LMX2572_VCO_MIN_HZ   3200000000ULL
#define LMX2572_VCO_MAX_HZ   6400000000ULL
#define LMX2572_MAX_DEN     1048575UL   //conservative upper bound


#define STATUS_ADDR     0x0
#define DOUBLER_ADDR 	0x9
#define PRE_R_ADDR   	0xC
#define MULTIPLIER_ADDR	0xA
#define R_ADDR			0xB
#define CHARGE_P_ADDR	0xE		//0 - tristate, 1 - 15 -> 625 uA to 6875 uA
#define N_DIVIDER_ADDR1 0x22
#define N_DIVIDER_ADDR2 0x24
#define NUMERATOR_ADDR1	0x2A
#define NUMERATOR_ADDR2 0x2B
#define Denom_ADDR1		0x26
#define Denom_ADDR2     0x27
#define CHN_DIVIDE_ADDR 0x4B //0-14 2,4,8,16,32,54,128,256 10:6
#define RFA_OUT_ADDR	0x2C
#define RFB_OUT_ADDR	0x2D
#define PFD_DELAY_ADDR  0x25

#define LMX2572_R0_FCAL_EN_Pos  3
#define LMX2572_R0_FCAL_EN_Msk  (1U << LMX2572_R0_FCAL_EN_Pos)
#define LMX2572_R2C_OUTAPWR_Pos 8
#define LMX2572_R2C_OUTAPWR_Msk (0x3F << LMX2572_R2C_OUTAPWR_Pos)
#define LMX2572_R2C_OUTB_PD_Pos 7
#define LMX2572_R2C_OUTA_PD_Pos 6
#define LMX2572_RFB_PWR_LVL_Msk 0x3F
#define LMX2572_R4B_OUTDIV_Pos  6
#define LMX2572_OUTDIV_Msk      (0x1F << LMX2572_OUTDIV_Pos)
#define LMX2572_R0B_PLL_R_Pos   4
#define LMX2572_R25_MashEN_Pos  15
#define LMX2572_R25_PFD_DLY_Pos 8
#define LMX2572_R25_PFD_DLY_Msk (0x3FU << LMX2572_R25_PFD_DLY_Pos)
#define LMX2572_R25_MASH_EN_Msk (1U << LMX2572_R25_MashEN_Pos)
#define LMX2572_R2C_MASH_OR_Pos 0
#define LMX2572_R2C_MASH_OR_Msk (0x7U << LMX2572_R2C_MASH_OR_Pos)


void LMX2572_init(LMX2572_HandleTypeDef *dev);
void LMX2572_writeSPI(LMX2572_HandleTypeDef *dev, uint8_t addr, uint16_t data);

void LMX2572_CalVCO(LMX2572_HandleTypeDef *dev);
void LMX2572_SetNdivider(LMX2572_HandleTypeDef *dev, uint32_t Ndivider);
void LMX2572_SetNumerator(LMX2572_HandleTypeDef *dev, uint32_t Numerator);
void LMX2572_SetDenominator(LMX2572_HandleTypeDef *dev, uint32_t Denominator);
void LMX2572_SetOutDivider(LMX2572_HandleTypeDef *dev, uint16_t OutDivider);

void LMX2572_SetRefDivider(LMX2572_HandleTypeDef *dev, uint8_t ref_div);

void LMX2572_SetMASH_EN(LMX2572_HandleTypeDef *dev, bool enable);
void LMX2572_SetPFD_DLY(LMX2572_HandleTypeDef *dev, uint8_t pfd_fly);
void LMX2572_SetMASH_Order(LMX2572_HandleTypeDef *dev, uint8_t mash_order);

void LMX2572_SetRFLvlA(LMX2572_HandleTypeDef *dev, uint8_t lvl);
void LMX2572_SetRFLvlB(LMX2572_HandleTypeDef *dev, uint8_t lvl);

void LMX2572_SetOutA(LMX2572_HandleTypeDef *dev, bool state);
void LMX2572_SetOutB(LMX2572_HandleTypeDef *dev, bool state);

void LMX2572_SetChannel(LMX2572_HandleTypeDef *dev, uint32_t channel);
void LMX2572_SetFrequency(LMX2572_HandleTypeDef *dev, uint32_t freq);

bool LMX2572_ComputePLL(LMX2572_HandleTypeDef *dev, uint32_t fout_hz, uint32_t step_hz);
uint32_t gcd_u32(uint32_t a, uint32_t b);

bool LMX2572_GetMashLimits(uint64_t f_vco, uint8_t mash_order, uint8_t *pfd_dly, uint32_t *n_min);

#endif /* INC_LMX2572_H_ */



