/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _KEY_H
#define _KEY_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm8l15x.h"
#include "stdbool.h"

#define KEY_GPIO_PORT GPIOG
#define KEY_0_PIN GPIO_Pin_0
#define KEY_1_PIN GPIO_Pin_1
#define KEY_2_PIN GPIO_Pin_2
#define KEY_3_PIN GPIO_Pin_3

enum
{
    EV_KEY_IDLE = 0,
    EV_KEY_PRESS,
    EV_KEY_SHORTPRESS,
    EV_KEY_LONG_PRESS,
    EV_KEY_LONG_RELEASE,
};

enum
{
    KEY_INPUT0 = 0,
    KEY_INPUT1,
    KEY_INPUT2,
    KEY_INPUT3,
    KEY_MAX
};

typedef struct
{
    uint8_t nPortValue;
    uint8_t nOldPortValue;
    uint8_t nLastValue;
    uint8_t nEvent;
    uint8_t nLongEvent;
    uint8_t long_press_event;
    uint32_t lDebouncingTime;
    uint32_t lLongpressTime;
    uint32_t tick_Debounce;
    uint32_t tick_long_press;
} sKeyRAMData_t;

void keyInit(void);
unsigned char keyGetInputValue(unsigned char keyIndex);
void keyInputProcess(void);
unsigned char AppKey_GetEvent(unsigned char nChannel);
#endif
