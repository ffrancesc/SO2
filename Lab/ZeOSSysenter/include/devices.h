#ifndef DEVICES_H__
#define  DEVICES_H__

#include <io.h>

#define NR_SCREENS_PER_PROCESS 10

struct screen_struct {
  char buffer[NUM_COLUMNS*NUM_ROWS];
  Byte x; /* posición x del próximo carácter a escribir */
  Byte y; /* posición y del próximo carácter a escribir */
};

struct screen_struct *screen_focus; /* pantalla que tiene el foco */

void switchScreen();
void moveCursor(char dir); 
void deleteChar();
void refresh();

int sys_write_console(char *buffer,int size);


#endif /* DEVICES_H__*/
