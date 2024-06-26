/*
 * motor.c
 *
 *  Created on: Mar 19, 2023
 *      Author: ADMIN
 */


#include "main.h"
#include "stdint.h"
#include "motor.h"
#include "stdlib.h"
#include "dw_display.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;

#define Z_HOME_SWICH_OPEN	 (HAL_GPIO_ReadPin(Z_HOME_SWITCH_GPIO_Port, Z_HOME_SWITCH_Pin))
#define Z_HOME_SWICH_CLOSE	 (!HAL_GPIO_ReadPin(Z_HOME_SWITCH_GPIO_Port, Z_HOME_SWITCH_Pin))

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance ==  htim2.Instance)
	{
		htim2.Instance->CNT = 0;
		x_mt_stop();
	} else if(htim->Instance ==  htim5.Instance)
	{
		htim5.Instance->CNT = 0;
		z_mt_stop();
	}
}
_motor_typedef x_motor;


void mt_move_to_home(_motor_typedef *motor){
	if(motor->command == CMD_IDLE){
		motor->command = CMD_REQUEST_HOME;
	}
}
int isMotor_atHome(_motor_typedef *motor){
	return(!motor->current_pos);
}

void mt_set_target_position(_motor_typedef *motor,uint32_t new_position){
	motor->next_pos = new_position;
}

void x_mt_set_dir(){
	if(x_motor.current_pos < x_motor.next_pos){
		x_motor.current_dir = DIR_FORWARD;
		HAL_GPIO_WritePin(X_MOTOR_DIR_GPIO_Port, X_MOTOR_DIR_Pin, DIR_FORWARD);
		htim2.Instance->CR1 &= ~(1<<4);
	}
	else
	{
		x_motor.current_dir = DIR_BACKWARD;
		HAL_GPIO_WritePin(X_MOTOR_DIR_GPIO_Port, X_MOTOR_DIR_Pin, DIR_BACKWARD);
		htim2.Instance->CR1 |= (1<<4);
	}

}


void x_mt_start(){
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}




void x_mt_stop()
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	x_motor.command = CMD_IDLE;
	x_motor.next_pos = x_motor.current_pos = htim2.Instance->CNT;

}



void x_mt_move_to_pos(uint32_t pos){
	x_motor.next_pos = pos;
}

void x_set_current_position(uint32_t pos){
	x_motor.current_pos = pos;
	htim2.Instance->CNT = pos;
}


void x_step_mt_int(void){
	x_motor.current_pos= x_motor.next_pos = htim2.Instance->CNT = 0;
	x_motor.old_pos = x_motor.ud_time =0;
	x_motor.home_achieve = 0;
	x_motor.set_current_position = x_set_current_position;
	HAL_TIM_Base_Start_IT(&htim2);
}

void step_mt_move_foward(_motor_typedef *step,uint32_t numstep){
	step->next_pos +=numstep;
	if(step->next_pos > 2000000000)
	{
		step->next_pos =2000000000;
	}
}

void step_mt_move_backward(_motor_typedef *step,uint32_t numstep){
	step->next_pos -=numstep;
	if(step->next_pos > 2000000000)
	{
		step->next_pos =0;
	}
}

void x_step_motor_home_position()
{
	if(!HAL_GPIO_ReadPin(X_HOME_SWITCH_GPIO_Port, X_HOME_SWITCH_Pin) && (!x_motor.is_home))
	{
		x_motor.home_achieve = 1;
		x_motor.is_home = 1;
		x_mt_stop();
		x_motor.current_dir = htim2.Instance->CNT =0;
	}
	if(htim2.Instance->CNT)
	{
		x_motor.is_home = 0;
	}
}

void home_process(_motor_typedef *motor)
{
	switch (motor->command) {
		case CMD_REQUEST_HOME:
			if(motor->home_achieve && (motor->current_pos == 0))
			{
				motor->command = CMD_HOME_FINSH;
				break;
			}
			if(!motor->home_achieve)
			{
				motor->set_current_position(200000000);
			}
			motor ->next_pos = 0;
			motor->command = CMD_WAIT_HOME;
			break;
		case CMD_WAIT_HOME:
			if(motor->current_pos == 0)
			{
				motor->command = CMD_HOME_FINSH;
			}
			break;
		case CMD_HOME_FINSH:
			motor->command = CMD_IDLE;
			break;
		default:
			break;
	}
}
void x_step_motor_process(void){
	static uint32_t time_x_check_home = 0;
	x_motor.current_pos = htim2.Instance->CNT;
	if(HAL_GetTick() > time_x_check_home){
		x_step_motor_home_position();
	}
	home_process(&x_motor);
	switch (x_motor.state) {
		case MT_STATE_IDLE:
			if(abs(x_motor.current_pos- x_motor.next_pos) > STEP_LOSS){
				x_motor.state = MT_STATE_START;
			}
			break;
		case MT_STATE_START:
			x_mt_set_dir();
			if(x_motor.is_home == 1)
			{
				time_x_check_home = HAL_GetTick()+100;
			}
			x_mt_start();
			x_motor.state = MT_STATE_RUNING;
			break;
		case MT_STATE_RUNING:
			if(x_motor.current_dir == DIR_FORWARD){
				if(x_motor.current_pos >= x_motor.next_pos){
					x_motor.state = MT_STATE_STTOP;
				}
			} else {
				if(x_motor.current_pos <= x_motor.next_pos){
					x_motor.state = MT_STATE_STTOP;
				}
			}
//			if(abs(x_motor.current_pos- x_motor.next_pos) < STEP_LOSS){
//							x_motor.state = MT_STATE_STTOP;
//			}
			break;
		case MT_STATE_STTOP:
			x_mt_stop();
			x_motor.state = MT_STATE_IDLE;
			break;
		default:
			break;
	}
}
//-------------------------------------------------------------------------------------------------------

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;

