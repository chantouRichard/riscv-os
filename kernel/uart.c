#include <stdint.h>
#include "defs.h"

#define UART_BASE 0x10000000UL
#define UART_THR (UART_BASE + 0x0)   // 发送寄存器
#define UART_LSR (UART_BASE + 0x5)   // 状态寄存器
#define UART_LSR_THRE 0x20           // THR 空（可发送）

static inline void mmio_write(uintptr_t addr, uint8_t val) {
    *(volatile uint8_t *)addr = val;
}

static inline uint8_t mmio_read(uintptr_t addr) {
    return *(volatile uint8_t *)addr;
}

// 输出一个字符
void uart_putc(char c) {
    // 等待 THR 空
    while ((mmio_read(UART_LSR) & UART_LSR_THRE) == 0);
    mmio_write(UART_THR, c);
}

// 输出字符串
void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}