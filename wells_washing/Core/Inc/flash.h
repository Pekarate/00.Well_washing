/*
 * flash.c
 *
 *  Created on: Mar 15, 2023
 *      Author: ADMIN
 */

#ifndef INC_FLASH_C_
#define INC_FLASH_C_
#include "stm32f4xx_hal.h"
HAL_StatusTypeDef write_flash(uint8_t* data, uint32_t size, uint32_t address);
void read_flash(uint8_t* data, uint32_t size, uint32_t address);


#endif /* INC_FLASH_C_ */
