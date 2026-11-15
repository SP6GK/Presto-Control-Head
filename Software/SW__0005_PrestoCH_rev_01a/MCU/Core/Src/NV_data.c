/*
 * NV_data.c
 *
 *  Created on: Oct 3, 2025
 *      Author: sp6gk
 */

#include "NV_data.h"

// Non volatile data (EEPROMs)
//Struct initialization
 MB_NV_t MB_NV = {0};

//Addresses, macro offsetof proven to be more useful
 uint16_t MB_NV_ProjectID_addr = 0X0;
 uint16_t MB_NV_HW_major_rev_addr = 0X2;
 uint16_t MB_NV_HW_minor_rev_addr = 0X4;
 uint16_t MB_NV_Serial_NoA_addr = 0X6;
 uint16_t MB_NV_Serial_NoB_addr = 0X8;
 uint16_t MB_NV_TestVar0_addr = 0XA;
 uint16_t MB_NV_RF_Det_RX_calA_addr = 0XC;
 uint16_t MB_NV_RF_Det_RX_calB_addr = 0XE;
 uint16_t MB_NV_RF_Det_RX_calC_addr = 0X10;
 uint16_t MB_NV_RF_Det_TX_calA_addr = 0X12;
 uint16_t MB_NV_RF_Det_TX_calB_addr = 0X14;
 uint16_t MB_NV_RF_Det_TX_calC_addr = 0X16;
 uint16_t MB_NV_Vin_sense_calA_addr = 0X18;
 uint16_t MB_NV_Vin_sense_calB_addr = 0X1A;
 uint16_t MB_NV_Vin_sense_calC_addr = 0X1C;
 uint16_t MB_NV_LNBV_sense_calA_addr = 0X1E;
 uint16_t MB_NV_LNBV_sense_calB_addr = 0X20;
 uint16_t MB_NV_LNBV_sense_calC_addr = 0X22;
 uint16_t MB_NV_Imon_calA_addr = 0X24;
 uint16_t MB_NV_Imon_calB_addr = 0X26;
 uint16_t MB_NV_P5v5_sense_calA_addr = 0X28;
 uint16_t MB_NV_P5v5_sense_calB_addr = 0X2A;
 uint16_t MB_NV_P3V6_sense_calA_addr = 0X2C;
 uint16_t MB_NV_P3v6_sense_calB_addr = 0X2E;
 uint16_t MB_NV_Sampler_Cal_addr = 0X30;
 uint16_t MB_NV_Cal_Date_DDMM_addr = 0X32;
 uint16_t MB_NV_Cal_Date_YYYY_addr = 0X34;
 uint16_t MB_NV_Build_Config_Flags1_addr = 0X36;
 uint16_t MB_NV_Build_Config_Flags2_addr = 0X38;
 uint16_t MB_NV_Reserved1_addr = 0X3A;
 uint16_t MB_NV_Reserved2_addr = 0X3C;
 uint16_t MB_NV_CRC_addr = 0x40;

 uint32_t PowerOn = 0;
 uint16_t ErrorHist_addr = 0;
 uint16_t LastError[ERROR_LIST_LEN] = {0};

