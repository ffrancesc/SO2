/*
 * io.c - 
 */

#include <io.h>

#include <types.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

Byte x, y=19;

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
  Byte v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
}

void printc(char c)
{
     __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
  Word *screen = (Word *)0xb8000;
  if (c == '\n')
  {
    x = 0;
    y++;
  }
  else
  {
    Word ch = (Word) (c & 0x00FF) | 0x0200;
    screen[(y * NUM_COLUMNS + x)] = ch;
    if (++x >= NUM_COLUMNS)
    {
      x = 0;
      y++;
    }
  }

  // Handle scroll
  if (y >= NUM_ROWS)
  {
    y = NUM_ROWS - 1;
    int i, j;
    for (i = 0; i < NUM_COLUMNS; ++i)
      for (j = 0; j < NUM_ROWS - 1; ++j)
        screen[j * NUM_COLUMNS + i] = screen[(j+1) * NUM_COLUMNS + i];
  }
}

void printc_xy(Byte mx, Byte my, char c)
{
  Byte cx, cy;
  cx=x;
  cy=y;
  x=mx;
  y=my;
  printc(c);
  x=cx;
  y=cy;
}

void printk(char *string)
{
  int i;
  for (i = 0; string[i]; i++)
    printc(string[i]);
}
