/*
 * NV_data.h
 *
 *  Created on: Oct 3, 2025
 *      Author: sp6gk
 */

#ifndef INC_NV_DATA_H_
#define INC_NV_DATA_H_

#define ERROR_LIST_LEN 16
#define NON_CRC_OFFSET_STRUCT 16

#include "main.h"
#include "cmsis_os.h"
#include "stdbool.h"
#include <semphr.h>

// Non volatile data (EEPROMs)
#pragma pack(push, 1)
typedef struct{
    uint16_t ProjectID;
    uint16_t HW_major_rev;
    uint16_t HW_minor_rev;
    uint16_t Serial_NoA;
    uint16_t Serial_NoB;
    uint16_t TestVar0;
    uint16_t RF_Det_RX_calA;
    uint16_t RF_Det_RX_calB;
    uint16_t RF_Det_RX_calC;
    uint16_t RF_Det_TX_calA;
    uint16_t RF_Det_TX_calB;
    uint16_t RF_Det_TX_calC;
    uint16_t Vin_sense_calA;
    uint16_t Vin_sense_calB;
    uint16_t Vin_sense_calC;
    uint16_t LNBV_sense_calA;
    uint16_t LNBV_sense_calB;
    uint16_t LNBV_sense_calC;
    uint16_t Imon_calA;
    uint16_t Imon_calB;
    uint16_t P5v5_sense_calA;
    uint16_t P5v5_sense_calB;
    uint16_t P3V6_sense_calA;
    uint16_t P3v6_sense_calB;
    uint16_t Sampler_Cal;
    uint16_t Cal_Date_DDMM;
    uint16_t Cal_Date_YYYY;
    uint16_t Build_Config_Flags1;
    uint16_t Build_Config_Flags2;
    uint16_t Reserved1;
    uint16_t Reserved2;
} MB_NV_t;
#pragma pack(pop)

extern MB_NV_t MB_NV;

extern uint16_t MB_NV_ProjectID_addr;
extern uint16_t MB_NV_HW_major_rev_addr;
extern uint16_t MB_NV_HW_minor_rev_addr;
extern uint16_t MB_NV_Serial_NoA_addr;
extern uint16_t MB_NV_Serial_NoB_addr;
extern uint16_t MB_NV_TestVar0_addr;
extern uint16_t MB_NV_RF_Det_RX_calA_addr;
extern uint16_t MB_NV_RF_Det_RX_calB_addr;
extern uint16_t MB_NV_RF_Det_RX_calC_addr;
extern uint16_t MB_NV_RF_Det_TX_calA_addr;
extern uint16_t MB_NV_RF_Det_TX_calB_addr;
extern uint16_t MB_NV_RF_Det_TX_calC_addr;
extern uint16_t MB_NV_Vin_sense_calA_addr;
extern uint16_t MB_NV_Vin_sense_calB_addr;
extern uint16_t MB_NV_Vin_sense_calC_addr;
extern uint16_t MB_NV_LNBV_sense_calA_addr;
extern uint16_t MB_NV_LNBV_sense_calB_addr;
extern uint16_t MB_NV_LNBV_sense_calC_addr;
extern uint16_t MB_NV_Imon_calA_addr;
extern uint16_t MB_NV_Imon_calB_addr;
extern uint16_t MB_NV_P5v5_sense_calA_addr;
extern uint16_t MB_NV_P5v5_sense_calB_addr;
extern uint16_t MB_NV_P3V6_sense_calA_addr;
extern uint16_t MB_NV_P3v6_sense_calB_addr;
extern uint16_t MB_NV_Sampler_Cal_addr;
extern uint16_t MB_NV_Cal_Date_DDMM_addr;
extern uint16_t MB_NV_Cal_Date_YYYY_addr;
extern uint16_t MB_NV_Build_Config_Flags1_addr;
extern uint16_t MB_NV_Build_Config_Flags2_addr;
extern uint16_t MB_NV_Reserved1_addr;
extern uint16_t MB_NV_Reserved2_addr;
extern uint16_t MB_NV_CRC_addr;



