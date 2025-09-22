// #include "types.h"

// printf.c
// int             printf(char*, ...) __attribute__ ((format (printf, 1, 2)));
// void            panic(char*) __attribute__((noreturn));
// void            printfinit(void);

// uart.c
void            uartinit(void);
void            uartintr(void);
void            uartwrite(char [], int);
void            uartputc_sync(int);
void            uart_putc(char);
void            uart_puts(const char *);
int             uartgetc(void);
