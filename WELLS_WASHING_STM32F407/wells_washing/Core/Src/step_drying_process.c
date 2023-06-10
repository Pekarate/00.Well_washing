/*
 * drying_step_proceess.c
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
#define LOG_TAG "DRY  "
#define LOGI(fmt, ...) printf("[%s] " fmt "\r\n", LOG_TAG, ##__VA_ARGS__)
#define LOG3(fmt, ...) while(0);//printf("[%s] " fmt "\n", LOG_TAG, ##__VA_ARGS__)
static uint32_t t_time = 0;
extern uint8_t running_pg;
extern uint8_t running_step;
_step_dy_state dy_state = DY_STATE_IDE;
_step_dy_state old_dy_state = DY_STATE_IDE;
_def_drying_step *drying_step;
int step_drying_start()
{
	if(dy_state == DY_STATE_IDE)
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