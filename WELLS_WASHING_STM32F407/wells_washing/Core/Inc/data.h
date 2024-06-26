/*
 * data.h
 *
 *  Created on: Mar 15, 2023
 *      Author: ADMIN
 */

#ifndef INC_DATA_H_
#define INC_DATA_H_
#include "main.h"
#include "stdint.h"




typedef struct{
	uint8_t wells;
	uint16_t timing[7];
}_def_step;

typedef struct{
	uint8_t wells;
	uint16_t wait1;
	uint16_t wait2;
	uint16_t shake;
	uint16_t wait4;
	uint16_t wait5;
	uint16_t reserve[2];
}_def_shake_step;

typedef struct{
	uint8_t wells;
	uint16_t wait1;
	uint16_t fill;
	uint16_t wait2;
	uint16_t shake;
	uint16_t wait4;
	uint16_t drain;
	uint16_t wait5;
}_def_washing_step;

typedef struct{
	uint8_t wells;
	uint16_t heater_on_off;
	uint16_t wait1;
	uint16_t reserve[5];
}_def_drying_step;
typedef struct{
	uint32_t Z_bottom_pos;
	uint32_t Well_position[NUM_MAX_WELL];
	_def_step Program_para[MAX_PROGRAM_NUM][MAX_STEP_NUM];
	uint16_t factory_code;

}_flash_data;

typedef struct{
	_flash_data flash_data;
	uint16_t pg_stepnumber[MAX_PROGRAM_NUM];
	uint16_t pg_total_time[MAX_PROGRAM_NUM];
}_system_data;

typedef enum{
	STEP_TYPE_NONE =0,
	STEP_TYPE_SHAKE,
	STEP_TYPE_WASHING,
	STEP_TYPE_DRYING
}_step_type;

extern _system_data system_data;

void dt_system_data_init(void);
void dt_Modify_step(uint8_t pg,uint8_t stepindex ,_def_step step);
int dt_get_well_num(uint8_t pg,uint8_t stepindex);
int dt_set_well_num(uint8_t pg,uint8_t stepindex,int wellnum);
void dt_calculator_pg_stepnumber(void);
_step_type dt_calculator_step_type(uint8_t wells);
HAL_StatusTypeDef dt_store_system_data(void);

#endif /* INC_DATA_H_ */
