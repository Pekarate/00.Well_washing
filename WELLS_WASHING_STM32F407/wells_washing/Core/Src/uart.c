/*
 * uart.c
 *
 *  Created on: Mar 18, 2023
 *      Author: ADMIN
 */

#include  "uart.h"
#include "string.h"

extern UART_HandleTypeDef huart2;
uint8_t uart_rxbuf[UART_BUF_SIZE];
uint8_t uart_txbuf[UART_BUF_SIZE];
uint16_t UART_Rx_recved=UART_BUF_SIZE+1;
uint32_t UART_Rx_Time =0;

uint32_t uart_rx_cnt =0;
void uart_dma_start(void);
void uart_process(void);
void uart_dma_stop(void);

_uart_frame tx[10];
static uint8_t tc=0,ti=0,to=0;
uint32_t uart_tc =0,uart_itc =0;
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	uart_itc++;
	if(tc)
	{
		if(HAL_UART_Transmit_DMA(&huart2, tx[to].data, tx[to].size) == HAL_OK){
				to++;
				tc--;
				if(to ==TX_SIZE)
					to = 0;
		}
	}

}
void uart_transmit(uint8_t *data,uint8_t size)
{
	uart_tc++;
	tx[ti].size = size;
	memcpy(tx[ti].data,data,size);
	ti++;
	if(ti ==TX_SIZE)
		ti = 0;
	tc++;
	if(HAL_UART_Transmit_DMA(&huart2, tx[to].data, tx[to].size) == HAL_OK){
		to++;
		tc--;
		if(to ==TX_SIZE)
			to = 0;

	}
}


void uart_dma_start(void){
	HAL_UART_Receive_DMA(&huart2, uart_rxbuf, UART_BUF_SIZE);
	UART_Rx_recved= UART_BUF_SIZE+1;
}

void uart_dma_stop(void){
	HAL_UART_DMAStop(&huart2);			//stop DMA
}

__weak int dw_process_rx_buffer(uint8_t *data,uint16_t size){
	return 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	uart_dma_stop();
	uart_dma_start();
}
void uart_process(void){
	if(huart2.hdmarx->Instance->NDTR != UART_BUF_SIZE) {
	  if(huart2.hdmarx->Instance->NDTR != UART_Rx_recved) {
		  UART_Rx_recved = huart2.hdmarx->Instance->NDTR;
		  UART_Rx_Time = HAL_GetTick() + 2;
	  }else{
		  if(HAL_GetTick()> UART_Rx_Time)  // timeoutframe
		  {
			  uart_dma_stop();
			  uart_rx_cnt++;
			  dw_process_rx_buffer(uart_rxbuf, UART_BUF_SIZE - huart2.hdmarx->Instance->NDTR);
			  uart_dma_start();
//			  process_data_rx_uart();
		  }
	  }
	}
}

HAL_StatusTypeDef uart_transmit_block(uint8_t *data,uint16_t size,uint16_t timeout){
	//return HAL_UART_Transmit(&huart2, data, size, timeout);
	uart_transmit(data,size);
	return HAL_OK;
}
