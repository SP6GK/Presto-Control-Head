/*
 * 24LC_EEPROM.c
 *
 *  Created on: Sep 27, 2025
 *      Author: sp6gk
 */

#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "globals.h"
#include "NV_data.h"
#include "usart.h"
#include "24LC_EEPROM.h"

#include "crc.h"

#include <stdio.h>
#include <string.h>

HAL_StatusTypeDef EEPROM_Write(uint16_t i2c_addr,
                               uint16_t memory_addr,
                               uint8_t *data,
                               uint16_t size)
{
    const uint16_t PAGE_SIZE = 32;
    const uint32_t WRITE_TIMEOUT = 1000;
    const uint32_t READY_TIMEOUT = 1000;

    HAL_StatusTypeDef status = HAL_OK;

    //xSemaphoreTake(i2cMutex, portMAX_DELAY);

    uint16_t remaining = size;
    uint16_t addr = memory_addr;
    uint8_t *ptr = data;
    uint16_t devAddr = i2c_addr << 1;

    while (remaining > 0)
    {
        uint16_t page_offset = addr % PAGE_SIZE;
        uint16_t bytes_in_page = PAGE_SIZE - page_offset;
        uint16_t to_write =
            (remaining < bytes_in_page) ? remaining : bytes_in_page;

        status = HAL_I2C_Mem_Write(&hi2c1, devAddr, addr,
                                   I2C_MEMADD_SIZE_16BIT,
                                   ptr, to_write, WRITE_TIMEOUT);
        osDelay(10);
        if (status != HAL_OK)
            break;

        status = HAL_I2C_IsDeviceReady(&hi2c1, devAddr, 20, READY_TIMEOUT);
        if (status != HAL_OK)
            break;

        remaining -= to_write;
        addr += to_write;
        ptr += to_write;
    }

    //xSemaphoreGive(i2cMutex);
    return status;
}


HAL_StatusTypeDef EEPROM_Read(uint16_t i2c_addr, uint16_t memory_addr, uint8_t *data, uint16_t size){
	HAL_StatusTypeDef status = HAL_OK;
	//xSemaphoreTake(i2cMutex, portMAX_DELAY);
    status =  HAL_I2C_Mem_Read(&hi2c1, (i2c_addr << 1), memory_addr, I2C_MEMADD_SIZE_16BIT, data, size, HAL_MAX_DELAY);
    //xSemaphoreGive(i2cMutex);
    return status;

}

HAL_StatusTypeDef EEPROM_Check(uint16_t i2c_addr){
	HAL_StatusTypeDef status = HAL_OK;
	//xSemaphoreTake(i2cMutex, portMAX_DELAY);		//Note, mutexes cannot be released or taken by XSemaphorGive from IS (xSemaphoreGiveFromISR dont work with mutex), TODO in future projects use direct task notification to process commands from ISR
	status = HAL_I2C_IsDeviceReady(&hi2c1, (i2c_addr << 1), 1000, HAL_MAX_DELAY);
    //xSemaphoreGive(i2cMutex);
    return status;
}


