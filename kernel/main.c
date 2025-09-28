#include "defs.h"

void main() {
    // ==== 实验一：初始化 UART 并输出字符串 ====
    // const char ch1 = '6';
    // const char msg2[] = "第一个实验终于成功了啊！";
    // uart_putc(ch1);
    // uart_putc(ch1);
    // uart_putc(ch1);
    // uart_puts(msg2);

    // ==== 实验二：初始化控制台并清屏 ====
    // printf("\n\n实验二：\n");
    // test_printf_basic();
    // test_printf_edge_cases();

    // consoleclear();
    // printf("Screen cleared!\n");

    // ==== 实验三：内存分配器 ====
    printf("\n\n实验三：\n");
    kinit();
    // printf("测试一\n");
    // test_physical_memory();
    printf("测试二\n");
    test_pagetable();
    
    while (1) {}
}