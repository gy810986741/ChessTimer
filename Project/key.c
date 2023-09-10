#include "key.h"
#include "LCD.h"
#include "stm8l15x_it.h"

volatile sKeyRAMData_t KeyRamData[KEY_MAX] = {0};

void keyInit(void)
{
    GPIO_Init(GPIOG, KEY_0_PIN, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOG, KEY_1_PIN, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOG, KEY_2_PIN, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOG, KEY_3_PIN, GPIO_Mode_In_PU_No_IT);
    // GPIO_Init(GPIOF, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);

    for (unsigned char nChannel = 0; nChannel < KEY_MAX; nChannel++)
    {
        KeyRamData[nChannel].nOldPortValue = 0x00;
        KeyRamData[nChannel].nLastValue = 0x0;
        KeyRamData[nChannel].nEvent = EV_KEY_IDLE;
        KeyRamData[nChannel].nLongEvent = 0;
        KeyRamData[nChannel].lDebouncingTime = 10;
        KeyRamData[nChannel].tick_Debounce = 0;
        KeyRamData[nChannel].lLongpressTime = 2000;
    }
}
unsigned char keyGetInputValue(unsigned char keyIndex)
{
    unsigned char inputValue = 0;
    if (keyIndex > KEY_MAX)
    {
        return 0;
    }
    switch (keyIndex)
    {
    case 0:
        if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_0_PIN) == RESET)
        {
            inputValue = 0x1;
        }
        break;
    case 1:
        if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_1_PIN) == RESET)
        {
            inputValue = 0x1;
        }
        break;
    case 2:
        if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_2_PIN) == RESET)
        {
            inputValue = 0x1;
        }
        break;
    case 3:
        if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_3_PIN) == RESET)
        {
            inputValue = 0x1;
        }
        break;
    default:
        break;
    }
    return inputValue;
}
void keyInputProcess(void)
{
    unsigned char nChannel = 0;
    for (nChannel = 0; nChannel < KEY_MAX; nChannel++)
    {
        KeyRamData[nChannel].nPortValue = keyGetInputValue(nChannel);
        if (KeyRamData[nChannel].nLongEvent == 1) // START
        {
            if ((systickGet() - KeyRamData[nChannel].tick_long_press) >= KeyRamData[nChannel].lLongpressTime)
            {
                KeyRamData[nChannel].nEvent = EV_KEY_LONG_PRESS;
                KeyRamData[nChannel].nLongEvent = 2; // END
                KeyRamData[nChannel].tick_long_press = systickGet();
            }
        }
        else
        {
            KeyRamData[nChannel].tick_long_press = systickGet();
        }

        if (KeyRamData[nChannel].nPortValue == KeyRamData[nChannel].nOldPortValue)
        {
            if ((systickGet() - KeyRamData[nChannel].tick_Debounce) >= KeyRamData[nChannel].lDebouncingTime)
            {
                KeyRamData[nChannel].tick_Debounce = systickGet();
                if (KeyRamData[nChannel].nLastValue != KeyRamData[nChannel].nPortValue)
                {
                    if (KeyRamData[nChannel].nPortValue == 0x01)
                    {
                        KeyRamData[nChannel].nEvent = EV_KEY_PRESS;
                        KeyRamData[nChannel].nLongEvent = 1;
                        KeyRamData[nChannel].tick_long_press = systickGet();
                    }
                    else
                    {
                        if (KeyRamData[nChannel].nLongEvent == 2)
                        {
                            KeyRamData[nChannel].nEvent = EV_KEY_LONG_RELEASE;
                            KeyRamData[nChannel].nLongEvent = 0;
                        }
                        else
                        {
                            KeyRamData[nChannel].nEvent = EV_KEY_SHORTPRESS;
                        }
                    }
                }
                KeyRamData[nChannel].nLastValue = KeyRamData[nChannel].nPortValue;
            }
        }
        else
        {
            KeyRamData[nChannel].tick_Debounce = systickGet();
        }
        KeyRamData[nChannel].nOldPortValue = KeyRamData[nChannel].nPortValue;
    }
}
unsigned char AppKey_GetEvent(unsigned char nChannel)
{
    unsigned char nEvent = EV_KEY_IDLE;

    if (nChannel < KEY_MAX)
    {
        nEvent = KeyRamData[nChannel].nEvent;      // Get actual event
        KeyRamData[nChannel].nEvent = EV_KEY_IDLE; // Clear the event
    }

    return nEvent;
}