uint8_t EEPROM_Read_MB(void){
    uint8_t status = HAL_ERROR;
    uint8_t data_page[68] = {0};       //64 bytes buffer for 32 fields (each 16 bit)

    status = EEPROM_Read(MB_EEPROM_ADDR, MB_NV_ProjectID_addr, data_page, sizeof(data_page));
    if (status != HAL_OK){
    	return 1;
    }

    MB_NV.ProjectID      = (data_page[1] << 8) | data_page[0];
    MB_NV.HW_major_rev   = (data_page[3] << 8) | data_page[2];
    MB_NV.HW_minor_rev   = (data_page[5] << 8) | data_page[4];
    MB_NV.Serial_NoA     = (data_page[7] << 8) | data_page[6];
    MB_NV.Serial_NoB     = (data_page[9] << 8) | data_page[8];
    MB_NV.TestVar0       = (data_page[11] << 8) | data_page[10];

    MB_NV.RF_Det_RX_calA = (data_page[13] << 8) | data_page[12];
    MB_NV.RF_Det_RX_calB = (data_page[15] << 8) | data_page[14];
    MB_NV.RF_Det_RX_calC = (data_page[17] << 8) | data_page[16];

    MB_NV.RF_Det_TX_calA = (data_page[19] << 8) | data_page[18];
    MB_NV.RF_Det_TX_calB = (data_page[21] << 8) | data_page[20];
    MB_NV.RF_Det_TX_calC = (data_page[23] << 8) | data_page[22];

    MB_NV.Vin_sense_calA = (data_page[25] << 8) | data_page[24];
    MB_NV.Vin_sense_calB = (data_page[27] << 8) | data_page[26];
    MB_NV.Vin_sense_calC = (data_page[29] << 8) | data_page[28];

    MB_NV.LNBV_sense_calA = (data_page[31] << 8) | data_page[30];
    MB_NV.LNBV_sense_calB = (data_page[33] << 8) | data_page[32];
    MB_NV.LNBV_sense_calC = (data_page[35] << 8) | data_page[34];

    MB_NV.Imon_calA       = (data_page[37] << 8) | data_page[36];
    MB_NV.Imon_calB       = (data_page[39] << 8) | data_page[38];

    MB_NV.P5v5_sense_calA = (data_page[41] << 8) | data_page[40];
    MB_NV.P5v5_sense_calB = (data_page[43] << 8) | data_page[42];


    MB_NV.P3V6_sense_calA = (data_page[45] << 8) | data_page[44];
    MB_NV.P3v6_sense_calB = (data_page[47] << 8) | data_page[46];

    MB_NV.Sampler_Cal = (data_page[49] << 8) | data_page[48];

    MB_NV.Cal_Date_DDMM         = (data_page[51] << 8) | data_page[50];
    MB_NV.Cal_Date_YYYY         = (data_page[53] << 8) | data_page[52];

    MB_NV.Build_Config_Flags1   = (data_page[55] << 8) | data_page[54];
    MB_NV.Build_Config_Flags2   = (data_page[57] << 8) | data_page[56];
    MB_NV.Reserved1             = (data_page[59] << 8) | data_page[58];
    MB_NV.Reserved2             = (data_page[61] << 8) | data_page[60];
    //61 and 62 set to 0 ad padding
    uint32_t crc_read = (data_page[67] << 24) | (data_page[66] << 16) | (data_page[65] << 8) | (data_page[64]); //CRC programmed

    //Calculate CRC based on struct
    uint16_t offset = offsetof(MB_NV_t, Reserved2) + 2*sizeof(MB_NV.Reserved2); //Offset to end of struct and padding
    __HAL_CRC_DR_RESET(&hcrc);
    uint32_t crc_calc = HAL_CRC_Calculate(&hcrc, (uint32_t*)&MB_NV, offset/4);

    status = EEPROM_Read(MB_EEPROM_ADDR, CH_NV_ErrorHist_addr_addr, data_page, sizeof(data_page));
    if (status != HAL_OK){
    	return 1;
    }

    error_pointer = (data_page[1] << 8 | data_page[0]);

    for(uint8_t i = 0; i < NERRORS; i++){
    	error_codes[i] = (data_page[3+(2*i)] << 8 | data_page[2+(2*i)]);
    }

    if(crc_read == crc_calc){
    	return 0;
    }
    else{
    	return 1;
    }
}


