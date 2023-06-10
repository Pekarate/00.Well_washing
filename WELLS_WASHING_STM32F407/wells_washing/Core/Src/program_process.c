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
#define DEBUG 1
#define LOG_TAG "PRO"
#define LOGI(fmt, ...) printf("[%s] " fmt "\r\n", LOG_TAG, ##__VA_ARGS__)
#define LOG3(fmt, ...) while(0);//printf("[%s] " fmt "\n", LOG_TAG, ##__VA_ARGS__)




uint8_t running_pg = 0;
uint8_t running_step= 0;
//static _def_step step_para;





_program_state pgstate = PG_STATE_IDLE;


int pg_start(uint8_t pg,uint8_t stepindex)
{
	if(pgstate == PG_STATE_IDLE)
	{
		LOGI("start step: %d of program : %d",stepindex,pg);
		running_pg = pg;
		running_step = stepindex;
		pgstate = PG_STATE_START;
		return 1;
	}
	return 0;
}
int pg_stop(void)
{
	pgstate = PG_STATE_STOP;
	return 1;
}
_step_type start_step(){
	Dwin_switch_running_page(running_pg,running_step);
	switch (system_data.flash_data.Program_para[running_pg][running_step].type) {
		case STEP_TYPE_NONE:
			LOGI("step :%d  isn't active",running_step);
			break;
		case STEP_TYPE_SHAKE:
			LOGI("Shaking step (%d)  is currently active",running_step);
			step_shake_start();
			break;
		case STEP_TYPE_WASHING:
			LOGI("Washing step (%d)  is currently active",running_step);
			step_washing_start();
			break;
		case STEP_TYPE_DRYING:
			LOGI("Drying step (%d) is currently active",running_step);
			step_drying_start();
			break;
		default:
			LOGI("Unknown step type");
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
				pgstate= PG_STATE_END;
				break;
			}
			if(start_step()!= STEP_TYPE_NONE) {
				pgstate= PG_STATE_RUNNING;
			}
			else {
				running_step++;
			}
			// Code to handle the start state
			break;
		case PG_STATE_RUNNING:
			switch (system_data.flash_data.Program_para[running_pg][running_step].type) {
				case STEP_TYPE_SHAKE:
					if(step_shake_process())
					{
						LOGI("Shaking step (%d) finish",running_step);
						running_step++;
						pgstate = PG_STATE_START;
					}
					break;
				case STEP_TYPE_WASHING:
					if(step_washing_process())
					{
						LOGI("washing step (%d) finish",running_step);
						running_step++;
						pgstate = PG_STATE_START;
					}
					break;
				case STEP_TYPE_DRYING:
					if(step_drying_process())
					{
						LOGI("drying step (%d) finish",running_step);
//						running_step++;
						pgstate = PG_STATE_END;
					}
					break;
				default:
					LOGI("Unknown step type");
					break;
			}
			break;
		case PG_STATE_STOP:
			x_mt_stop();
			z_mt_stop();
			LOGI("Program is stopping\n");
			pgstate = PG_STATE_END;
			break;
		case PG_STATE_END:
//			printf("Program has ended\n");
			Dwin_switch_page(PAGE_RUNNING_END_PG);
			LOGI("Program %d has ended",running_pg);
			pgstate = PG_STATE_IDLE;
			// Code to handle the end state
			break;
		default:
//			printf("Unknown program state\n");
			// Code to handle unknown state
			break;
	}
}
