/*
 * LMX2572.c
 *
 *  Created on: Dec 26, 2025
 *      Author: sp6gk
 */

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "LMX2572.h"
#include "spi.h"
#include "stdbool.h"


static const uint16_t lmx2572_chdiv_table[] =
{
    2, 4, 8, 16, 32, 64, 128, 256
};

typedef struct {
    uint64_t vco_min;
    uint64_t vco_max;
    uint8_t  mash_order;
    uint8_t  pfd_dly;
    uint32_t n_min;
} LMX2572_MashConstraint;

static const LMX2572_MashConstraint mash_constraints[] = {
    /* f_vco < 4 GHz */
    {0,        4000000000ULL, 0, 0, 20},
    {0,        4000000000ULL, 1, 1, 25},
    {0,        4000000000ULL, 2, 1, 26},
    {0,        4000000000ULL, 3, 2, 32},
    {0,        4000000000ULL, 4, 4, 44},

    /* 4.0 – 4.9 GHz */
    {4000000000ULL, 4900000000ULL, 0, 1, 24},
    {4000000000ULL, 4900000000ULL, 1, 2, 29},
    {4000000000ULL, 4900000000ULL, 2, 2, 30},
    {4000000000ULL, 4900000000ULL, 3, 2, 32},
    {4000000000ULL, 4900000000ULL, 4, 4, 44},

    /* 4.9 – 6.4 GHz */
    {4900000000ULL, 6400000000ULL, 0, 1, 24},
    {4900000000ULL, 6400000000ULL, 1, 2, 29},
    {4900000000ULL, 6400000000ULL, 2, 2, 30},
    {4900000000ULL, 6400000000ULL, 3, 3, 36},
    {4900000000ULL, 6400000000ULL, 4, 5, 48},
};

