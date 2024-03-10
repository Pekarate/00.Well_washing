/*
 * dw_display.h
 *
 *  Created on: Mar 18, 2023
 *      Author: ADMIN
 */

#ifndef INC_DW_DISPLAY_H_
#define INC_DW_DISPLAY_H_

#include "main.h"

#define PAGE_SETUP_STEP_NONE 		2
#define PAGE_SETUP_USER_CTL 		3
#define PAGE_SETUP_STEP_SHAKE 		6
#define PAGE_SETUP_STEP_WASHING 	7
#define PAGE_SETUP_STEP_DRYING 		8

#define PAGE_RUNNING_STEP_SHAKE 	9
#define PAGE_RUNNING_STEP_WASHING 	10
#define PAGE_RUNNING_STEP_DRYING 	11

#define PAGE_RUNNING_END_PG 		13
#define PAGE_INPUT_WELL_ERROR	 	15

#define PAGE_MANUAL_CONTROL     	5
#define PAGE_RUNNING 				6


#define CompareLOGIN				0x1000

#define VP_LOG_ADDRESS				0x2000

#define VP_SETUP_PARA				0x3000
#define VP_WELLS_ADDR				0x3003
#define VP_X_STEP_MOTOR 			0x2200
#define VP_Z_STEP_MOTOR 			0x2202

#define VP_TEXT_LOG 				0x3800


#define BT_SETUP_CODE 				0x3000
#define BT_SWICH_SETUP_PAGE 		0x3001

#define BT_MOVE_X_FOWARD 			0x2205
#define BT_MOVE_X_BACKWARD 			0x2204

#define BT_MOVE_Z_FOWARD 			0x2305
#define BT_MOVE_Z_BACKWARD 			0x2304

#define BT_X_HOME		 			0x2500
#define BT_Z_HOME		 			0x2501



#define BT_START_PG		 				0x4000
#define BT_STOP_PG		 				0x4001

#define BT_START_PROCEED				0x4002
#define BT_ERROR_WELL_BACK				0x4100

#define BT_SETUP_TAR_GET_WELLS 			0x3003
#define BT_SETUP_FILL_ONOFF				0x3005
#define BT_SETUP_DRAIN_ONOFF	 		0x3009
#define BT_SETUP_HEATER_ONOFF	 		0x3004

#define BT_SWICH_SETUP_APPLY 			0x3A00
#define BT_SWICH_SETUP_EXIT 			0x3101

#define MAX_LOG_LINE_NUMS  22

#define SHOW_LOG_ENABLE	 1

#define LOG_SP_ADDRESS			0x6000
#define LOG_COLOR_ADDRESS		LOG_SP_ADDRESS + 3
#define LOG_LEN_ADDRESS			LOG_SP_ADDRESS + 8
#define LOG_INFOR_COLOR			0x1F72
#define LOG_WARNING_COLOR		0xF5C9
#define LOG_ERROR_COLOR			0xF800
#define DWIN_COLOR_RED			0xF800
#define DWIN_COLOR_BLACK			0x0000

typedef enum{
	LOG_INFOR =LOG_INFOR_COLOR,
	LOG_WARNING =LOG_WARNING_COLOR,
	LOG_ERROR = LOG_ERROR_COLOR
}_Log_type;

void Dwin_switch_page(int page_index);
int dw_update_step_numbers(void);
void Dwin_init(void);
void Dwin_reset(void);

void s_log_add_1_line(char *line);
void dw_log_deamon(void);
void Dwin_switch_running_page(uint8_t pg,uint8_t stepnumber);

void dwin_log_change_color(uint16_t color);
void dwin_log_visiable(uint16_t visible);
void dwin_log_text(_Log_type type,char *data, int len,uint32_t time);
void dwin_log_timeout(void);
void dwin_change_color_sp(uint16_t sp,uint16_t color);

int checkData(const uint8_t* arr, size_t arrSize, const char* strfind, size_t strSize);
#endif /* INC_DW_DISPLAY_H_ */
