/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/** Screen functions **/
/**********************/

Byte inb (unsigned short port);
void printc(char c);
void printc_xy(Byte x, Byte y, char c);
void printk(char *string);
void set_style(int foreground, int background, int blink);

/** Color constants **/
/*********************/
#define C_DARK_BLUE 0
#define C_GREEN 1
#define C_BLUE 2
#define C_DARK_RED 3
#define C_PINK 4
#define C_YELLOW 5
#define C_GREY 6
#define C_BLACK 7
#define C_VIOLET 8
#define C_LIGHT_GREEN 9
#define C_CYAN 10
#define C_RED 11

#endif  /* __IO_H__ */
