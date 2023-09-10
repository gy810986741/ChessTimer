/**
 ******************************************************************************
 * @file    Project/STM8L15x_StdPeriph_Template/main.c
 * @author  MCD Application Team
 * @version V1.6.1
 * @date    30-September-2014
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "stm8l15x.h"
// #include "stm8l1528_eval_glass_lcd.h"
#include "stm8_eval.h"
#include "stm8l15x_it.h"
#include "key.h"
#include "switch.h"
#include "LCD.h"
#include "systick.h"
#include "usart.h"
#include "jsmn.h"
// #include "u8g2.h"

/** @addtogroup STM8L15x_StdPeriph_Template
 * @{
 */

#define SCROLL_SPEED 100 /* Low value gives higher speed */
#define SCROLL_NUM 5

#define Bit_RESET 0
#define Bit_SET 1
#define MODE_RUN 0
#define MODE_HALT 1
typedef enum
{
    MODE_IDLE = 0,
    MODE_RUN,
    MODE_PAUSE,
    MODE_SET,
} typdefWorkMode;
typedef struct
{
    unsigned char pos_x;
    unsigned char pos_y;

    uint32_t timeTick;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    // unsigned int bonus;
    unsigned int steps;

} sPlayerInfo;
typedef struct
{
    unsigned char pos_x;
    unsigned char pos_y;

    uint32_t timeTick;
    unsigned char mode;
    unsigned char bonus;
    uint32_t period_length;
    uint32_t increment;
    uint32_t delay;
    uint32_t minimum_moves;

} sGameInfo;
sPlayerInfo playerA = {0};
sPlayerInfo playerB = {0};

sGameInfo GAME[2] = {0};

typdefWorkMode workMode = MODE_PAUSE;

static unsigned char periodsStep = 0;
static unsigned char whoisplay = 0;

static unsigned char gameMode = 0;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