//Initialize LMX2572 IC with register map generated in TICS PRO SW, use TICS_RegisterMap_txt_to_CInit.py for generation of C code
void LMX2572_init(LMX2572_HandleTypeDef *dev){

	if(dev->id == 1){

		dev->LMX2572_R00 = 0x209C;
		dev->LMX2572_R2C = 0x0863;
		dev->LMX2572_R2D = 0xC60A;
		dev->LMX2572_R25 = 0x0305;

		LMX2572_writeSPI(dev, 0x00, 0x02); //Reset
		osDelay(50);
		LMX2572_writeSPI(dev, 0x7D, 0x0820);
		LMX2572_writeSPI(dev, 0x7C, 0x0000);
		LMX2572_writeSPI(dev, 0x7B, 0x0000);
		LMX2572_writeSPI(dev, 0x7A, 0x0000);
		LMX2572_writeSPI(dev, 0x79, 0x0000);
		LMX2572_writeSPI(dev, 0x78, 0x0000);
		LMX2572_writeSPI(dev, 0x77, 0x0000);
		LMX2572_writeSPI(dev, 0x76, 0x0000);
		LMX2572_writeSPI(dev, 0x75, 0x0000);
		LMX2572_writeSPI(dev, 0x74, 0x0000);
		LMX2572_writeSPI(dev, 0x73, 0x0000);
		LMX2572_writeSPI(dev, 0x72, 0x7802);
		LMX2572_writeSPI(dev, 0x71, 0x0000);
		LMX2572_writeSPI(dev, 0x70, 0x0000);
		LMX2572_writeSPI(dev, 0x6F, 0x0000);
		LMX2572_writeSPI(dev, 0x6E, 0x0000);
		LMX2572_writeSPI(dev, 0x6D, 0x0000);
		LMX2572_writeSPI(dev, 0x6C, 0x0000);
		LMX2572_writeSPI(dev, 0x6B, 0x0000);
		LMX2572_writeSPI(dev, 0x6A, 0x0007);
		LMX2572_writeSPI(dev, 0x69, 0x4440);
		LMX2572_writeSPI(dev, 0x68, 0x12C0);
		LMX2572_writeSPI(dev, 0x67, 0x0000);
		LMX2572_writeSPI(dev, 0x66, 0x0000);
		LMX2572_writeSPI(dev, 0x65, 0x0000);
		LMX2572_writeSPI(dev, 0x64, 0x12C0);
		LMX2572_writeSPI(dev, 0x63, 0x1C72);
		LMX2572_writeSPI(dev, 0x62, 0x000C);
		LMX2572_writeSPI(dev, 0x61, 0x0000);
		LMX2572_writeSPI(dev, 0x60, 0x0000);
		LMX2572_writeSPI(dev, 0x5F, 0x0000);
		LMX2572_writeSPI(dev, 0x5E, 0x0000);
		LMX2572_writeSPI(dev, 0x5D, 0x0000);
		LMX2572_writeSPI(dev, 0x5C, 0x0000);
		LMX2572_writeSPI(dev, 0x5B, 0x0000);
		LMX2572_writeSPI(dev, 0x5A, 0x0000);
		LMX2572_writeSPI(dev, 0x59, 0x0000);
		LMX2572_writeSPI(dev, 0x58, 0x0000);
		LMX2572_writeSPI(dev, 0x57, 0x0000);
		LMX2572_writeSPI(dev, 0x56, 0xAC3E);
		LMX2572_writeSPI(dev, 0x55, 0xE7AA);
		LMX2572_writeSPI(dev, 0x54, 0x0001);
		LMX2572_writeSPI(dev, 0x53, 0x528A);
		LMX2572_writeSPI(dev, 0x52, 0x8E55);
		LMX2572_writeSPI(dev, 0x51, 0x0000);
		LMX2572_writeSPI(dev, 0x50, 0x0000);
		LMX2572_writeSPI(dev, 0x4F, 0x00A0);
		LMX2572_writeSPI(dev, 0x4E, 0x0001);
		LMX2572_writeSPI(dev, 0x4D, 0x0000);
		LMX2572_writeSPI(dev, 0x4C, 0x000C);
		LMX2572_writeSPI(dev, 0x4B, 0x09C0);
		LMX2572_writeSPI(dev, 0x4A, 0x0000);
		LMX2572_writeSPI(dev, 0x49, 0x003F);
		LMX2572_writeSPI(dev, 0x48, 0x0001);
		LMX2572_writeSPI(dev, 0x47, 0x0081);
		LMX2572_writeSPI(dev, 0x46, 0xC350);
		LMX2572_writeSPI(dev, 0x45, 0x0000);
		LMX2572_writeSPI(dev, 0x44, 0x03E8);
		LMX2572_writeSPI(dev, 0x43, 0x0000);
		LMX2572_writeSPI(dev, 0x42, 0x01F4);
		LMX2572_writeSPI(dev, 0x41, 0x0000);
		LMX2572_writeSPI(dev, 0x40, 0x1388);
		LMX2572_writeSPI(dev, 0x3F, 0x0000);
		LMX2572_writeSPI(dev, 0x3E, 0x00AF);
		LMX2572_writeSPI(dev, 0x3D, 0x00A8);
		LMX2572_writeSPI(dev, 0x3C, 0x03E8);
		LMX2572_writeSPI(dev, 0x3B, 0x0001);
		LMX2572_writeSPI(dev, 0x3A, 0x9001);
		LMX2572_writeSPI(dev, 0x39, 0x0020);
		LMX2572_writeSPI(dev, 0x38, 0x0000);
		LMX2572_writeSPI(dev, 0x37, 0x0000);
		LMX2572_writeSPI(dev, 0x36, 0x0000);
		LMX2572_writeSPI(dev, 0x35, 0x0000);
		LMX2572_writeSPI(dev, 0x34, 0x0421);
		LMX2572_writeSPI(dev, 0x33, 0x0080);
		LMX2572_writeSPI(dev, 0x32, 0x0080);
		LMX2572_writeSPI(dev, 0x31, 0x4180);
		LMX2572_writeSPI(dev, 0x30, 0x03E0);
		LMX2572_writeSPI(dev, 0x2F, 0x0300);
		LMX2572_writeSPI(dev, 0x2E, 0x07F0);
		LMX2572_writeSPI(dev, 0x2D, 0xC60A);
		LMX2572_writeSPI(dev, 0x2C, 0x0863);
		LMX2572_writeSPI(dev, 0x2B, 0x03E8);
		LMX2572_writeSPI(dev, 0x2A, 0x0000);
		LMX2572_writeSPI(dev, 0x29, 0x0000);
		LMX2572_writeSPI(dev, 0x28, 0x0000);
		LMX2572_writeSPI(dev, 0x27, 0x05DC);
		LMX2572_writeSPI(dev, 0x26, 0x0000);
		LMX2572_writeSPI(dev, 0x25, 0x0305);
		LMX2572_writeSPI(dev, 0x24, 0x0042);
		LMX2572_writeSPI(dev, 0x23, 0x0004);
		LMX2572_writeSPI(dev, 0x22, 0x0010);
		LMX2572_writeSPI(dev, 0x21, 0x1E01);
		LMX2572_writeSPI(dev, 0x20, 0x05BF);
		LMX2572_writeSPI(dev, 0x1F, 0xC3E6);
		LMX2572_writeSPI(dev, 0x1E, 0x0CA6);
		LMX2572_writeSPI(dev, 0x1D, 0x0000);
		LMX2572_writeSPI(dev, 0x1C, 0x0488);
		LMX2572_writeSPI(dev, 0x1B, 0x0002);
		LMX2572_writeSPI(dev, 0x1A, 0x0808);
		LMX2572_writeSPI(dev, 0x19, 0x0624);
		LMX2572_writeSPI(dev, 0x18, 0x071A);
		LMX2572_writeSPI(dev, 0x17, 0x007C);
		LMX2572_writeSPI(dev, 0x16, 0x0001);
		LMX2572_writeSPI(dev, 0x15, 0x0409);
		LMX2572_writeSPI(dev, 0x14, 0x5048);
		LMX2572_writeSPI(dev, 0x13, 0x27B7);
		LMX2572_writeSPI(dev, 0x12, 0x0064);
		LMX2572_writeSPI(dev, 0x11, 0x0096);
		LMX2572_writeSPI(dev, 0x10, 0x0080);
		LMX2572_writeSPI(dev, 0x0F, 0x060E);
		LMX2572_writeSPI(dev, 0x0E, 0x1878);
		LMX2572_writeSPI(dev, 0x0D, 0x4000);
		LMX2572_writeSPI(dev, 0x0C, 0x5001);
		LMX2572_writeSPI(dev, 0x0B, 0xB018);
		LMX2572_writeSPI(dev, 0x0A, 0x10F8);
		LMX2572_writeSPI(dev, 0x09, 0x0004);
		LMX2572_writeSPI(dev, 0x08, 0x2000);
		LMX2572_writeSPI(dev, 0x07, 0x00B2);
		LMX2572_writeSPI(dev, 0x06, 0xC802);
		LMX2572_writeSPI(dev, 0x05, 0x30C8);
		LMX2572_writeSPI(dev, 0x04, 0x0A43);
		LMX2572_writeSPI(dev, 0x03, 0x0782);
		LMX2572_writeSPI(dev, 0x02, 0x0500);
		LMX2572_writeSPI(dev, 0x01, 0x0808);
		LMX2572_writeSPI(dev, 0x00, 0x209C);
	}
	else if(dev->id == 2){

		dev->LMX2572_R00 = 0x207C;
		dev->LMX2572_R2C = 0x0860;
		dev->LMX2572_R2D = 0xC61F;
		dev->LMX2572_R25 = 0x8005;

		LMX2572_writeSPI(dev, 0x7D, 0x0820);
		LMX2572_writeSPI(dev, 0x7C, 0x0000);
		LMX2572_writeSPI(dev, 0x7B, 0x0000);
		LMX2572_writeSPI(dev, 0x7A, 0x0000);
		LMX2572_writeSPI(dev, 0x79, 0x0000);
		LMX2572_writeSPI(dev, 0x78, 0x0000);
		LMX2572_writeSPI(dev, 0x77, 0x0000);
		LMX2572_writeSPI(dev, 0x76, 0x0000);
		LMX2572_writeSPI(dev, 0x75, 0x0000);
		LMX2572_writeSPI(dev, 0x74, 0x0000);
		LMX2572_writeSPI(dev, 0x73, 0x0000);
		LMX2572_writeSPI(dev, 0x72, 0x7802);
		LMX2572_writeSPI(dev, 0x71, 0x0000);
		LMX2572_writeSPI(dev, 0x70, 0x0000);
		LMX2572_writeSPI(dev, 0x6F, 0x0000);
		LMX2572_writeSPI(dev, 0x6E, 0x0000);
		LMX2572_writeSPI(dev, 0x6D, 0x0000);
		LMX2572_writeSPI(dev, 0x6C, 0x0000);
		LMX2572_writeSPI(dev, 0x6B, 0x0000);
		LMX2572_writeSPI(dev, 0x6A, 0x0007);
		LMX2572_writeSPI(dev, 0x69, 0x4440);
		LMX2572_writeSPI(dev, 0x68, 0x0014);
		LMX2572_writeSPI(dev, 0x67, 0xFFFF);
		LMX2572_writeSPI(dev, 0x66, 0x3FFF);
		LMX2572_writeSPI(dev, 0x65, 0x0000);
		LMX2572_writeSPI(dev, 0x64, 0x0014);
		LMX2572_writeSPI(dev, 0x63, 0xFFFF);
		LMX2572_writeSPI(dev, 0x62, 0xFFFC);
		LMX2572_writeSPI(dev, 0x61, 0x0000);
		LMX2572_writeSPI(dev, 0x60, 0x0000);
		LMX2572_writeSPI(dev, 0x5F, 0x0000);
		LMX2572_writeSPI(dev, 0x5E, 0x0000);
		LMX2572_writeSPI(dev, 0x5D, 0x0000);
		LMX2572_writeSPI(dev, 0x5C, 0x0000);
		LMX2572_writeSPI(dev, 0x5B, 0x0000);
		LMX2572_writeSPI(dev, 0x5A, 0x0000);
		LMX2572_writeSPI(dev, 0x59, 0x0000);
		LMX2572_writeSPI(dev, 0x58, 0x0000);
		LMX2572_writeSPI(dev, 0x57, 0x0000);
		LMX2572_writeSPI(dev, 0x56, 0x0001);
		LMX2572_writeSPI(dev, 0x55, 0x0000);
		LMX2572_writeSPI(dev, 0x54, 0x0001);
		LMX2572_writeSPI(dev, 0x53, 0xFFFF);
		LMX2572_writeSPI(dev, 0x52, 0xFFFF);
		LMX2572_writeSPI(dev, 0x51, 0x0000);
		LMX2572_writeSPI(dev, 0x50, 0x0000);
		LMX2572_writeSPI(dev, 0x4F, 0x9600);
		LMX2572_writeSPI(dev, 0x4E, 0x0001);
		LMX2572_writeSPI(dev, 0x4D, 0x0000);
		LMX2572_writeSPI(dev, 0x4C, 0x000C);
		LMX2572_writeSPI(dev, 0x4B, 0x0800);
		LMX2572_writeSPI(dev, 0x4A, 0x0000);
		LMX2572_writeSPI(dev, 0x49, 0x003F);
		LMX2572_writeSPI(dev, 0x48, 0x0001);
		LMX2572_writeSPI(dev, 0x47, 0x0081);
		LMX2572_writeSPI(dev, 0x46, 0xC350);
		LMX2572_writeSPI(dev, 0x45, 0x0000);
		LMX2572_writeSPI(dev, 0x44, 0x03E8);
		LMX2572_writeSPI(dev, 0x43, 0x0000);
		LMX2572_writeSPI(dev, 0x42, 0x01F4);
		LMX2572_writeSPI(dev, 0x41, 0x0000);
		LMX2572_writeSPI(dev, 0x40, 0x1388);
		LMX2572_writeSPI(dev, 0x3F, 0x0000);
		LMX2572_writeSPI(dev, 0x3E, 0x00AF);
		LMX2572_writeSPI(dev, 0x3D, 0x00A8);
		LMX2572_writeSPI(dev, 0x3C, 0x03E8);
		LMX2572_writeSPI(dev, 0x3B, 0x0001);
		LMX2572_writeSPI(dev, 0x3A, 0x9001);
		LMX2572_writeSPI(dev, 0x39, 0x0020);
		LMX2572_writeSPI(dev, 0x38, 0x0000);
		LMX2572_writeSPI(dev, 0x37, 0x0000);
		LMX2572_writeSPI(dev, 0x36, 0x0000);
		LMX2572_writeSPI(dev, 0x35, 0x0000);
		LMX2572_writeSPI(dev, 0x34, 0x0421);
		LMX2572_writeSPI(dev, 0x33, 0x0080);
		LMX2572_writeSPI(dev, 0x32, 0x0080);
		LMX2572_writeSPI(dev, 0x31, 0x4180);
		LMX2572_writeSPI(dev, 0x30, 0x03E0);
		LMX2572_writeSPI(dev, 0x2F, 0x0300);
		LMX2572_writeSPI(dev, 0x2E, 0x07F0);
		LMX2572_writeSPI(dev, 0x2D, 0xC61F);
		LMX2572_writeSPI(dev, 0x2C, 0x0860);
		LMX2572_writeSPI(dev, 0x2B, 0x0000);
		LMX2572_writeSPI(dev, 0x2A, 0x0000);
		LMX2572_writeSPI(dev, 0x29, 0x0064);
		LMX2572_writeSPI(dev, 0x28, 0x0000);
		LMX2572_writeSPI(dev, 0x27, 0x000A);
		LMX2572_writeSPI(dev, 0x26, 0x0000);
		LMX2572_writeSPI(dev, 0x25, 0x8005);
		LMX2572_writeSPI(dev, 0x24, 0x4CCC);
		LMX2572_writeSPI(dev, 0x23, 0x0004);
		LMX2572_writeSPI(dev, 0x22, 0x0010);
		LMX2572_writeSPI(dev, 0x21, 0x1E01);
		LMX2572_writeSPI(dev, 0x20, 0x05BF);
		LMX2572_writeSPI(dev, 0x1F, 0xC3E6);
		LMX2572_writeSPI(dev, 0x1E, 0x0CA6);
		LMX2572_writeSPI(dev, 0x1D, 0x0000);
		LMX2572_writeSPI(dev, 0x1C, 0x0488);
		LMX2572_writeSPI(dev, 0x1B, 0x0002);
		LMX2572_writeSPI(dev, 0x1A, 0x0808);
		LMX2572_writeSPI(dev, 0x19, 0x0624);
		LMX2572_writeSPI(dev, 0x18, 0x071A);
		LMX2572_writeSPI(dev, 0x17, 0x007C);
		LMX2572_writeSPI(dev, 0x16, 0x0001);
		LMX2572_writeSPI(dev, 0x15, 0x0409);
		LMX2572_writeSPI(dev, 0x14, 0x4848);
		LMX2572_writeSPI(dev, 0x13, 0x27B7);
		LMX2572_writeSPI(dev, 0x12, 0x0064);
		LMX2572_writeSPI(dev, 0x11, 0x0096);
		LMX2572_writeSPI(dev, 0x10, 0x0080);
		LMX2572_writeSPI(dev, 0x0F, 0x060E);
		LMX2572_writeSPI(dev, 0x0E, 0x1808);
		LMX2572_writeSPI(dev, 0x0D, 0x4000);
		LMX2572_writeSPI(dev, 0x0C, 0x5030);
		LMX2572_writeSPI(dev, 0x0B, 0xB058);
		LMX2572_writeSPI(dev, 0x0A, 0x10F8);
		LMX2572_writeSPI(dev, 0x09, 0x0004);
		LMX2572_writeSPI(dev, 0x08, 0x2000);
		LMX2572_writeSPI(dev, 0x07, 0x00B2);
		LMX2572_writeSPI(dev, 0x06, 0xC802);
		LMX2572_writeSPI(dev, 0x05, 0x30C8);
		LMX2572_writeSPI(dev, 0x04, 0x0A43);
		LMX2572_writeSPI(dev, 0x03, 0x0782);
		LMX2572_writeSPI(dev, 0x02, 0x0500);
		LMX2572_writeSPI(dev, 0x01, 0x0808);
		LMX2572_writeSPI(dev, 0x00, 0x207C);
	}
}

