.section .text
.global _start
_start:
    # UART 基地址
    li t0, 0x10000000

    # 输出 'S' 表示启动
    # li t1, 'S'
    # sb t1, 0(t0)

    # 设置栈指针
    la sp, stack_top

    # 输出 'P' 表示栈设置完成
    # li t1, 'P'
    # sb t1, 0(t0)

    # 跳转到 C 主函数
    call main

spin:
    j spin

# 栈空间定义
.section .bss
.align 16
.globl stack_top
stack_top:
    .skip 4096