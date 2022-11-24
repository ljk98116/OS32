# valid interrupt
.globl interrupt0,interrupt1,interrupt2,interrupt3,interrupt4,interrupt5,interrupt6,interrupt7,interrupt8,interrupt9,interrupt10
.globl interrupt11,interrupt12,interrupt13,interrupt14,interrupt15,interrupt16,interrupt17,interrupt18,interrupt19
# reserved interrupt
.globl interrupt20,interrupt21,interrupt22,interrupt23,interrupt24,interrupt25,interrupt26,interrupt26,interrupt27,interrupt28,interrupt29,interrupt30,interrupt31
# special interrupt
.globl interrupt255

interrupt0:
    cli
    pushl $0
    pushl $0
    jmp interrupt_common_stub

interrupt1:
    cli
    pushl $0
    pushl $1
    jmp interrupt_common_stub

interrupt2:
    cli
    pushl $0
    pushl $2
    jmp interrupt_common_stub

interrupt3:
    cli
    pushl $0
    pushl $3
    jmp interrupt_common_stub

interrupt4:
    cli
    pushl $0
    pushl $4
    jmp interrupt_common_stub

interrupt5:
    cli
    pushl $0
    pushl $5
    jmp interrupt_common_stub

interrupt6:
    cli
    pushl $0
    pushl $6
    jmp interrupt_common_stub

interrupt7:
    cli
    pushl $0
    pushl $7
    jmp interrupt_common_stub

interrupt8:
    cli
    pushl $8
    jmp interrupt_common_stub

interrupt9:
    cli
    pushl $0
    pushl $9
    jmp interrupt_common_stub

interrupt10:
    cli
    pushl $10
    jmp interrupt_common_stub

interrupt11:
    cli
    pushl $11
    jmp interrupt_common_stub

interrupt12:
    cli
    pushl $12
    jmp interrupt_common_stub

interrupt13:
    cli
    pushl $13
    jmp interrupt_common_stub

interrupt14:
    cli
    pushl $14
    jmp interrupt_common_stub

interrupt15:
    cli
    pushl $0
    pushl $15
    jmp interrupt_common_stub

interrupt16:
    cli
    pushl $0
    pushl $16
    jmp interrupt_common_stub

interrupt17:
    cli
    pushl $17
    jmp interrupt_common_stub

interrupt18:
    cli
    pushl $0
    pushl $18
    jmp interrupt_common_stub

interrupt19:
    cli
    pushl $0
    pushl $19
    jmp interrupt_common_stub

interrupt20:
    cli
    pushl $0
    pushl $20
    jmp interrupt_common_stub

interrupt21:
    cli
    pushl $0
    pushl $21
    jmp interrupt_common_stub

interrupt22:
    cli
    pushl $0
    pushl $22
    jmp interrupt_common_stub

interrupt23:
    cli
    pushl $0
    pushl $23
    jmp interrupt_common_stub

interrupt24:
    cli
    pushl $0
    pushl $24
    jmp interrupt_common_stub

interrupt25:
    cli
    pushl $0
    pushl $25
    jmp interrupt_common_stub

interrupt26:
    cli
    pushl $0
    pushl $26
    jmp interrupt_common_stub

interrupt27:
    cli
    pushl $0
    pushl $27
    jmp interrupt_common_stub

interrupt28:
    cli
    pushl $0
    pushl $28
    jmp interrupt_common_stub

interrupt29:
    cli
    pushl $0
    pushl $29
    jmp interrupt_common_stub

interrupt30:
    cli
    pushl $0
    pushl $30
    jmp interrupt_common_stub

interrupt31:
    cli
    pushl $0
    pushl $31
    jmp interrupt_common_stub

.globl interrupt32
interrupt32:
    cli
    pushl $0
    pushl $32
    jmp interrupt_common_stub

.globl interrupt33
interrupt33:
    cli
    pushl $0
    pushl $33
    jmp interrupt_common_stub

.globl interrupt34
interrupt34:
    cli
    pushl $0
    pushl $34
    jmp interrupt_common_stub

.globl interrupt35
interrupt35:
    cli
    pushl $0
    pushl $35
    jmp interrupt_common_stub

.globl interrupt36
interrupt36:
    cli
    pushl $0
    pushl $36
    jmp interrupt_common_stub

.globl interrupt37
interrupt37:
    cli
    pushl $0
    pushl $37
    jmp interrupt_common_stub

.globl interrupt38
interrupt38:
    cli
    pushl $0
    pushl $38
    jmp interrupt_common_stub

.globl interrupt39
interrupt39:
    cli
    pushl $0
    pushl $39
    jmp interrupt_common_stub

.globl interrupt40
interrupt40:
    cli
    pushl $0
    pushl $40
    jmp interrupt_common_stub

.globl interrupt41
interrupt41:
    cli
    pushl $0
    pushl $41
    jmp interrupt_common_stub

.globl interrupt42
interrupt42:
    cli
    pushl $0
    pushl $42
    jmp interrupt_common_stub

.globl interrupt43
interrupt43:
    cli
    pushl $0
    pushl $43
    jmp interrupt_common_stub

.globl interrupt44
interrupt44:
    cli
    pushl $0
    pushl $44
    jmp interrupt_common_stub

.globl interrupt45
interrupt45:
    cli
    pushl $0
    pushl $45
    jmp interrupt_common_stub

.globl interrupt46
interrupt46:
    cli
    pushl $0
    pushl $46
    jmp interrupt_common_stub

.globl interrupt47
interrupt47:
    cli
    pushl $0
    pushl $47
    jmp interrupt_common_stub

interrupt255:
    cli
    pushl $0
    pushl $255
    jmp interrupt_common_stub

.globl idt_flush
idt_flush:
    movl 4(%esp),%eax
    lidt (%eax)
    ret

interrupt_common_stub:
    pushal
    pushl %ds
    
    # load kernel data seg
    movw $0x10,%ax
    movw %ax,%ds
    movw %ax,%es

    pushl %esp
    call interrupt_handler
    addl $4,%esp

    popl %ds
    popal
    addl $8,%esp

    iret

.globl local_intr_save
local_intr_save:
    cli
    ret

.globl local_intr_restore
local_intr_restore:
    sti
    ret