void LMX2572_writeSPI(LMX2572_HandleTypeDef *dev, uint8_t addr, uint16_t data){
	HAL_StatusTypeDef debug_spi_status = 0;
    uint8_t tx[3];

    tx[0] = addr & 0x7F;          // R/W = 0
    tx[1] = (data >> 8) & 0xFF;
    tx[2] = data & 0xFF;

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    debug_spi_status = HAL_SPI_Transmit(dev->hspi, tx, 3, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);

    if(debug_spi_status != HAL_OK){
    	//Handle broken SPI
    }
}

void LMX2572_CalVCO(LMX2572_HandleTypeDef *dev){
	dev->LMX2572_R00 |= LMX2572_R0_FCAL_EN_Msk;
	LMX2572_writeSPI(dev, STATUS_ADDR, dev->LMX2572_R00);
}

void LMX2572_SetNdivider(LMX2572_HandleTypeDef *dev, uint32_t Ndivider){
	uint16_t n_div_hi_bits = (Ndivider >> 16) & 0x7;
	uint16_t n_div_hi = (2U << 3) | n_div_hi_bits;
	uint16_t n_div_lo = (uint16_t)(Ndivider & 0xFFFF);

	LMX2572_writeSPI(dev, N_DIVIDER_ADDR1, n_div_hi);
	LMX2572_writeSPI(dev, N_DIVIDER_ADDR2, n_div_lo);
}

