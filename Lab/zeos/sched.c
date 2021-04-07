/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

// Constants for process transitions (useful for stats)
#define READY2RUN 1
#define RUN2READY 2
#define USER2SYSTEM 3
#define SYSTEM2USER 4

void update_stats(struct task_struct *ts, int transition);
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
    set_quantum(ts,1000);
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
    set_quantum(ts,1000);
    currentQuantum = 1000;
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

void update_sched_data_rr()
{
    --currentQuantum;
}

int needs_sched_rr()
{
    if (currentQuantum == 0 && list_empty(&readyqueue) == 0) return 1;
    return 0;
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest)
{
    list_add(&(t->list), dest);
}

void sched_next_rr()
{
    struct list_head *e = list_first(&readyqueue);
    struct task_struct *ts = list_head_to_task_struct(e);
    list_del(e);
    union task_union *tu = (union task_union *) ts;
    currentQuantum = get_quantum(ts);

    update_stats(ts, READY2RUN); 
    task_switch(tu);
}

void schedule()
{
    update_sched_data_rr();
    if (needs_sched_rr()) {
        if (current()->PID != 0) {
            update_stats(current(), RUN2READY);
            update_process_state_rr(current(), &readyqueue);
        }
        sched_next_rr();
    }
    else if (currentQuantum == 0 && current()->PID != 0) { // context switch to idle process
        update_process_state_rr(current(), &freequeue);
        currentQuantum = get_quantum(idle_task);
        task_switch((union task_union *)idle_task);
    }
}

int get_quantum(struct task_struct *t)
{
    return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum)
{
    t->quantum = new_quantum;
}

void enter_system() {
    update_stats(current(), USER2SYSTEM);
}

void leave_system() {
    update_stats(current(), SYSTEM2USER);
}

void update_stats(struct task_struct *ts, int transition) {
    struct stats *st = &ts->stats;
    int current_ticks = get_ticks();
    int delta = current_ticks - st->elapsed_total_ticks;
    st->elapsed_total_ticks = current_ticks;
    switch (transition) {
        case READY2RUN:
            st->ready_ticks += delta;
            st->total_trans++;
            break;
        case SYSTEM2USER:
        case RUN2READY:
            st->system_ticks += delta;
            break;
        case USER2SYSTEM:
            st->user_ticks += delta;
            break;
    }
}

