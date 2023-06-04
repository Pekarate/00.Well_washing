/*
 * dw_display.c
 *
 *  Created on: Mar 18, 2023
 *      Author: ADMIN
 */


#include <program_process.h>
#include "main.h"
#include "uart.h"
#include "string.h"
#include "data.h"
#include "dw_display.h"
#include "motor.h"
#include "stdio.h"
uint8_t	current_rx_index = 0;
uint8_t *dw_rx_buf[10] = {0};

int Dwin_Write_VP_String(uint16_t Addr,char *data,uint16_t slen);
int dw_update_setup_page(uint8_t pg,uint8_t stepnumber);
typedef struct{
	char log[512];
	uint8_t line_nums;
	uint16_t s_size;
	uint8_t is_change;
}_system_log;

_system_log s_log;

void s_log_add_1_line(char *line)
{
	s_log.is_change=1;
	if(s_log.line_nums  == MAX_LOG_LINE_NUMS) // remove first line
	{
		char *seconnd = strstr(s_log.log,"\r\n");
		if(seconnd)
		{
			seconnd += 2;
			s_log.s_size -= (seconnd - s_log.log);
			for(int i =0;i<s_log.s_size;i++)
			{
				s_log.log[i] = seconnd[i];
			}
		}
		s_log.line_nums--;
	}
	s_log.s_size += sprintf(s_log.log+s_log.s_size,"%s\r\n",line);
	s_log.line_nums ++;
}
void s_log_clear(void)
{
	s_log.is_change =1;
	memset(&s_log,0,sizeof(s_log));
}


static inline void Dw_write(uint8_t *data,uint16_t len)
{
	uart_transmit_block(data, len, 1000);
}
void dw_log_deamon(void)
{
	if(s_log.is_change){
		s_log.is_change=0;
		if(s_log.s_size > 480)
		{
			Dwin_Write_VP_String(VP_TEXT_LOG, s_log.log, 240);
			HAL_Delay(20);
			Dwin_Write_VP_String(VP_TEXT_LOG+120, s_log.log+240, 240);
			HAL_Delay(20);
			Dwin_Write_VP_String(VP_TEXT_LOG+240, s_log.log+480, s_log.s_size- 480);
		} else if(s_log.s_size > 240) {
			Dwin_Write_VP_String(VP_TEXT_LOG, s_log.log, 240);
			HAL_Delay(20);
			Dwin_Write_VP_String(VP_TEXT_LOG+120, s_log.log+240, s_log.s_size - 240);
		}
		else{
			Dwin_Write_VP_String(VP_TEXT_LOG, s_log.log, s_log.s_size);
		}


	}
}

int Dwin_Write_VP(uint16_t Addr,uint16_t *data,uint16_t len)
{
	uint8_t DwinBuf[100];
	DwinBuf[0]= 0x5A;
	DwinBuf[1]= 0xA5;
	DwinBuf[2]= (len+1)*2 +1;  //
	DwinBuf[3]= 0x82;
	DwinBuf[4]= Addr>>8;
	DwinBuf[5]= Addr;
	for(int i=0;i<len;i++)
	{
		DwinBuf[6+i*2] = data[i]/256;
		DwinBuf[6+i*2+1] = data[i];
	}
    len = (len+1)*2 +4;
    Dw_write(DwinBuf, len);
	return 1;
}

