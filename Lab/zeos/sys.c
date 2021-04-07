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

  // creates the child process
  
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
