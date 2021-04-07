#include <libc.h>
#include <types.h>
#include <interrupt.h>

char buff[24];

int pid,l;
int gettime(void);

int add(int par1, int par2) {
  return par1 + par2;
}

int addAsm(int, int);

static char* greet = "User here saying hello!\n";

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  //~ int result = add(0x42, 0x666);
  //~ int res = addAsm(32, 32);
  write(1, greet, strlen(greet));
  
  int pid = fork();
  while(1) {}
}
