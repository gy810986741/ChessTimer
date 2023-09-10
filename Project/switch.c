#include "switch.h"
#include "LCD.h"
#include "stm8l15x_it.h"

volatile sSwitchRAMData_t SwitchRamData[SWITCH_MAX] = {0};

void switchInit(void)
{
    GPIO_Init(GPIOF, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);

    for (unsigned char nChannel = 0; nChannel < SWITCH_MAX; nChannel++)
    {
        SwitchRamData[nChannel].nOldPortValue = 0x00;
        SwitchRamData[nChannel].nLastValue = 0x0;
        SwitchRamData[nChannel].nEvent = EV_SWITCH_IDLE;
        SwitchRamData[nChannel].nLongEvent = 0;
        SwitchRamData[nChannel].lDebouncingTime = 10;
        SwitchRamData[nChannel].tick_Debounce = 0;
        SwitchRamData[nChannel].lLongpressTime = 2000;
    }
}
unsigned char switchGetInputValue(unsigned char keyIndex)
{
    unsigned char inputValue = 0;
    if (keyIndex > SWITCH_MAX)
    {
        return 0;
    }
    switch (keyIndex)
    {
    case 0:
        if (GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_0_PIN) == RESET)
        {
            inputValue = 0x1;
        }
        break;
    case 1:
        if (GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_1_PIN) == RESET)
        {
            inputValue = 0x1;
        }
        break;
    case 2:
        if (GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_2_PIN) == RESET)
        {
            inputValue = 0x1;
        }
        break;
    case 3:
        if (GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_3_PIN) == RESET)
        {
            inputValue = 0x1;
        }
        break;

    default:
        break;
    }
    return inputValue;
}
void switchInputProcess(void)
{
    unsigned char nChannel = 0;
    for (nChannel = 0; nChannel < SWITCH_MAX; nChannel++)
    {
        SwitchRamData[nChannel].nPortValue = switchGetInputValue(nChannel);
        // if (SwitchRamData[nChannel].nLongEvent == 1) // START
        // {
        //     if ((systickGet() - SwitchRamData[nChannel].tick_long_press) >= SwitchRamData[nChannel].lLongpressTime)
        //     {
        //         SwitchRamData[nChannel].nEvent = EV_SWITCH_LONG_PRESS;
        //         SwitchRamData[nChannel].nLongEvent = 2; // END
        //         SwitchRamData[nChannel].tick_long_press = systickGet();
        //     }
        // }
        // else
        // {
        //     SwitchRamData[nChannel].tick_long_press = systickGet();
        // }

        if (SwitchRamData[nChannel].nPortValue == SwitchRamData[nChannel].nOldPortValue)
        {
            if ((systickGet() - SwitchRamData[nChannel].tick_Debounce) >= SwitchRamData[nChannel].lDebouncingTime)
            {
                SwitchRamData[nChannel].tick_Debounce = systickGet();
                if (SwitchRamData[nChannel].nLastValue != SwitchRamData[nChannel].nPortValue)
                {
                    if (SwitchRamData[nChannel].nPortValue == 0x01)
                    {
                        SwitchRamData[nChannel].nEvent = EV_SWITCH_PRESS;
                        // SwitchRamData[nChannel].nLongEvent = 1;
                        // SwitchRamData[nChannel].tick_long_press = systickGet();
                    }
                    else
                    {
                        // if (SwitchRamData[nChannel].nLongEvent == 2)
                        // {
                        //     SwitchRamData[nChannel].nEvent = EV_SWITCH_LONG_RELEASE;
                        //     SwitchRamData[nChannel].nLongEvent = 0;
                        // }
                        // else
                        // {
                        SwitchRamData[nChannel].nEvent = EV_SWITCH_RELEASE;
                        // }
                    }
                }
                SwitchRamData[nChannel].nLastValue = SwitchRamData[nChannel].nPortValue;
            }
        }
        else
        {
            SwitchRamData[nChannel].tick_Debounce = systickGet();
        }
        SwitchRamData[nChannel].nOldPortValue = SwitchRamData[nChannel].nPortValue;
    }
}
unsigned char AppSwitch_GetEvent(unsigned char nChannel)
{
    unsigned char nEvent = EV_SWITCH_IDLE;

    if (nChannel < SWITCH_MAX)
    {
        nEvent = SwitchRamData[nChannel].nEvent;         // Get actual event
        SwitchRamData[nChannel].nEvent = EV_SWITCH_IDLE; // Clear the event
    }

    return nEvent;
}
