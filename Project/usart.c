#include "usart.h"
#include "stm8_eval.h"

struct uartInfo_t
{ /* Rx info */
    uint8_t *RxBuf;
    uint16_t RxBufSize;
    volatile uint16_t RxBufWrite;
    volatile uint16_t RxBufRead;
    volatile uint16_t RxCnt;
};
static uint8_t Uart1RxBuf[700];
struct uartInfo_t Uart1Info =
    {
        Uart1RxBuf,
        sizeof(Uart1RxBuf),
        0,
        0,
        0,
};

void Uart1_Init(void)
{
    STM_EVAL_COMInit(COM1, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1,
                     USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));

    SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, ENABLE);
    /* Enable general interrupts */
    enableInterrupts();
    /* Enable the USART Receive interrupt: this interrupt is generated when the USART receive data register is not empty */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);
}

int halUartRecv(uint8_t port, uint8_t *buf, int bufSize)
{
    int len = 0;
    struct uartInfo_t *pUartInfo;

    pUartInfo = &Uart1Info;

    // EnterCritical(hwlock);

    while ((pUartInfo->RxCnt > 0) && (len < bufSize))
    {
        *buf++ = pUartInfo->RxBuf[pUartInfo->RxBufRead++];
        if (pUartInfo->RxBufRead == pUartInfo->RxBufSize)
            pUartInfo->RxBufRead = 0;

        pUartInfo->RxCnt--;
        len++;
    }

    // ExitCritical(hwlock);

    return len;
}

void UART1_RX_ISR(void)
{
    uint8_t temp;

    /* Read one byte from the receive data register and send it back */
    temp = (USART_ReceiveData8(USART1) & 0x7F);

    if (Uart1Info.RxCnt < Uart1Info.RxBufSize)
    {
        Uart1Info.RxBuf[Uart1Info.RxBufWrite++] = temp;
        if (Uart1Info.RxBufWrite == Uart1Info.RxBufSize)
            Uart1Info.RxBufWrite = 0;
        Uart1Info.RxCnt++;
    }
}