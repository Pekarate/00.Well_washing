#include "stm32f4xx_hal.h"

//#define FLASH_SECTOR_7 ((uint32_t)0x08060000) // Example: Sector 7 on STM32F407VET6

/**
 * @brief Writes data to flash memory
 * @param data Pointer to the data to be written
 * @param size Size of the data to be written, in bytes
 * @param address Memory address where the data will be written
 * @return HAL status
 */
HAL_StatusTypeDef write_flash(uint8_t* data, uint32_t size, uint32_t address) {
    HAL_StatusTypeDef status = HAL_OK;
    // Unlock flash
    HAL_FLASH_Unlock();

    // Erase sector
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
	FLASH_Erase_Sector(FLASH_SECTOR_7, VOLTAGE_RANGE_3);

    if (status == HAL_OK) {
        // Write data
        for (uint32_t i = 0; i < size; i++) {
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, address + i, data[i]);
            if (status != HAL_OK) {
                break;
            }
        }
    }

    // Lock flash
    HAL_FLASH_Lock();

    return status;
}

/**
 * @brief Reads data from flash memory
 * @param data Pointer to the buffer where the data will be stored
 * @param size Size of the data to be read, in bytes
 * @param address Memory address where the data will be read from
 */
void read_flash(uint8_t* data, uint32_t size, uint32_t address) {
    for (uint32_t i = 0; i < size; i++) {
        data[i] = *((uint8_t*)(address + i));
    }
}
