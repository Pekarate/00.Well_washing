/*
 * data.c
 *
 *  Created on: Mar 15, 2023
 *      Author: ADMIN
 */
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include "string.h"
#include "flash.h"
#include "data.h"
#include "motor.h"
_system_data system_data;

const _flash_data flash_defaul = {
									.Z_bottom_pos = 5000,
									.Well_position ={5000,1000,2000,3000,4000,5000,6000,7000,8000,9000},
									.Program_para ={
														{ //program 1
																{.type = 1,.wells = 1,.timing = {1,2,3,4,5,6,7}}, //step1
																{.type = 1,.wells = 2,.timing = {1,1,3,4,5,6,7}}, //step2
																{.type = 1,.wells = 3,.timing = {1,2,3,4,5,6,7}}, //step3
																{.type = 1,.wells = 4}, //step4
																{.type = 1,.wells = 5}, //step5
																{.type = 0,.wells = 6}, //step6
//																{.type = 1,.wells = 7}  //step7
														},
														{ //program 2
																{.type = 1,.wells = 1,.timing = {1,2,3,4,5,6}}, //step1
																{.type = 1,.wells = 1}, //step2
																{.type = 1,.wells = 1}, //step3
																{.type = 1,.wells = 1}, //step4
																{.type = 1,.wells = 1}, //step5
//																{.type = 1,.wells = 1}, //step6
//																{.type = 1,.wells = 1}  //step7
														},
														{ //program 3
																{.type = 3,.wells = 1,.timing = {1,2,3,4,5,6}}, //step1
																{.type = 2,.wells = 1}, //step2
																{.type = 1,.wells = 1}, //step3
//																{.type = 1,.wells = 1}, //step4
//																{.type = 1,.wells = 1}, //step5
//																{.type = 1,.wells = 1}, //step6
//																{.type = 1,.wells = 1}  //step7
														},
														{ //program 4
																{.type = 1,.wells = 1}, //step1
																{.type = 1,.wells = 1}, //step2
																{.type = 1,.wells = 1}, //step3
																{.type = 1,.wells = 1}, //step4
																{.type = 1,.wells = 1}, //step5
																{.type = 1,.wells = 1}, //step6
																{.type = 1,.wells = 1}  //step7
														},
														{ //program 5
																{.type = 1,.wells = 1}, //step1
																{.type = 1,.wells = 1}, //step2
																{.type = 1,.wells = 1}, //step3
																{.type = 1,.wells = 1}, //step4
																{.type = 1,.wells = 1}, //step5
																{.type = 1,.wells = 1}, //step6
																{.type = 1,.wells = 1}  //step7
														}
													},
									.factory_code = FACTORY_CODE
									};


_def_step step;


int is_data_init()
{
	if(system_data.flash_data.factory_code != FACTORY_CODE)
	{
		return 0;
	}
	return 1;
}

void dt_calculator_pg_stepnumber(void){
	for(int i=0;i<10;i++)
	{
		system_data.pg_stepnumber[i] =0;
		for(int j=0;j<25;j++)
		{
			if(system_data.flash_data.Program_para[i][j].type == STEP_TYPE_NONE )
			{
				break;
			}
			system_data.pg_stepnumber[i]++;
		}
	}
}
void dt_calculator_pg_total_time()
{
	for(int i=0;i<10;i++)
	{
//		@TODO
//		system_data.pg_stepnumber[i] =0;
//		for(int j=0;j<25;i++)
//		{
//			if(system_data.flash_data.Program_para[i][j].type == STEP_TYPE_NONE )
//			{
//				break;
//			}
//			system_data.pg_stepnumber[i]++;
//		}
	}
}

int is_step_differrent(uint8_t pg,uint8_t stepindex ,_def_step step){
	return memcmp(&system_data.flash_data.Program_para[pg][stepindex], &step, sizeof(_def_step));
}

void dt_Modify_step(uint8_t pg,uint8_t stepindex ,_def_step step)
{
	if(is_step_differrent(pg,stepindex ,step))
	{
		memcpy(&system_data.flash_data.Program_para[pg][stepindex],&step,sizeof(_def_step));
		dt_store_system_data();
	}
}

void dt_system_data_init(void)
{
	read_flash((uint8_t *)&system_data.flash_data, sizeof(_flash_data), FLASH_START_ADDRESS);
	if(!is_data_init())
	{
		memcpy(&system_data.flash_data,&flash_defaul,sizeof(_flash_data));
		if(write_flash((uint8_t *)&system_data.flash_data, sizeof(_flash_data), FLASH_START_ADDRESS) != HAL_OK)
		{
			//do when write flash fail
			__NOP();
		}
	}
	dt_calculator_pg_stepnumber();
}

HAL_StatusTypeDef dt_store_system_data(void)
{
	HAL_StatusTypeDef res = write_flash((uint8_t *)&system_data.flash_data, sizeof(_flash_data), FLASH_START_ADDRESS);
	if( res != HAL_OK){
		printf("wirte flash fail! error code : %d",res);
	}
	return res;
}



