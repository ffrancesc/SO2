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
  
  for (i=0; i<size; i++) {
    char aux = buffer[i];
    printc(aux);
  }
  
  return size;
}

void switchScreen() 
{ 
  ++focus_screen_id;
  if (focus_screen_id >= last_screen_id) focus_screen_id = 0;
  while (focus_screen_id < last_screen_id && all_screens[focus_screen_id].active != 1) {
    ++focus_screen_id;
  }

  if (focus_screen_id >= last_screen_id) return 0; // all screens are inactive

  screen_focus = &all_screens[focus_screen_id];
  refresh();
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
  int x = screen_focus->x;
  int y = screen_focus->y;
  if (x == 0 && y != 1) {
    x = NUM_COLUMNS-1;
    --y;
  }
  else if (x == 0 && y == 1) return;
  else --x;
  screen_focus->buffer[y * NUM_COLUMNS + x] = ' ';
  screen_focus->x = x;
  screen_focus->y = y;
  set_cursor(x,y);
  printc(' ');
}

// Updates console with the content of the focused screen
void refresh() {
  set_cursor(0,0);
  sys_write_console(screen_focus->buffer, NUM_ROWS*NUM_COLUMNS);
  set_cursor(screen_focus->x, screen_focus->y);
}

void printScreen(char c) {
  int x = screen_focus->x;
  int y = screen_focus->y;
  screen_focus->buffer[y * NUM_COLUMNS + x] = c;
  if (screen_focus->x + 1 >= NUM_COLUMNS) {
    screen_focus->x = 0;
    ++screen_focus->y;
  }
  else ++screen_focus->x;
  set_cursor(x,y);
  printc(c);
  set_cursor(screen_focus->x,screen_focus->y);
}