#pragma pack(push, 1)
typedef struct{
    uint16_t ProjectID;
    uint16_t HW_major_rev;
    uint16_t HW_minor_rev;
    uint16_t Serial_NoA;
    uint16_t Serial_NoB;
    uint16_t TestVar0;
    uint64_t LO1_F;
    uint64_t LO2_F;
    uint64_t TRX_F;
    uint64_t UP_Link_F;
    uint64_t DOWN_Link_F;
    uint16_t f_step;
    uint64_t Up_Link_High_f;
    uint64_t Up_Link_low_f;
    uint64_t DOWN_link_high_f;
    uint64_t DOWN_link_low_f;
    uint64_t LNB_LO;
    uint16_t attenuation_code_LNB;
    uint16_t attenuation_code_TRX;
    uint16_t tx_pwr;
    uint16_t AGC_setting;
    uint16_t TRX_RX;
    uint16_t LNB_Setting;
    uint16_t VFO_Settings;
    uint16_t RX_LNA;
    uint16_t temp_sens_disp;
    uint16_t Pin_Settings;
    uint16_t Cal_Date_DDMM;
    uint16_t Cal_Date_YYYY;
    uint16_t Build_Config_Flags1;
    uint16_t Build_Config_Flags2;
    uint16_t Brightness;			//This is overkill to handle brightness alone, 4 bits is enough, rest can be reused.
    uint16_t Reserved1;
} CH_NV_t;
#pragma pack(pop)

extern uint32_t PowerOn;
extern uint16_t ErrorHist_addr;
extern uint16_t LastError[ERROR_LIST_LEN];

extern CH_NV_t CH_NV;

extern uint16_t CH_NV_ProjectID_addr;
extern uint16_t CH_NV_HW_major_rev_addr;
extern uint16_t CH_NV_HW_minor_rev_addr;
extern uint16_t CH_NV_Serial_NoA_addr;
extern uint16_t CH_NV_Serial_NoB_addr;
extern uint16_t CH_NV_TestVar0_addr;
extern uint16_t CH_NV_LO1_F_addr;
extern uint16_t CH_NV_LO2_F_addr;
extern uint16_t CH_NV_TRX_F_addr;
extern uint16_t CH_NV_UP_Link_F_addr;
extern uint16_t CH_NV_DOWN_Link_F_addr;
extern uint16_t CH_NV_f_step_addr;
extern uint16_t CH_NV_Up_Link_High_f_addr;
extern uint16_t CH_NV_Up_Link_low_f_addr;
extern uint16_t CH_NV_DOWN_link_high_f_addr;
extern uint16_t CH_NV_DOWN_link_low_f_addr;
extern uint16_t CH_NV_LNB_LO_addr;
extern uint16_t CH_NV_attenuation_code_LNB_addr;
extern uint16_t CH_NV_attenuation_code_TRX_addr;
extern uint16_t CH_NV_tx_pwr_addr;
extern uint16_t CH_NV_AGC_setting_addr;
extern uint16_t CH_NV_TRX_RX_addr;
extern uint16_t CH_NV_LNB_Setting_addr;
extern uint16_t CH_NV_VFO_Settings_addr;
extern uint16_t CH_NV_RX_LNA_addr;
extern uint16_t CH_NV_temp_sens_disp_addr;
extern uint16_t CH_NV_Pin_Settings_addr;
extern uint16_t CH_NV_Cal_Date_DDMM_addr;
extern uint16_t CH_NV_Cal_Date_YYYY_addr;
extern uint16_t CH_NV_Build_Config_Flags1_addr;
extern uint16_t CH_NV_Build_Config_Flags2_addr;
extern uint16_t CH_NV_Brightness_addr;
extern uint16_t CH_NV_Reserved1_addr;

extern uint16_t CH_NV_PowerOn_addr;
extern uint16_t CH_NV_ErrorHist_addr_addr;

extern uint16_t CH_NV_Error_Code1_addr;
extern uint16_t CH_NV_Error_Code2_addr;
extern uint16_t CH_NV_Error_Code3_addr;
extern uint16_t CH_NV_Error_Code4_addr;

extern uint16_t CH_NV_CRC32_addr;

