#include "kdebug.h"
#include "schedule.h"
#include "assert.h"

#include <stddef.h>

static void print_pid(proc_node* p, void*dummy)
{
	kprintf("\tpid = %d\r\n", p->proc.pid);
}

void test_proc_list()
{
	proc_node *p, *q, *r;
	proc_node *cur, *prev;
	kprintf("test_proc_list 进入\r\n");
	
	/* 初始化三个进程列表 */
	kprintf("初始化进程列表\r\n");
	init_proc_list(&ready);
	init_proc_list(&inactive);
	init_proc_list(&pending);
	
	kprintf("测试是否为空\r\n");
	if (first_proc_list(&ready) == NULL)
		kprintf("first(ready) = NULL\r\n");
	if (first_proc_list(&inactive) == NULL)
		kprintf("first(inactive) = NULL\r\n");
	if (first_proc_list(&pending) == NULL)
		kprintf("first(pending) = NULL\r\n");
	
	/* 创建三个进程 */
	p = create_proc_node();
	q = create_proc_node();
	r = create_proc_node();
	/* 打印p,q,r的pid */
	kprintf("p:\r\n");
	print_pid(p, NULL);
	kprintf("q:\r\n");
	print_pid(q, NULL);
	kprintf("r:\r\n");
	print_pid(r, NULL);
	
	/* 全部放入ready列表 */
	enqueue_proc_list(&ready, p);
	enqueue_proc_list(&ready, q);
	enqueue_proc_list(&ready, r);
	p = q = r = NULL; /* 保持一处有引用 */
	
	/* 打印first(ready)的pid */
	kprintf("first(ready).pid = %d\r\n", first_proc_list(&ready)->proc.pid);
	
	/* 打印ready所有的pid */
	kprintf("ready:\r\n");
	iter_proc_list(&ready, print_pid, NULL);
	
	kprintf("模拟SysTick和schedule\r\n");
	/* 模拟SysTick */
	cur = first_proc_list(&ready);
	cur->proc.state = PROC_INACTIVE;
	
	/* 模拟schedule */
	/* 1.把current放入合适的位置 */
	if (cur->proc.state == PROC_INACTIVE)
	{
		z_assert(cur == dequeue_proc_list(&ready));
		enqueue_proc_list(&inactive, cur);
		
		/* 2.prev = current */
		prev = cur;
		
		/* 3.current = ready.first() 简略版 */
		cur = first_proc_list(&ready);
	}
	/* 打印ready所有的pid */
	kprintf("ready:\r\n");
	iter_proc_list(&ready, print_pid, NULL);
	/* 打印inactive所有的pid */
	kprintf("inactive:\r\n");
	iter_proc_list(&inactive, print_pid, NULL);
	
	/* 打印当前运行进程 */
	kprintf("current: pid = %d\r\n", cur->proc.pid);
	/* 打印上一个运行进程 */
	kprintf("prev: pid = %d\r\n", prev->proc.pid);
	
	/* 模拟删除进程, 只有正在运行的进程可以关闭 */
	kprintf("模拟删除进程\r\n");
	/* 两步: 1.从队列中移除, 2.从内存中删除 */
	z_assert(cur == dequeue_proc_list(&ready));
	free_proc_node(cur);
	
	/* 打印ready所有的pid */
	kprintf("ready:\r\n");
	iter_proc_list(&ready, print_pid, NULL);
	/* 打印inactive所有的pid */
	kprintf("inactive:\r\n");
	iter_proc_list(&inactive, print_pid, NULL);
	
	kprintf("交换ready和inactive\r\n");
	swap_proc_list(&ready, &inactive);
	/* 打印ready所有的pid */
	kprintf("ready:\r\n");
	iter_proc_list(&ready, print_pid, NULL);
	/* 打印inactive所有的pid */
	kprintf("inactive:\r\n");
	iter_proc_list(&inactive, print_pid, NULL);
	
	kprintf("test_proc_list 完成\r\n");
}
