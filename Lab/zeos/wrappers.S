#include <asm.h>
#include <segment.h>

ENTRY(write)
	push %ebp            
	movl %esp, %ebp
	push %ebx
	movl 8(%ebp), %ebx   // Parameter passing
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx 
	movl $4, %eax        // Identifier for the system call (4 for write)
	int $0x80            // Generate the trap
	cmpl $0, %eax        // Check for errors
	jge returnwrite              
	negl %eax
	leal errno, %ebx     // Load error to variable errno
	movl %eax, (%ebx)
	movl $-1, %eax
returnwrite:             // No errors
	pop %ebx
	pop %ebp
	ret

ENTRY(gettime)
	push %ebp
	movl %esp, %ebp
	push %ebx
	movl $10, %eax
	int $0x80
	cmpl $0, %eax
	jg returngettime
	negl %eax
	leal errno, %ebx   
	movl %eax, (%ebx)
	movl $-1, %eax
returngettime:
	pop %ebx
	pop %ebp
	ret

ENTRY(getpid)
    push %ebp
    movl %esp, %ebp
    push %ebx
	movl $20, %eax
	int $0x80
	cmpl $0, %eax
	jg returnpid
	negl %eax
	leal errno, %ebx
	movl %eax, (%ebx)
	movl $-1, %eax
returnpid:
	pop %ebx
	pop %ebp
	ret

ENTRY(fork)
	push %ebp
	movl %esp, %ebp
	push %ebx
	movl $2, %eax
	int $0x80
	cmpl $0, %eax
	jg returnfork
	negl %eax
	leal errno, %ebx   
	movl %eax, (%ebx)
	movl $-1, %eax
returnfork:
	pop %ebx
	pop %ebp
	ret

ENTRY(exit)
    push %ebp
    movl %esp, %ebp
    push %ebx
	movl $1, %eax
	int $0x80
	cmpl $0, %eax
	jg returnexit
	negl %eax
	leal errno, %ebx
	movl %eax, (%ebx)
	movl $-1, %eax
returnexit:
	pop %ebx
	pop %ebp
	ret

ENTRY(get_stats)
	push %ebp
	movl %esp, %ebp
	push %ebx
	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl $35, %eax      
	int $0x80
	cmpl $0, %eax 
	jge returnget_stats             
	negl %eax
	leal errno, %ebx  
	movl %eax, (%ebx)
	movl $-1, %eax
returnget_stats:             // No errors
	pop %ebx
	pop %ebp
	ret