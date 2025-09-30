#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern char trampoline[]; // 用于用户态 trap 的跳板

// 时钟计数
uint ticks = 0;

// user 态 trap 处理
void usertrap(void)
{
  int which_dev = 0;

  if ((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // 保存用户程序计数器
  struct proc *p = myproc();
  p->trapframe->epc = r_sepc();

  if (r_scause() == 8)
  {
    // 系统调用
    if (killed(p))
      kexit(-1);

    p->trapframe->epc += 4; // 跳过 ecall 指令
    intr_on();
    // syscall();
  }
  else if ((which_dev = devintr()) != 0)
  {
    // 外设中断
  }
  else
  {
    printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    setkilled(p);
  }

  if (killed(p))
    kexit(-1);

  // 时钟中断触发调度
  if (which_dev == 2)
  {
    yield();
  }

  // usertrapret();
}

// kernel 态 trap 处理
void kerneltrap(void)
{
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();

  int which_dev = 0;

  // 1. Supervisor timer interrupt
  if ((scause & 0x8000000000000000L) && (scause & 0xff) == 5)
  {
    // 时钟中断
    clockintr();
    which_dev = 2;
  }
  // 2. 其他设备中断
  else if ((which_dev = devintr()) != 0)
  {
    // 设备中断已经处理
  }
  // 3. 异常（非中断）
  else
  {
    // 打印异常类型和相关寄存器
    printf("kerneltrap: sepc=%p scause=%p sstatus=%p\n",
           sepc, scause, sstatus);
    // 将 sepc+4 直接跳过异常指令，继续执行
    w_sepc(sepc + 4);
    return; // 不再 panic
  }

  // 如果是时钟中断，可能要 yield
  if (which_dev == 2 && myproc() && myproc()->state == RUNNING)
  {
    yield();
  }

  // 恢复 sepc、sstatus
  w_sepc(sepc);
  w_sstatus(sstatus);
}

// 设备中断分发
int devintr()
{
  uint64 scause = r_scause();
  printf("scause: %p\n", scause);
  if (scause == 0x8000000000000009L)
  {
    int irq = plic_claim();

    if (irq == UART0_IRQ)
    {
      // uartintr();
    }
    else if (irq == VIRTIO0_IRQ)
    {
      // virtio_disk_intr();
    }
    else if (irq)
    {
      printf("unexpected interrupt irq=%d\n", irq);
    }

    if (irq)
      plic_complete(irq);

    return 1;
  }
  else if (scause == 0x8000000000000005L)
  {
    clockintr();
    return 2;
  }
  else
  {
    return 0;
  }
}

// 时钟中断处理函数
void clockintr()
{
  if (cpuid() == 0)
  {
    ticks++;
  }
  w_stimecmp(r_time() + 1000000);
}

// extern volatile uint64 ticks;

void test_timer_interrupt(void)
{
  printf("开始中断功能测试...\n");

  uint64 start_ticks = ticks;

  // 等待 5 次时钟中断
  uint64 start_time = r_time();
  while (ticks < start_ticks + 5)
  {
    printf("等待时钟中断处理... Current ticks: %d\n", ticks);
    for (volatile int i = 0; i < 40000000; i++)
    {
    }
  }

  uint64 end_time = r_time();

  printf("中断功能测试完成: %d interrupts in %d cycles\n",
         (int)(ticks - start_ticks), end_time - start_time);
}

void test_exception_handling(void)
{
  printf("开始异常处理测试...\n");

  // -----------------------------------------------------
  // 1. 除零异常（Divide by zero）
  // 注意：RISC-V 除零会触发异常 scause=0
  // -----------------------------------------------------
  volatile int a = 1;
  volatile int b = 0;
  volatile int c;

  printf("除零异常（Divide by zero）...\n");
  // 使用 volatile 防止编译器优化
  asm volatile(
      "div %0, %1, %2\n"
      : "=r"(c)
      : "r"(a), "r"(b)
      : "t0");

  // -----------------------------------------------------
  // 2. 非法指令异常
  // RISC-V 里可以写一个无效 opcode
  // -----------------------------------------------------
  printf("非法指令异常...\n");
  asm volatile(".word 0x00000000"); // 空操作码，通常非法

  // -----------------------------------------------------
  // 3. 内存访问异常
  // 访问非法地址触发 load/store fault
  // -----------------------------------------------------
  printf("内存访问异常...\n");
  volatile int *ptr = (int *)0xdeadbeef;
  int val = *ptr;

  printf("异常处理测试完成\n");
}

void test_interrupt_overhead(void)
{
  printf("开始性能测试...\n");

  uint64 start_ticks = ticks;
  uint64 start_time = r_time();

  int num_interrupts = 10;

  // 等待 10 次时钟中断
  while (ticks < start_ticks + num_interrupts)
  {
    // 可以做一点空转或者 nop
    for (volatile int i = 0; i < 100000; i++)
    {
    }
  }

  uint64 end_time = r_time();
  uint64 elapsed_cycles = end_time - start_time;

  printf("处理 %d 次时钟中断需要 %d 次 cycles\n",
         num_interrupts, elapsed_cycles);
  printf("平均每次中断的 cycles: %d\n",
         elapsed_cycles / num_interrupts);

  printf("性能测试完成\n");
}
