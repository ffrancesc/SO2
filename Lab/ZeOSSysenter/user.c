#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
 main(void)
{
	/* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
	/* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	int fd = create_screen();
	set_focus(fd);
	write(fd, "prueba", sizeof("prueba"));

	int fd2 = create_screen();
	write(fd2, "prueba1", sizeof("prueba1"));

	int i = fork();
	if (i == 0) {
		write(fd, "HIJO", sizeof("HIJO"));
		int fd3 = create_screen();
		write(fd3, "HIJOHIJOHIJO", sizeof("HIJOHIJOHIJO"));
	} else {
		write(fd2, "PADRE", sizeof("PADRE"));
	}
  	while(1) { }
}