uint8_t EEPROM_Read_CH(void){
    uint8_t status = HAL_ERROR;
    uint8_t data_page[140] = {0};       //64 bytes buffer for 32 fields (each 16 bit)

    status = EEPROM_Read(CH_EEPROM_ADDR, CH_NV_ProjectID_addr, data_page, sizeof(data_page));
    if (status != HAL_OK){
    	return 1;
    }
    memset(&CH_NV, 0, sizeof(CH_NV));
    CH_NV.ProjectID      = read_u16_le(&data_page[0]);
    CH_NV.HW_major_rev   = read_u16_le(&data_page[2]);
    CH_NV.HW_minor_rev   = read_u16_le(&data_page[4]);
    CH_NV.Serial_NoA     = read_u16_le(&data_page[6]);
    CH_NV.Serial_NoB     = read_u16_le(&data_page[8]);
    CH_NV.TestVar0       = read_u16_le(&data_page[10]);

    CH_NV.LO1_F       = read_u64_le(&data_page[12]);
    CH_NV.LO2_F       = read_u64_le(&data_page[20]);
    CH_NV.TRX_F       = read_u64_le(&data_page[28]);
    CH_NV.UP_Link_F   = read_u64_le(&data_page[36]);		//POKE64 does not work for 5th byte, 10500000000 does not work max is 4294967295
    CH_NV.DOWN_Link_F = read_u64_le(&data_page[44]);

    CH_NV.f_step = read_u16_le(&data_page[52]);

    CH_NV.Up_Link_High_f   = read_u64_le(&data_page[54]);
    CH_NV.Up_Link_low_f    = read_u64_le(&data_page[62]);
    CH_NV.DOWN_link_high_f = read_u64_le(&data_page[70]);
    CH_NV.DOWN_link_low_f  = read_u64_le(&data_page[78]);
    CH_NV.LNB_LO		   = read_u64_le(&data_page[86]);

    CH_NV.attenuation_code_LNB = read_u16_le(&data_page[94]);
    CH_NV.attenuation_code_TRX = read_u16_le(&data_page[96]);

    CH_NV.tx_pwr 		 = read_u16_le(&data_page[98]);
    CH_NV.AGC_setting    = read_u16_le(&data_page[100]);
    CH_NV.TRX_RX 		 = read_u16_le(&data_page[102]);
    CH_NV.LNB_Setting    = read_u16_le(&data_page[104]);
    CH_NV.VFO_Settings	 = read_u16_le(&data_page[106]);
    CH_NV.RX_LNA 		 = read_u16_le(&data_page[108]);
    CH_NV.temp_sens_disp = read_u16_le(&data_page[110]);
    CH_NV.Pin_Settings   = read_u16_le(&data_page[112]);

    CH_NV.Cal_Date_DDMM = read_u16_le(&data_page[114]);
    CH_NV.Cal_Date_YYYY = read_u16_le(&data_page[116]);
    CH_NV.Build_Config_Flags1 = read_u16_le(&data_page[118]);
    CH_NV.Build_Config_Flags2 = read_u16_le(&data_page[120]);
    CH_NV.Brightness = read_u16_le(&data_page[122]);
    CH_NV.Reserved1 = read_u16_le(&data_page[124]);

    //126, 127 are padding, DNC

    uint32_t crc_read = (data_page[131] << 24) | (data_page[130] << 16) | (data_page[129] << 8) | (data_page[128]); //CRC programmed

    //Calculate CRC based on struct
    uint16_t offset = offsetof(CH_NV_t, Reserved1) + 2*sizeof(CH_NV.Reserved1); //Offset to end of struct and padding
    __HAL_CRC_DR_RESET(&hcrc);
    uint32_t crc_calc = HAL_CRC_Calculate(&hcrc, (uint32_t*)&CH_NV, offset/4);



    //Power on read
    status = EEPROM_Read(CH_EEPROM_ADDR, CH_NV_PowerOn_addr, data_page, sizeof(data_page));
    if (status != HAL_OK){
    	return 1;
    }

    PowerOn = ((data_page[3] << 24) | (data_page[2] << 16) | (data_page[1] << 8) | (data_page[0]));
    ErrorHist_addr = ((data_page[5] << 8) | (data_page[4]));

    uint8_t err_idx = 6;

    for(uint8_t i = 0; i < ERROR_LIST_LEN; i++){
    	LastError[i] = ((data_page[(err_idx+1)] << 8) | (data_page[err_idx]));
    	err_idx += 2;
    }

    //Increment power on counter
    PowerOn++;
    EEPROM_Write(CH_EEPROM_ADDR, CH_NV_PowerOn_addr, (uint8_t*)&PowerOn, sizeof(PowerOn));


    if(crc_read == crc_calc){
    	return 0;
    }
    else{
    	return 1;
    }


}