void LMX2572_SetNumerator(LMX2572_HandleTypeDef *dev, uint32_t Numerator){
    uint16_t num_hi = (uint16_t)(Numerator >> 16);
    uint16_t num_lo = (uint16_t)(Numerator & 0xFFFF);

    LMX2572_writeSPI(dev, NUMERATOR_ADDR1, num_hi);
    LMX2572_writeSPI(dev, NUMERATOR_ADDR2, num_lo);
}

void LMX2572_SetDenominator(LMX2572_HandleTypeDef *dev, uint32_t Denominator){
    uint16_t dem_hi = (uint16_t)(Denominator >> 16);
    uint16_t dem_lo = (uint16_t)(Denominator & 0xFFFF);

    LMX2572_writeSPI(dev, Denom_ADDR1, dem_hi);
    LMX2572_writeSPI(dev, Denom_ADDR2, dem_lo);
}

//Accepts normal decimal value of actual divider and encodes it to 0-7 register values
void LMX2572_SetOutDivider(LMX2572_HandleTypeDef *dev, uint16_t outdiv)
{
    uint8_t code;
    uint16_t reg;

    switch (outdiv)
    {
        case 2:   code = 0; break;
        case 4:   code = 1; break;
        case 8:   code = 3; break;
        case 16:  code = 5; break;
        case 32:  code = 7; break;
        case 64:  code = 9; break;
        case 128: code = 12; break;
        case 256: code = 14; break;
        default:
            return; //invalid divider
    }

    reg = (1U << 11);  // bits 15:11 = 1h
    reg |= (uint16_t)(code << LMX2572_R4B_OUTDIV_Pos);

    LMX2572_writeSPI(dev, CHN_DIVIDE_ADDR, reg);
}

