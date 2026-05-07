/*
 * Error.h
 *
 *  Created on: Apr 30, 2026
 *      Author: dawid
 */

#ifndef BASE_ERROR_H_
#define BASE_ERROR_H_

#include "Defines.h"
#include "board.h"

void Error_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);


#endif /* BASE_ERROR_H_ */
