/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>
#include <system.h>

// File permisions
#define LECTURA 0
#define ESCRIPTURA 1

// Number of consecutive bytes copied on write call.
#define SYS_WRITE_CHUNK 64

void * get_ebp();

extern int zeos_ticks;
int next_pid = 10;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF;
  if (permissions!=ESCRIPTURA) return -EACCES;
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS;
}

int sys_getpid()
{
	return current()->PID;
}

int ret_from_fork() {
    return 0;
}

int sys_fork()
{
    // a) create the child process
    if (list_empty(&freequeue)) 
        return -ENOMEM; // No free process left.
    struct list_head *e = list_first(&freequeue); 
    struct task_struct *ts_child = list_head_to_task_struct(e);
    union task_union *tu_child = (union task_union *) ts_child;
    list_del(e);

    // b) copy paren's task_union
//    struct task_struct *ts_paren = current();
//    union task_union *tu_paren  = (union task_union *) ts_paren;
    copy_data(current(), tu_child, sizeof(union task_union));

    // c) initialize dir_pages_baseAddr with a new directory
    allocate_DIR(ts_child);

    // d) Search physical pages (frames) for child
//    int frames_child[NUM_PAG_DATA];
    page_table_entry *pt_child = get_PT(ts_child);
    for (int i = 0; i < NUM_PAG_DATA; ++i) {
        int frame = alloc_frame();
        if (frame != -1)
            set_ss_pag(pt_child, PAG_LOG_INIT_DATA+i, frame);
        else {
            for (int j = 0; j < i; j++) {
                // No available frame. Free the previously allocated frames and return
                // memory error.
                free_frame(get_frame(pt_child, PAG_LOG_INIT_DATA+j));
                del_ss_pag(pt_child, PAG_LOG_INIT_DATA+j);
            }
            list_add_tail(e, &freequeue);
            return -EAGAIN;
        }
    }

    // e) Memory shenanigans
    page_table_entry *pt_paren = get_PT(current());
    // e.i.A) map child's system code and data to its parent's (shared)
    for (int i = 0; i < NUM_PAG_KERNEL; ++i)
        set_ss_pag(pt_child, i, get_frame(pt_paren, i));

    for (int i = 0; i < NUM_PAG_CODE; ++i)
        set_ss_pag(pt_child, PAG_LOG_INIT_CODE+i, get_frame(pt_paren, PAG_LOG_INIT_CODE+i));

    for (int i = NUM_PAG_KERNEL + NUM_PAG_CODE; i < NUM_PAG_KERNEL+NUM_PAG_CODE + NUM_PAG_DATA; i++) {
        set_ss_pag(pt_paren, i+NUM_PAG_DATA, get_frame(pt_child, i));
        copy_data((void*)(i<<12), (void*)((i+NUM_PAG_DATA)<<12), PAGE_SIZE);
        del_ss_pag(pt_paren, i+NUM_PAG_DATA);
    }

    set_cr3(get_DIR(current()));
    
    // f) assign PID to the process.
    //    PID = next_pid++;
    ts_child->PID = ++next_pid;
    ts_child->state = ST_READY;

    int ebp = (int)get_ebp();
    ebp = (ebp - (int)current()) + (int)(tu_child);

    ts_child->kernel_esp = ebp + sizeof(DWord);

    DWord temp_ebp =* (DWord*)ebp;

    ts_child->kernel_esp -= sizeof(DWord);
    *(DWord*)(ts_child->kernel_esp) = (DWord)&ret_from_fork;
    ts_child->kernel_esp -= sizeof(DWord);
    *(DWord*)(ts_child->kernel_esp) = temp_ebp;

    ts_child->state = ST_READY;
    list_add_tail(&(ts_child->list), &readyqueue);

    return ts_child->PID;

    // g, h) 
//    ts_child->quantum = 1000;
//    tu_child->stack[KERNEL_STACK_SIZE-18] = &ret_from_fork;
//    tu_child->stack[KERNEL_STACK_SIZE-19] = 0;
//    ts_child->kernel_esp = &tu_child->stack[KERNEL_STACK_SIZE-19];
//    list_add_tail(&(ts_child->list), &readyqueue);
    // j)
//    return PID;


}

void sys_exit()
{
    current()->PID=-1;
    page_table_entry *pt = get_PT(current());
    for (int i=0; i<NUM_PAG_DATA; i++) {
        free_frame(get_frame(pt, PAG_LOG_INIT_DATA+i));
        del_ss_pag(pt, PAG_LOG_INIT_DATA+i);
    }
    list_add_tail(&(current()->list), &freequeue);
    sched_next_rr();
}

int sys_write(int fd, char* buffer, int size) {
    int res = check_fd(fd, ESCRIPTURA);
    if (res < 0) return res;
    if (buffer == NULL) return -EFAULT;
    if (size < 0) return -EINVAL;
    char dest[SYS_WRITE_CHUNK];
    int written = 0;
    while (written < size) {
        int chunk_s = min(size - written, SYS_WRITE_CHUNK);
        copy_from_user(buffer + written, dest, chunk_s);
        sys_write_console(dest, chunk_s);
        written += chunk_s;
    }
    return written;
}

int sys_gettime() {
    return zeos_ticks;
}

int sys_get_stats(int pid, struct stats *st) {
    if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT;
    for (int i = 0; i < NR_TASKS; ++i) {
        struct task_struct *ts = &task[i].task;
        if (ts->PID == pid) {
            ts->stats.remaining_ticks = currentQuantum;
            return copy_to_user(&ts->stats, st, sizeof(struct stats));
        }
    }
    return -ESRCH;
}
