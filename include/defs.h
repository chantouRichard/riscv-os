#include "types.h"

// uart.c
void            uart_init(uint32 baud_rate);
void            uart_putc(char);
void            uart_puts(const char *);

// console.c
void            consoleclear(void);
void            consputc(int);

// printf.c
int             printf(const char* fmt, ...);
void            test_printf_basic();
void            test_printf_edge_cases();

