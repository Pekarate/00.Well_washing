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

#define DEBUG 1
#define LOG_TAG "MY_APP"
#define LOGI(fmt, ...) printf("[%s] " fmt "\n", LOG_TAG, ##__VA_ARGS__)

static uint8_t running_pg = 0;
static uint8_t running_step= 0;
static _def_step step_para;
static uint32_t t_time = 0;
_step_shake_state shake_state = SHAKE_STATE_IDE;
int step_shake_start(void)
{
	if(shake_state == SHAKE_STATE_IDE)
	{
		LOGI("start step: %d of program : %d",running_step,running_pg);
		step_para = system_data.flash_data.Program_para[running_pg][running_step];
		shake_state = SHAKE_STATE_START;
		return 1;
	}
	return 0;
}
//return 1 mean step done
void show_infor_shake_step()
{
	LOGI("1:Move to Wells: %d",step_para.wells);
	LOGI("2:wait1 : %ds",step_para.timing[0]);
	LOGI("3:move z to bottom");
	LOGI("4:wait2 : %ds",step_para.timing[1]);
	LOGI("5:shake on : %ds",step_para.timing[2]);
	LOGI("6:wait4 : %ds",step_para.timing[3]);
	LOGI("7:move z to top");
	LOGI("8:wait5 : %ds",step_para.timing[4]);
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
	            	show_infor_shake_step();
	            	LOGI("move x to %lu",system_data.flash_data.Well_position[step_para.wells-1]);
	            	mt_set_target_position(&x_motor, system_data.flash_data.Well_position[step_para.wells-1]);
	                old_state = shake_state;
	                shake_state = SHAKE_STATE_MOVE_WELLS;
	                break;
	            case SHAKE_STATE_MOVE_WELLS:
	                // handle SHAKE_STATE_MOVE_WELLS
	            	if(Mt_get_current_prosition(x_motor) == system_data.flash_data.Well_position[step_para.wells-1])
	            	{
	            		LOGI("move x done, wait %ds",step_para.timing[0]);
						old_state = shake_state;
						shake_state = SHAKE_STATE_WAIT;
						t_time = HAL_GetTick() +  (uint32_t)step_para.timing[0] * 1000;
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
	            		    	t_time = HAL_GetTick() +  (uint32_t)step_para.timing[2] * 1000;
	            		    	LOGI("start shake in %ds",step_para.timing[2]);
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
						LOGI("wait2 %d",step_para.timing[1]);
						t_time = HAL_GetTick() +  (uint32_t)step_para.timing[1] * 1000;
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
	                		LOGI("shake done, wait4 : %d",step_para.timing[3]);
	    	                old_state = shake_state;
	    	                t_time = HAL_GetTick() +  (uint32_t)step_para.timing[3] * 1000;
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
	            		LOGI("all done wait5: %d",step_para.timing[4]);
	            		old_state = shake_state;
						t_time = HAL_GetTick() +  (uint32_t)step_para.timing[4] * 1000;
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
	LOGI("start fill washing solution ");
}
int is_washing_solution_full()
{
	return 1;
}
void stop_fill_washing_solution(){
	LOGI("stop fill washing solution ");
}


void start_drain_washing_solution(){
	LOGI("start drain fill solution ");
}
int is_washing_solution_empty()
{
	return 1;
}
void stop_drain_washing_solution(){
	LOGI("stop drain washing solution ");
}
int step_washing_start()
{
	if(ws_state == WS_STATE_IDE)
	{
		LOGI("--------------- washing step: %d of program : %d----------------",running_step,running_pg);
		step_para = system_data.flash_data.Program_para[running_pg][running_step];
		shake_state = WS_STATE_START;
		return 1;
	}
	return 0;
}

void show_infor_drying_step()
{
	LOGI("1:Move to Wells: %d",step_para.wells);
	LOGI("2:wait1 : %ds",step_para.timing[0]);
	LOGI("3:fill washing solution\n4:move z to bottom");
	LOGI("5:wait2 : %ds",step_para.timing[1]);
	LOGI("6:shake on : %ds",step_para.timing[2]);
	LOGI("7:wait4 : %ds",step_para.timing[3]);
	LOGI("8:move z to top\n9:Drain washing solution");
	LOGI("10:wait5 : %ds",step_para.timing[4]);
}

int step_washing_process(void)
{
	    switch(ws_state) {
	        case WS_STATE_IDE:
	            // Do something when in state WS_STATE_IDE
	            ws_state = WS_STATE_START; // Example transition to next state
	            break;
	        case WS_STATE_START:
	        	show_infor_drying_step();
	            LOGI("move x to %lu",system_data.flash_data.Well_position[step_para.wells-1]);
            	mt_set_target_position(&x_motor, system_data.flash_data.Well_position[step_para.wells-1]); // m
	            ws_state = WS_STATE_MOVE_WELLS; // Example transition to next state
	            break;
	        case WS_STATE_MOVE_WELLS:
	        	if(Mt_get_current_prosition(x_motor) == system_data.flash_data.Well_position[step_para.wells-1])
				{
					LOGI("move x done, wait1 %ds",step_para.timing[0]);
					old_ws_state = ws_state;
					ws_state = WS_STATE_WAIT;
					t_time = HAL_GetTick() +  (uint32_t)step_para.timing[0] * 1000;
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
            		LOGI("move z done, wait2 %ds",step_para.timing[1]);
					old_ws_state = ws_state;
	            	t_time = HAL_GetTick() +  (uint32_t)step_para.timing[1] * 1000;
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
                		LOGI("shake done, wait4 : %d",step_para.timing[3]);
                		old_ws_state = ws_state;
    	                t_time = HAL_GetTick() +  (uint32_t)step_para.timing[3] * 1000;
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
            		start_drain_washing_solution();
//					t_time = HAL_GetTick() +  (uint32_t)step_para.timing[4] * 1000;
            		ws_state = WS_STATE_DRAIN_WS_SOLUTION;
				}
	            break;
	        case WS_STATE_DRAIN_WS_SOLUTION:
	            if(is_washing_solution_empty())
	            {
	            	LOGI("solution empty,wait5 :%ds",step_para.timing[4]);
	            	stop_drain_washing_solution();
	            	old_ws_state = ws_state;
	            	t_time = HAL_GetTick() +  (uint32_t)step_para.timing[4] * 1000;
	            	ws_state = WS_STATE_WAIT; // Example transition to next state
	            }
	            break;
	        case WS_STATE_WAIT:
	            if(HAL_GetTick() > t_time)
	            {
	            	switch (old_ws_state) {
						case WS_STATE_MOVE_WELLS:
							LOGI("wait1 done");
							ws_state = WS_STATE_FILL_WS_SOLUTION;
							start_fill_washing_solution();
							break;
						case WS_STATE_Z_BOTTOM:
							LOGI("wait2 done");
							mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos-1000);
							t_time = HAL_GetTick() +  (uint32_t)step_para.timing[2] * 1000;
							LOGI("start shake in %ds",step_para.timing[2]);
							ws_state = WS_STATE_SHAKE;
							break;
						case WS_STATE_SHAKE:
							LOGI("wait4 done, move z to top");
							mt_set_target_position(&z_motor,0);
							ws_state = WS_STATE_Z_TOP;
							break;
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
		step_para = system_data.flash_data.Program_para[running_pg][running_step];
		dy_state = DY_STATE_START;
		return 1;
	}
	return 0;
}

int step_drying_process(void)
{
	switch (dy_state) {
	    case DY_STATE_IDE:
	        break;
	    case DY_STATE_START:
//        	show_infor_drying_step();
            LOGI("move x to %lu",system_data.flash_data.Well_position[step_para.wells-1]);
        	mt_set_target_position(&x_motor, system_data.flash_data.Well_position[step_para.wells-1]); // m
        	old_dy_state = dy_state;
        	dy_state = DY_STATE_MOVE_WELLS; // Example transition to next state
            break;
	    case DY_STATE_MOVE_WELLS:
        	if(Mt_get_current_prosition(x_motor) == system_data.flash_data.Well_position[step_para.wells-1])
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
				dy_state = DY_HEATER_ON; // Example transition to next state

			}
			break;
	    case DY_HEATER_ON:
	    	LOGI("heater on, wait : %ds",step_para.timing[0]);
	    	old_dy_state = dy_state;
	    	t_time = HAL_GetTick() +  (uint32_t)step_para.timing[0] * 1000;
	    	dy_state = DY_STATE_WAIT; // Example transition to next state
	        break;
	    case DY_HEATER_OFF:
	    	LOGI("heater off");
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
						dy_state = DY_HEATER_OFF; // Example transition to next state
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

_step_type start_step(){
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
						running_step++;
						pgstate = PG_STATE_START;
					}
					break;
				default:
					LOGI("Unknown step type");
					break;
			}
			break;
		case PG_STATE_STOP:
//			printf("Program is stopping\n");
			// Code to handle the stop state
			break;
		case PG_STATE_END:
//			printf("Program has ended\n");
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
