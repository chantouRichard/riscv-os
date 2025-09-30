#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"

pagetable_t kernel_pagetable;

pte_t *walk(pagetable_t pagetable, uint64 va, int alloc)
{
    for (int level = 2; level > 0; level--)
    {
        pte_t *pte = &pagetable[PX(level, va)];
        if (*pte & PTE_V)
        {
            pagetable = (pagetable_t)PTE2PA(*pte);
        }
        else
        {
            if (!alloc)
                return 0;
            pagetable = (pagetable_t)kalloc();
            memset(pagetable, 0, PGSIZE);
            *pte = PA2PTE(pagetable) | PTE_V;
        }
    }
    return &pagetable[PX(0, va)];
}

int mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm)
{
    uint64 a, last;
    pte_t *pte;
    a = PGROUNDUP(va);
    last = PGROUNDUP(va + size - 1);

    for (;;)
    {
        pte = walk(pagetable, a, 1);
        *pte = PA2PTE(pa) | perm | PTE_V;
        if (a == last)
            break;
        a += PGSIZE;
        pa += PGSIZE;
    }

    printf("mappages: va=0x%p -> pa=0x%p, perm=0x%x\n", va, pa, PTE_R | PTE_W);

    return 0;
}

pagetable_t uvmcreate()
{
    pagetable_t pagetable;
    pagetable = (pagetable_t)kalloc();
    if (pagetable == 0)
        return 0;
    // printf("开始测试, 地址: %p",V2P(pagetable));
    // for (int i = 0; i < 16; i++)
    // {
    //     ((char *)V2P(pagetable))[i] = 0;
    // }
    // printf("测试完成");
    memset(pagetable, 0, PGSIZE);
    return pagetable;
}

// kinit
// 初始化内核页表
void kvminit(void)
{
    pagetable_t kpgtbl = (pagetable_t)kalloc();
    if (!kpgtbl)
        return;
    memset(kpgtbl, 0, PGSIZE);

    // 假设你有一些内核段需要映射，例如 text/data/bss/IO
    extern char text_start[], text_end[], rodata_start[], rodata_end[], data_start[], data_end[];

    // text: 可执行、只读
    mappages(kpgtbl, KERNBASE, KERNBASE, (uint64)text_start - KERNBASE, PTE_R | PTE_X);

    // rodata: 只读
    mappages(kpgtbl, (uint64)text_start, (uint64)text_start, PHYSTOP - (uint64)text_start, PTE_R | PTE_W);

    // data+bss: 可读写
    // mappages(kpgtbl, (uint64)data_start, (uint64)(data_end - data_start),
    //          V2P(data_start), PTE_R | PTE_W);

    kernel_pagetable = kpgtbl;
}

// 每个 hart 的页表初始化
void kvminithart()
{
    asm volatile("sfence.vma"); // 刷新 TLB
    // 写 satp 寄存器，使 MMU 使用这个页表
    uint64 satp_value = MAKE_SATP(kernel_pagetable);
    asm volatile("sfence.vma"); // 刷新 TLB
}

void freewalk(pagetable_t pagetable)
{
    // there are 2^9 = 512 PTEs in a page table.
    for (int i = 0; i < 512; i++)
    {
        pte_t pte = pagetable[i];
        if ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0)
        {
            // this PTE points to a lower-level page table.
            uint64 child = PTE2PA(pte);
            freewalk((pagetable_t)child);
            pagetable[i] = 0;
        }
        else if (pte & PTE_V)
        {
            panic("freewalk: leaf");
        }
    }
    kfree((void *)pagetable);
}

// 释放用户页表和它映射的物理内存
void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free)
{
    uint64 a;
    pte_t *pte;

    if ((va % PGSIZE) != 0)
        panic("uvmunmap: not aligned");

    for (a = va; a < va + npages * PGSIZE; a += PGSIZE)
    {
        if ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
            continue;
        if ((*pte & PTE_V) == 0) // has physical page been allocated?
            continue;
        if (do_free)
        {
            uint64 pa = PTE2PA(*pte);
            kfree((void *)pa);
        }
        *pte = 0;
    }
}
void uvmfree(pagetable_t pagetable, uint64 sz)
{
    if (sz > 0)
        uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
    freewalk(pagetable);
}

void test_virtual_memory(void)
{
    printf("Before enabling paging...\n");
    // 启用分页
    kvminit();
    kvminithart();
    printf("After enabling paging...\n");
    // 测试内核代码仍然可执行
    // 测试内核数据仍然可访问
    // 测试设备访问仍然正常
}