uint8_t EEPROM_Read_LO1(void){
    uint8_t status = HAL_ERROR;
    uint8_t data_page[140] = {0};       //64 bytes buffer for 32 fields (each 16 bit)

    status = EEPROM_Read(PLL1_EEPROM_ADDR, PLL_NV_ProjectID_addr, data_page, sizeof(data_page));
    if (status != HAL_OK){
    	return 1;
    }

    PLL1_NV.ProjectID      = read_u16_le(&data_page[PLL_NV_ProjectID_addr]);
    PLL1_NV.HW_major_rev   = read_u16_le(&data_page[PLL_NV_HW_major_rev_addr]);
    PLL1_NV.HW_minor_rev   = read_u16_le(&data_page[PLL_NV_HW_minor_rev_addr]);
    PLL1_NV.Serial_NoA     = read_u16_le(&data_page[PLL_NV_Serial_NoA_addr]);
    PLL1_NV.Serial_NoB     = read_u16_le(&data_page[PLL_NV_Serial_NoB_addr]);
    PLL1_NV.TestVar0       = read_u16_le(&data_page[PLL_NV_TestVar0_addr]);

    PLL1_NV.Doubler_RX        = read_u16_le(&data_page[PLL_NV_Doubler_RX_addr]);
    PLL1_NV.PreR_RX       	  = read_u16_le(&data_page[PLL_NV_PreR_RX_addr]);
    PLL1_NV.Multiplier_RX     = read_u16_le(&data_page[PLL_NV_Multiplier_RX_addr]);
    PLL1_NV.Rdivider_RX	      = read_u16_le(&data_page[PLL_NV_Rdivider_RX_addr]);
    PLL1_NV.Doubler_TX 		  = read_u16_le(&data_page[PLL_NV_Doubler_TX_addr]);
    PLL1_NV.PreR_TX           = read_u16_le(&data_page[PLL_NV_PreR_TX_addr]);
    PLL1_NV.Multiplier_TX     = read_u16_le(&data_page[PLL_NV_Multiplier_TX_addr]);
    PLL1_NV.Rdivider_TX       = read_u16_le(&data_page[PLL_NV_Rdivider_TX_addr]);
    PLL1_NV.Fraction_Nu_RX    = read_u32_le(&data_page[PLL_NV_Fraction_Nu_RX_addr]);
    PLL1_NV.Fraction_De_RX    = read_u32_le(&data_page[PLL_NV_Fraction_De_RX_addr]);
    PLL1_NV.Ndivider_RX       = read_u32_le(&data_page[PLL_NV_Ndivider_RX_addr]);
    PLL1_NV.ChannelDivider_RX = read_u16_le(&data_page[PLL_NV_ChannelDivider_RX_addr]);
    PLL1_NV.Fraction_Nu_TX    = read_u32_le(&data_page[PLL_NV_Fraction_Nu_TX_addr]);
    PLL1_NV.Fraction_De_TX    = read_u32_le(&data_page[PLL_NV_Fraction_De_TX_addr]);
    PLL1_NV.Ndivider_TX       = read_u32_le(&data_page[PLL_NV_Ndivider_TX_addr]);
    PLL1_NV.ChannelDivider_TX = read_u16_le(&data_page[PLL_NV_ChannelDivider_TX_addr]);
    PLL1_NV.ChargePump     	  = read_u16_le(&data_page[PLL_NV_ChargePump_addr]);
    PLL1_NV.OUTA_PWR          = read_u16_le(&data_page[PLL_NV_OUTA_PWR_addr]);
    PLL1_NV.OUTA_EN           = read_u16_le(&data_page[PLL_NV_OUTA_EN_addr]);
    PLL1_NV.OUTB_PWR          = read_u16_le(&data_page[PLL_NV_OUTB_PWR_addr]);
    PLL1_NV.OUTB_EN           = read_u16_le(&data_page[PLL_NV_OUTB_EN_addr]);
    PLL1_NV.VCTCXO_cal        = read_u16_le(&data_page[PLL_NV_VCTCXO_cal_addr]);

    PLL1_NV.Cal_Date_DDMM 		= read_u16_le(&data_page[PLL_NV_Cal_Date_DDMM_addr]);
    PLL1_NV.Cal_Date_YYYY 		= read_u16_le(&data_page[PLL_NV_Cal_Date_YYYY_addr]);
    PLL1_NV.Build_Config_Flags1 = read_u16_le(&data_page[PLL_NV_Build_Config_Flags1_addr]);
    PLL1_NV.Build_Config_Flags2 = read_u16_le(&data_page[PLL_NV_Build_Config_Flags2_addr]);
    PLL1_NV.Reserved1 		    = read_u16_le(&data_page[PLL_NV_Reserved1_addr]);
    PLL1_NV.Reserved2 		    = read_u16_le(&data_page[PLL_NV_Reserved2_addr]);
    PLL1_NV.Reserved3 		    = read_u16_le(&data_page[PLL_NV_Reserved3_addr]);


    uint32_t crc_read = read_u32_le(&data_page[PLL_NV_CRC_addr]);

    //Calculate CRC based on struct
    uint16_t offset = offsetof(PLL_NV_t, Reserved3) + sizeof(PLL1_NV.Reserved3); //Offset to end of struct and padding
    __HAL_CRC_DR_RESET(&hcrc);
    uint32_t crc_calc = HAL_CRC_Calculate(&hcrc, (uint32_t*)&PLL1_NV, offset/4);

    if(crc_read == crc_calc){
    	return 0;
    }
    else{
    	return 1;
    }

}

