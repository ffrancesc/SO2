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
	jge end              
	negl %eax
	leal errno, %ebx     // Load error to variable errno
	movl %eax, (%ebx)
	movl $-1, %eax

end:                    // No errors
	pop %ebx
	pop %ebp
	ret