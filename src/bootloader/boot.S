.section .text

.code32
.globl start
.globl glb_mboot_ptr

start:
    cli
    movl $(stack + 32768),%esp
    movl $0,%ebp
    andl $0xFFFFFFF0,%esp
    movl %ebx,[glb_mboot_ptr]
    call kern_entry

stop:
    hlt
    jz stop

glb_mboot_ptr:
    .space 4

# stack starts from here
.comm stack,32768


