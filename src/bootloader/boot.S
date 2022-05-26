.section .init.text

.code32
.globl start

start:
    #cli
    movl $(stack + 32768),%esp
    movl $0,%ebp
    andl $0xFFFFFFF0,%esp
    movl %ebx,[mboot_ptr_tmp]
    call kern_entry

stop:
    hlt
    jz stop

.section .init.data
# stack starts from here
.comm stack,32768

.globl mboot_ptr_tmp
mboot_ptr_tmp:
    .long 0