int Dwin_Write_VP_String(uint16_t Addr,char *data,uint16_t slen)
{
	if(slen %2)
	{
		data[slen] =0;
		slen++;
	}
	uint8_t DwinBuf[300];
	DwinBuf[0]= 0x5A;
	DwinBuf[1]= 0xA5;
	DwinBuf[2]= slen+2 +1;  // 2byte for address and 1 code 0x82
	DwinBuf[3]= 0x82;
	DwinBuf[4]= Addr>>8;
	DwinBuf[5]= Addr;
	for(int i=0;i<slen;i++)
	{
		DwinBuf[6+i] = data[i];
	}
    slen = slen +6;
    Dw_write(DwinBuf, slen);
	return 1;
}
int Dwin_read_VP(uint16_t Addr,uint16_t *data,uint16_t len)
{
	uint8_t DwinBuf[100] ={0x5A,0xA5};
	DwinBuf[2]= 0x04;  //
	DwinBuf[3]= 0x83;
	DwinBuf[4]= Addr>>8;
	DwinBuf[5]= Addr;
    len = (len+1)*2 +4;
    Dw_write(DwinBuf, len);
	return 1;
}
void Dwin_switch_page(int page_index)
{
	uint16_t Buf[2] = {0x5A01,00};
	Buf[1] = page_index;
	Dwin_Write_VP(0X0084,Buf,2);
}
void Dwin_switch_running_page(uint8_t pg,uint8_t stepnumber)
{
	switch (system_data.flash_data.Program_para[pg][stepnumber].type) {
		case STEP_TYPE_SHAKE:
			Dwin_switch_page(PAGE_RUNNING_STEP_SHAKE);
			break;
		case STEP_TYPE_WASHING:
			Dwin_switch_page(PAGE_RUNNING_STEP_WASHING);
			break;
		case STEP_TYPE_DRYING:
			Dwin_switch_page(PAGE_RUNNING_STEP_DRYING);
			break;
		default:
			break;
	}
	dw_update_setup_page(pg,stepnumber);
}

void Dwin_init(void)
{
	Dwin_reset();
	HAL_Delay(1000);
//	Dwin_switch_page(05);
	memset(&s_log,0,sizeof(s_log));
}


void Dwin_reset(void)
{
	uint16_t Buf[2] = {0x55AA,0x5AA5};
	Dwin_Write_VP(0X0004,Buf,2);
}

int dw_update_setup_page(uint8_t pg,uint8_t stepnumber){
	if((pg > NUM_MAX_WELL ) || (stepnumber >24)){
			return -1;
	}
	uint16_t data[12];

	data[0] = pg+1;
	data[1] = stepnumber+1;
	data[2] = system_data.flash_data.Program_para[pg][stepnumber].type;
	data[3] = system_data.flash_data.Program_para[pg][stepnumber].wells;
	if((data[3] == 0) || (data[3] >= MAX_WELLS_NUM) )
		data[3] = 1;
	for(int i=4;i<11;i++)
	{
		data[i] = system_data.flash_data.Program_para[pg][stepnumber].timing[i-4];
	}
	Dwin_Write_VP(VP_SETUP_PARA,data,11);
	return 1;
}

void show_user_page(){
	Dwin_switch_page(PAGE_SETUP_USER_CTL);
}
void show_setup_page(uint8_t pg,uint8_t stepnumber){
	if((pg > NUM_MAX_WELL ) || (stepnumber >24)){
		return;
	}
	switch(system_data.flash_data.Program_para[pg][stepnumber].type){
		case (STEP_TYPE_WASHING):
				Dwin_switch_page(PAGE_SETUP_STEP_WASHING);
				break;
		case (STEP_TYPE_DRYING):
				Dwin_switch_page(PAGE_SETUP_STEP_DRYING);
				break;
		default:
				Dwin_switch_page(PAGE_SETUP_STEP_SHAKE);
			break;
	}
	dw_update_setup_page(pg,stepnumber);
}
//                 2F  FF      2F FF   30  00
//                                  8      10  11  12  13 14  15  16
//{5A}{A5}{18}{83}{3A}{00}{0A}{00}{00}{00}{01}{00}{01}{00}{00}{00}{00}{00}{00}{00}{00}{00}{00}{00}{00}{00}{00}
void dwin_update_step(uint8_t *data){
	_def_step step;
	uint8_t pg = data[8]; // 0x3000
	uint8_t stepindex = data[10]; // 0x3001
	step.wells =data[14];
	if(step.wells> NUM_MAX_WELL)
			return;
	switch (step.wells) {
		case 1:
			step.type = STEP_TYPE_SHAKE;
			break;
		case NUM_MAX_WELL:
			step.type = STEP_TYPE_DRYING;
			break;
		default:
			step.type = STEP_TYPE_WASHING;
			break;
	}
	for(int i=0;i<7;i++){
		step.timing[i] = (uint16_t)data[15+i*2]*256 + data[16+i*2];
	}
	dt_Modify_step(pg-1, stepindex-1, step);
}

