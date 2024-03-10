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
#include "define.h"

#ifdef ENABLE_LOG_DWIN_DISPLAY
	#define LOG_TAG "DWIN "
#else
	#undef LOGI
	#define LOGI(fmt, ...)
#endif

uint8_t	current_rx_index = 0;
uint8_t *dw_rx_buf[10] = {0};
uint8_t old_setup_page =0;

int Dwin_Write_VP_String(uint16_t Addr,char *data,uint16_t slen);
int dw_update_setup_page(uint8_t pg,uint8_t stepnumber);
void Update_color(uint8_t pg,uint8_t index_stepnumber,uint8_t well);
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


static void Dw_write(uint8_t *data,uint16_t len)
{
	static uint32_t timesend = 0;
	while(HAL_GetTick() <timesend)
	{
		__NOP();
	}
	uart_transmit_block(data, len, 1000);
	timesend = HAL_GetTick()+10;
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
	uint8_t DwinBuf[200] = {0};
	uint16_t slentmp = slen;
	if(slen %2)
	{
		slentmp++;
	}
	DwinBuf[0]= 0x5A;
	DwinBuf[1]= 0xA5;
	DwinBuf[2]= slentmp+2 +1 +2 ;  // 2byte for address and 1 code 0x82 2 byte 0
	DwinBuf[3]= 0x82;
	DwinBuf[4]= Addr>>8;
	DwinBuf[5]= Addr;
	for(int i=0;i<slen;i++)
	{
		DwinBuf[6+i] = data[i];
	}
	slentmp = slentmp +6+2;
    Dw_write(DwinBuf, slentmp);
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
	if(page_index != PAGE_INPUT_WELL_ERROR )
		old_setup_page = page_index;
	uint16_t Buf[2] = {0x5A01,00};
	Buf[1] = page_index;
	Dwin_Write_VP(0X0084,Buf,2);
}
void Dwin_switch_running_page(uint8_t pg,uint8_t stepnumber)
{
	dw_update_setup_page(pg,stepnumber);
	switch (dt_calculator_step_type(system_data.flash_data.Program_para[pg][stepnumber].wells)) {
		case STEP_TYPE_SHAKE:
			Dwin_switch_page(PAGE_RUNNING_STEP_SHAKE);
			LOGI(LOG_TAG,"SWITCH TO PAGE_RUNNING_STEP_SHAKE");
			break;
		case STEP_TYPE_WASHING:
			Dwin_switch_page(PAGE_RUNNING_STEP_WASHING);
			LOGI(LOG_TAG,"SWITCH TO PAGE_RUNNING_STEP_WASHING");
			break;
		case STEP_TYPE_DRYING:
			Dwin_switch_page(PAGE_RUNNING_STEP_DRYING);
			LOGI(LOG_TAG,"SWITCH TO PAGE_RUNNING_STEP_DRYING");
			break;
		default:
			break;
	}
	__NOP();
	__NOP();

}

void Dwin_init(void)
{
	Dwin_reset();
	HAL_Delay(1000);
	Dwin_switch_page(HMI_START_PAGE);
	memset(&s_log,0,sizeof(s_log));
}


void Dwin_reset(void)
{
	uint16_t Buf[2] = {0x55AA,0x5AA5};
	Dwin_Write_VP(0X0004,Buf,2);
}
int dw_get_well_number(uint8_t index_pg,uint8_t index_stepnumber)
{
	if((index_pg >= MAX_PROGRAM_NUM ) || (index_stepnumber >=MAX_STEP_NUM)){
				return 0;
	}
	return system_data.flash_data.Program_para[index_pg][index_stepnumber].wells;
}
int dw_update_setup_page(uint8_t pg,uint8_t index_stepnumber){
	if((pg >= MAX_PROGRAM_NUM ) || (index_stepnumber >=MAX_STEP_NUM)){
			return -1;
	}
	uint16_t data[13];

	data[0] = pg+1;
	data[1] = index_stepnumber+1;
	data[2] = dt_calculator_step_type(system_data.flash_data.Program_para[pg][index_stepnumber].wells); //not use
	data[3] = dw_get_well_number(pg,index_stepnumber);
	if((data[3] > NUM_MAX_WELL) )
		data[3] = 1;
	for(int i=4;i<11;i++)
	{
		data[i] = system_data.flash_data.Program_para[pg][index_stepnumber].timing[i-4];
	}
	data[11] = dw_get_well_number(pg,index_stepnumber+1);
	Dwin_Write_VP(VP_SETUP_PARA,data,12);
	return 1;
}

