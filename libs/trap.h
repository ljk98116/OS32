#ifndef TRAP_H
#define TRAP_H

#include "mmu.h"
#include "x86.h"

typedef struct idt_ptr_t {
    ushort limit;
    uint base;      
} __attribute__((packed)) idt_ptr_t;

extern void idt_flush();

// 寄存器类型
typedef struct pt_regs_t {
    uint ds;        // 用于保存用户的数据段描述符
    uint edi;       // 从 edi 到 eax 由 pusha 指令压入
    uint esi; 
    uint ebp;
    uint esp;
    uint ebx;
    uint edx;
    uint ecx;
    uint eax;
    uint int_no;    // 中断号
    uint err_code;      // 错误代码(有中断错误代码的中断会由CPU压入)
    uint eip;       // 以下由处理器自动压入
    uint cs;        
    uint eflags;
    uint useresp;
    uint ss;
} pt_regs;

void init_idt();

typedef void (*interrupt_handler_t)(pt_regs *);

void register_interrupt_handler(uchar n, interrupt_handler_t h);

void isr_handler(pt_regs *regs);
void irq_handler(pt_regs *regs);

//ISR
extern void isr0();        // 0 #DE 除 0 异常 
extern void isr1();        // 1 #DB 调试异常 
extern void isr2();        // 2 NMI 
extern void isr3();        // 3 BP 断点异常 
extern void isr4();        // 4 #OF 溢出 
extern void isr5();        // 5 #BR 对数组的引用超出边界 
extern void isr6();        // 6 #UD 无效或未定义的操作码 
extern void isr7();        // 7 #NM 设备不可用(无数学协处理器) 
extern void isr8();        // 8 #DF 双重故障(有错误代码) 
extern void isr9();        // 9 协处理器跨段操作 
extern void isr10();       // 10 #TS 无效TSS(有错误代码) 
extern void isr11();       // 11 #NP 段不存在(有错误代码) 
extern void isr12();       // 12 #SS 栈错误(有错误代码) 
extern void isr13();       // 13 #GP 常规保护(有错误代码) 
extern void isr14();       // 14 #PF 页故障(有错误代码) 
extern void isr15();       // 15 CPU 保留 
extern void isr16();       // 16 #MF 浮点处理单元错误 
extern void isr17();       // 17 #AC 对齐检查 
extern void isr18();       // 18 #MC 机器检查 
extern void isr19();       // 19 #XM SIMD(单指令多数据)浮点异常

// 20 ~ 31 Intel reserved
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// 32 ~ 255 用户自定义异常
extern void isr255();

//IRQ
#define  IRQ0     32    // system timer
#define  IRQ1     33    // keyboard
#define  IRQ2     34    // IRQ9 related，MPU-401 MD
#define  IRQ3     35    // serial
#define  IRQ4     36    // serial
#define  IRQ5     37    // sound card
#define  IRQ6     38    // softdriver transport
#define  IRQ7     39    // printer transport
#define  IRQ8     40    // normal timer
#define  IRQ9     41    // connect with IRQ2
#define  IRQ10    42    // wirecard
#define  IRQ11    43    // AGP VGA card
#define  IRQ12    44    // PS/2 Mouse
#define  IRQ13    45    // CP
#define  IRQ14    46    // IDE0
#define  IRQ15    47    // IDE1

#endif