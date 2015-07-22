#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>


#include "kdebug.h"
#include "init_device.h"
#include "zImage.h"
#include "schedule.h"
#include "system_exception.h"

#include <stdlib.h>

#define BitP(addr, n) (*(volatile unsigned int *)(0x42000000 + (((unsigned)addr - 0x40000000) * 8 + n) * 4))
#define BitM(addr, n) (*(volatile unsigned int *)(0x22000000 + (((unsigned)addr - 0x20000000) * 8 + n) * 4))

/* Unit Test */
void test_proc_list();
void test_pend_sv();
void dump_scb();
void test_schedule();
void test_newlib();

int init(void)
{
	void* zImage_start;
	zImage* imgs;
	int i, n;

	init_device();

	/* 设置SysTick中断 */
	SysTick_Config(72 * 1000);

	zImage_start = get_zimage_start();

	kprintf("flash_end=0x%.8p\r\n", zImage_start);

	imgs = malloc(5 * sizeof(zImage));
	n = find_zImage(zImage_start, imgs, 5 * sizeof(zImage));
	kprintf("%d zImage(s) on flash\r\n", n);
	for (i = 0; i < n; ++i)
	{
		kprintf("zImage #%d\r\n", i);
		kprintf("\tlocation: %.8p\r\n", imgs[i].location);
		kprintf("\tname: %s\r\n", imgs[i].header.name);
		kprintf("\tsize: 0x%.4x\r\n", imgs[i].header.size);
	}

	init_system_exception();
	dump_scb();
	
	/* 初始化三个进程列表 */
	kprintf("初始化进程列表\r\n");
	init_proc_list(&ready);
	init_proc_list(&inactive);
	init_proc_list(&pending);
	
	//test_proc_list();
	//test_pend_sv();
	//test_newlib();
	test_schedule();
	
	/*zImage_start = (char*)zImage_start + sizeof(zImage_header);
	BitM(&zImage_start, 0) = 1;
	__asm__(
		"blx %[addr]"
		:
		: [addr]"r"(zImage_start)
		:
	);*/

    while(1)
    	__WFI();
}
