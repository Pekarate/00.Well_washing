/*
 * shacke_step_process.c
 *
 *  Created on: Jun 10, 2023
 *      Author: ADMIN
 */

#include <program_process.h>
#include "main.h"
#include "data.h"
#include "motor.h"
#include "dw_display.h"
#include "define.h"
#define DEBUG 1
#define LOG_TAG "SHAKE"
#define LOGI(fmt, ...) printf("[%s] " fmt "\r\n", LOG_TAG, ##__VA_ARGS__)
#define LOG3(fmt, ...) while(0);//printf("[%s] " fmt "\n", LOG_TAG, ##__VA_ARGS__)

static uint32_t t_time = 0;
extern uint8_t running_pg;
extern uint8_t running_step;
_def_shake_step *shake_step;
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
