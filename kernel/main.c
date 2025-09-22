#include "defs.h"

void main() {
    const char ch1 = '6';
    const char msg2[] = "终于成功了啊！";
    uart_putc(ch1);
    uart_putc(ch1);
    uart_putc(ch1);
    uart_puts(msg2);
    while (1) {}
}