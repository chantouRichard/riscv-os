#include "defs.h"
#include "types.h"
#include "riscv.h"

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

    printf("mappages: va=0x%lx -> pa=0x%lx, perm=0x%x\n", va, pa, PTE_R | PTE_W);

    return 0;
}

pagetable_t uvmcreate()
{
    pagetable_t pagetable;
    pagetable = (pagetable_t)kalloc();
    if (pagetable == 0)
        return 0;
    printf("开始测试, 地址: %p",P2V(pagetable));
    for (int i = 0; i < 16; i++)
    {
        ((char *)P2V(pagetable))[i] = 0;
    }
    printf("测试完成");
    memset(pagetable, 0, PGSIZE);
    return pagetable;
}