//PLL
#pragma pack(push, 1)
typedef struct{
    uint16_t ProjectID;
    uint16_t HW_major_rev;
    uint16_t HW_minor_rev;
    uint16_t Serial_NoA;
    uint16_t Serial_NoB;
    uint16_t TestVar0;
    uint16_t Doubler_RX;
    uint16_t PreR_RX;
    uint16_t Multiplier_RX;
    uint16_t Rdivider_RX;
    uint16_t Doubler_TX;
    uint16_t PreR_TX;
    uint16_t Multiplier_TX;
    uint16_t Rdivider_TX;
    uint32_t Fraction_Nu_RX;
    uint32_t Fraction_De_RX;
    uint32_t Ndivider_RX;
    uint16_t ChannelDivider_RX;
    uint32_t Fraction_Nu_TX;
    uint32_t Fraction_De_TX;
    uint32_t Ndivider_TX;
    uint16_t ChannelDivider_TX;
    uint16_t ChargePump;
    uint16_t OUTA_PWR;
    uint16_t OUTA_EN;
    uint16_t OUTB_PWR;
    uint16_t OUTB_EN;
    uint16_t VCTCXO_cal;
    uint16_t Cal_Date_DDMM;
    uint16_t Cal_Date_YYYY;
    uint16_t Build_Config_Flags1;
    uint16_t Build_Config_Flags2;
    uint16_t Reserved1;
    uint16_t Reserved2;
    uint16_t Reserved3;
} PLL_NV_t;
#pragma pack(pop)

extern PLL_NV_t PLL1_NV;
extern PLL_NV_t PLL2_NV;

extern uint16_t PLL_NV_ProjectID_addr;
extern uint16_t PLL_NV_HW_major_rev_addr;
extern uint16_t PLL_NV_HW_minor_rev_addr;
extern uint16_t PLL_NV_Serial_NoA_addr;
extern uint16_t PLL_NV_Serial_NoB_addr;
extern uint16_t PLL_NV_TestVar0_addr;
extern uint16_t PLL_NV_Doubler_RX_addr;
extern uint16_t PLL_NV_PreR_RX_addr;
extern uint16_t PLL_NV_Multiplier_RX_addr;
extern uint16_t PLL_NV_Rdivider_RX_addr;
extern uint16_t PLL_NV_Doubler_TX_addr;
extern uint16_t PLL_NV_PreR_TX_addr;
extern uint16_t PLL_NV_Multiplier_TX_addr;
extern uint16_t PLL_NV_Rdivider_TX_addr;
extern uint16_t PLL_NV_Fraction_Nu_RX_addr;
extern uint16_t PLL_NV_Fraction_De_RX_addr;
extern uint16_t PLL_NV_Ndivider_RX_addr;
extern uint16_t PLL_NV_ChannelDivider_RX_addr;
extern uint16_t PLL_NV_Fraction_Nu_TX_addr;
extern uint16_t PLL_NV_Fraction_De_TX_addr;
extern uint16_t PLL_NV_Ndivider_TX_addr;
extern uint16_t PLL_NV_ChannelDivider_TX_addr;
extern uint16_t PLL_NV_ChargePump_addr;
extern uint16_t PLL_NV_OUTA_PWR_addr;
extern uint16_t PLL_NV_OUTA_EN_addr;
extern uint16_t PLL_NV_OUTB_PWR_addr;
extern uint16_t PLL_NV_OUTB_EN_addr;
extern uint16_t PLL_NV_VCTCXO_cal_addr;
extern uint16_t PLL_NV_Cal_Date_DDMM_addr;
extern uint16_t PLL_NV_Cal_Date_YYYY_addr;
extern uint16_t PLL_NV_Build_Config_Flags1_addr;
extern uint16_t PLL_NV_Build_Config_Flags2_addr;
extern uint16_t PLL_NV_Reserved1_addr;
extern uint16_t PLL_NV_Reserved2_addr;
extern uint16_t PLL_NV_Reserved3_addr;
extern uint16_t PLL_NV_CRC_prog_addr;

extern uint16_t PLL_NV_CRC_addr;


#endif /* INC_NV_DATA_H_ */