_motor_typedef z_motor;



void z_mt_set_dir(){
	if(z_motor.current_pos < z_motor.next_pos){
		z_motor.current_dir = DIR_FORWARD;
		HAL_GPIO_WritePin(Z_MOTOR_DIR_GPIO_Port, Z_MOTOR_DIR_Pin, DIR_FORWARD);
		htim5.Instance->CR1 &= ~(1<<4);
	}
	else
	{
		z_motor.current_dir = DIR_BACKWARD;
		HAL_GPIO_WritePin(Z_MOTOR_DIR_GPIO_Port, Z_MOTOR_DIR_Pin, DIR_BACKWARD);
		htim5.Instance->CR1 |= (1<<4);
	}

}

void z_mt_start(){
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void z_mt_stop()
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	z_motor.command = CMD_IDLE;
	z_motor.next_pos = z_motor.current_pos = htim5.Instance->CNT;

}

void z_mt_move_to_pos(uint32_t pos){
	z_motor.next_pos = pos;
	z_motor.current_pos = htim5.Instance->CNT;
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	z_mt_set_dir(z_motor.current_pos,z_motor.next_pos);
}

void z_set_current_position(uint32_t pos){
	z_motor.current_pos = pos;
	htim5.Instance->CNT = pos;
}
void z_step_mt_int(void){
	z_motor.current_pos= z_motor.next_pos = htim5.Instance->CNT = 0;
	z_motor.old_pos = z_motor.ud_time =0;
	z_motor.set_current_position = z_set_current_position;
	HAL_TIM_Base_Start_IT(&htim5);
}


void z_step_motor_home_position()
{
	if(!HAL_GPIO_ReadPin(Z_HOME_SWITCH_GPIO_Port, Z_HOME_SWITCH_Pin) && (!z_motor.is_home))
	{
		z_motor.home_achieve = 1;
		z_motor.is_home = 1;
		z_mt_stop();
		z_motor.current_pos = htim5.Instance->CNT =0;
	}
	if(htim5.Instance->CNT)
	{
		z_motor.is_home = 0;
	}
}



void z_step_motor_process(void){
	static uint32_t time_z_check_home = 0;
	z_motor.current_pos = htim5.Instance->CNT;
	if(HAL_GetTick() > time_z_check_home){
		z_step_motor_home_position();
	}
	home_process(&z_motor);
//	static uint32_t znext =0;
//	if(znext!= z_motor.next_pos)
//	{
//		znext= z_motor.next_pos;
//		printf("z next: %lu\n",z_motor.next_pos);
//	}
	switch (z_motor.state) {
		case MT_STATE_IDLE:
			if(abs(z_motor.current_pos- z_motor.next_pos) > STEP_LOSS){
				z_motor.state = MT_STATE_START;
			}
			break;
		case MT_STATE_START:
			z_mt_set_dir();
			if(z_motor.is_home == 1)
			{
				time_z_check_home = HAL_GetTick()+100;
			}
			z_mt_start();
			z_motor.state = MT_STATE_RUNING;
			break;
		case MT_STATE_RUNING:
			if(z_motor.current_dir == DIR_FORWARD){
				if(z_motor.current_pos >= z_motor.next_pos){
					z_motor.state = MT_STATE_STTOP;
				}
			} else {
				if(z_motor.current_pos <= z_motor.next_pos){
					if((z_motor.next_pos == 0) && Z_HOME_SWICH_OPEN ) //move until home switch enable
					{
						z_set_current_position(10);
					} else {
						z_motor.state = MT_STATE_STTOP;
					}
				}
			}
			break;
		case MT_STATE_STTOP:
			z_mt_stop();
			z_motor.state = MT_STATE_IDLE;
			break;
		default:
			break;
	}
}
void x_motor_boot_start(void)
{
	if(!HAL_GPIO_ReadPin(X_HOME_SWITCH_GPIO_Port, X_HOME_SWITCH_Pin))
	  {
		  printf("start x at home position, move x to 100 and go home");
		  step_mt_move_foward(&x_motor,1000);
		  x_mt_set_dir();
		  x_mt_start();
		  while(htim2.Instance->CNT < 1000);
		  x_mt_stop();
	  } else {
		 printf("start x not at home position, move x go home");
	  }
	  HAL_Delay(500);
	  mt_move_to_home(&x_motor);
	  while(1){  // home X
		  x_step_motor_process();
	//	  dw_update_steper_positon();
		  if(x_motor.is_home)
			  break;
	  }
}

void z_motor_boot_start(void)
{
	if(!HAL_GPIO_ReadPin(Z_HOME_SWITCH_GPIO_Port, Z_HOME_SWITCH_Pin))
	  {
		 printf("start Z at home position, move Z to 100 and go home\n");
		  step_mt_move_foward(&z_motor,1000);
		  z_mt_set_dir();
		  z_mt_start();
		  while(htim5.Instance->CNT < 1000);
		  z_mt_stop();
	  } else {
		 printf("start Z not at home position, move x go home\n");
	  }
	  HAL_Delay(500);
	  mt_move_to_home(&z_motor);
	  while(1){  // home Z
		  z_step_motor_process();
	//	  dw_update_steper_positon();
		  if(z_motor.is_home)
			  break;
	  }
}

