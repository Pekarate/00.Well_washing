/*
 * dw_display.h
 *
 *  Created on: Mar 18, 2023
 *      Author: ADMIN
 */

#ifndef INC_DW_DISPLAY_H_
#define INC_DW_DISPLAY_H_

#include "main.h"

#define PAGE_SETUP_STEP_NONE 2
#define PAGE_SETUP_STEP_SHAKE 2
#define PAGE_SETUP_STEP_WASHING 3
#define PAGE_SETUP_STEP_DRYING 4
#define PAGE_RUNNING 			6

#define VP_STEP_NUMBERS		0x2000
#define VP_SETUP_PARA		0x3000
#define VP_X_STEP_MOTOR 	0x2200
#define VP_Z_STEP_MOTOR 	0x2202

#define VP_TEXT_LOG 		0x3800


#define BT_SETUP_CODE 		0x3000
#define BT_SWICH_SETUP_PAGE 0x3001

#define BT_MOVE_X_FOWARD 	0x2205
#define BT_MOVE_X_BACKWARD 	0x2204

#define BT_MOVE_Z_FOWARD 	0x2305
#define BT_MOVE_Z_BACKWARD 	0x2304

#define BT_X_HOME		 	0x2500
#define BT_Z_HOME		 	0x2501

#define BT_START_PG		 	0x4000


#define BT_SWICH_SETUP_APPLY 0x3A00
#define BT_SWICH_SETUP_EXIT 0x3101

#define MAX_LOG_LINE_NUMS  22


int dw_update_step_numbers(void);
void Dwin_init(void);
void Dwin_reset(void);

void s_log_add_1_line(char *line);
void dw_log_deamon(void);

#endif /* INC_DW_DISPLAY_H_ */
