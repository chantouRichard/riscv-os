// kernel/printf.c
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include <stdarg.h>

// 外部函数声明（由 console.c 提供）
// extern void consputc(int c);

// 打印整数（支持十进制、十六进制，负数处理）
static void printint(int xx, int base, int sign)
{
    static char digits[] = "0123456789abcdef";
    char buf[16];
    int i = 0;
    unsigned int x;

    if (sign && xx < 0)
    {
        x = -xx;
    }
    else
    {
        x = xx;
    }

    // 转换为字符串（逆序）
    do
    {
        buf[i++] = digits[x % base];
        x /= base;
    } while (x != 0);

    if (sign && xx < 0)
        buf[i++] = '-';

    // 倒序输出
    while (--i >= 0)
        consputc(buf[i]);
}

// 打印指针
static void printptr(unsigned long x)
{
    int i;
    consputc('0');
    consputc('x');
    for (i = (sizeof(unsigned long) * 8) - 4; i >= 0; i -= 4)
    {
        int digit = (x >> i) & 0xF;
        consputc("0123456789abcdef"[digit]);
    }
}

// 格式化输出
int printf(const char *fmt, ...)
{
    va_list ap;
    const char *p;
    int c;

    va_start(ap, fmt);
    for (p = fmt; (c = *p & 0xff) != 0; p++)
    {
        if (c != '%')
        {
            consputc(c);
            continue;
        }
        p++; // 跳过 '%'
        if (*p == 0)
            break;
        switch (*p)
        {
        case 'd': // 十进制
            printint(va_arg(ap, int), 10, 1);
            break;
        case 'x': // 十六进制
            printint(va_arg(ap, int), 16, 0);
            break;
        case 'p': // 指针
            printptr(va_arg(ap, unsigned long));
            break;
        case 's':
        { // 字符串
            char *s = va_arg(ap, char *);
            if (s == 0)
                s = "(null)";
            while (*s)
                consputc(*s++);
            break;
        }
        case 'c': // 单字符
            consputc(va_arg(ap, int));
            break;
        case '%': // 打印 '%'
            consputc('%');
            break;
        default: // 未知格式，直接输出
            consputc('%');
            consputc(*p);
            break;
        }
    }
    va_end(ap);

    return 0;
}
void panic(const char *msg) {
    printf("PANIC: %s\n", msg);
    for (;;); // 死循环停机
}

void test_printf_basic()
{
    printf("Testing integer: %d\n", 42);
    printf("Testing negative: %d\n", -123);
    printf("Testing zero: %d\n", 0);
    printf("Testing hex: 0x%x\n", 10);
    printf("Testing string: %s\n", "Hello");
    printf("Testing char: %c\n", 'X');
    printf("Testing percent: %%\n");
}
void test_printf_edge_cases()
{
    printf("INT_MAX: %d\n", 2147483647);
    printf("INT_MIN: %d\n", -2147483648);
    printf("NULL string: %s\n", (char *)0);
    printf("Empty string: %s\n", "");
}