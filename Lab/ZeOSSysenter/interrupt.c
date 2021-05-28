/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <devices.h>

#include <sched.h>

#include <zeos_interrupt.h>

Gate idt[IDT_ENTRIES];
Register    idtR;

char char_map[] = 
{ 
  '\0','\0','1','2','3','4','5','6', 
  '7','8','9','0','\'','¡','B','T', 
  'q','w','e','r','t','y','u','i', 
  'o','p','`','+','\0','\0','a','s', 
  'd','f','g','h','j','k','l','ñ', 
  '\0','º','S','ç','z','x','c','v', 
  'b','n','m',',','.','-','\0','*', 
  '\0','\0','\0','\0','\0','\0','\0','\0', 
  '\0','\0','\0','\0','\0','\0','\0','7', 
  'U','9','-','L','5','R','+','1', 
  'D','3','0','\0','\0','\0','<','\0', 
  '\0','\0','\0','\0','\0','\0','\0','\0', 
  '\0','\0' 
}; 

int zeos_ticks = 0;

int space_rec = 0;
int tab = 0;
int shift = 0;

void clock_routine()
{
  zeos_show_clock();
  zeos_ticks ++;
  
  schedule();
}

void keyboard_routine()
{
  unsigned char c = inb(0x60);
  
  char input = char_map[c&0x7f];

  // if (!(c&0x80)) return;
  if (input == '\000' && (c&0x80)) {
    ++space_rec;
    return;
  }
 
  if (!(c&0x80) && input == 'S') {
    shift = 1;
    return;
  }
  else if ((c&0x80) && input == 'S') {
    shift = 0;
    return;
  }

  if (!(c&0x80) && input == 'T') 
    tab = 1;
  else if ((c&0x80) && input == 'T') {
    tab = 0;
    return;
  }

  if (shift == 1) {
    if (tab == 1)
      switchScreen();
    else if (input == '1') 
      setBackgroundColor(0x0); 
    else if (input == '2') 
      setBackgroundColor(0x1); 
    else if (input == '3') 
      setBackgroundColor(0x2); 
    else if (input == '4') 
      setBackgroundColor(0x3); 
    else if (input == '5') 
      setBackgroundColor(0x4); 
  }

  else if (tab == 1) {
    if (input == '1') 
      setTextColor(0x1); 
    else if (input == '2') 
      setTextColor(0x2); 
    else if (input == '3') 
      setTextColor(0x3); 
    else if (input == '4') 
      setTextColor(0x4); 
    else if (input == '5') 
      setTextColor(0x5); 
  }

  else if ((input == 'U' || input == 'L' || input == 'D' || input == 'R') && (c&0x80)) {
    space_rec = 0;
    moveCursor(input);
  }
 
  else if (input == 'B' && (c&0x80)) {
    space_rec = 0;
    deleteChar();
  }
 
  else if (c&0x80) {
    for (int i = 0; i < space_rec; ++i) printScreen('\000');
    space_rec = 0;
    printScreen(input);
  }
}

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void clock_handler();
void keyboard_handler();
void system_call_handler();

void setMSR(unsigned long msr_number, unsigned long high, unsigned long low);

void setSysenter()
{
  setMSR(0x174, 0, __KERNEL_CS);
  setMSR(0x175, 0, INITIAL_ESP);
  setMSR(0x176, 0, (unsigned long)system_call_handler);
}

void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();

  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
  setInterruptHandler(32, clock_handler, 0);
  setInterruptHandler(33, keyboard_handler, 0);

  setSysenter();

  set_idt_reg(&idtR);
}

