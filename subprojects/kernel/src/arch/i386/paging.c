 #include "paging.h"
#include "pmm.h"
#include "kio.h"

struct pagedirtable_t* vmm_current_pdir = 0;

/*
This function sets up a page directory for the kernel to use.
The memory which the kernel is using already shall be mapped directly
to itself, so that the kernel can keep running.
Only pages which are part of the kernel have to be mapped to a frame
in the kernel page directory, and then when new pages need to be accessed
the page fault handler can allocate a frame for them.
*/
void vmm_init() {
    struct pagedirtable_t *dir = (struct pagedirtable_t*)pmm_alloc();
    pmm_memset(dir, 0, sizeof(struct pagedirtable_t));

    struct pagetable_t *table1 = (struct pagetable_t*)pmm_alloc();
    pmm_memset(table1, 0, sizeof(struct pagetable_t));
    for (uint32_t p = 0, f = 0; p < VMM_PAGES_PER_PAGETABLE; p++, f += 4096) {
        table1->ptables[p].present = 1;
        table1->ptables[p].rw = 1;
        table1->ptables[p].frame = f >> 12;
    }

    struct pde_t *entry = &dir->pdirs[PAGEDIR_INDEX(0x00000000)];
    entry->present = 1;
    entry->rw = 1;
    entry->addr = (uint32_t)table1 >> 12;

    vmm_change_current_pdir(dir);
    vmm_set_paging_enabled(1);
}

uint8_t vmm_alloc_mem_for_page(struct pte_t *page) {
    uint32_t frameaddr;

    if (!page) return 0;
    if (!(frameaddr = (uint32_t)pmm_alloc())) return 0;

    page->frame = frameaddr >> 12;
    page->present = 1;
    return 1;
}

uint8_t vmm_free_page_mem(struct pte_t *page) {
    if (!page) return 0;

    if (page->frame) {
        pmm_free((void*)((uint32_t)page->frame << 12));
    }

    page->present = 0;
    return 1;
}

struct pte_t* vmm_pte_from_addr(struct pte_t *ptable, uint32_t vaddr) {
    if (!ptable) return 0;
    return &(ptable[PAGETABLE_INDEX(vaddr)]);
}

struct pde_t* vmm_pde_from_addr(struct pde_t *pdtable, uint32_t vaddr) {
    if (!pdtable) return 0;
    return &(pdtable[PAGEDIR_INDEX(vaddr)]);
}

uint8_t vmm_change_current_pdir(struct pagedirtable_t *dir) {
    if (!dir) return 0;

    vmm_current_pdir = dir;
    
    asm("mov %0, %%cr3"
        :
        : "r"(vmm_current_pdir));

    return 1;
}

inline void vmm_invalidate_page(void* addr) {
    asm volatile ("invlpg (%0)"
        :
        : "b"(addr) 
        : "memory" );
}

uint8_t vmm_map_page_to_frame(void *vaddr, void *paddr) {
    struct pde_t *relevant_pde = &(vmm_current_pdir->pdirs[PAGEDIR_INDEX((uint32_t)vaddr)]);

    if (!relevant_pde->present) {
        // If the current page directory entry (referring to one page table)
        // is not currently in memory, then memory must be allocated for it
        struct pagetable_t *ptable = pmm_alloc();

        // Set all the pages in the pagetable to 0
        for (uint32_t c = 0; c < sizeof(struct pagetable_t); c++) {
            *((uint8_t*)(ptable+c)) = 0;
        }

        relevant_pde->present = 1; // Now in memory
        relevant_pde->rw = 1; // Writeable page
        relevant_pde->pagesize = 0; // 4KB pages
        relevant_pde->addr = (uint32_t)ptable >> 12;
    }

    // Now get the relevant page to map to the frame
    struct pagetable_t *ptable = (struct pagetable_t *)(uint32_t)relevant_pde->addr;
    struct pte_t *page = &(ptable->ptables[PAGETABLE_INDEX((uint32_t)vaddr)]);
    page->frame = (uint32_t)paddr >> 12;
    page->present = 1;
    page->rw = 1;

    return 1;
}

void vmm_set_paging_enabled(uint8_t enabled) {
    if (enabled) {
        // Set the paging bit in cr0
        asm(
"mov %cr0, %eax\n\
\tor $0x80000001, %eax\n\
\tmov %eax, %cr0");
    }
}