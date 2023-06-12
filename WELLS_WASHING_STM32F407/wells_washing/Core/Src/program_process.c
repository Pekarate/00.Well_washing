/*
 * steps_process.c
 *
 *  Created on: Mar 27, 2023
 *      Author: ADMIN
 */
#include <program_process.h>
#include "main.h"
#include "data.h"
#include "motor.h"
#include "dw_display.h"
#include "define.h"

#ifdef ENABLE_LOG_PROGRAM_PROCESS
	#define LOG_TAG "PRO "
#endif




uint8_t running_pg = 0;
uint8_t running_step= 0;
//static _def_step step_para;





_program_state pgstate = PG_STATE_IDLE;


int pg_start(uint8_t pg,uint8_t stepindex)
{
	if(pgstate == PG_STATE_IDLE)
	{
		LOGI(LOG_TAG,"start step: %d of program : %d",stepindex,pg);
		running_pg = pg;
		running_step = stepindex;
		pgstate = PG_STATE_START;
		return 1;
	} else {
		LOGE(LOG_TAG,"pg_start fail running_pg  %d, pgstate: %d",running_pg,pgstate);
	}
	return 0;
}
int pg_stop(void)
{
	pgstate = PG_STATE_STOP;
	return 1;
}
_step_type start_step(){
//	Dwin_switch_running_page(running_pg,running_step);
	switch (system_data.flash_data.Program_para[running_pg][running_step].type) {
		case STEP_TYPE_NONE:
			LOGW(LOG_TAG,"step :%d  isn't active",running_step);
			break;
		case STEP_TYPE_SHAKE:
			{
				_step_shake_state tmp = step_shake_start();
				if( tmp== SHAKE_STATE_START) {
					LOGI(LOG_TAG,"step_shake_start done index: %d",running_step);
				}else {
					LOGE(LOG_TAG,"step_shake_start fail index: %d _step_shake_state: %d",running_step,tmp);
				}
			}
			break;
		case STEP_TYPE_WASHING:
		{
			_step_ws_state ws =step_washing_start();
			if(ws == WS_STATE_START) {
				LOGI(LOG_TAG,"step_washing_start done index: %d",running_step);
			}else {
				LOGE(LOG_TAG,"step_washing_start fail index: %d _step_ws_state: %d",running_step,ws);
			}
		}
			break;
		case STEP_TYPE_DRYING:
			{
				_step_dy_state dy = step_drying_start();
				if(dy == DY_STATE_START) {
					LOGI(LOG_TAG,"step_drying_start done index: %d",running_step);
				}else {
					LOGE(LOG_TAG,"step_drying_start fail index: %d _step_dy_state:%d",running_step,dy);
				}
			}
			break;
		default:
			LOGE(LOG_TAG,"Unknown step type");
			// Code to handle unknown step type
			break;
	}
	return system_data.flash_data.Program_para[running_pg][running_step].type;
}

void pg_process_loop(void) {
	switch (pgstate) {
		case PG_STATE_IDLE:
//			printf("Program is idle\n");
			// Code to handle the idle state
			break;

		case PG_STATE_START:
			if(running_step == MAX_STEP_NUM){
				pgstate= PG_STATE_START_HOMEX;
				break;
			}
			Dwin_switch_running_page(running_pg,running_step);
			if(isMotor_atHome(&z_motor))
			{
				if(start_step()!= STEP_TYPE_NONE) {

						pgstate= PG_STATE_RUNNING;
				}
				else {
					running_step++;
				}
			}else {
				LOGW(LOG_TAG,"start program without Z at home");
				mt_move_to_home(&z_motor);
				pgstate= PG_STATE_WAIT_HOMEZ;
			}
			// Code to handle the start state
			break;
		case PG_STATE_WAIT_HOMEZ:
				if(isMotor_atHome(&z_motor)){
					pgstate= PG_STATE_START;
				}
				break;

		case PG_STATE_RUNNING:
			switch (system_data.flash_data.Program_para[running_pg][running_step].type) {
				case STEP_TYPE_SHAKE:
					if(step_shake_process())
					{
						LOGI(LOG_TAG,"Shaking step (%d) finish",running_step);
						running_step++;
						pgstate = PG_STATE_START;
					}
					break;
				case STEP_TYPE_WASHING:
					if(step_washing_process())
					{
						LOGI(LOG_TAG,"washing step (%d) finish",running_step);
						running_step++;
						pgstate = PG_STATE_START;
					}
					break;
				case STEP_TYPE_DRYING:
					if(step_drying_process())
					{
						LOGI(LOG_TAG,"drying step (%d) finish",running_step);
//						running_step++;
						pgstate = PG_STATE_START_HOMEX;
					}
					break;
				default:
					LOGI(LOG_TAG,"Unknown step type");
					break;
			}
			break;
		case PG_STATE_START_HOMEX:
			LOGW(LOG_TAG,"all steps done, return x to home");
			mt_move_to_home(&x_motor);
			pgstate= PG_STATE_WAIT_HOMEX;
			break;
		case PG_STATE_WAIT_HOMEX:
			if(isMotor_atHome(&x_motor)){
				LOGI(LOG_TAG,"return x to home done");
				pgstate= PG_STATE_END;
			}
			break;
		case PG_STATE_STOP:
			x_mt_stop();
			z_mt_stop();
			step_shake_stop();
			step_washing_stop();
			step_drying_stop();
			LOGW(LOG_TAG,"Program is stopping\n");
			pgstate = PG_STATE_END;
			break;
		case PG_STATE_END:
//			printf("Program has ended\n");
			Dwin_switch_page(PAGE_RUNNING_END_PG);
			LOGI(LOG_TAG,"Program %d has ended",running_pg);
			pgstate = PG_STATE_IDLE;
			// Code to handle the end state
			break;
		default:
//			printf("Unknown program state\n");
			// Code to handle unknown state
			break;
	}
}
