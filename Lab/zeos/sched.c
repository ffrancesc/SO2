/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

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
    ts->PID = 1;
    allocate_DIR(ts);
    tu->stack[1023] = (int)cpu_idle;
    tu->stack[1022] = 0;
    int aux = &(tu->stack[1022]);
    ts->kernel_esp = aux;
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

void task_switch(union task_union*t)
{
//    __asm__ __volatile__(
//        "pushl %esi\n\t"
//        "pushl %edi\n\t"
//        "pushl %ebx"
//    );
//
//    inner_task_switch(t);
//
//    __asm__ __volatile__(
//        "popl %ebx\n\t"
//        "popl %edi\n\t"
//        "popl %esi"
//    );
}

void inner_task_switch(union task_union*t)
{
//    tss.esp0 = &(t->stack[1023]);
//    set_cr3(t->task.dir_pages_baseAddr);
//    __asm__ __volatile__(
//        // current()->kernel_esp = %ebp;
//        "movl %esp , %ebx\n\t"
//        "and $0xfffff000, %ebx\n\t"    // & current task_struct
//        "movl %ebp, 4(%ebx)\n\t"
//
//        // %esp = t->task.kernel_esp;
//        "movl 8(%ebp), %ebx\n\t" // & t task_struct
//        "movl 4(%ebx), %esp"
//    );
}