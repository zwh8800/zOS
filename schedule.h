#ifndef Z_SCHEDULE_H
#define Z_SCHEDULE_H
#include <stdint.h>

/*************** macros ***************/

#define TIME_SLICE 1000		/* 时间片为1000ms */

/*************** structures ***************/

typedef enum _proc_state
{
	PROC_READY,
	PROC_INACTIVE,
	PROC_PENDING,
} proc_state;

/*
 * 不直接使用process结构, 使用proc_node
 */
typedef int32_t pid_t;
typedef struct _process
{
	uint32_t* psp;			/* program stack pointer */
	uint32_t regs[8]; 		/* r4 - r11 */
	
	pid_t pid;
	proc_state state;
	int timeslice;
} process;

typedef struct _proc_node
{
	process proc;
	struct _proc_node* next;
} proc_node;

typedef struct _proc_list
{
	proc_node head;
	proc_node* tail;
} proc_list;


/*************** functions ***************/

proc_node* create_proc_node();
void free_proc_node(proc_node* _this);

void init_proc_list(proc_list* _this);
proc_node* first_proc_list(proc_list* _this);
void enqueue_proc_list(proc_list* _this, proc_node* node);
proc_node* dequeue_proc_list(proc_list* _this);
void iter_proc_list(proc_list* _this, void (*f)(proc_node*, void*), void* data);
void swap_proc_list(proc_list* a, proc_list* b);

void schedule();

void create_process(uint32_t* stack, void* program);
void kill_process();

/*************** global variables ***************/
extern proc_list ready;
extern proc_list inactive;
extern proc_list pending;

extern proc_node* current;
extern proc_node* prev_proc;

#endif
