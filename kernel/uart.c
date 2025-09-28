#include "defs.h"
#include "types.h"

#define UART_BASE 0x10000000UL
#define UART_THR (UART_BASE + 0x0)   // 发送寄存器
#define UART_LSR (UART_BASE + 0x5)   // 状态寄存器  
#define UART_LSR_THRE 0x20           // THR 空（可发送）
#define UART_IER (UART_BASE + 0x1)  // 中断使能寄存器
#define UART_FCR (UART_BASE + 0x2)  // FIFO 控制寄存器
#define UART_LCR (UART_BASE + 0x3)  // 线控制寄存器（设置数据格式）
#define UART_DLL (UART_BASE + 0x0)  // 波特率低字节（需先设置 LCR 的 DLAB 位）
#define UART_DLM (UART_BASE + 0x1)  // 波特率高字节


static inline void mmio_write(unsigned long addr, uint8 val) {
    *(volatile uint8 *)addr = val;
}

static inline uint8 mmio_read(unsigned long addr) {
    return *(volatile uint8 *)addr;
}

void uart_init(uint32 baud_rate) {
    // 1. 禁用中断（IER）
    mmio_write(UART_IER, 0x00);
    
    // 2. 启用 FIFO（FCR）
    mmio_write(UART_FCR, 0x01);
    
    // 3. 设置数据格式（8N1）
    mmio_write(UART_LCR, 0x03);
    
    // 4. 设置波特率（需先设置 LCR 的 DLAB 位）
    mmio_write(UART_LCR, mmio_read(UART_LCR) | 0x80);  // 启用 DLAB
    uint32 divisor = 115200 / baud_rate;
    mmio_write(UART_DLL, divisor & 0xFF);      // 低字节
    mmio_write(UART_DLM, (divisor >> 8) & 0xFF); // 高字节
    mmio_write(UART_LCR, mmio_read(UART_LCR) & ~0x80); // 关闭 DLAB
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