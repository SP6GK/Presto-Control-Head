/*
 * 24LC_EEPROM.h
 *
 *  Created on: Sep 27, 2025
 *      Author: sp6gk
 */

#ifndef INC_24LC_EEPROM_H_
#define INC_24LC_EEPROM_H_

#define MB_EEPROM_ADDR 0x51
#define CH_EEPROM_ADDR 0x50
#define PLL1_EEPROM_ADDR 0x53
#define PLL2_EEPROM_ADDR 0x52 //TODO TBD addresses

//Queque Flags
#define WRITE_PENDING 1

extern I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef EEPROM_Write(uint16_t i2c_addr, uint16_t memory_addr,uint8_t *data, uint16_t size);
HAL_StatusTypeDef EEPROM_Read(uint16_t i2c_addr, uint16_t memory_addr,uint8_t *data, uint16_t size);
HAL_StatusTypeDef EEPROM_Check(uint16_t i2c_addr);

uint8_t EEPROM_Read_MB(void);
uint8_t EEPROM_Read_CH(void);
uint8_t EEPROM_Read_LO1(void);
uint8_t EEPROM_Read_LO2(void);


//Maintenance /Factory functions
uint8_t EEPROM_Write_MB_Calibration(void);
uint8_t EEPROM_Write_FactoryData(void);

void I2C_Scan(void);


//Part of BASIC
HAL_StatusTypeDef PEEK(uint16_t target, uint16_t address);
HAL_StatusTypeDef PEEK2(uint16_t target, uint16_t address, uint16_t length);
HAL_StatusTypeDef POKE(uint16_t target, uint16_t address, uint16_t data);
HAL_StatusTypeDef POKE32(uint16_t target, uint16_t address, uint32_t data);
HAL_StatusTypeDef POKE64(uint16_t target, uint16_t address, uint64_t data);

uint32_t EEPROM_Calculate_CRC(uint8_t EEPROM_addr);

uint32_t MB_NV_compute_crc32();

static uint16_t read_u16_le(uint8_t *p);
static uint32_t read_u32_le(uint8_t *p);
static uint64_t read_u64_le(uint8_t *p);


//TODO check what functions can be set to static and remove them from headers

#endif /* INC_24LC_EEPROM_H_ */
