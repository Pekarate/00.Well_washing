/*
 * washing_step_process.c
 *
 *  Created on: Jun 10, 2023
 */
#include <program_process.h>
#include "main.h"
#include "data.h"
#include "motor.h"
#include "dw_display.h"
#include "define.h"


#if ENABLE_LOG_STEP_WASHING
	#define LOG_TAG "WASH "
#else
	#undef LOGI
	#undef LOGE
	#define LOGI(LOG_TAG,fmt, ...)
	#define LOGE(LOG_TAG,fmt, ...)
#endif
_def_washing_step *washing_step;
extern uint8_t running_pg;
extern uint8_t running_step;
_step_ws_state ws_state = WS_STATE_IDE;
_step_ws_state old_ws_state = WS_STATE_IDE;
static uint32_t ws_time = 0;
void start_fill_washing_solution(){
	LOGI(LOG_TAG,"start fill washing solution");
	HAL_GPIO_WritePin(PUMP1_GPIO_Port, PUMP1_Pin, PUMP1_ON_LEVEL);
}
int is_washing_solution_full()
{
	if(HAL_GPIO_ReadPin(WS_SOLUTION_FULL_GPIO_Port, WS_SOLUTION_FULL_Pin) == WS_SOLUTION_FULL_LEVEL)
		return 1;
	return 0;
}
void stop_fill_washing_solution(){
	LOGI(LOG_TAG,"stop fill washing solution");
	HAL_GPIO_WritePin(PUMP1_GPIO_Port, PUMP1_Pin, PUMP1_OFF_LEVEL);
}