void show_user_page(){
	Dwin_switch_page(PAGE_SETUP_USER_CTL);
}
void show_setup_page(uint8_t pg,uint8_t index_stepnumber){
	if((pg >= MAX_PROGRAM_NUM ) || (index_stepnumber >=MAX_STEP_NUM)){
		return;
	}
	Update_color(pg,index_stepnumber,system_data.flash_data.Program_para[pg][index_stepnumber].wells);
	dw_update_setup_page(pg,index_stepnumber);
}
//                 2F  FF      2F FF   30  00
//                                  8      10  11  12  13 14  15  16
//{5A}{A5}{18}{83}{3A}{00}{0A}{00}{00}{00}{01}{00}{01}{00}{00}{00}{00}{00}{00}{00}{00}{00}{00}{00}{00}{00}{00}
void dwin_update_step(uint8_t *data){
	_def_step step;
	uint8_t pg = data[8]; // 0x3000
	uint8_t stepindex = data[10]; // 0x3001
	step.wells =data[14];
	for(int i=0;i<7;i++){
		step.timing[i] = (uint16_t)data[15+i*2]*256 + data[16+i*2];
	}
	dt_Modify_step(pg-1, stepindex-1, step);
}

void dwin_start_program(uint8_t pg){

	uint8_t index_pg = pg-1;
	if(index_pg < MAX_PROGRAM_NUM){
		LOGI(LOG_TAG,"Request start program index: %d",index_pg);
		pg_start(index_pg, 0);
	}
	else{
		LOGE(LOG_TAG,"Request start fail pg: %d",pg);
	}

//	s_log_add_1_line(tmp);
//	Dwin_Write_VP_String(0x3800,s_log.log, s_log.s_size);
}
void dwin_stop_program(void){


}
void Update_color(uint8_t pg,uint8_t index_stepnumber,uint8_t well)
{
	switch(dt_calculator_step_type(well)){
		case (STEP_TYPE_WASHING):
				Dwin_switch_page(PAGE_SETUP_STEP_WASHING);
				if(system_data.flash_data.Program_para[pg][index_stepnumber].timing[1]) {
					dwin_change_color_sp(0x7000,DWIN_COLOR_BLACK);
					dwin_change_color_sp(0x7010,DWIN_COLOR_RED);
				}
				else {
					dwin_change_color_sp(0x7000,DWIN_COLOR_RED);
					dwin_change_color_sp(0x7010,DWIN_COLOR_BLACK);
				}
				if(system_data.flash_data.Program_para[pg][index_stepnumber].timing[5]) {
					dwin_change_color_sp(0x7020,DWIN_COLOR_BLACK);
					dwin_change_color_sp(0x7030,DWIN_COLOR_RED);
				}
				else {
					dwin_change_color_sp(0x7020,DWIN_COLOR_RED);
					dwin_change_color_sp(0x7030,DWIN_COLOR_BLACK);
				}
				break;
		case (STEP_TYPE_DRYING):
				Dwin_switch_page(PAGE_SETUP_STEP_DRYING);
				if(system_data.flash_data.Program_para[pg][index_stepnumber].timing[0]) {
					dwin_change_color_sp(0x7000,DWIN_COLOR_BLACK);
					dwin_change_color_sp(0x7010,DWIN_COLOR_RED);
				}
				else {
					dwin_change_color_sp(0x7000,DWIN_COLOR_RED);
					dwin_change_color_sp(0x7010,DWIN_COLOR_BLACK);
				}
				break;
		default:
				Dwin_switch_page(PAGE_SETUP_STEP_SHAKE);
			break;
	}
}


void dw_return_previous_page(void)
{
	uint16_t well_tmp=1;
	switch (old_setup_page) {
		case PAGE_SETUP_STEP_DRYING:
			well_tmp = NUM_MAX_WELL;
			break;
		case PAGE_SETUP_STEP_WASHING:
			well_tmp = NUM_MAX_WELL -1;
			break;
		default:
			break;
	}
	Dwin_Write_VP(VP_WELLS_ADDR,&well_tmp,1);
	Dwin_switch_page(old_setup_page);
}
static uint8_t current_pg_setup;
static uint8_t current_step_setup;

