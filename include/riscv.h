#define PGSIZE 4096
#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE-1))

#define PTE_V (1L << 0)  // 有效位
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4)

#define PTE2PA(pte) (((pte) >> 10) << 12)   // 从 PTE 提取物理地址
#define PA2PTE(pa)  ((((uint64)(pa)) >> 12) << 10)
#define PX(level, va) ((((uint64)(va)) >> (12 + (9 * (level)))) & 0x1FF)

typedef uint64 pte_t;
typedef uint64* pagetable_t;  // 其实就是物理地址，指向一页

#define KERNBASE 0x80000000L
#define PHYSTOP (KERNBASE + 128*1024*1024)

#define P2V(pa) ((void *)((uint64)(pa) + KERNBASE))
#define V2P(va) ((uint64)(va) - KERNBASE)