void start_drain_washing_solution(){
	LOGI(LOG_TAG,"start drain fill solution ");
	HAL_GPIO_WritePin(PUMP2_GPIO_Port, PUMP2_Pin, PUMP2_ON_LEVEL);
}
int is_washing_solution_empty()
{
	if(HAL_GPIO_ReadPin(WS_SOLUTION_EMPTY_GPIO_Port, WS_SOLUTION_EMPTY_Pin) == WS_SOLUTION_EMPTY_LEVEL)
			return 1;
	return 0;
}
void stop_drain_washing_solution(){

	LOGI(LOG_TAG,"stop drain washing solution ");
	HAL_GPIO_WritePin(PUMP2_GPIO_Port, PUMP2_Pin, PUMP2_OFF_LEVEL);
}
int step_washing_start()
{
	if(ws_state == WS_STATE_IDE)
	{
		LOGI(LOG_TAG,"--------------- washing step: %d of program : %d----------------",running_step,running_pg);
		washing_step = (_def_washing_step *)&system_data.flash_data.Program_para[running_pg][running_step];
		ws_state = WS_STATE_START;
		return 1;
	}
	return 0;
}
int step_washing_stop(void){
	ws_state = WS_STATE_IDE;
	stop_fill_washing_solution();
	stop_drain_washing_solution();
	return 1;
}
void show_infor_washing_step(_def_washing_step ws_step)
{
	LOGI(LOG_TAG,"---------------INFOR_WASHING_STEP------------");
	LOGI(LOG_TAG,"1:Move to Wells: %d",ws_step.wells);
	LOGI(LOG_TAG,"2:wait1 : %ds",ws_step.wait1);
	LOGI(LOG_TAG,"3:fill washing solution: %d \n4:move z to bottom",ws_step.fill);
	LOGI(LOG_TAG,"5:wait2 : %ds",ws_step.wait2);
	LOGI(LOG_TAG,"6:shake on : %ds",ws_step.shake);
	LOGI(LOG_TAG,"7:wait4 : %ds",ws_step.wait4);
	LOGI(LOG_TAG,"8:move z to top %d\n9:Drain washing solution",ws_step.drain);
	LOGI(LOG_TAG,"10:wait5 : %ds",ws_step.wait5);
	LOGI(LOG_TAG,"-------------------------------------------");
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
	            LOGI(LOG_TAG,"move x to %lu",system_data.flash_data.Well_position[washing_step->wells-1]);
            	mt_set_target_position(&x_motor, system_data.flash_data.Well_position[washing_step->wells-1]); // m
	            ws_state = WS_STATE_MOVE_WELLS; // Example transition to next state
	            break;
	        case WS_STATE_MOVE_WELLS:
	        	if(Mt_get_current_prosition(x_motor) == system_data.flash_data.Well_position[washing_step->wells-1])
				{
					LOGI(LOG_TAG,"move x done, wait1 %ds",washing_step->wait1);
					old_ws_state = ws_state;
					ws_state = WS_STATE_WAIT;
					ws_time = HAL_GetTick() +  (uint32_t)washing_step->wait1 * 1000;
				}
	            break;
	        case WS_STATE_FILL_WS_SOLUTION:
	            if(is_washing_solution_full())
	            {
	            	stop_fill_washing_solution();
					LOGI(LOG_TAG,"Fill done,move z to bottom %lu",system_data.flash_data.Z_bottom_pos);
					mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos); //@TODO
					ws_state = WS_STATE_Z_BOTTOM;

	            }
	            break;
	        case WS_STATE_Z_BOTTOM:
            	if(Mt_get_current_prosition(z_motor) == system_data.flash_data.Z_bottom_pos)
				{
            		LOGI(LOG_TAG,"move z done, wait2 %ds",washing_step->wait2);
					old_ws_state = ws_state;
	            	ws_time = HAL_GetTick() +  (uint32_t)washing_step->wait2 * 1000;
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
                	if(HAL_GetTick() > ws_time)
                	{
                		LOGI(LOG_TAG,"shake done, wait4 : %d",washing_step->wait4);
                		old_ws_state = ws_state;
    	                ws_time = HAL_GetTick() +  (uint32_t)washing_step->wait4 * 1000;
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
            		LOGI(LOG_TAG,"ztop done, start drain ws solution");
            		old_ws_state = ws_state;
//					ws_time = HAL_GetTick() +  (uint32_t)step_para.timing[4] * 1000;
            		if(washing_step->drain)
            		{
						start_drain_washing_solution();
						ws_state = WS_STATE_DRAIN_WS_SOLUTION;
            		}
            		else
            		{
            			LOGI(LOG_TAG,"skip drain fill solution ");
            			old_ws_state = ws_state;
						ws_time = HAL_GetTick() +  (uint32_t)washing_step->wait5 * 1000;
						ws_state = WS_STATE_WAIT; // Example transition to next state
            		}
				}
	            break;
	        case WS_STATE_DRAIN_WS_SOLUTION:
	            if(is_washing_solution_empty())
	            {
	            	LOGI(LOG_TAG,"solution empty,wait5 :%ds",washing_step->wait5);
	            	stop_drain_washing_solution();
	            	old_ws_state = ws_state;
	            	ws_time = HAL_GetTick() +  (uint32_t)washing_step->wait5 * 1000;
	            	ws_state = WS_STATE_WAIT; // Example transition to next state isn't active
	            }
	            break;
	        case WS_STATE_WAIT:
	            if(HAL_GetTick() > ws_time)
	            {
	            	switch (old_ws_state) {
						case WS_STATE_MOVE_WELLS:
							LOGI(LOG_TAG,"wait1 done");
							if(washing_step->fill)
							{
								ws_state = WS_STATE_FILL_WS_SOLUTION;
								start_fill_washing_solution();
							}
							else
							{
								LOGI(LOG_TAG,"skip fill washing solution");
								LOGI(LOG_TAG,"move z to bottom %lu",system_data.flash_data.Z_bottom_pos);
								mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos); //@TODO
								ws_state= WS_STATE_Z_BOTTOM;
							}
							break;
						case WS_STATE_Z_BOTTOM:
							LOGI(LOG_TAG,"wait2 done");
							mt_set_target_position(&z_motor,system_data.flash_data.Z_bottom_pos-1000);
							LOGI(LOG_TAG,"start shake in %ds",washing_step->shake);
							ws_time = HAL_GetTick() +  (uint32_t)washing_step->shake * 1000;

							ws_state = WS_STATE_SHAKE;
							break;
						case WS_STATE_SHAKE:
							LOGI(LOG_TAG,"wait4 done, move z to top");
							mt_set_target_position(&z_motor,0);
							ws_state = WS_STATE_Z_TOP;
							break;
						case WS_STATE_Z_TOP:
						case WS_STATE_DRAIN_WS_SOLUTION:
							LOGI(LOG_TAG,"wait5 done");
							ws_state = WS_STATE_Z_FINISH;
							break;
						default:
							break;
					}
	            }
	            break;
	        case WS_STATE_Z_FINISH:
	        	LOGI(LOG_TAG,"-------------------------------washing step done----------------------");
	            ws_state = WS_STATE_IDE;
	        	return 1;
	            break;
	        default:
	            // Handle an invalid state
	            break;
	    }
	    return 0;
}
