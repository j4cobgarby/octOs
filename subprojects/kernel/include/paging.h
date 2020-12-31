#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define VMM_PAGES_PER_PAGETABLE 1024
#define VMM_TABLES_PER_PAGEDIR  1024

#define PAGEDIR_INDEX(vaddr) ((vaddr >> 22) & 0x3ff)
#define PAGETABLE_INDEX(vaddr) ((vaddr >> 12) & 0x3ff)

/*
Format of a virtual address:

|31                 22|21                 12|11                         0|
+---------------------+---------------------+----------------------------+
|   Index in page dir | Index in page Table |             Offset in page |

*/

/*
Page table entry
*/
struct __attribute__((__packed__)) pte_t {
    uint8_t present : 1; // 0: not in memory, 1 : present in memory
    uint8_t rw      : 1; // 0: page is read only, 1: page is r/w
    uint8_t mode    : 1; // 0: kernel mode, 1: user mode
    uint8_t rsvd0   : 2; // reserved
    uint8_t access  : 1; // SET BY CPU. 0: not accessed, 1: accessed
    uint8_t dirty   : 1; // SET BY CPU. 0: not been written to, 1: written to
    uint8_t rsvd1   : 2; // reserved
    uint8_t avail   : 3; // available to store data
    uint32_t frame  : 20; // The frame address that the page refers to (bits 12-31)
};

/*
Page directory entry
*/
struct __attribute__((__packed__)) pde_t {
    uint8_t present         : 1; // 0 : page table is not in memory, 1: page table is in memory
    uint8_t rw              : 1; // 0 : page table is read only, 1 : page table is r/w
    uint8_t mode            : 1; // 0 : kernel mode page table, 1 : user mode page table
    uint8_t writethrough    : 1; // is write through caching enabled
    uint8_t cachedisable    : 1; // 0 : cache disabled, 1 : cache enabled
    uint8_t access          : 1; // SET BY CPU. 0: not accessed, 1 : accessed
    uint8_t rsvd            : 1; // reserved
    uint8_t pagesize        : 1; // 0 : 4KB pages, 1 : 4MB pages
    uint8_t global          : 1; // global pages (ignored by cpu)
    uint8_t avail           : 3; // available to store data
    uint32_t addr           : 20; // Physical address of the page table (bits 12-31)
};

/*
Page table
Among the 1024 pages in a page table, 4MB of memory is
represented.
*/
struct pagetable_t {
    struct pte_t ptables[VMM_PAGES_PER_PAGETABLE];
};

/*
Page directory
A single page directory describes a full 4GB of memory.
*/
struct pagedirtable_t {
    struct pde_t pdirs[VMM_TABLES_PER_PAGEDIR];
};

extern struct pagedirtable_t* vmm_current_pdir;

/*
Initialise the virtual memory manager
*/
void vmm_init();

/*
Allocate a memory frame for a given page. If not able to allocate memory,
return 0.
*/
uint8_t vmm_alloc_mem_for_page(struct pte_t *page);

/*
Free the frame allocated to the page.
*/
uint8_t vmm_free_page_mem(struct pte_t *page);

/*
From a page table, gets the page table entry containing to a
given virtual address.
*/
struct pte_t* vmm_pte_from_addr(struct pte_t *ptable, uint32_t vaddr);

/*
From an array of page directories, get the page directory containing
a given virtual address.
*/
struct pde_t* vmm_pde_from_addr(struct pde_t *pdtable, uint32_t vaddr);

/*
Change which page directory is currently being used by the processor.
*/
uint8_t vmm_change_current_pdir(struct pagedirtable_t *dir);

/*
Invalidate a page in the TLB, so that it will be taken from the page
tables next time it's needed.
*/
inline void vmm_invalidate_page(void *addr);

/*
Map the page at a virtual address to the frame at a physical address
Return 0 if not able to do so.
*/
uint8_t vmm_map_page_to_frame(void *vaddr, void *paddr);

void vmm_set_paging_enabled(uint8_t enabled);

#endif