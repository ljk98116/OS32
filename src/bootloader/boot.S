.section .text

.code32
.globl start
.globl glb_mboot_ptr

start:
    cli
    movl $STACK_TOP,%esp
    movl $0,%ebp
    andl $0xFFFFFFF0,%esp
    movl %ebx,[glb_mboot_ptr]
    call kern_entry

stop:
    hlt
    jz stop

# stack starts from here
stack:
    .space 32768

glb_mboot_ptr:
    .space 4

stack_end:

STACK_TOP:
    .word stack_end - stack - 1


