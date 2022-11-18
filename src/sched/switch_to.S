.globl switch_to
switch_to:
    //第一个上下文的地址存入eax
    movl 4(%esp),%eax
    //第一个上下文保存当前的寄存器值
    movl %esp, (%eax)
    movl %ebp,4(%eax)
    movl %ebx,8(%eax)
    movl %esi,12(%eax)
    movl %edi,16(%eax)
    //保存eflags
    pushf
    popl %ecx
    movl %ecx,20(%eax)
    //第二个上下文的地址存入eax
    movl 8(%esp),%eax
    //将第二个上下文的寄存器值写入当前寄存器
    movl (%eax),%esp
    movl 4(%eax),%ebp
    movl 8(%eax),%ebx
    movl 12(%eax),%esi
    movl 16(%eax),%edi
    //将eflag值先写入eax,将eax压栈后，用popf弹栈到当前的eflags
    movl 20(%eax),%eax
    pushl %eax
    popf
    ret