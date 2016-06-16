#include "stm32f0xx_hal.h"
#include "cmsis_os.h"
#include "uart.h"

extern UART_HandleTypeDef huart1;

static SemaphoreHandle_t uartSem[8] = {NULL};
static UART_HandleTypeDef* uartHandle[8] = {NULL};
int fputc(int ch, FILE *p)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
	return ch;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
	if(huart->Instance == USART1)
	{	
		xSemaphoreGiveFromISR(uartSem[0],&pxHigherPriorityTaskWoken);
	}
}



void uartInit(int uart)
{
	if (uart == 0) {
		uartSem[0] = xSemaphoreCreateBinary();
		uartHandle[0] = &huart1;
	}
}

void uartTX(int uart,char *data,int size,int timeOut) 
{
	HAL_UART_Transmit(uartHandle[uart],(uint8_t*)data,size,timeOut);
}

int uartRx(int uart,char *data,int size,int timeOut)
{
	HAL_UART_Receive_IT(&huart1,(uint8_t*)data,size);
	return xSemaphoreTake(uartSem[0],timeOut);
	
}



