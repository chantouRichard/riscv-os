#include "defs.h"

void main() {
    // ==== 实验一：初始化 UART 并输出字符串 ====
    // const char ch1 = '6';
    // const char msg2[] = "终于成功了啊！";
    // uart_putc(ch1);
    // uart_putc(ch1);
    // uart_putc(ch1);
    // uart_puts(msg2);

    // ==== 实验二：初始化控制台并清屏 ====
    test_printf_basic();
    test_printf_edge_cases();

    consoleclear();
    printf("Screen cleared!\n");

    
    while (1) {}
}