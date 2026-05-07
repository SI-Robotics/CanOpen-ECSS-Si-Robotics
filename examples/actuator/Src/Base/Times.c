/*
 * Times.c
 *
 *  Created on: Apr 30, 2026
 *      Author: dawid
 */


#include "Times.h"

Time_t Time;

void Time_IncreaseTime(void)
{
	__disable_irq();
	Time.timeMs++;
	__enable_irq();
}


Time_t* Time_GetModuleStruct(void)
{
	return &Time;
}

void Time_Init(void)
{
	memset(&Time, 0, sizeof(Time_t));
}

void Time_Delay(uint32_t timeout)
{
	if (timeout == 0)
	{
		return;
	}
	uint32_t tick = Time.timeMs;
	uint32_t tim = tick + timeout;
	uint32_t lastTime = tick;
	uint32_t stuckCounter = 0;

	if (tim > tick)
	{
		while (!(tim <= Time.timeMs))
		{
			if (Time.timeMs == lastTime)
			{
				if (++stuckCounter > 100000)
				{
					return;  // SysTick not running - bail out
				}
			}
			else
			{
				lastTime = Time.timeMs;
				stuckCounter = 0;
			}
		}
		return;
	}
	while (!(tim <= Time.timeMs && Time.timeMs < tick))
	{
		if (Time.timeMs == lastTime)
		{
			if (++stuckCounter > 100000)
			{
				return;
			}
		}
		else
		{
			lastTime = Time.timeMs;
			stuckCounter = 0;
		}
	}
	return;
}

bool Time_TimeoutExpired(uint32_t tick, uint32_t timeout)
{
	__disable_irq();
	uint32_t currentTime = Time.timeMs;
	__enable_irq();

	uint32_t tim = tick + timeout;
	if (tim > tick)
	{
		return tim <= currentTime;
	}
	else
	{
		return (tim <= currentTime && currentTime < tick);
	}
}

uint32_t Time_GetTimeMs(void)
{
	return Time.timeMs;
}
