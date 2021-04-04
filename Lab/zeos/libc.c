/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

#include <errno.h>

static char* EBADF_str = "Bad file descriptor";
static char* EACCES_str = "Permission denied";
static char* ENOSYS_str= "Function not implemented";
static char* EFAULT_str = "Bad address";
static char* EINVAL_str = "Invalid argument";

int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

static char* default_str = "Hey! If you want to know the error, implement missing error codes from errno.h (and its corresponding messages) to libc.c's perror function";
void perror(void){
	char* err_message;
	switch (errno) {
		case EBADF:  err_message = EBADF_str; break;
		case EACCES: err_message = EACCES_str; break;
		case ENOSYS: err_message = ENOSYS_str; break;
		case EFAULT: err_message = EFAULT_str; break;
		case EINVAL: err_message = EINVAL_str; break;
		default: err_message = default_str; break;
	}
	
	write(1, err_message, strlen(err_message));
}