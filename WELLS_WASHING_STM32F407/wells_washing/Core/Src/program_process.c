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

_def_shake_step *shake_step;
_def_drying_step *drying_step;
_def_washing_step *washing_step;
static uint8_t running_pg = 0;
static uint8_t running_step= 0;
//static _def_step step_para;
static uint32_t t_time = 0;
_step_shake_state shake_state = SHAKE_STATE_IDE;
int step_shake_start(void)
{
	if(shake_state == SHAKE_STATE_IDE)
	{
		LOGI("start step: %d of program : %d",running_step,running_pg);
		shake_step = (_def_shake_step *)&system_data.flash_data.Program_para[running_pg][running_step];
		shake_state = SHAKE_STATE_START;
		return 1;
	}
	return 0;
}
//return 1 mean step done
void show_infor_shake_step(_def_shake_step shake_step)
{
	LOGI("---------------INFOR_SHAKE_STEP------------");
	LOGI("1:Move to Wells: %d",shake_step.wells);
	LOGI("2:wait1 : %ds",shake_step.wait1);
	LOGI("3:move z to bottom");
	LOGI("4:wait2 : %ds",shake_step.wait2);
	LOGI("5:shake on : %ds",shake_step.shake);
	LOGI("6:wait4 : %ds",shake_step.wait4);
	LOGI("7:move z to top");
	LOGI("8:wait5 : %ds",shake_step.wait5);
	LOGI("------------------------------------------");
}
int step_shake_process(void){
	static _step_shake_state old_state = SHAKE_STATE_IDE;

	switch (shake_state) {
	            case SHAKE_STATE_IDE:
//	                // handle SHAKE_STATE_IDE
//	                old_state = shake_state;
//	                shake_state = SHAKE_STATE_START;
	                break;
	            case SHAKE_STATE_START:
	                // handle SHAKE_STATE_START -> move x to well
	            	show_infor_shake_step(*shake_step);

	            	LOGI("move x to %lu",system_data.flash_data.Well_position[shake_step->wells-1]);
	            	mt_set_target_position(&x_motor, system_data.flash_data.Well_position[shake_step->wells-1]);
	                old_state = shake_state;
	                shake_state = SHAKE_STATE_MOVE_WELLS;
	                break;
	            case SHAKE_STATE_MOVE_WELLS:
	                // handle SHAKE_STATE_MOVE_WELLS
	            	if(Mt_get_current_prosition(x_motor) == system_data.flash_data.Well_position[shake_step->wells-1])
	            	{
	            		LOGI("move x done, wait %ds",shake_step->wait1);
						old_state = shake_state;
						shake_state = SHAKE_STATE_WAIT;
						t_time = HAL_GetTick() +  (uint32_t)shake_step->wait1 * 1000;
	            	}
	                break;
	            case SHAKE_STATE_WAIT:
	                // handle SHAKE_STATE_WAIT
	            	if(HAL_GetTick() > t_time)
	            	{
	            		switch (old_state) {
	            		    case SHAKE_STATE_MOVE_WELLS:
	            		    	LOGI("move z to bottom %lu",system_data.flash_data.Z_bottom_pos);
	            		    	mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos); //@TODO
	            		    	shake_state = SHAKE_STATE_Z_BOTTOM;
	            		        break;
	            		    case SHAKE_STATE_Z_BOTTOM:
	            		    	mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos-1000);
	            		    	t_time = HAL_GetTick() +  (uint32_t)shake_step->shake * 1000;
	            		    	LOGI("start shake in %ds",shake_step->shake);
	            		    	shake_state = SHAKE_STATE_SHAKE;
	            		        break;
	            		    case SHAKE_STATE_SHAKE:
	            		    	LOGI("move z to top");
	            		    	mt_set_target_position(&z_motor,0);
	            		    	shake_state = SHAKE_STATE_Z_TOP;
	            		        break;
	            		    case SHAKE_STATE_Z_TOP:
	            		    	shake_state = SHAKE_STATE_Z_FINISH;
	            		        break;
	            		    case SHAKE_STATE_Z_FINISH:
	            		        // handle SHAKE_STATE_Z_FINISH
	            		        break;
	            		    default:
	            		        // handle unknown state
	            		        break;
	            		}
	            	}
	                break;
	            case SHAKE_STATE_Z_BOTTOM:
	                // handle SHAKE_STATE_Z_BOTTOM
	            	if(Mt_get_current_prosition(z_motor) == system_data.flash_data.Z_bottom_pos)
					{
						old_state = shake_state;
						shake_state = SHAKE_STATE_WAIT;
						LOGI("wait2 %d",shake_step->wait2);
						t_time = HAL_GetTick() +  (uint32_t)shake_step->wait2 * 1000;
					}