void LMX2572_SetRFLvlA(LMX2572_HandleTypeDef *dev, uint8_t lvl){
    lvl &= 0x3F;
    dev->LMX2572_R2C &= ~LMX2572_R2C_OUTAPWR_Msk;
    dev->LMX2572_R2C |= (lvl << LMX2572_R2C_OUTAPWR_Pos);

    LMX2572_writeSPI(dev, RFA_OUT_ADDR, dev->LMX2572_R2C);
}

void LMX2572_SetRFLvlB(LMX2572_HandleTypeDef *dev, uint8_t lvl){
    uint16_t reg;
    lvl &= LMX2572_RFB_PWR_LVL_Msk;
    reg = (0x318 << 6) | lvl;

    LMX2572_writeSPI(dev, RFB_OUT_ADDR, reg);
}

void LMX2572_SetOutA(LMX2572_HandleTypeDef *dev, bool enable){
    if(enable){
    	dev->LMX2572_R2C &= ~(1U << LMX2572_R2C_OUTA_PD_Pos);
    }
    else{
    	dev->LMX2572_R2C |=  (1U << LMX2572_R2C_OUTA_PD_Pos);
    }

    LMX2572_writeSPI(dev, RFA_OUT_ADDR, dev->LMX2572_R2C);
}

void LMX2572_SetOutB(LMX2572_HandleTypeDef *dev, bool enable){
    if(enable){
    	dev->LMX2572_R2C &= ~(1U << LMX2572_R2C_OUTB_PD_Pos);
    }
    else{
    	dev->LMX2572_R2C |=  (1U << LMX2572_R2C_OUTB_PD_Pos);
    }

    LMX2572_writeSPI(dev, RFA_OUT_ADDR, dev->LMX2572_R2C);
}