void dwin_start_program(uint8_t pg){

//	s_log.s_size = sprintf(s_log.log,"\r\nIF I DIE");
//	Dwin_switch_page(PAGE_MANUAL_CONTROL);
//	s_log_clear();
	HAL_Delay(1);
	char tmp[100];
	sprintf(tmp,"START PROGRAM %d",pg);
	pg_start(pg-1, 0);

//	s_log_add_1_line(tmp);
//	Dwin_Write_VP_String(0x3800,s_log.log, s_log.s_size);
}
void dwin_stop_program(void){


}
void dwin_change_target_well(uint8_t well){
	if(well> NUM_MAX_WELL)
		return;
	uint8_t target_page = PAGE_SETUP_STEP_SHAKE;
	switch (well) {
		case 1:
			break;
		case NUM_MAX_WELL:
			target_page = PAGE_SETUP_STEP_DRYING;
			break;
		default:
			target_page = PAGE_SETUP_STEP_WASHING;
			break;
	}
	Dwin_switch_page(target_page);
}
static uint8_t current_pg_setup;
static uint8_t current_step_setup;
int dw_process_rx_buffer(uint8_t *data,uint16_t size){ //USART_CR2_TOEN

	if(size < 5){
		return -1;
	}
	if(data[0] != 0x5A && data[1] != 0xA5 ){
		return -2;
	}
	uint16_t code = data[4];
	code = code*256+ data[5];
	uint16_t value;
	switch (code) {
		case BT_SETUP_CODE:
				current_pg_setup=value = data[8];
				current_step_setup =1;
				show_setup_page(value-1,0);
			break;
		case BT_SETUP_TAR_GET_WELLS:
				value = (uint16_t)data[7]*256+data[8];
				dwin_change_target_well(value);
				break;
		case BT_SWICH_SETUP_PAGE:
				current_step_setup = value = data[8];
				show_setup_page(current_pg_setup-1,value-1);
			break;
		case BT_SWICH_SETUP_EXIT:
				dw_update_step_numbers();
				current_pg_setup =0;
			break;
		case BT_SWICH_SETUP_APPLY:
				dwin_update_step(data);
				break;
		case BT_MOVE_X_FOWARD:
				value = (uint16_t)data[7]*256+data[8];
				step_mt_move_foward(&x_motor, value);
				break;
		case BT_MOVE_X_BACKWARD:
				value = (uint16_t)data[7]*256+data[8];
				step_mt_move_backward(&x_motor, value);
				break;
		case BT_MOVE_Z_FOWARD:
				value = (uint16_t)data[7]*256+data[8];
				step_mt_move_foward(&z_motor, value);
				break;
		case BT_MOVE_Z_BACKWARD:
				value = (uint16_t)data[7]*256+data[8];
				step_mt_move_backward(&z_motor, value);
				break;
		case BT_X_HOME:
				mt_move_to_home(&x_motor);
				break;
		case BT_Z_HOME:
				mt_move_to_home(&z_motor);
				break;
		case BT_START_PG:
				dwin_start_program(data[8]);
				break;
		case BT_STOP_PG:
				pg_stop();
				break;
		case BT_START_PROCEED:
				value = (uint16_t)data[7]*256+data[8];
				if(value == 1)
				{
					show_user_page();
				}
				break;

		default:
			break;
	}
	return 1;
}

int dw_update_step_numbers(void){
	dt_calculator_pg_stepnumber();
	Dwin_Write_VP(VP_STEP_NUMBERS,system_data.pg_stepnumber,10);
	return 1;
}
float xxxx;
int dw_update_steper_positon(void){
	static uint32_t time_tmp =0;
	if( (HAL_GetTick() > time_tmp)&&((x_motor.old_pos != x_motor.current_pos) || (z_motor.old_pos != z_motor.current_pos)))
	{
		time_tmp = HAL_GetTick() +50;
		x_motor.old_pos = x_motor.current_pos;
		z_motor.old_pos = z_motor.current_pos;
		uint16_t data[4];
		float tmp = (float)x_motor.current_pos/PULSES_PER_MM;
		uint16_t *p = (uint16_t *)&tmp;
		data[0] = p[1];
		data[1] = p[0];
		tmp = (float)z_motor.current_pos/PULSES_PER_MM;
		xxxx= tmp;
		data[2] = p[1];
		data[3] = p[0];
		Dwin_Write_VP(VP_X_STEP_MOTOR,data,4);
		return 1;
	}
	return 0;
}