//					break;
//	                old_state = shake_state;
//	                shake_state = SHAKE_STATE_SHAKE;
	                break;
	            case SHAKE_STATE_SHAKE:
	                if(Mt_get_current_prosition(z_motor) == system_data.flash_data.Z_bottom_pos - 1000)
	            	{
	                	mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos);
	            	}
	                if(Mt_get_current_prosition(z_motor) == system_data.flash_data.Z_bottom_pos)
					{
	                	if(HAL_GetTick() > t_time)
	                	{
	                		LOGI("shake done, wait4 : %d",shake_step->wait4);
	    	                old_state = shake_state;
	    	                t_time = HAL_GetTick() +  (uint32_t)shake_step->wait4 * 1000;
	    	                shake_state = SHAKE_STATE_WAIT;
	                	}
	                	else
	                	{
	                		mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos-1000);
	                	}
					}
	                break;
	            case SHAKE_STATE_Z_TOP:
	            	if(Mt_get_current_prosition(z_motor) == 0)
					{
	            		LOGI("all done wait5: %d",shake_step->wait5);
	            		old_state = shake_state;
						t_time = HAL_GetTick() +  (uint32_t)shake_step->wait5 * 1000;
						shake_state = SHAKE_STATE_WAIT;
					}
	                break;
	            case SHAKE_STATE_Z_FINISH:
	                // handle SHAKE_STATE_Z_FINISH
	            	LOGI("step finish");
	                old_state = shake_state;
	                shake_state = SHAKE_STATE_IDE;
	                return 1;
	                break;
	            default:
	                // handle unknown state
//	                old_state = shake_state;
//	                shake_state = SHAKE_STATE_IDE;
	                break;
	        }
	return 0;
}

_step_ws_state ws_state = WS_STATE_IDE;
_step_ws_state old_ws_state = WS_STATE_IDE;
void start_fill_washing_solution(){
	LOGI("start fill washing solution");
	HAL_GPIO_WritePin(PUMP1_GPIO_Port, PUMP1_Pin, PUMP1_ON_LEVEL);
}
int is_washing_solution_full()
{
	if(HAL_GPIO_ReadPin(WS_SOLUTION_FULL_GPIO_Port, WS_SOLUTION_FULL_Pin) == WS_SOLUTION_FULL_LEVEL)
		return 1;
	return 0;
}
void stop_fill_washing_solution(){
	LOGI("stop fill washing solution");
	HAL_GPIO_WritePin(PUMP1_GPIO_Port, PUMP1_Pin, PUMP1_OFF_LEVEL);
}


void start_drain_washing_solution(){
	LOGI("start drain fill solution ");
}
int is_washing_solution_empty()
{
	if(HAL_GPIO_ReadPin(WS_SOLUTION_EMPTY_GPIO_Port, WS_SOLUTION_EMPTY_Pin) == WS_SOLUTION_EMPTY_LEVEL)
			return 1;
	return 0;
}
void stop_drain_washing_solution(){
	LOGI("stop drain washing solution ");
}
int step_washing_start()
{
	if(ws_state == WS_STATE_IDE)
	{
		LOGI("--------------- washing step: %d of program : %d----------------",running_step,running_pg);
		washing_step = (_def_washing_step *)&system_data.flash_data.Program_para[running_pg][running_step];
		shake_state = WS_STATE_START;
		return 1;
	}
	return 0;
}

void show_infor_washing_step(_def_washing_step ws_step)
{
	LOGI("---------------INFOR_WASHING_STEP------------");
	LOGI("1:Move to Wells: %d",ws_step.wells);
	LOGI("2:wait1 : %ds",ws_step.wait1);
	LOGI("3:fill washing solution: %d \n4:move z to bottom",ws_step.fill);
	LOGI("5:wait2 : %ds",ws_step.wait2);
	LOGI("6:shake on : %ds",ws_step.shake);
	LOGI("7:wait4 : %ds",ws_step.wait4);
	LOGI("8:move z to top %d\n9:Drain washing solution",ws_step.drain);
	LOGI("10:wait5 : %ds",ws_step.wait5);
	LOGI("-------------------------------------------");
}