void dwin_change_target_well(uint8_t well){
	if(well> NUM_MAX_WELL || well == 0 )
	{
//		well = NUM_MAX_WELL;
//		uint16_t wells = well;
//		Dwin_Write_VP(VP_WELLS_ADDR, &wells, 1);
		LOGE(LOG_TAG,"input target well error: %d",well);
		Dwin_switch_page(PAGE_INPUT_WELL_ERROR);
		return;
	}
	uint8_t target_page = PAGE_SETUP_STEP_SHAKE;
	switch (well) {
		case (NUM_MAX_WELL-1):
			target_page = PAGE_SETUP_STEP_WASHING;
			break;
		case NUM_MAX_WELL:
			target_page = PAGE_SETUP_STEP_DRYING;
			break;
		default:
			target_page = PAGE_SETUP_STEP_SHAKE;
			break;
	}
	Update_color(current_pg_setup-1,current_step_setup -1,well);
	Dwin_switch_page(target_page);
}

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

		case CompareLOGIN:
			uint8_t checkDT  = 0;
			if(checkData(data, size, "root", 4))
			{
				checkDT = 1;
			}
			if(checkData(data, size, "1234", 4) && checkDT == 1)
			{
				/// page continue is 3 //
				Dwin_switch_page(3);
				checkData = 0;
			}
			break;

		case BT_SETUP_CODE:
				current_pg_setup=value = data[8];
				current_step_setup =1;
				show_setup_page(value-1,0);
			break;
		case BT_SWICH_SETUP_PAGE:
				current_step_setup = value = data[8];
				show_setup_page(current_pg_setup-1,value-1);
			break;
		case BT_SETUP_TAR_GET_WELLS:
				value = (uint16_t)data[7]*256+data[8];
				dwin_change_target_well(value);
				break;
		case BT_SETUP_FILL_ONOFF:
		case BT_SETUP_HEATER_ONOFF:
				value = (uint16_t)data[7]*256+data[8];
				if(value) {
					dwin_change_color_sp(0x7000,DWIN_COLOR_BLACK);
					dwin_change_color_sp(0x7010,DWIN_COLOR_RED);
				}
				else {
					dwin_change_color_sp(0x7000,DWIN_COLOR_RED);
					dwin_change_color_sp(0x7010,DWIN_COLOR_BLACK);
				}
				break;
		case BT_SETUP_DRAIN_ONOFF:
				value = (uint16_t)data[7]*256+data[8];
				if(value) {
					dwin_change_color_sp(0x7020,DWIN_COLOR_BLACK);
					dwin_change_color_sp(0x7030,DWIN_COLOR_RED);
				}
				else {
					dwin_change_color_sp(0x7020,DWIN_COLOR_RED);
					dwin_change_color_sp(0x7030,DWIN_COLOR_BLACK);
				}
				break;
		case BT_SWICH_SETUP_EXIT:
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
		case BT_ERROR_WELL_BACK:
				dw_return_previous_page();
				break;

		default:
			break;
	}
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
		data[0] = x_motor.current_pos/65535;
		data[1] = x_motor.current_pos;
		data[2] = z_motor.current_pos/65535;
		data[3] = z_motor.current_pos;
		Dwin_Write_VP(VP_X_STEP_MOTOR,data,4);
		return 1;
	}
	return 0;
}

void dwin_log_change_color(uint16_t color){
	Dwin_Write_VP(LOG_COLOR_ADDRESS, &color, 1);
}

void dwin_change_color_sp(uint16_t sp,uint16_t color){
	Dwin_Write_VP(sp+3, &color, 1);
}


void dwin_log_change_len(uint16_t len){
	static uint16_t oldlen = 200;
	if(oldlen != len)
	{
		oldlen = len;
		Dwin_Write_VP(LOG_LEN_ADDRESS, &len, 1);
	}
}

void dwin_log_visiable(uint16_t visible){
	static uint16_t oldvisible = 1;
	if( oldvisible == visible)
		return;
	oldvisible = visible;
	if(visible){
		dwin_log_change_len(200);
	} else {
		dwin_log_change_len(0);
	}
}
#define MAX_UINT32 (0-1)
uint32_t log_timout= MAX_UINT32;
void dwin_log_text(_Log_type type,char *data, int len,uint32_t time){
#if SHOW_LOG_ENABLE
	static _Log_type oldtype = -1;
	if(oldtype != type) {
		oldtype = type;
		dwin_log_change_color(oldtype);
	}
	dwin_log_visiable(1);
	Dwin_Write_VP_String(VP_LOG_ADDRESS, data, len);
	log_timout = HAL_GetTick() + time;
	if(log_timout < HAL_GetTick())
		log_timout= MAX_UINT32;
#endif
}

void dwin_log_timeout(void){
	if(HAL_GetTick() > log_timout)
	{
		dwin_log_visiable(0);
		log_timout= MAX_UINT32;
	}
}


////////////// N code /////////////////
int checkData(const uint8_t* arr, size_t arrSize, const char* strfind, size_t strSize)
{

	for(uint8_t i = 0; i <= arrSize - 1; i++)
	{
		if(memcmp(&arr[i], strfind, strSize) == 0)
		{
			return 1;
		}
	}
		return 0;
}