//Jumps to channel when PLL is manually set for stepping by incrementation of numerator
void LMX2572_SetChannel(LMX2572_HandleTypeDef *dev, uint32_t channel){
	LMX2572_SetNumerator(dev, (dev->numerator)+channel);
}

void LMX2572_SetFrequency(LMX2572_HandleTypeDef *dev, uint32_t freq){

	  if(LMX2572_ComputePLL(dev, freq, 10UL)){		//10UL - minimal step/resolution of solution
		 LMX2572_SetNumerator(dev, dev->numerator);
		 LMX2572_SetDenominator(dev, dev->denominator);
		 LMX2572_SetNdivider(dev, dev->n_div);
		 LMX2572_SetOutDivider(dev, dev->chdiv);

		 LMX2572_SetMASH_EN(dev, dev->mash_order != 0);
		 LMX2572_SetPFD_DLY(dev, dev->pfd_dly);
		 LMX2572_SetMASH_Order(dev, dev->mash_order);

		 osDelay(20);
		 LMX2572_CalVCO(dev);

		 //dev->ld_flag = HAL_GPIO_ReadPin(LO1_LD_GPIO_Port, LO1_LD_Pin); //In case of single PLL it is convinient to read LD status here
	  }

}

bool LMX2572_ComputePLL(LMX2572_HandleTypeDef *dev, uint32_t fout_hz, uint32_t step_hz){
    if (dev == NULL ||
        step_hz == 0 ||
        dev->ref_div == 0) {
        return false;
    }

    uint64_t f_pfd = (uint64_t)dev->ref_freq_hz / dev->ref_div;

    bool     found        = false;
    uint32_t best_den     = 0xFFFFFFFFUL;
    uint32_t best_num     = 0xFFFFFFFFUL;
    uint16_t best_chdiv   = 0xFFFF;
    //uint8_t  best_pfd_dly = 0;
    uint8_t  best_mash    = 0;

    //Try MASH orders from lowest noise to highest
    for(uint8_t mash = 0; mash <= 4; mash++){

        for(size_t i = 0; i < ARRAY_SIZE(lmx2572_chdiv_table); i++){

            uint16_t chdiv = lmx2572_chdiv_table[i];
            uint64_t f_vco = (uint64_t)fout_hz * chdiv;

            /* Enforce VCO limits */
            if (f_vco < LMX2572_VCO_MIN_HZ ||
                f_vco > LMX2572_VCO_MAX_HZ){
                continue;
            }

            /*
             * step = fPFD / (DEN * CHDIV)
             * => DEN = fPFD / (step * CHDIV)
             */
            uint32_t den = (uint32_t)(f_pfd /
                                     ((uint64_t)step_hz * chdiv));

            if(den == 0 || den > LMX2572_MAX_DEN){
                continue;
            }

            /* Integer N */
            uint32_t n_int = (uint32_t)(f_vco / f_pfd);
            uint64_t rem   = f_vco - (uint64_t)n_int * f_pfd;

            /* Datasheet constraints */
            uint8_t  pfd_dly;
            uint32_t n_min;

            if(!LMX2572_GetMashLimits(f_vco, mash, &pfd_dly, &n_min)){
                continue;
            }

            if(n_int < n_min){
                continue;
            }

            /* Fractional numerator */
            uint32_t num = (uint32_t)((rem * den + f_pfd / 2) / f_pfd);

            /* Handle carry */
            if (num >= den) {
                n_int++;
                num = 0;

                if (n_int < n_min) {
                    continue;
                }
            }

            if (num == 0) {
                if (mash != 0) {
                    continue;
                }
            } else {
                if (mash == 0) {
                    continue;
                }
            }

            /* Reduce fraction */
            uint32_t g = gcd_u32(num, den);
            num /= g;
            den /= g;

            /*
             * Selection priority:
             * 1) lowest MASH order (lowest phase noise)
             * 2) lowest denominator
             * 3) lowest numerator
             * 4) lowest channel divider
             */
            if (!found ||
                mash < best_mash ||
                (mash == best_mash && den < best_den) ||
                (mash == best_mash && den == best_den && num < best_num) ||
                (mash == best_mash && den == best_den &&
                 num == best_num && chdiv < best_chdiv)) {

                dev->n_div       = n_int;
                dev->numerator   = num;
                dev->denominator = den;
                dev->chdiv       = chdiv;
                dev->pfd_dly     = pfd_dly;
                dev->mash_order = mash;

                best_mash    = mash;
                best_den     = den;
                best_num     = num;
                best_chdiv   = chdiv;
                //best_pfd_dly = pfd_dly;

                found = true;
            }
        }

        /* If we found a valid solution at this MASH order,
           do NOT try higher orders (lower noise wins). */
        if (found) {
            break;
        }
    }

    if (!found) {
        return false;
    }

    dev->fout_hz = fout_hz;
    dev->step_hz = step_hz;

    return true;
}

