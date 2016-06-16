#ifndef _UART_H
#define _UART_H



void uartInit(int uart);
void uartTX(int uart,char *data,int size,int timeOut);
int uartRx(int uart,char *data,int size,int timeOut);


	
#endif

