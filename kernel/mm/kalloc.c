#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
extern char end[]; // 内核末尾
struct run
{
    struct run *next;
};

struct
{
    struct run *freelist;
} kmem;

void kinit()
{
    printf("\n===pa_start 地址: %p, pa_end 地址:%p===\n", end, (void *)PHYSTOP);

    freerange(end, (void *)PHYSTOP);
}

void freerange(void *pa_start, void *pa_end)
{
    char *p;
    p = (char *)PGROUNDUP((uint64)pa_start);
    for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
        kfree(p);
}

void *kalloc(void)
{
    struct run *r = kmem.freelist;
    if (r)
    {
        kmem.freelist = r->next;
    }
    printf("kalloc -> %p\n", r); // 打印分配的页地址
    return (void *)r;
}

void kfree(void *pa)
{
    struct run *r;

    if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
        panic("kfree");

    // Fill with junk to catch dangling refs.
    //   printf("kfree %p\n", pa); // 打印释放的页地址
    //   memset(pa, 1, PGSIZE);

    r = (struct run *)pa;

    r->next = kmem.freelist;
    kmem.freelist = r;
}

void test_physical_memory(void)
{
    // 测试基本分配和释放
    void *page1 = kalloc();
    void *page2 = kalloc();
    assert(page1 != page2);
    assert(((uint64)page1 & 0xFFF) == 0); // 页对齐检查

    // 测试数据写入
    *(int *)page1 = 0x12345678;
    assert(*(int *)page1 == 0x12345678);

    // 测试释放和重新分配
    kfree(page1);
    void *page3 = kalloc();
    // page3 可能等于 page1（取决于 free list 分配策略）

    kfree(page2);
    kfree(page3);
}

void test_pagetable(void)
{
    // // for(int i=0;i<100;i++){
    // //     kalloc();
    // // }
    // // 创建一个新的用户页表
    // pagetable_t pt = uvmcreate();
    // assert(pt != 0);

    // // 虚拟地址和物理地址
    // uint64 va = 0x1000000;
    // uint64 pa = (uint64)kalloc();
    // assert(pa != 0);

    // // 建立虚拟->物理映射
    // assert(mappages(pt, va, PGSIZE, pa, PTE_R | PTE_W) == 0);

    // // 测试地址转换
    // pte_t *pte = walk(pt, va, 0); // 第三个参数=0，不创建
    // printf("walk: va=0x%p -> pte=%p, pa=0x%p, perm=0x%x\n", va, pte, PTE2PA(*pte), *pte & 0xFFF);

    pagetable_t pt = (pagetable_t)kalloc();
    memset(pt, 0, PGSIZE);

    uint64 va = 0x12345000;
    uint64 pa = 0x80001000;

    pte_t *p1 = walk(pt, va, 0);
    printf("映射前：walk返回=%p\n", p1);
    mappages(pt, va, PGSIZE, pa, PTE_R | PTE_W);

    pte_t *pte = walk(pt, va, 0);
    assert(pte != 0);
    printf("映射后：pte=%p, PA=0x%p, V=%d, R=%d, W=%d\n",
           pte,
           PTE2PA(*pte),
           !!(*pte & PTE_V),
           !!(*pte & PTE_R),
           !!(*pte & PTE_W));
}
