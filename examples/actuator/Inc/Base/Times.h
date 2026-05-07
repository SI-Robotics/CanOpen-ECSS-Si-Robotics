#ifndef BASE_TIMES_H_
#define BASE_TIMES_H_
#include "Defines.h"


typedef struct
{
    volatile uint32_t timeMs;
} Time_t;


Time_t* Time_GetModuleStruct(void);
void Time_IncreaseTime(void);
void Time_Init(void);
void Time_Delay(uint32_t timeout);
bool Time_TimeoutExpired(uint32_t tick, uint32_t timeout);
uint32_t Time_GetTimeMs(void);


#endif /* BASE_TIMES_H_ */
