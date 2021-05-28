/*
 * io.c - 
 */

#include <io.h>
#include <list.h>
#include <types.h>

/**************/
/** Screen  ***/
/**************/

Byte x, y=19;
backgroundColor = 0x0; 
textColor = 0x2; 

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
  Byte v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
}

void printc(char c)
{
     __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c));
  if (c=='\n')
  {
    x = 0;
    y=(y+1)%NUM_ROWS;
  }
  else
  {
    Word ch = (Word) ((c & 0x00FF) | (backgroundColor << 12) | (textColor << 8));
    DWord screen = 0xb8000 + (y * NUM_COLUMNS + x) * 2;
    if (++x >= NUM_COLUMNS)
    {
      x = 0;
      y=(y+1)%NUM_ROWS;
    }
    asm("movw %0, (%1)" : : "g"(ch), "g"(screen));
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

void setBackgroundColor(int c) 
{ 
  backgroundColor = c; 
  refresh();
} 
 
void setTextColor(int c) 
{ 
  textColor = c;
  refresh();
}

void set_cursor(Byte cx, Byte cy)
{
  x = cx;
  y = cy;
}