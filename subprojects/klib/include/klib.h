#ifndef KLIB_H
#define KLIB_H

#include <stdint.h>
#include "pmm.h"

#define NULL 0
#define HEAP_SUBBLOCKSIZE 32 // Size in bytes of each block in the heap
#define HEAP_SBLK_PER_BLOCK (PMM_BLOCKSIZE / HEAP_SUBBLOCKSIZE)

// Gets the amount of subblocks needed to contain a certain amount of bytes.
// Don't worry about the division, as long as HEAP_SUBBLOCKSIZE is a power of 2
// it'll automatically be optimised to a bit shift!
#define N_SUBBLOCKS_CONTAINING(size) ((size + HEAP_SUBBLOCKSIZE) / HEAP_SUBBLOCKSIZE)

typedef unsigned int size_t;

struct block_meta_t {
    struct block_meta_t *next; // Next block in the linked list, 0 if none
    uint32_t free; // Amount of free subblocks in this block
    uint32_t blocks; // Amount of memory blocks that make up this block
    // The bitmap of subblocks starts at the second subblock of this block.
    // The metadata is in the first subblock. Therefore, HEAP_SUBBLOCKSIZE
    // must be >= the size of the metadata.
};

void *kmalloc(size_t size);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t size);

void *kmemset(void *ptr, char c, size_t n);
void *kmemcpy(void *dest, void *src, size_t n);
char *kstrchr(const char *s, char c);
char *kstrrchr(const char *s, char c);
size_t kstrlen(const char *s);

/*
Returns 
 - 0, if s1 and s2 are equal
 - a negative value, if s1 < s2
 - a positive value, if s1 > s2
*/
int kstrcmp(const char *s1, const char *s2);
int kstrncmp(const char *s1, const char *s2, int n);

double katof(const char *);
int katoi(const char *);
long katol(const char *);
long long katoll(const char *);

void ksrand(uint32_t);
int krand();

#endif