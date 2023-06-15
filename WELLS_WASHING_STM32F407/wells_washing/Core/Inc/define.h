/*
 * define.h
 *
 *  Created on: Apr 2, 2023
 *      Author: ADMIN
 */

#ifndef INC_DEFINE_H_
#define INC_DEFINE_H_

#define SIMULATOR_MOD 0


#define NUM_MAX_WELL		 		15

#define PUMP1_ON_LEVEL 				1
#define PUMP1_OFF_LEVEL 			0
#define WS_SOLUTION_FULL_LEVEL 		1

#define PUMP2_ON_LEVEL 				1
#define PUMP2_OFF_LEVEL 			0
#define WS_SOLUTION_EMPTY_LEVEL 	1

#define HEATER_ON_LEVEL 			1
#define HEATER_OFF_LEVEL 			0


#define TIME_FILL_WS_SL				2	//time for fill washing solution
#define TIME_DRAIN_WS_SL			2	//time for drain washing solution

#define SHAKING_DISTANCE 		 	1000

#define PULSES_PER_MM 		 		7


#define MAX_PROGRAM_NUM 10
#define MAX_STEP_NUM 	25
#define FACTORY_CODE 	0x2222

#define FLASH_START_ADDRESS  0x08060000


#define STEP_LOSS 0

#define ENABLE_DEBUG 1
#if ENABLE_DEBUG
	#define ENABLE_LOG_PROGRAM_PROCESS 		1
	#define ENABLE_LOG_STEP_SHAKEING 		1
	#define ENABLE_LOG_STEP_DRYING 			1
	#define ENABLE_LOG_STEP_WASHING			1
	#define ENABLE_LOG_DWIN_DISPLAY			1

	#define LOGI(LOG_TAG,fmt, ...) printf("\033[1;32m[%s][%lu] " fmt "\033[0m\r\n", LOG_TAG,HAL_GetTick(), ##__VA_ARGS__)
	#define LOGW(LOG_TAG,fmt, ...) printf("\033[1;33m[%s][%lu] " fmt "\033[0m\r\n", LOG_TAG,HAL_GetTick(), ##__VA_ARGS__)
	#define LOGE(LOG_TAG,fmt, ...) printf("\033[1;31m[%s][%lu] " fmt "\033[0m\r\n", LOG_TAG,HAL_GetTick(), ##__VA_ARGS__)
#else
	#define LOGI(LOG_TAG,fmt, ...)
	#define LOGW(LOG_TAG,fmt, ...)
	#define LOGE(LOG_TAG,fmt, ...)
#endif

#define MAX_UINT32_T 				(0-1)

#endif /* INC_DEFINE_H_ */
