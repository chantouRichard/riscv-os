// kernel/console.c
#include "defs.h"

// 外部函数声明（来自 uart.c）
// extern void uart_putc(char c);

// 向控制台输出一个字符
void consputc(int c)
{
    uart_putc(c);
}

// 清屏：发送 ANSI 转义序列
// \033[2J 清除整个屏幕
// \033[H  将光标移动到左上角
void consoleclear(void)
{
    consputc('\033');
    consputc('[');
    consputc('2');
    consputc('J');

    consputc('\033');
    consputc('[');
    consputc('H');

    consputc('\033');
    consputc('[');
    consputc('K');
}
