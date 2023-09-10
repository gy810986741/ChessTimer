#ifndef _SYSTICK_H
#define _SYSTICK_H
#include "stm8l15x.h"
uint32_t systickGet(void);

void systickIncrement(void);
void systickInit(void);

#endif