int step_washing_process(void)
{
	    switch(ws_state) {
	        case WS_STATE_IDE:
	            // Do something when in state WS_STATE_IDE
	            ws_state = WS_STATE_START; // Example transition to next state
	            break;
	        case WS_STATE_START:
	        	show_infor_washing_step(*washing_step);
	            LOGI("move x to %lu",system_data.flash_data.Well_position[washing_step->wells-1]);
            	mt_set_target_position(&x_motor, system_data.flash_data.Well_position[washing_step->wells-1]); // m
	            ws_state = WS_STATE_MOVE_WELLS; // Example transition to next state
	            break;
	        case WS_STATE_MOVE_WELLS:
	        	if(Mt_get_current_prosition(x_motor) == system_data.flash_data.Well_position[washing_step->wells-1])
				{
					LOGI("move x done, wait1 %ds",washing_step->wait1);
					old_ws_state = ws_state;
					ws_state = WS_STATE_WAIT;
					t_time = HAL_GetTick() +  (uint32_t)washing_step->wait1 * 1000;
				}
	            break;
	        case WS_STATE_FILL_WS_SOLUTION:
	            if(is_washing_solution_full())
	            {
					LOGI("Fill done,move z to bottom %lu",system_data.flash_data.Z_bottom_pos);
					mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos); //@TODO
					ws_state = WS_STATE_Z_BOTTOM;
	            	stop_fill_washing_solution();
	            }
	            break;
	        case WS_STATE_Z_BOTTOM:
            	if(Mt_get_current_prosition(z_motor) == system_data.flash_data.Z_bottom_pos)
				{
            		LOGI("move z done, wait2 %ds",washing_step->wait2);
					old_ws_state = ws_state;
	            	t_time = HAL_GetTick() +  (uint32_t)washing_step->wait2 * 1000;
		            ws_state = WS_STATE_WAIT; // Example transition to next state

				}
	            break;
	        case WS_STATE_SHAKE:
                if(Mt_get_current_prosition(z_motor) == system_data.flash_data.Z_bottom_pos - 1000)
            	{
                	mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos);
            	}
                if(Mt_get_current_prosition(z_motor) == system_data.flash_data.Z_bottom_pos)
				{
                	if(HAL_GetTick() > t_time)
                	{
                		LOGI("shake done, wait4 : %d",washing_step->wait4);
                		old_ws_state = ws_state;
    	                t_time = HAL_GetTick() +  (uint32_t)washing_step->wait4 * 1000;
    	                ws_state = WS_STATE_WAIT;
                	}
                	else
                	{
                		mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos-1000);
                	}
				}
	            break;
	        case WS_STATE_Z_TOP:
            	if(Mt_get_current_prosition(z_motor) == 0){
            		LOGI("ztop done, start drain ws solution");
            		old_ws_state = ws_state;

//					t_time = HAL_GetTick() +  (uint32_t)step_para.timing[4] * 1000;
            		if(washing_step->drain)
            		{
						start_drain_washing_solution();
						ws_state = WS_STATE_DRAIN_WS_SOLUTION;
            		}
            		else
            		{
            			LOGI("skip drain fill solution ");
            			old_ws_state = ws_state;
						t_time = HAL_GetTick() +  (uint32_t)washing_step->wait5 * 1000;
						ws_state = WS_STATE_WAIT; // Example transition to next state
            		}
				}
	            break;
	        case WS_STATE_DRAIN_WS_SOLUTION:
	            if(is_washing_solution_empty())
	            {
	            	LOGI("solution empty,wait5 :%ds",washing_step->wait5);
	            	stop_drain_washing_solution();
	            	old_ws_state = ws_state;
	            	t_time = HAL_GetTick() +  (uint32_t)washing_step->wait5 * 1000;
	            	ws_state = WS_STATE_WAIT; // Example transition to next state
	            }
	            break;
	        case WS_STATE_WAIT:
	            if(HAL_GetTick() > t_time)
	            {
	            	switch (old_ws_state) {
						case WS_STATE_MOVE_WELLS:
							LOGI("wait1 done");
							if(washing_step->fill)
							{
								ws_state = WS_STATE_FILL_WS_SOLUTION;
								start_fill_washing_solution();
							}
							else
							{
								LOGI("skip fill washing solution");
								LOGI("move z to bottom %lu",system_data.flash_data.Z_bottom_pos);
								mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos); //@TODO
								ws_state= WS_STATE_Z_BOTTOM;
							}
							break;
						case WS_STATE_Z_BOTTOM:
							LOGI("wait2 done");
							mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos-1000);
							LOGI("start shake in %ds",washing_step->shake);
							t_time = HAL_GetTick() +  (uint32_t)washing_step->shake * 1000;

							ws_state = WS_STATE_SHAKE;
							break;
						case WS_STATE_SHAKE:
							LOGI("wait4 done, move z to top");
							mt_set_target_position(&z_motor,0);
							ws_state = WS_STATE_Z_TOP;
							break;
						case WS_STATE_Z_TOP:
						case WS_STATE_DRAIN_WS_SOLUTION:
							LOGI("wait5 done");
							ws_state = WS_STATE_Z_FINISH;
							break;
						default:
							break;
					}
	            }
	            break;
	        case WS_STATE_Z_FINISH:
	        	LOGI("-------------------------------washing step done----------------------");
	            ws_state = WS_STATE_IDE;
	        	return 1;
	            break;
	        default:
	            // Handle an invalid state
	            break;
	    }
	    return 0;
}

