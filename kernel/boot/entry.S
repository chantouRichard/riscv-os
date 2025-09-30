.section .text
.global _start
_start:
    # UART 基地址
    li t0, 0x10000000

    # 设置栈指针
    la sp, stack_top

    # 清零 BSS 段
    la t1, data_end      # BSS 起始地址
    la t2, end        # BSS 结束地址
    li t3, 0          # 清零用

1:
    bge t1, t2, 2f    # 如果 t1 >= t2，跳出循环
    sb t3, 0(t1)      # *(t1) = 0
    addi t1, t1, 1    # t1++
    j 1b
2:

    call uart_init
    # 跳转到 C 主函数
    call start

spin:
    j spin

# 栈空间定义
.section .bss
.align 16
.globl stack_top
stack_top:
    .skip 4096