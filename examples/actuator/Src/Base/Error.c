/*
 * Error.c
 *
 *  Created on: Apr 30, 2026
 *      Author: dawid
 */
#include "Error.h"




void Error_Handler(void)
{

	__disable_irq();
	__DSB();
	__ISB();
	while (1)
	{
#ifndef DEBUG
		NVIC_SystemReset();
#endif
	}
}

void NMI_Handler(void)
{
	__disable_irq();
	__DSB();
	__ISB();
	while (1)
	{
#ifndef DEBUG
		NVIC_SystemReset();
#endif
	}
}

void HardFault_Handler(void)
{
	__disable_irq();
	__DSB();
	__ISB();
	while (1)
	{
#ifndef DEBUG
		NVIC_SystemReset();
#endif
	}
}

void MemManage_Handler(void)
{
	__disable_irq();
	__DSB();
	__ISB();
	while (1)
	{
#ifndef DEBUG
		NVIC_SystemReset();
#endif
	}
}

void BusFault_Handler(void)
{
	__disable_irq();
	__DSB();
	__ISB();
	while (1)
	{
#ifndef DEBUG
		NVIC_SystemReset();
#endif
	}
}

void UsageFault_Handler(void)
{
	__disable_irq();
	__DSB();
	__ISB();
	while (1)
	{
#ifndef DEBUG
		NVIC_SystemReset();
#endif
	}
}

void DebugMon_Handler(void)
{
	__disable_irq();
	__DSB();
	__ISB();
	while (1)
	{
#ifndef DEBUG
		NVIC_SystemReset();
#endif
	}
}