uint8_t EEPROM_Read_LO2(void){
    uint8_t status = HAL_ERROR;
    uint8_t data_page[140] = {0};       //64 bytes buffer for 32 fields (each 16 bit)

    status = EEPROM_Read(PLL2_EEPROM_ADDR, PLL_NV_ProjectID_addr, data_page, sizeof(data_page));
    if (status != HAL_OK){
    	return 1;
    }

    PLL2_NV.ProjectID      = read_u16_le(&data_page[PLL_NV_ProjectID_addr]);
    PLL2_NV.HW_major_rev   = read_u16_le(&data_page[PLL_NV_HW_major_rev_addr]);
    PLL2_NV.HW_minor_rev   = read_u16_le(&data_page[PLL_NV_HW_minor_rev_addr]);
    PLL2_NV.Serial_NoA     = read_u16_le(&data_page[PLL_NV_Serial_NoA_addr]);
    PLL2_NV.Serial_NoB     = read_u16_le(&data_page[PLL_NV_Serial_NoB_addr]);
    PLL2_NV.TestVar0       = read_u16_le(&data_page[PLL_NV_TestVar0_addr]);

    PLL2_NV.Doubler_RX        = read_u16_le(&data_page[PLL_NV_Doubler_RX_addr]);
    PLL2_NV.PreR_RX       	  = read_u16_le(&data_page[PLL_NV_PreR_RX_addr]);
    PLL2_NV.Multiplier_RX     = read_u16_le(&data_page[PLL_NV_Multiplier_RX_addr]);
    PLL2_NV.Rdivider_RX	      = read_u16_le(&data_page[PLL_NV_Rdivider_RX_addr]);
    PLL2_NV.Doubler_TX 		  = read_u16_le(&data_page[PLL_NV_Doubler_TX_addr]);
    PLL2_NV.PreR_TX           = read_u16_le(&data_page[PLL_NV_PreR_TX_addr]);
    PLL2_NV.Multiplier_TX     = read_u16_le(&data_page[PLL_NV_Multiplier_TX_addr]);
    PLL2_NV.Rdivider_TX       = read_u16_le(&data_page[PLL_NV_Rdivider_TX_addr]);
    PLL2_NV.Fraction_Nu_RX    = read_u32_le(&data_page[PLL_NV_Fraction_Nu_RX_addr]);
    PLL2_NV.Fraction_De_RX    = read_u32_le(&data_page[PLL_NV_Fraction_De_RX_addr]);
    PLL2_NV.Ndivider_RX       = read_u32_le(&data_page[PLL_NV_Ndivider_RX_addr]);
    PLL2_NV.ChannelDivider_RX = read_u16_le(&data_page[PLL_NV_ChannelDivider_RX_addr]);
    PLL2_NV.Fraction_Nu_TX    = read_u32_le(&data_page[PLL_NV_Fraction_Nu_TX_addr]);
    PLL2_NV.Fraction_De_TX    = read_u32_le(&data_page[PLL_NV_Fraction_De_TX_addr]);
    PLL2_NV.Ndivider_TX       = read_u32_le(&data_page[PLL_NV_Ndivider_TX_addr]);
    PLL2_NV.ChannelDivider_TX = read_u16_le(&data_page[PLL_NV_ChannelDivider_TX_addr]);
    PLL2_NV.ChargePump     	  = read_u16_le(&data_page[PLL_NV_ChargePump_addr]);
    PLL2_NV.OUTA_PWR          = read_u16_le(&data_page[PLL_NV_OUTA_PWR_addr]);
    PLL2_NV.OUTA_EN           = read_u16_le(&data_page[PLL_NV_OUTA_EN_addr]);
    PLL2_NV.OUTB_PWR          = read_u16_le(&data_page[PLL_NV_OUTB_PWR_addr]);
    PLL2_NV.OUTB_EN           = read_u16_le(&data_page[PLL_NV_OUTB_EN_addr]);
    PLL2_NV.VCTCXO_cal        = read_u16_le(&data_page[PLL_NV_VCTCXO_cal_addr]);

    PLL2_NV.Cal_Date_DDMM 		= read_u16_le(&data_page[PLL_NV_Cal_Date_DDMM_addr]);
    PLL2_NV.Cal_Date_YYYY 		= read_u16_le(&data_page[PLL_NV_Cal_Date_YYYY_addr]);
    PLL2_NV.Build_Config_Flags1 = read_u16_le(&data_page[PLL_NV_Build_Config_Flags1_addr]);
    PLL2_NV.Build_Config_Flags2 = read_u16_le(&data_page[PLL_NV_Build_Config_Flags2_addr]);
    PLL2_NV.Reserved1 		    = read_u16_le(&data_page[PLL_NV_Reserved1_addr]);
    PLL2_NV.Reserved2 		    = read_u16_le(&data_page[PLL_NV_Reserved2_addr]);
    PLL2_NV.Reserved3 		    = read_u16_le(&data_page[PLL_NV_Reserved3_addr]);


    uint32_t crc_read = read_u32_le(&data_page[PLL_NV_CRC_addr]);

    //Calculate CRC based on struct
    uint16_t offset = offsetof(PLL_NV_t, Reserved3) + sizeof(PLL2_NV.Reserved3); //Offset to end of struct and padding
    __HAL_CRC_DR_RESET(&hcrc);
    uint32_t crc_calc = HAL_CRC_Calculate(&hcrc, (uint32_t*)&PLL2_NV, offset/4);

    if(crc_read == crc_calc){
    	return 0;
    }
    else{
    	return 1;
    }

}

