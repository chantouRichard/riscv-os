#include "types.h"
#define assert(x)                        \
    do                                   \
    {                                    \
        if (!(x))                        \
        {                                \
            panic("assert failed: " #x); \
        }                                \
    } while (0)

// uart.c
void uart_init(uint32 baud_rate);
void uart_putc(char);
void uart_puts(const char *);

// console.c
void consoleclear(void);
void consputc(int);

// printf.c
int printf(const char *fmt, ...);
void test_printf_basic();
void test_printf_edge_cases();
void panic(const char *msg);

// string.c
int memcmp(const void *, const void *, uint);
void *memmove(void *, const void *, uint);
void *memset(void *, int, uint);
char *safestrcpy(char *, const char *, int);
int strlen(const char *);
int strncmp(const char *, const char *, uint);
char *strncpy(char *, const char *, int);

// kalloc.c
void kinit();
void *kalloc(void);
void kfree(void *pa);
void test_physical_memory(void);
void test_pagetable(void);

// vm.c
pte_t *walk(pagetable_t pagetable, uint64 va, int alloc);
int mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm);
pagetable_t uvmcreate(void);
void kvminit(void);
void kvminithart(void);
void test_virtual_memory(void);
void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free);
void freewalk(pagetable_t pagetable);
void uvmfree(pagetable_t pagetable, uint64 sz);

// trap.c
extern uint ticks;
void trapinit(void);
void trapinithart(void);
extern struct spinlock tickslock;
void prepare_return(void);
void test_timer_interrupt(void);
void test_exception_handling(void);
void test_interrupt_overhead(void);

// proc.c
int cpuid(void);
struct cpu *mycpu(void);
struct proc *myproc();
void procinit(void);
void yield(void);
void setkilled(struct proc *p);