void Delay(uint32_t nCount);
void key_process(void);
void switch_process(void);
void display_process(void);
void QR_process(void);
void timer_process(void);
static void TimeConvert(uint32_t tick, sPlayerInfo *playerInfo)
{
    sPlayerInfo *tempInfo;
    tempInfo = playerInfo;
    if (tick > 3600)
    {
        tempInfo->hour = tick / 3600;
        tempInfo->min = (tick % 3600) / 60;
        tempInfo->sec = (tick % 3600) % 60;
    }
}
static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    {
        return 0;
    }
    return -1;
}
char rxBuffer[500] = {0};
int json_Parse(const char *json)
{
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[128]; /* We expect no more than 128 tokens */
    char temp[10];

    jsmn_init(&p);
    r = jsmn_parse(&p, json, strlen(json), t, sizeof(t) / sizeof(t[0]));
    if (r < 0)
    {
        return 1;
    }

    // /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT)
    {
        return 1;
    }

    /* Loop over all keys of the root object */
    for (i = 1; i < r; i++)
    {
        if (jsoneq(json, &t[i], "periods") == 0)
        {
            int j = 0;
            i++;
            int max_j = t[i].size;
            if (max_j <= 1)
            {
                gameMode = 0;
            }
            else
            {
                gameMode = 1;
            }

            i++;
            for (j = 0; j < max_j; j++) // get array size and start a loop
            {
                /* code */
                int max_k = t[i].size;
                i++;
                for (int k = 0; k < max_k; k++)
                {
                    if (jsoneq(json, &t[i], "period_length") == 0)
                    {
                        memset(temp, 0, 10);
                        memcpy(temp, json + t[i + 1].start, t[i + 1].end - t[i + 1].start);
                        GAME[j].period_length = atoi(temp);
                        i += 2;
                    }
                    else if (jsoneq(json, &t[i], "increment") == 0)
                    {
                        memset(temp, 0, 10);
                        memcpy(temp, json + t[i + 1].start, t[i + 1].end - t[i + 1].start);
                        GAME[j].increment = atoi(temp);
                        i += 2;
                    }
                    else if (jsoneq(json, &t[i], "delay") == 0)
                    {
                        memset(temp, 0, 10);
                        memcpy(temp, json + t[i + 1].start, t[i + 1].end - t[i + 1].start);
                        GAME[j].delay = atoi(temp);
                        i += 2;
                    }
                    else if (jsoneq(json, &t[i], "minimum_moves") == 0)
                    {
                        memset(temp, 0, 10);
                        memcpy(temp, json + t[i + 1].start, t[i + 1].end - t[i + 1].start);
                        GAME[j].minimum_moves = atoi(temp);
                        i += 2;
                    }
                }
            }
        }
    }

    if (GAME[0].period_length > 3600)
    {
        playerA.hour = GAME[0].period_length / 3600;
        playerB.hour = GAME[0].period_length / 3600;
        playerA.min = (GAME[0].period_length % 3600) / 60;
        playerB.min = (GAME[0].period_length % 3600) / 60;
        playerA.sec = (GAME[0].period_length % 3600) % 60;
        playerB.sec = (GAME[0].period_length % 3600) % 60;
    }
    else
    {
        playerA.hour = 0;
        playerB.hour = 0;
        playerA.min = GAME[0].period_length / 60;
        playerB.min = GAME[0].period_length / 60;
        playerA.sec = GAME[0].period_length % 60;
        playerB.sec = GAME[0].period_length % 60;
    }

    if (gameMode == 0)
    {
        /* code */
        LCD_ShowString(4, 0, "    G/   d     ", 16);
        LCD_ShowNum(4 + 8 * 6, 0, GAME[0].period_length / 60, 2, 1, 16);
        LCD_ShowNum(4 + 8 * 10, 0, GAME[0].delay, 2, 1, 16);
        playerA.steps = 0;
        playerB.steps = 0;
        LCD_ShowString(52, 20, "1/1", 16);
    }
    else
    {
        LCD_ShowString(4, 0, "  /   SD/   +  ", 16);
        LCD_ShowNum(4 + 8 * 0, 0, GAME[0].minimum_moves, 2, 1, 16);
        LCD_ShowNum(4 + 8 * 3, 0, GAME[0].period_length / 60, 2, 1, 16);
        LCD_ShowNum(4 + 8 * 9, 0, GAME[1].period_length / 60, 2, 1, 16);
        LCD_ShowNum(4 + 8 * 13, 0, GAME[1].increment, 2, 1, 16);

        playerA.steps = GAME[0].minimum_moves;
        playerB.steps = GAME[0].minimum_moves;
        LCD_ShowString(52, 20, "1/2", 16);
        /* code */
    }

    return 0;
}

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
void main(void)
{
    /* High speed internal clock prescaler: 1 */
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
    while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
    {
    }
    /* Enable RTC clock */
    // CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

    /* Configures the RTC */
    // RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
    // RTC_ITConfig(RTC_IT_WUT, ENABLE);

    /* Enable general Interrupt*/
    enableInterrupts();

    /* Initialize BEEP and Scanner trigger pin */
    GPIO_Init(GPIOG, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT); // beep
    GPIO_Init(GPIOG, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);
    GPIO_SetBits(GPIOF, GPIO_Pin_1); // Trigger

    /*initialize hall enable pin */
    GPIO_Init(GPIOF, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_SetBits(GPIOF, GPIO_Pin_1); // Trigger

    systickInit();
    keyInit();
    switchInit();
    lcd_init();

    Uart1_Init();

    LCD_ShowString(4, 0, "  Chess Timer ", 16);
    GUI_Line(0, 18, 127, 18, 1);
    GUI_Line(0, 19, 127, 19, 1);

    LCD_ShowString(32, 20, "A", 16);
    // LCD_ShowString(52, 20, "1/ ", 16);
    LCD_ShowString(88, 20, "B", 16);

    LCD_ShowString(8, 44, ":", 16);
    LCD_ShowString(127 - 6 * 2 - 8 * 2 - 8 - 2, 44, ":", 16);

    /* Infinite loop */

    while (1)
    {
        QR_process();
        key_process();
        switch_process();
        display_process();
    }
}

void timer_process(void)
{
    static uint32_t timeUpdateTick = 0;
    if ((systickGet() - timeUpdateTick) >= 1000 * TIME_1MS)
    {
        timeUpdateTick = systickGet();
        if (workMode == MODE_RUN)
        {
            if (periodsStep == 0)
            {
                if (whoisplay == 1)
                {
                    playerA.timeTick--;
                    TimeConvert(playerA.timeTick, playerA);
                }
                else if (whoisplay == 2)
                {
                    playerB.timeTick--;
                    TimeConvert(playerB.timeTick, playerB);
                }
            }
        }
    }
}
#define SWITCH_PLAY KEY_INPUT0
#define SWITCH_BEEP_ON KEY_INPUT1
#define SWITCH_BEEP_OFF KEY_INPUT2
#define SWITCH_OFF KEY_INPUT3
void switch_process(void)
{
    static uint32_t timeUpdateTick = 0;

    static unsigned char switchPlayEVT = 0;

    if ((systickGet() - timeUpdateTick) >= 1 * TIME_1MS)
    {
        timeUpdateTick = systickGet();
        switchInputProcess();
    }
    switchPlayEVT = AppSwitch_GetEvent(SWITCH_PLAY); //

    if (EV_SWITCH_PRESS == switchPlayEVT)
    {
    }
    if (EV_SWITCH_RELEASE == switchPlayEVT)
    {
    }
}
void QR_process(void)
{
    static uint32_t usartExpiredTick = 0;
    uint8_t ch;
    static uint32_t index = 0;

    while (1 == halUartRecv(1, &ch, 1))
    {

        rxBuffer[index++] = ch;
        usartExpiredTick = systickGet();
    }
    if (((systickGet() - usartExpiredTick) >= 1 * TIME_1MS) && (index != 0))
    {
        /* code */
        usartExpiredTick = systickGet();
        json_Parse(rxBuffer);
        memset(rxBuffer, 0, index + 1);
        index = 0;
    }
}
static unsigned char indexShift = 0;
void settingTime(void)
{
    indexShift++;
    switch (indexShift)
    {
    case 0:

        break;

    default:
        break;
    }
}

#define KEY_START KEY_INPUT1
#define KEY_SET KEY_INPUT2
#define KEY_UP KEY_INPUT3
#define KEY_DOWN KEY_INPUT0
#define KEY_SWITCH KEY_INPUT4

#define ENABLE_HALL GPIO_SetBits(GPIOF, GPIO_Pin_1)
#define DISABLE_HALL GPIO_ResetBits(GPIOF, GPIO_Pin_1)

void key_process(void)
{
    static uint32_t timeUpdateTick = 0;
    // static uint32_t timeSwitchDebounceTick = 0;

    static unsigned char keyDownEVT = 0;
    static unsigned char keyUpEVT = 0;
    static unsigned char keySetEVT = 0;
    static unsigned char keyStartEVT = 0;

    if ((systickGet() - timeUpdateTick) >= 1 * TIME_1MS)
    {
        timeUpdateTick = systickGet();
        keyInputProcess();
    }
    keyDownEVT = AppKey_GetEvent(KEY_DOWN);   //
    keyStartEVT = AppKey_GetEvent(KEY_START); //
    keyUpEVT = AppKey_GetEvent(KEY_UP);       //
    keySetEVT = AppKey_GetEvent(KEY_SET);     //

    //-----------------------------------------------------------------------------------------------------------
    // key driver needs to be done.
    //-----------------------------------------------------------------------------------------------------------

    if (EV_KEY_PRESS == keyStartEVT)
    {
    }
    if (EV_KEY_PRESS == keyUpEVT)
    {
    }
    if (EV_KEY_PRESS == keySetEVT)
    {
    }

    if (EV_KEY_PRESS == keyDownEVT)
    {
    }
    if (EV_KEY_LONG_PRESS == keyDownEVT)
    {
        GPIO_ResetBits(GPIOG, GPIO_Pin_4);
    }
    if (EV_KEY_SHORTPRESS == keyDownEVT)
    {
    }
    if (EV_KEY_LONG_RELEASE == keyDownEVT)
    {
        GPIO_SetBits(GPIOG, GPIO_Pin_4);
    }
}
void display_process(void)
{
    static uint32_t timeUpdateTick, flashTick;
    static unsigned char reverseFlag = 0;
    if ((systickGet() - timeUpdateTick) >= 10 * TIME_1MS)
    {
        timeUpdateTick = systickGet();
        LCD_Refresh_Gram();
    }
    if ((systickGet() - flashTick) >= 500 * TIME_1MS)
    {
        flashTick = systickGet();

        if (reverseFlag == 0)
        {
            reverseFlag = 1;
        }
        else
        {
            reverseFlag = 0;
        }
    }
    if (workMode == MODE_RUN)
    {
        if (reverseFlag == 0)
        {
            LCD_ShowString(8, 44, " ", 16);
            LCD_ShowString(127 - 6 * 2 - 8 * 2 - 8 - 2, 44, " ", 16);
        }
        else
        {
            LCD_ShowString(8, 44, ":", 16);
            LCD_ShowString(127 - 6 * 2 - 8 * 2 - 8 - 2, 44, ":", 16);
        }
        LCD_ShowNum(0, 32, playerA.steps, 3, 1, 12);
        LCD_ShowNum(109, 32, playerB.steps, 3, 1, 12);

        LCD_ShowNum(0, 44, playerA.hour, 1, 1, 16);
        LCD_ShowNum(16, 44, playerA.min, 2, 1, 16);
        LCD_ShowNum(32 + 2, 48, playerA.sec, 2, 1, 12);

        LCD_ShowNum(127 - 6 * 2 - 8 * 2 - 8 - 8 - 2, 44, playerB.hour, 1, 1, 16);
        LCD_ShowNum(127 - 6 * 2 - 8 * 2 - 2, 44, playerB.min, 2, 1, 16);
        LCD_ShowNum(127 - 6 * 2, 48, playerB.sec, 2, 1, 12);
    }
    if (workMode == MODE_SET)
    {
        switch (indexShift)
        {
        case 0:
            if (reverseFlag == 0)
            {
                LCD_ShowNum(0, 44, playerA.hour, 1, 1, 16);
            }
            else
            {
                LCD_ShowString(0, 44, " ", 1, 1, 16);
            }
            LCD_ShowNum(0, 32, playerA.steps, 3, 1, 12);
            LCD_ShowNum(109, 32, playerB.steps, 3, 1, 12);

            // LCD_ShowNum(0, 44, playerA.hour, 1, 1, 16);
            LCD_ShowNum(16, 44, playerA.min, 2, 1, 16);
            LCD_ShowNum(32 + 2, 48, playerA.sec, 2, 1, 12);

            LCD_ShowNum(127 - 6 * 2 - 8 * 2 - 8 - 8 - 2, 44, playerB.hour, 1, 1, 16);
            LCD_ShowNum(127 - 6 * 2 - 8 * 2 - 2, 44, playerB.min, 2, 1, 16);
            LCD_ShowNum(127 - 6 * 2, 48, playerB.sec, 2, 1, 12);
            break;
        case 1:
            if (reverseFlag == 0)
            {
                LCD_ShowNum(16, 44, playerA.min, 2, 1, 16);
            }
            else
            {
                LCD_ShowString(16, 44, " ", 2, 1, 16);
            }
            LCD_ShowNum(0, 32, playerA.steps, 3, 1, 12);
            LCD_ShowNum(109, 32, playerB.steps, 3, 1, 12);

            LCD_ShowNum(0, 44, playerA.hour, 1, 1, 16);
            // LCD_ShowNum(16, 44, playerA.min, 2, 1, 16);
            LCD_ShowNum(32 + 2, 48, playerA.sec, 2, 1, 12);

            LCD_ShowNum(127 - 6 * 2 - 8 * 2 - 8 - 8 - 2, 44, playerB.hour, 1, 1, 16);
            LCD_ShowNum(127 - 6 * 2 - 8 * 2 - 2, 44, playerB.min, 2, 1, 16);
            LCD_ShowNum(127 - 6 * 2, 48, playerB.sec, 2, 1, 12);
            break;

        default:
            break;
        }
    }

    LCD_ShowNum(56, 36, GAME[0].delay, 2, 1, 16);
}
/**
 * @brief Delay.
 * @param[in] nCount
 * @retval None
 */
void Delay(uint32_t nCount)
{
    /* Decrement nCount value */
    while (nCount != 0)
    {
        nCount--;
    }
}
#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
