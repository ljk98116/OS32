#ifndef TRAP_H
#define TRAP_H

#include "mmu.h"
#include "x86.h"

typedef struct idt_ptr_t {
    ushort limit;
    uint base;      
} __attribute__((packed)) idt_ptr_t;

extern void idt_flush();

// stack top has higher addr
// CPU pushes (SS,USER_ESP)[USER_MODE],eflags,cs,eip before interrupt function
// interrupt function pushes error_code(if it has),int_no into stack
// the last one(ds) being pushed is the first element of pt_regs
// esp value is the addr of the pt_regs struct which is also the param of interrupt
// so we also push esp value(pt_regs 's addr,param pt_regs* of interrupt) into stack

typedef struct pt_regs_t {
    uint ds;        
    uint edi;       
    uint esi; 
    uint ebp;
    uint esp;
    uint ebx;
    uint edx;
    uint ecx;
    uint eax;
    uint int_no;    
    uint err_code;     
    //CPU automarically pushed 
    uint eip;
    uint cs;        
    uint eflags;
    uint useresp;
    uint ss;
} pt_regs;

void init_idt();

typedef void (*interrupt_handler_t)(pt_regs *);

void register_interrupt_handler(uchar n, interrupt_handler_t h);

void interrupt_handler(pt_regs *regs);

//interrupt
extern void interrupt0();        // 0 #DE 除 0 异常 
extern void interrupt1();        // 1 #DB 调试异常 
extern void interrupt2();        // 2 NMI 
extern void interrupt3();        // 3 BP 断点异常 
extern void interrupt4();        // 4 #OF 溢出 
extern void interrupt5();        // 5 #BR 对数组的引用超出边界 
extern void interrupt6();        // 6 #UD 无效或未定义的操作码 
extern void interrupt7();        // 7 #NM 设备不可用(无数学协处理器) 
extern void interrupt8();        // 8 #DF 双重故障(有错误代码) 
extern void interrupt9();        // 9 协处理器跨段操作 
extern void interrupt10();       // 10 #TS 无效TSS(有错误代码) 
extern void interrupt11();       // 11 #NP 段不存在(有错误代码) 
extern void interrupt12();       // 12 #SS 栈错误(有错误代码) 
extern void interrupt13();       // 13 #GP 常规保护(有错误代码) 
extern void interrupt14();       // 14 #PF 页故障(有错误代码) 
extern void interrupt15();       // 15 CPU 保留 
extern void interrupt16();       // 16 #MF 浮点处理单元错误 
extern void interrupt17();       // 17 #AC 对齐检查 
extern void interrupt18();       // 18 #MC 机器检查 
extern void interrupt19();       // 19 #XM SIMD(单指令多数据)浮点异常

// 20 ~ 31 Intel reserved
extern void interrupt20();
extern void interrupt21();
extern void interrupt22();
extern void interrupt23();
extern void interrupt24();
extern void interrupt25();
extern void interrupt26();
extern void interrupt27();
extern void interrupt28();
extern void interrupt29();
extern void interrupt30();
extern void interrupt31();

//32-47 IRQ
extern void interrupt32();
extern void interrupt33();
extern void interrupt34();
extern void interrupt35();
extern void interrupt36();
extern void interrupt37();
extern void interrupt38();
extern void interrupt39();
extern void interrupt40();
extern void interrupt41();
extern void interrupt42();
extern void interrupt43();
extern void interrupt44();
extern void interrupt45();
extern void interrupt46();
extern void interrupt47();

// 32 ~ 255 用户自定义异常
extern void interrupt255();


#endif