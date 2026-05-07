/*
 * System.h
 *
 *  Created on: Apr 30, 2026
 *      Author: dawid
 */

#ifndef BASE_SYSTEM_H_
#define BASE_SYSTEM_H_
#include "Defines.h"

#define SCB_CPACR_FULLACCESS_COPROCESSORS (0x0F << 20)


void System_Init(void);
void System_EnableFPU(void);


#endif /* BASE_SYSTEM_H_ */
