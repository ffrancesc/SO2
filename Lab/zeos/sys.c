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

#define LECTURA 0
#define ESCRIPTURA 1

extern int zeos_ticks;
int ret_from_fork(void);
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

int sys_fork()
{
    int PID=-1;
    
    // a) create the child process
    if (list_empty(&freequeue)) return -ENOMEM; // No free process left.
    struct list_head *e = list_first(&freequeue); 
    struct task_struct *ts_child = list_head_to_task_struct(e);
    union task_union *tu_child = (union task_union *) ts_child;
    list_del(e);

    // b) copy parent's 
    struct task_struct *ts_parent = current();
    union task_union *tu_parent  = (union task_union *) ts_parent;
    copy_data(ts_parent, ts_child, sizeof(union task_union));

    // c) initialize dir_pages_baseAddr with a new directory
    allocate_DIR(ts_child);

    // TODO: page shenanigans
    // d)
    // e)


    // f) assign PID to the process.
    PID = next_pid++;
    ts_child->PID = PID;

    // g) 
    set_quantum(ts_child, 1000);

    // h)
    tu_child->stack[1023] = ret_from_fork;
    tu_child->stack[1022] = 0;

    // i)
    list_add_tail(&ts_child->list, &readyqueue);

    // j)
    return PID;
}

void sys_exit()
{
    free_user_pages(current());
    if (list_empty(&readyqueue) == 1) {
        currentQuantum = get_quantum(idle_task);
        task_switch((union task_union *)idle_task);
    }
    else sched_next_rr();
}

#define CHUNK_SIZE 64
int sys_write(int fd, char* buffer, int size) {
    int res = check_fd(fd, ESCRIPTURA);
    if (res < 0) return res;
    if (buffer == NULL) return -EFAULT;
    if (size < 0) return -EINVAL;
    char dest[CHUNK_SIZE];
    int written = 0;
    while (written < size) {
        int chunk_s = min(size - written, CHUNK_SIZE);
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
