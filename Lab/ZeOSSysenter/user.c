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

int test_1() {
	int fd = create_screen();
	return write(fd, "Hola!", sizeof("Hola!"));
}

int test_2() {
	int fd = create_screen();
	return write(fd + 1, "Hola!", sizeof("Hola!"));
}

int test_3() {
	int fd0 = create_screen();
	int fd1 = create_screen();
	write(fd1, "Hola!", sizeof("Hola!"));
	return set_focus(fd1);
}

int test_4() {
	int fd = create_screen();
	write(fd, "Hola!", sizeof("Hola!"));
	return set_focus(fd + 1);
}

int test_5() {
	int fd0 = create_screen();
	int fd1 = create_screen();
	write(fd1, "Hola!", sizeof("Hola!"));
	// TEST MANUAL, esperamos a que el usuario pulse 'CTRL+TAB'
	while(1);
	return 0;
}

int test_6() {
	int fd = create_screen();
	write(fd, "Hola!", sizeof("Hola!"));
	return close(fd);
}

int test_7() {
	int fd = create_screen();
	return close(fd+1);
}

int test_8() {
	int fd = create_screen();
	write(fd, "Hola ", sizeof("Hola "));
	int pid = fork();
	if (pid == 0) {
		write(fd, "mundo!", sizeof("mundo!"));
	}
	return 0;
}

int test_9() {
	int fd = create_screen();
	write(fd, "Hola ", sizeof("Hola "));
	int pid = fork();
	if (pid == 0) {
		close(fd);
	}
}

int test_10() {
	int fd = create_screen();
	write(fd, "Hola!", sizeof("Hola!"));
	int pid1 = fork();
	if (pid1 == 0) {
		int fd1 = create_screen();
		write(fd1, "!aloh", sizeof("!aloh"));
	} else {
		int pid2 = fork();
		if (pid2 == 0) {
			int fd2 = create_screen();
			write(fd2, "!aloh", sizeof("!aloh"));
		}
	}
	while(1);
	return 0;
}

