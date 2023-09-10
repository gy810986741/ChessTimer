/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _USART_H
#define _USART_H
#include "stm8l15x.h"

// #define PUTCHAR_PROTOTYPE int putchar(int c)
// #define GETCHAR_PROTOTYPE int getchar(void)

void Uart1_Init(void);
int halUartRecv(uint8_t port, uint8_t *buf, int bufSize);
void UART1_RX_ISR(void);
#endif
