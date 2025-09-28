#include "defs.h"
#include "types.h"
#include "riscv.h"
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
  freerange(end, (void*)PHYSTOP);
}

void freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

void *kalloc(void)
{
    struct run *r = kmem.freelist;
    if (r)
    {
        kmem.freelist = r->next;
    }
    printf("kalloc -> %p\n", r);  // 打印分配的页地址
    return (void *)r;
}

void kfree(void *pa)
{
    struct run *r = (struct run *)pa;
    r->next = kmem.freelist;
    kmem.freelist = r;

    // printf("kfree <- %p\n", pa);  // 打印释放的页地址
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
    // 创建一个新的用户页表
    pagetable_t pt = uvmcreate();
    assert(pt != 0);

    // 虚拟地址和物理地址
    uint64 va = 0x1000000;
    uint64 pa = (uint64)kalloc();
    assert(pa != 0);

    // 建立虚拟->物理映射
    assert(mappages(pt, va, PGSIZE, pa, PTE_R | PTE_W) == 0);

    // 测试地址转换
    pte_t *pte = walk(pt, va, 0);  // 第三个参数=0，不创建
    printf("walk: va=0x%lx -> pte=%p, pa=0x%lx, perm=0x%x\n", va, pte, PTE2PA(*pte), *pte & 0xFFF);
    assert(pte != 0 && (*pte & PTE_V));
    assert(PTE2PA(*pte) == pa);

    // 测试权限位
    assert(*pte & PTE_R);
    assert(*pte & PTE_W);
    assert(!(*pte & PTE_X));
}