uint32_t EEPROM_Calculate_CRC(uint8_t EEPROM_addr){
	uint8_t data_page[140] = {0};
	uint32_t CRC32 = 0;
	uint16_t offset = 0;	//Offset to end of struct and padding
	HAL_StatusTypeDef status = HAL_OK;

    status = EEPROM_Read(EEPROM_addr, 0x0, data_page, sizeof(data_page));
    if (status != HAL_OK){
    	return 1;
    }

    //Calculate CRC based on struct
    __HAL_CRC_DR_RESET(&hcrc);

    if(EEPROM_addr == MB_EEPROM_ADDR){
    	offset = offsetof(MB_NV_t, Reserved2) + 2*sizeof(MB_NV.Reserved2);
    	CRC32 = HAL_CRC_Calculate(&hcrc, (uint32_t*)&MB_NV, offset/4);

    }
    else if(EEPROM_addr == CH_EEPROM_ADDR){
    	offset = offsetof(CH_NV_t, Reserved1) + 2*sizeof(CH_NV.Reserved1);
    	CRC32 = HAL_CRC_Calculate(&hcrc, (uint32_t*)&CH_NV, offset/4);
    }
    else{
    	offset = offsetof(PLL_NV_t, Reserved3) + 2*sizeof(PLL1_NV.Reserved3);
    	CRC32 = HAL_CRC_Calculate(&hcrc, (uint32_t*)&PLL1_NV, offset/4);
    }

	return CRC32;
}