_step_dy_state dy_state = DY_STATE_IDE;
_step_dy_state old_dy_state = DY_STATE_IDE;

int step_drying_start()
{
	if(ws_state == WS_STATE_IDE)
	{
		LOGI("--------------- drying step: %d of program : %d----------------",running_step,running_pg);
		drying_step = (_def_drying_step *)&system_data.flash_data.Program_para[running_pg][running_step];
		dy_state = DY_STATE_START;
		return 1;
	}
	return 0;
}
void heater_on()
{
	LOG3("%u heater_on",HAL_GetTick());
	HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, HEATER_ON_LEVEL);
}
void heater_off()
{
	LOG3("%u heater_off",HAL_GetTick());
	HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, HEATER_OFF_LEVEL);
}
void show_infor_drying_step(_def_drying_step dr_step)
{
	LOGI("---------------INFOR_DRYING_STEP------------");
	LOGI("1:Move to Wells: %d",dr_step.wells);
	LOGI("2:hearter on : %d",dr_step.heater_on);
	LOGI("3:move z to bottom");
	LOGI("4:wait1 : %ds",dr_step.wait1);
	LOGI("5:hearter off : %d",dr_step.heater_off);
	LOGI("------------------------------------------");
}
int step_drying_process(void)
{
	switch (dy_state) {
	    case DY_STATE_IDE:
	        break;
	    case DY_STATE_START:
        	show_infor_drying_step(*drying_step);
            LOGI("move x to %lu",system_data.flash_data.Well_position[drying_step->wells-1]);
        	mt_set_target_position(&x_motor, system_data.flash_data.Well_position[drying_step->wells-1]); // m
        	old_dy_state = dy_state;
        	dy_state = DY_STATE_MOVE_WELLS; // Example transition to next state
            break;
	    case DY_STATE_MOVE_WELLS:
        	if(Mt_get_current_prosition(x_motor) == system_data.flash_data.Well_position[drying_step->wells-1])
			{
				LOGI("move x done, move z to bottom");
				old_dy_state = dy_state;
				mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos); //@TODO
				dy_state = DY_STATE_Z_BOTTOM;
			}
	        break;
	    case DY_STATE_Z_BOTTOM:
	    	if(Mt_get_current_prosition(z_motor) == system_data.flash_data.Z_bottom_pos)
			{
				LOGI("move z done");
				old_dy_state = dy_state;

				if(drying_step->heater_on)
				{
					dy_state = DY_HEATER_ON; // Example transition to next state
				}
				else
				{
					LOGI("skip heater on, wait : %ds",drying_step->wait1);
			    	t_time = HAL_GetTick() +  (uint32_t)drying_step->wait1 * 1000;
			    	dy_state = DY_STATE_WAIT; // Example transition to next state
				}
			}
			break;
	    case DY_HEATER_ON:
	    	heater_on();
	    	LOGI("heater on, wait : %ds",drying_step->wait1);
	    	old_dy_state = dy_state;
	    	t_time = HAL_GetTick() +  (uint32_t)drying_step->wait1 * 1000;
	    	dy_state = DY_STATE_WAIT; // Example transition to next state
	        break;
	    case DY_HEATER_OFF:

	    	LOGI("heater off");
	    	heater_off();
	    	mt_set_target_position(&z_motor,0);
	    	dy_state = DY_STATE_Z_TOP;
	        break;
	    case DY_STATE_Z_TOP:
        	if(Mt_get_current_prosition(z_motor) == 0){
        		LOGI("ztop done");
        		dy_state = DY_STATE_Z_FINISH;
			}
			break;
	    case DY_STATE_WAIT:
	        // code to execute when the state is DY_STATE_WAIT
	    	if(HAL_GetTick() > t_time)
			{
				switch (old_dy_state) {
					case DY_HEATER_ON:
						if(drying_step->heater_off) {
							dy_state = DY_HEATER_OFF; // Example transition to next state
						} else {
							LOGI("skip heater off");
							mt_set_target_position(&z_motor,0);
							dy_state = DY_STATE_Z_TOP;
							break;
						}
						break;
				    default:
				        break;
				}
			}
	        break;
	    case DY_STATE_Z_FINISH:
	    	return 1;
	        old_dy_state = DY_STATE_Z_FINISH;
	        break;
	    default:
	        break;
	}
	return 0;
}

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
