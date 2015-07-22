#include "system_exception.h"
#include "kdebug.h"
#include "stm32f10x.h"
#include "core_cm3.h"

void init_system_exception()
{
	/* 使能Fault异常 */
	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
	
	/* 设置系统异常优先级 */
	SCB->SHP[10] = 0xf0;
	SCB->SHP[11] = 0;
}

void NMI_Handler()
{
	kprintf("!---------------NMI---------------!\r\n");
	dump_scb();
	while(1);
}

void HardFault_Handler()
{
	kprintf("!---------------HARD FAULT---------------!\r\n");
	dump_scb();
	while(1);
}

void MemManage_Handler()
{
	kprintf("!---------------MEM FAULT---------------!\r\n");
	dump_scb();
	while(1);
}

void BusFault_Handler()
{
	kprintf("!---------------BUS FAULT---------------!\r\n");
	dump_scb();
	while(1);
}

void UsageFault_Handler()
{
	kprintf("!---------------USAGE FAULT---------------!\r\n");
	dump_ufsr();
	
	dump_scb();
	while(1);
}
