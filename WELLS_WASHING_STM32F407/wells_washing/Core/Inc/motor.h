/*
 * motor.h
 *
 *  Created on: Mar 19, 2023
 *      Author: ADMIN
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"
#include "stdint.h"
#include "motor.h"

#define STEP_LOSS 2

typedef enum
{
	CMD_IDLE =0,
	CMD_REQUEST_HOME,
	CMD_WAIT_HOME,
	CMD_HOME_FINSH
}_motor_commmand;


typedef enum
{
	DIR_FORWARD = 0,
	DIR_BACKWARD
}_motor_dir;

typedef enum
{
	MT_STATE_IDLE = 0,
	MT_STATE_START,
	MT_STATE_RUNING,
	MT_STATE_STTOP
}_motor_state;

typedef struct{
	uint32_t ud_time; // for display position 2 time diffrent 2 ms
	uint32_t old_pos; // for check is position changed
	uint32_t current_pos;
	uint32_t next_pos;
	_motor_dir current_dir;
	_motor_state state;
	uint8_t home_achieve;
	uint8_t is_home;
	_motor_commmand command;
	void (*set_current_position)(uint32_t);
	int (*is_mt_home_pos)(void);
	void (*mt_set_dir)(_motor_dir);
	void (*mt_move_to_pos)(uint32_t);
	void (*mt_start)();
	void (*mt_stop)();
}_motor_typedef;

extern _motor_typedef x_motor,z_motor;

#define Mt_get_current_prosition(X) X.current_pos

void mt_move_to_home(_motor_typedef *motor);
void mt_set_target_position(_motor_typedef *motor,uint32_t new_position);

void x_step_mt_int(void);
void x_step_motor_process(void);

void z_step_mt_int(void);
void z_step_motor_process(void);


int dw_update_steper_positon(void);
void step_mt_move_foward(_motor_typedef *step,uint32_t numstep);
void step_mt_move_backward(_motor_typedef *step,uint32_t numstep);


#endif /* INC_MOTOR_H_ */
