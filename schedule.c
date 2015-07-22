#include "schedule.h"
#include "assert.h"
#include <stdlib.h>
#include <string.h>
#include <stm32f10x.h>
#include <core_cm3.h>

/*************** proc_list ***************/

proc_list ready;
proc_list inactive;
proc_list pending;

static void cb_min_pid(proc_node* p, void* data)
{
	pid_t* min = data;
	if (p->proc.pid < *min)
		*min = p->proc.pid;
}

static pid_t min_pid()
{
	pid_t min = INT32_MAX;
	iter_proc_list(&ready, cb_min_pid, &min);
	iter_proc_list(&inactive, cb_min_pid, &min);
	iter_proc_list(&pending, cb_min_pid, &min);
	return min;
}

static void init_process(process* _this)
{
	static pid_t pid = 0;
	z_assert(_this != NULL);
	
	if (++pid == INT32_MAX)
	{
		if (min_pid() != 0)
			pid = 0;
	}
	
	_this->pid = pid;
	_this->state = PROC_READY;
	_this->timeslice = TIME_SLICE;
}

proc_node* create_proc_node()
{
	proc_node* _this = malloc(sizeof(proc_node));
	z_assert(_this != NULL);
	
	memset(_this, 0, sizeof(proc_node));
	init_process(&_this->proc);
	return _this;
}

void free_proc_node(proc_node* _this)
{
	free(_this);
}

void init_proc_list(proc_list* _this)
{
	z_assert(_this != NULL);
	
	_this->head.next = NULL;
	_this->tail = &_this->head;
}

proc_node* first_proc_list(proc_list* _this)
{
	z_assert(_this != NULL);
	
	return _this->head.next;
}

/*
 * 在链表尾部添加
 */
void enqueue_proc_list(proc_list* _this, proc_node* node)
{
	z_assert(_this != NULL);
	z_assert(node != NULL);
	
	node->next = NULL;
	_this->tail->next = node;
	_this->tail = _this->tail->next;
}

/*
 * 仅从链表头部移除, 不释放内存
 */
proc_node* dequeue_proc_list(proc_list* _this)
{
	z_assert(_this != NULL);
	
	proc_node* p = first_proc_list(_this);
	if (p == NULL)
		return NULL;
	
	_this->head.next = p->next;
	if (_this->head.next == NULL)
		_this->tail = &_this->head;
		
	p->next = NULL;
	
	return p;
}

void iter_proc_list(proc_list* _this, void (*f)(proc_node*, void*), void* data)
{
	z_assert(_this != NULL);
	z_assert(f != NULL);
	
	proc_node* p = first_proc_list(_this);
	while (p != NULL)
	{
		f(p, data);
		p = p->next;
	}
}

#define SWAP_PROC_NODE(a, b) do {	\
	proc_node* t = a;				\
	a = b;							\
	b = t;							\
}while (0)

void swap_proc_list(proc_list* a, proc_list* b)
{
	z_assert(a != NULL);
	z_assert(b != NULL);
	
	SWAP_PROC_NODE(a->head.next, b->head.next);
	SWAP_PROC_NODE(a->tail, b->tail);
	if (a->tail == &b->head)
		a->tail = &a->head;
	if (b->tail == &a->head)
		b->tail = &b->head;
}

/*************** schedule ***************/
proc_node* current;
proc_node* prev_proc;

inline static void pend_sv_set()
{
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

inline static void pend_sv_clr()
{
	SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
}

static void proc_set_ready(proc_node* p, void* dummy)
{
	p->proc.state = PROC_READY;
	p->proc.timeslice = TIME_SLICE;
}

void schedule()
{
	/* 1.把current放入合适位置 */
	if (current != NULL)
	{
		switch (current->proc.state)
		{
		case PROC_INACTIVE:
			z_assert(current == dequeue_proc_list(&ready));
			enqueue_proc_list(&inactive, current);
			break;
			
		case PROC_PENDING:
			z_assert(current == dequeue_proc_list(&ready));
			enqueue_proc_list(&pending, current);
			break;
			
		case PROC_READY:
			return;
			break;
			
		default:
			/* kernel panic */
			z_failed();
			break;
		}
	}
	
	/* 2.prev = current */
	prev_proc = current;
	
	/* 3.选出新进程 */ 
	if (first_proc_list(&ready) == NULL)
	{
		if (first_proc_list(&inactive) == NULL)
		{
			current = NULL;	/* current = NULL意味着会执行IDLE进程(WFI) */
			goto pend_sv;
		}
		else
		{
			swap_proc_list(&ready, &inactive);
			/* 还原时间片 */
			iter_proc_list(&ready, proc_set_ready, NULL);
		}
	}
	/* 现在, ready.first不为NULL了 */
	current = first_proc_list(&ready);
	
pend_sv:
	pend_sv_set();
	return;
}

/* 每毫秒调用一次 */
void SysTick_Handler()
{
	if (current != NULL)
	{
		if (--current->proc.timeslice <= 0)
		{
			current->proc.state = PROC_INACTIVE;
			schedule();
		}
	}
}


/*************** high level ***************/

inline static void push_stack(uint32_t** stack, uint32_t data)
{
	*(--*stack) = data;
}

inline static uint32_t pop_stack(uint32_t** stack)
{
	return *(*stack)++;
}

void create_process(uint32_t* stack, void* program)
{
	proc_node* proc = create_proc_node();
	process* p = &proc->proc;
	uint32_t pc = ((uint32_t)program) & 0xfffffffe;
	
	/* 压入中断现场 */
	push_stack(&stack, 0x01000000);			/* xPSR */
	push_stack(&stack, pc);					/* pc */
	push_stack(&stack, 0xffffffff);			/* lr */
	push_stack(&stack, 0);					/* r12 */
	push_stack(&stack, 0);					/* r3 */
	push_stack(&stack, 0);					/* r2 */
	push_stack(&stack, 0);					/* r1 */
	push_stack(&stack, 0);					/* r0 */
	p->psp = stack;
	
	enqueue_proc_list(&ready, proc);
}

/*
 * 杀死当前进程
 */
void kill_process()
{
	
}
