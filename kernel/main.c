#include "defs.h"

void main() {
    static const char msg1[] = "Hello, RISC-V !";
    static const char msg2[] = "成功了啊！！！";
    uart_puts(msg1);
    uart_puts(msg2);
    while (1) {}
}