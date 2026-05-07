/*
 * System.c
 *
 *  Created on: Apr 30, 2026
 *      Author: dawid
 */

#include "System.h"
void System_Init(void)
{
	System_EnableFPU();
}



void System_EnableFPU(void)
{

	SCB->CPACR |= SCB_CPACR_FULLACCESS_COPROCESSORS;
}
