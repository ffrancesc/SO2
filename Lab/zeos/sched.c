/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

void asm_inner_task_switch(union task_union * t);

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
    struct list_head *e = list_first(&freequeue);   // first element
    struct task_struct *ts = list_head_to_task_struct(e);   // get task_struct
    union task_union *tu = (union task_union *) ts;

    list_del(e);
    ts->PID = 0;
    allocate_DIR(ts);
    tu->stack[1023] = (int)cpu_idle;
    tu->stack[1022] = 0;
    ts->kernel_esp = (int)&(tu->stack[1022]);
    idle_task = ts;
}

void init_task1(void)
{
    struct list_head *e = list_first(&freequeue);
    struct task_struct *ts = list_head_to_task_struct(e);
    union task_union *tu = (union task_union *) ts;

    list_del(e);
    ts->PID = 1;
    allocate_DIR(ts);
    set_user_pages(ts);
    tss.esp0 = (int)&(tu->stack[1023]);
    set_cr3(ts->dir_pages_baseAddr);
}

void init_sched()
{
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

void inner_task_switch(union task_union*t)
{
    tss.esp0 = (int)&(t->stack[1023]);
    set_cr3(t->task.dir_pages_baseAddr);
    asm_inner_task_switch(t);
}