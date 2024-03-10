/*
 * uart.h
 *
 *  Created on: Mar 18, 2023
 *      Author: ADMIN
 */

#ifndef INC_UART_H_
#define INC_UART_H_


#include "main.h"

#define UART_BUF_SIZE 512
#define TX_SIZE 10
typedef struct
{
	uint16_t size;
	uint8_t data[600];
}_uart_frame;

// extern uint8_t uart_rxbuf[UART_BUF_SIZE];
void uart_dma_start(void);
void uart_process(void);
void uart_dma_stop(void);
HAL_StatusTypeDef uart_transmit_block(uint8_t *data,uint16_t size,uint16_t timeout);



#endif /* INC_UART_H_ */
