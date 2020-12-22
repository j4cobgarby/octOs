#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

/*
Page table entry
*/
struct pte_t {
    uint8_t present : 1; // 0: not in memory, 1 : present in memory
    uint8_t rw      : 1; // 0: page is read only, 1: page is r/w
    uint8_t mode    : 1; // 0: kernel mode, 1: user mode
    uint8_t rsvd0   : 2; // reserved
    uint8_t access  : 1; // SET BY CPU. 0: not accessed, 1: accessed
    uint8_t dirty   : 1; // SET BY CPU. 0: not been written to, 1: written to
    uint8_t rsvd1   : 2; // reserved
    uint8_t avail   : 3; // available to store data
    uint32_t frame  : 20; // The frame address that the page refers to
};

/*
Page table
*/
struct pagetable_t {
    struct pte_t ptables[1024];
};

/*
Page directory entry
*/
struct pde_t {
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
    uint32_t addr           : 20; // Address of the page table being referred to
};

/*
Page directory table
*/
struct pagedirdtable_t {
    struct pde_t pdirs[1024];
};

void paging_init() {
    
}

#endif