void LMX2572_SetRefDivider(LMX2572_HandleTypeDef *dev, uint8_t ref_div){
    uint16_t reg = 0;
    reg |= (0xB << 12);
    reg |= ((uint16_t)ref_div << LMX2572_R0B_PLL_R_Pos);
    reg |= 0x8;

    LMX2572_writeSPI(dev, R_ADDR, reg);

    dev->ref_div = ref_div;
}

void LMX2572_SetPFD_DLY(LMX2572_HandleTypeDef *dev, uint8_t pfd_dly){
    pfd_dly &= 0x3F;
    dev->LMX2572_R25 &= ~LMX2572_R25_PFD_DLY_Msk;
    dev->LMX2572_R25 |= ((uint16_t)pfd_dly << LMX2572_R25_PFD_DLY_Pos);

    dev->LMX2572_R25 &= ~(1U << 14);          //bit 14 must be 0
    dev->LMX2572_R25 |= 0x5;

    LMX2572_writeSPI(dev, PFD_DELAY_ADDR, dev->LMX2572_R25);
}

void LMX2572_SetMASH_EN(LMX2572_HandleTypeDef *dev, bool enable){
    if(enable){
    	dev->LMX2572_R25 |= LMX2572_R25_MASH_EN_Msk;
    }else{
    	dev->LMX2572_R25 &= ~LMX2572_R25_MASH_EN_Msk;
    }

    dev->LMX2572_R25 &= ~(1U << 14);
    dev->LMX2572_R25 |= 0x5;

    LMX2572_writeSPI(dev, PFD_DELAY_ADDR, dev->LMX2572_R25);
}

void LMX2572_SetMASH_Order(LMX2572_HandleTypeDef *dev, uint8_t mash_order){
	mash_order &= 0x7U; //bits 0-2 used
	dev->LMX2572_R2C &= ~LMX2572_R2C_MASH_OR_Msk;
	dev->LMX2572_R2C |= (mash_order << LMX2572_R2C_MASH_OR_Pos);

    LMX2572_writeSPI(dev, RFA_OUT_ADDR, dev->LMX2572_R2C);
}

uint32_t gcd_u32(uint32_t a, uint32_t b){
    while (b != 0)
    {
        uint32_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

bool LMX2572_GetMashLimits(uint64_t f_vco, uint8_t mash_order, uint8_t *pfd_dly, uint32_t *n_min){
    for (size_t i = 0; i < ARRAY_SIZE(mash_constraints); i++) {
        const LMX2572_MashConstraint *c = &mash_constraints[i];

        if (f_vco >= c->vco_min &&
            f_vco <  c->vco_max &&
            mash_order == c->mash_order) {

            *pfd_dly = c->pfd_dly;
            *n_min   = c->n_min;
            return true;
        }
    }
    return false;
}


