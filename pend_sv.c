#if 0
#include <stm32f10x.h>
#include <core_cm3.h>
#include <stdint.h>

#include "kdebug.h"


void pend_sv_set()
{
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void pend_sv_clr()
{
	SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
}

void test_pend_sv()
{
	kprintf("--------------------------\r\n");
	dump_scb();
	
	__asm__ volatile("svc #0");
}


void SVC_Handler()
{
	kprintf("--------------------------\r\n");
	dump_scb();
	
	kprintf("SVCall\r\n");
	pend_sv_set();
	
	dump_scb();
	/* pend_sv_clr(); */
}

void PendSV_Handler()
{
	kprintf("--------------------------\r\n");
	dump_scb();
	
	kprintf("PendSv\r\n");
}

#endif 
