/*
 * steps_process.h
 *
 *  Created on: Mar 27, 2023
 *      Author: ADMIN
 */

#ifndef INC_PROGRAM_PROCESS_H_
#define INC_PROGRAM_PROCESS_H_
#include "data.h"


typedef enum{
	PG_STATE_IDLE=0,
	PG_STATE_START,
	PG_STATE_RUNNING,
	PG_STATE_STOP,
	PG_STATE_END
}_program_state;

typedef enum{
	SHAKE_STATE_IDE=0,
	SHAKE_STATE_START,
	SHAKE_STATE_MOVE_WELLS,
	SHAKE_STATE_WAIT,
	SHAKE_STATE_Z_BOTTOM,
	SHAKE_STATE_SHAKE,
	SHAKE_STATE_Z_TOP,
	SHAKE_STATE_Z_FINISH

}_step_shake_state;


typedef enum{
	WS_STATE_IDE=0,
	WS_STATE_START,
	WS_STATE_MOVE_WELLS,
	WS_STATE_Z_BOTTOM,
	WS_STATE_FILL_WS_SOLUTION,
	WS_STATE_SHAKE,
	WS_STATE_Z_TOP,
	WS_STATE_DRAIN_WS_SOLUTION,
	WS_STATE_WAIT,
	WS_STATE_Z_FINISH

}_step_ws_state;

typedef enum{
	DY_STATE_IDE=0,
	DY_STATE_START,
	DY_STATE_MOVE_WELLS,
	DY_STATE_Z_BOTTOM,
	DY_HEATER_ON,
	DY_HEATER_OFF,
	DY_STATE_WAIT,
	DY_STATE_Z_TOP,
	DY_STATE_Z_FINISH

}_step_dy_state;


int pg_start(uint8_t pg,uint8_t stepindex);
int pg_stop(void);
void pg_process_loop(void);

#endif /* INC_PROGRAM_PROCESS_H_ */