void I2C_Scan(void){
	uint8_t scan_addr = 0;
	char msg[8] = {0};
	  for(uint8_t i = 0; i < 128; i++){
		  scan_addr = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 3, 10000);
		  if(scan_addr != HAL_OK){
			  //No ACK received at the address
		  }
		  else if(scan_addr == HAL_OK){
			  snprintf(msg, sizeof(msg), "0x%X \r\n", i);
			  HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
			  osDelay(200); 	//Found a device
		  }
	  }
}


HAL_StatusTypeDef PEEK(uint16_t target, uint16_t address) {
    HAL_StatusTypeDef status = HAL_OK;
    char msg[32];

        uint8_t read_val[2] = {0};
        status = EEPROM_Read(target, address, read_val, sizeof(read_val));

        uint16_t value = ((uint16_t)read_val[1] << 8) | read_val[0];
        snprintf(msg, sizeof(msg), "%u\r\n", value);     //print 16-bit value

    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);
    return status;
}


//Poke commands are split into 16, 32, 64 bits write because SCPI commands have limited length of syntax, these commands are used in service mode only so optimization is not important

HAL_StatusTypeDef POKE(uint16_t target, uint16_t address, uint16_t data){
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t data_arr[2] = {0};
	data_arr[0] = (uint8_t)(data & 0xFF);        //low byte
	data_arr[1] = (uint8_t)((data >> 8) & 0xFF); //high byte
    status = EEPROM_Write(target, address, data_arr, sizeof(data_arr));
	return status;
}

HAL_StatusTypeDef POKE32(uint16_t target, uint16_t address, uint32_t data){
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t data_arr[4] = {0};
	data_arr[0] = (uint8_t)(data & 0xFF);
	data_arr[1] = (uint8_t)((data >> 8) & 0xFF);
	data_arr[2] = (uint8_t)((data >> 16) & 0xFF);
	data_arr[3] = (uint8_t)((data >> 24) & 0xFF);
    status = EEPROM_Write(target, address, data_arr, sizeof(data_arr));
	return status;
}

HAL_StatusTypeDef POKE64(uint16_t i2c_addr, uint16_t address, uint64_t data) {
    uint8_t data_arr[8];

    for (int i = 0; i < 8; i++) {
        data_arr[i] = (uint8_t)(data >> (8 * i));   // little endian
    }

    return EEPROM_Write(i2c_addr, address, data_arr, 8);
}




uint32_t MB_NV_compute_crc32(const MB_NV_t *nv)
{
    const size_t len = sizeof(MB_NV_t);      // 62 bytes
    //STM32 CRC must process 32-bit words,round up to 4 bytes
    const size_t padded_len = (len + 3) & ~((size_t)3);

    //32-bit aligned temporary buffer for HAL_CRC
    uint32_t temp32[(padded_len + 3) / 4];

    memset(temp32, 0, sizeof(temp32));
    memcpy((uint8_t*)temp32, nv, len);

    __HAL_CRC_DR_RESET(&hcrc);
    return HAL_CRC_Calculate(&hcrc, temp32, padded_len / 4);
}

//Helper function that cast and apply shift to unsigned values from EEPROM, little endian encoding for all structs
static uint16_t read_u16_le(uint8_t *p){
    return ((uint16_t)p[1] << 8)  |
           ((uint16_t)p[0]);
}

static uint32_t read_u32_le(uint8_t *p){
    return ((uint64_t)p[3] << 24) |
           ((uint64_t)p[2] << 16) |
           ((uint64_t)p[1] << 8)  |
           ((uint64_t)p[0]);
}

static uint64_t read_u64_le(uint8_t *p){
    return ((uint64_t)p[7] << 56) |
           ((uint64_t)p[6] << 48) |
           ((uint64_t)p[5] << 40) |
           ((uint64_t)p[4] << 32) |
           ((uint64_t)p[3] << 24) |
           ((uint64_t)p[2] << 16) |
           ((uint64_t)p[1] << 8)  |
           ((uint64_t)p[0]);
}



