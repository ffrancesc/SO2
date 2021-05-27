#include <io.h>
#include <utils.h>
#include <list.h>
#include <devices.h>
#include <sched.h>

// Queue for blocked processes in I/O 
struct list_head blocked;

int sys_write_console(char *buffer,int size)
{
  int i;
  
  for (i=0; i<size; i++)
    printc(buffer[i]);
  
  return size;
}

void switchScreen() 
{ 
  struct task_struct* curr = current();
  // find current screen in vector
  int screen_i = -1, i;
  for (i = 0; i < NR_SCREENS_PER_PROCESS; ++i) {
    if (curr->p_screens[i] == screen_focus) {
      screen_i = i;
      break;
    }
  }
  // find next used screen in the vector (ciclicly)
  i = screen_i;
  do {
    i = (i+1) % NR_SCREENS_PER_PROCESS;
  } while (!curr->used_screens[i]);
  sys_set_focus(i);
} 
 
void moveCursor(char dir) 
{
  if (dir == 'U' && screen_focus->y > 0) --screen_focus->y; 
  else if (dir == 'R' && screen_focus->x < NUM_COLUMNS) ++screen_focus->x; 
  else if (dir == 'L' && screen_focus->x > 0) --screen_focus->x;  
  else if (screen_focus->y < NUM_ROWS) ++screen_focus->y; 
} 
 
void deleteChar() 
{ 
  // TOOD
}


// Updates console with the content of the focused screen
void refresh() {
  set_cursor(0,0);
  sys_write_console(screen_focus->buffer, NUM_ROWS*NUM_COLUMNS);
  set_cursor(screen_focus->x, screen_focus->y);
}