//Control head EEPROM struct
 CH_NV_t CH_NV = {0};

 uint16_t CH_NV_ProjectID_addr = 0X0;
 uint16_t CH_NV_HW_major_rev_addr = 0X2;
 uint16_t CH_NV_HW_minor_rev_addr = 0X4;
 uint16_t CH_NV_Serial_NoA_addr = 0X6;
 uint16_t CH_NV_Serial_NoB_addr = 0X8;
 uint16_t CH_NV_TestVar0_addr = 0XA;
 uint16_t CH_NV_LO1_F_addr = 0XC;
 uint16_t CH_NV_LO2_F_addr = 0X14;
 uint16_t CH_NV_TRX_F_addr = 0X1C;
 uint16_t CH_NV_UP_Link_F_addr = 0X24;
 uint16_t CH_NV_DOWN_Link_F_addr = 0X2C;
 uint16_t CH_NV_f_step_addr = 0X34;
 uint16_t CH_NV_Up_Link_High_f_addr = 0X36;
 uint16_t CH_NV_Up_Link_low_f_addr = 0X3E;
 uint16_t CH_NV_DOWN_link_high_f_addr = 0X46;
 uint16_t CH_NV_DOWN_link_low_f_addr = 0X4E;
 uint16_t CH_NV_LNB_LO_addr = 0X56;
 uint16_t CH_NV_attenuation_code_LNB_addr = 0X5E;
 uint16_t CH_NV_attenuation_code_TRX_addr = 0X60;
 uint16_t CH_NV_tx_pwr_addr = 0X62;
 uint16_t CH_NV_AGC_setting_addr = 0X64;
 uint16_t CH_NV_TRX_RX_addr = 0X66;
 uint16_t CH_NV_LNB_Setting_addr = 0X68;
 uint16_t CH_NV_VFO_Settings_addr = 0X6A;
 uint16_t CH_NV_RX_LNA_addr = 0X6C;
 uint16_t CH_NV_temp_sens_disp_addr = 0X6E;
 uint16_t CH_NV_Pin_Settings_addr = 0X70;
 uint16_t CH_NV_Cal_Date_DDMM_addr = 0X72;
 uint16_t CH_NV_Cal_Date_YYYY_addr = 0X74;
 uint16_t CH_NV_Build_Config_Flags1_addr = 0X76;
 uint16_t CH_NV_Build_Config_Flags2_addr = 0X78;
 uint16_t CH_NV_Brightness_addr = 0X7A;
 uint16_t CH_NV_Reserved1_addr = 0X7C;
 uint16_t CH_NV_CRC32_addr = 0X80;
 uint16_t CH_NV_PowerOn_addr = 0X8C;
 uint16_t CH_NV_ErrorHist_addr_addr = 0X90;

 uint16_t CH_NV_Error_Code1_addr = 0x92;
 uint16_t CH_NV_Error_Code2_addr = 0x94;
 uint16_t CH_NV_Error_Code3_addr = 0x96;
 uint16_t CH_NV_Error_Code4_addr = 0x98;

 //uint16_t CH_NV_CRC_addr = 0x9A;

 //PLL EEPROM
 PLL_NV_t PLL1_NV = {0};
 PLL_NV_t PLL2_NV = {0};

 uint16_t PLL_NV_ProjectID_addr = 0X0;
 uint16_t PLL_NV_HW_major_rev_addr = 0X2;
 uint16_t PLL_NV_HW_minor_rev_addr = 0X4;
 uint16_t PLL_NV_Serial_NoA_addr = 0X6;
 uint16_t PLL_NV_Serial_NoB_addr = 0X8;
 uint16_t PLL_NV_TestVar0_addr = 0XA;
 uint16_t PLL_NV_Doubler_RX_addr = 0XC;
 uint16_t PLL_NV_PreR_RX_addr = 0XE;
 uint16_t PLL_NV_Multiplier_RX_addr = 0X10;
 uint16_t PLL_NV_Rdivider_RX_addr = 0X12;
 uint16_t PLL_NV_Doubler_TX_addr = 0X14;
 uint16_t PLL_NV_PreR_TX_addr = 0X16;
 uint16_t PLL_NV_Multiplier_TX_addr = 0X18;
 uint16_t PLL_NV_Rdivider_TX_addr = 0X1A;
 uint16_t PLL_NV_Fraction_Nu_RX_addr = 0X1C;
 uint16_t PLL_NV_Fraction_De_RX_addr = 0X20;
 uint16_t PLL_NV_Ndivider_RX_addr = 0X24;
 uint16_t PLL_NV_ChannelDivider_RX_addr = 0X28;
 uint16_t PLL_NV_Fraction_Nu_TX_addr = 0X2A;
 uint16_t PLL_NV_Fraction_De_TX_addr = 0X2E;
 uint16_t PLL_NV_Ndivider_TX_addr = 0X32;
 uint16_t PLL_NV_ChannelDivider_TX_addr = 0X36;
 uint16_t PLL_NV_ChargePump_addr = 0X38;
 uint16_t PLL_NV_OUTA_PWR_addr = 0X3A;
 uint16_t PLL_NV_OUTA_EN_addr = 0X3C;
 uint16_t PLL_NV_OUTB_PWR_addr = 0X3E;
 uint16_t PLL_NV_OUTB_EN_addr = 0X40;
 uint16_t PLL_NV_VCTCXO_cal_addr = 0X42;
 uint16_t PLL_NV_Cal_Date_DDMM_addr = 0X44;
 uint16_t PLL_NV_Cal_Date_YYYY_addr = 0X46;
 uint16_t PLL_NV_Build_Config_Flags1_addr = 0X48;
 uint16_t PLL_NV_Build_Config_Flags2_addr = 0X4A;
 uint16_t PLL_NV_Reserved1_addr = 0X4C;
 uint16_t PLL_NV_Reserved2_addr = 0X4E;
 uint16_t PLL_NV_Reserved3_addr = 0X50;
 uint16_t PLL_NV_CRC_addr = 0X52;


