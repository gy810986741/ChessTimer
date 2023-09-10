/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _SWITCH_H
#define _SWITCH_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm8l15x.h"
#include "stdbool.h"

#define SWITCH_GPIO_PORT GPIOF
#define SWITCH_0_PIN GPIO_Pin_0
#define SWITCH_1_PIN GPIO_Pin_5
#define SWITCH_2_PIN GPIO_Pin_6
#define SWITCH_3_PIN GPIO_Pin_7

enum
{
    EV_SWITCH_IDLE = 0,
    EV_SWITCH_PRESS,
    EV_SWITCH_RELEASE,
};

enum
{
    SWITCH_INPUT0 = 0,
    SWITCH_INPUT1,
    SWITCH_INPUT2,
    SWITCH_INPUT3,
    // SWITCH_INPUT4,
    SWITCH_MAX
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
} sSwitchRAMData_t;

void switchInit(void);
unsigned char switchGetInputValue(unsigned char keyIndex);
void switchInputProcess(void);
unsigned char AppSwitch_GetEvent(unsigned char nChannel);
#endif
