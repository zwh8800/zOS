#include <stm32f10x.h>
#include <core_cm3.h>
#include <stdint.h>

#include "kdebug.h"

/* 消除对stdio.h的引用. 只在这一处出现 */
#include <stdio.h>
#include <stdarg.h>

int kprintf(const char* fmt, ...)
{
	int ret;
	va_list vl;
	va_start(vl, fmt);
	ret = vprintf(fmt, vl);
	va_end(vl);
	return ret;
}


#define PRIORITY_Msk 0xF0
#define PRIORITY_Pos 4
void dump_scb()
{
	uint32_t icsr = SCB->ICSR;
	uint32_t shcsr = SCB->SHCSR;
	
	kprintf("ICSR:\r\n");
	kprintf("\t there %s interrupt pending\r\n", 
			(icsr & SCB_ICSR_ISRPENDING_Msk) ? "is" : "isn't");
	kprintf("\t pending exception number:%u\r\n", 
			(icsr & SCB_ICSR_VECTPENDING_Msk) >> SCB_ICSR_VECTPENDING_Pos);
	kprintf("\t active exception number:%u\r\n", 
			(icsr & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos);
	
	kprintf("SHPRx:\r\n");
	kprintf("\t Mem Manage:%hhd\r\n", (SCB->SHP[0] & PRIORITY_Msk) >> PRIORITY_Pos);
	kprintf("\t Bus Fault:%hhd\r\n", (SCB->SHP[1] & PRIORITY_Msk) >> PRIORITY_Pos);
	kprintf("\t Usage Fault:%hhd\r\n", (SCB->SHP[2] & PRIORITY_Msk) >> PRIORITY_Pos);
	
	kprintf("\t SVC:%hhd\r\n", (SCB->SHP[7] & PRIORITY_Msk) >> PRIORITY_Pos);
	kprintf("\t Debug Monitor:%hhd\r\n", (SCB->SHP[8] & PRIORITY_Msk) >> PRIORITY_Pos);
	
	kprintf("\t PendSV:%hhd\r\n", (SCB->SHP[10] & PRIORITY_Msk) >> PRIORITY_Pos);
	kprintf("\t SysTick:%hhd\r\n", (SCB->SHP[11] & PRIORITY_Msk) >> PRIORITY_Pos);
	
	kprintf("SHCSR:\r\n");
	kprintf("\t Usage Fault enable:%u\r\n",
			(shcsr & SCB_SHCSR_USGFAULTENA_Msk) >> SCB_SHCSR_USGFAULTENA_Pos);
	kprintf("\t Bus Fault enable:%u\r\n",
			(shcsr & SCB_SHCSR_BUSFAULTENA_Msk) >> SCB_SHCSR_BUSFAULTENA_Pos);
	kprintf("\t Mem Fault enable:%u\r\n",
				(shcsr & SCB_SHCSR_MEMFAULTENA_Msk) >> SCB_SHCSR_MEMFAULTENA_Pos);
	
	kprintf("\r\n");
	
	kprintf("\t SVCall pended:%u\r\n",
					(shcsr & SCB_SHCSR_SVCALLPENDED_Msk) >> SCB_SHCSR_SVCALLPENDED_Pos);
	kprintf("\t Bus Fault pended:%u\r\n",
						(shcsr & SCB_SHCSR_BUSFAULTPENDED_Msk) >> SCB_SHCSR_BUSFAULTPENDED_Pos);
	kprintf("\t Mem Fault pended:%u\r\n",
						(shcsr & SCB_SHCSR_MEMFAULTPENDED_Msk) >> SCB_SHCSR_MEMFAULTPENDED_Pos);
	kprintf("\t Usage Fault pended:%u\r\n",
						(shcsr & SCB_SHCSR_USGFAULTPENDED_Msk) >> SCB_SHCSR_USGFAULTPENDED_Pos);
	
	kprintf("\r\n");
	
	kprintf("\t SysTick active:%u\r\n",
						(shcsr & SCB_SHCSR_SYSTICKACT_Msk) >> SCB_SHCSR_SYSTICKACT_Pos);
	kprintf("\t PendSV active:%u\r\n",
							(shcsr & SCB_SHCSR_PENDSVACT_Msk) >> SCB_SHCSR_PENDSVACT_Pos);
	kprintf("\t Monitor active:%u\r\n",
							(shcsr & SCB_SHCSR_MONITORACT_Msk) >> SCB_SHCSR_MONITORACT_Pos);
	kprintf("\t SVCall active:%u\r\n",
							(shcsr & SCB_SHCSR_SVCALLACT_Msk) >> SCB_SHCSR_SVCALLACT_Pos);
	kprintf("\t Usage Fault active:%u\r\n",
							(shcsr & SCB_SHCSR_USGFAULTACT_Msk) >> SCB_SHCSR_USGFAULTACT_Pos);
	kprintf("\t Bus Fault active:%u\r\n",
							(shcsr & SCB_SHCSR_BUSFAULTACT_Msk) >> SCB_SHCSR_BUSFAULTACT_Pos);
	kprintf("\t Mem Fault active:%u\r\n",
							(shcsr & SCB_SHCSR_MEMFAULTACT_Msk) >> SCB_SHCSR_MEMFAULTACT_Pos);
}

#define DIVBYZERO_Pos 9
#define DIVBYZERO_Msk (1ul << DIVBYZERO_Pos)
#define UNALIGNED_Pos 8
#define UNALIGNED_Msk (1ul << UNALIGNED_Pos)
#define NOCP_Pos 3
#define NOCP_Msk (1ul << NOCP_Pos)
#define INVPC_Pos 2
#define INVPC_Msk (1ul << INVPC_Pos)
#define INVSTATE_Pos 1
#define INVSTATE_Msk (1ul << INVSTATE_Pos)
#define UNDEFINSTR_Pos 0
#define UNDEFINSTR_Msk (1ul << UNDEFINSTR_Pos)
void dump_ufsr()
{
	uint16_t ufsr = (SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk) >> SCB_CFSR_USGFAULTSR_Pos;
	kprintf("UFSR:\r\n");
	kprintf("\t Divide by zero: %d\r\n", 
			(ufsr & DIVBYZERO_Msk) >> DIVBYZERO_Pos);
	kprintf("\t Unaligned access: %d\r\n", 
			(ufsr & UNALIGNED_Msk) >> UNALIGNED_Pos);
	kprintf("\t No coprocessor: %d\r\n", 
			(ufsr & NOCP_Msk) >> NOCP_Pos);
	kprintf("\t Invaild PC load: %d\r\n", 
			(ufsr & INVPC_Msk) >> INVPC_Pos);
	kprintf("\t Invaild state(attempt to entry ARM state): %d\r\n", 
			(ufsr & INVSTATE_Msk) >> INVSTATE_Pos);
	kprintf("\t Undefined instruction: %d\r\n", 
			(ufsr & UNDEFINSTR_Msk) >> UNDEFINSTR_Pos);
}
