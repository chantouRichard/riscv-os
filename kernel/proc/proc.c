#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct cpu cpus[NCPU];

struct proc proc[NPROC];

struct proc *initproc;

int killed(struct proc *p)
{
  int k;

  k = p->killed;
  return k;
}

void kexit(int status)
{
  struct proc *p = myproc();

  if (p == initproc)
    panic("init exiting");

  // Give any children to init.
  // reparent(p);

  // Parent might be sleeping in wait().
  // wakeup(p->parent);

  p->xstate = status;
  p->state = ZOMBIE;

  // Jump into the scheduler, never to return.
  // sched();
  panic("zombie exit");
}

int cpuid()
{
  int id = r_tp();
  return id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
struct cpu *
mycpu(void)
{
  int id = cpuid();
  struct cpu *c = &cpus[id];
  return c;
}

// Return the current struct proc *, or zero if none.
struct proc *
myproc(void)
{
  struct cpu *c = mycpu();
  struct proc *p = c->proc;
  return p;
}

void setkilled(struct proc *p)
{
  // acquire(&p->lock);
  p->killed = 1;
  // release(&p->lock);
}

void yield(void)
{
  struct proc *p = myproc();
  p->state = RUNNABLE;
}