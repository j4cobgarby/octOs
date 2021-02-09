#include "klib.h"
#include "pmm.h"
#include "kio.h"

struct block_meta_t *_kheap_first = NULL;

void _kheap_add_block(void *addr) {
    // Create block metadata at beginning of the block
    struct block_meta_t *meta = (struct block_meta_t *)addr;

    if (_kheap_first == NULL) {
        // Set beginning of the heap linked list
        _kheap_first = meta;
    } else {
        // Add the new block to the beginning of the linked list.
        meta->next = _kheap_first;
        _kheap_first = meta;
    }

    // Free all blocks
    meta->free = PMM_BLOCKSIZE / HEAP_SUBBLOCKSIZE;
    for (unsigned int i = 0; i < PMM_BLOCKSIZE / HEAP_SUBBLOCKSIZE; i++) {
        meta->subblocks[i] = 0; // Set all blocks to free
    }

    // Allocate subblocks for the metadata
    for (unsigned int i = 0; 
            i < N_SUBBLOCKS_CONTAINING(sizeof(struct block_meta_t)); i++) {
        meta->subblocks[i] = 1;
        meta->free--;
    }
}

inline uint8_t _kheap_make_uid(uint8_t left, uint8_t right) {
    uint8_t ret;
    for (ret = left+1; ret == left || ret == right || ret == 0; ret++);
    return ret;
}

// TODO: Make malloc be able to allocate more memory than there is in one block
void *kmalloc(size_t size) {
    struct block_meta_t *current_block;
    unsigned int subblocks = N_SUBBLOCKS_CONTAINING(size);
    unsigned int found = 0;
    unsigned int start;
    int added_new_block = 0;

    if (size > PMM_BLOCKSIZE) {
        return NULL;
    }

    if (_kheap_first == NULL) {
        _kheap_add_block(pmm_alloc());
        kio_printf("New block @ %x\n", _kheap_first);
    }

    for (current_block = _kheap_first; current_block != NULL; 
            current_block = current_block->next) {
        if (current_block->free < subblocks) continue;

        // Look for a run of size subblocks
        start = 0;
        for (unsigned int i = 0; i < PMM_BLOCKSIZE/HEAP_SUBBLOCKSIZE; i++) {
            if (current_block->subblocks[i] != 0) {
                // This subblock is taken
                found = 0;
            } else {
                // This subblock is free
                if (found++ == 0) start = i;
                if (found == subblocks) break;
            }
        }

        if (found == subblocks) {
            // Find an id for the subblock which is different the id of the
            // subblocks either side of it
            uint8_t left_uid = start == 0 ? 0 :
                current_block->subblocks[start-1];
            uint8_t right_uid = start == PMM_BLOCKSIZE/HEAP_SUBBLOCKSIZE - 1 ?
                0 : current_block->subblocks[start]+subblocks;
            uint8_t uid = _kheap_make_uid(left_uid, right_uid);

            

            // Return a pointer to the allocated memory
            for (unsigned int i = 0; i < subblocks; i++) {
                current_block->subblocks[start+i] = uid;
            }

            return ((uint8_t*)current_block) + start * HEAP_SUBBLOCKSIZE;
        } else {
            // Not enough free memory in this block
            if (current_block->next == NULL && !added_new_block) {
                // Add a new block to the heap, if memory wasn't found in this
                // block *and* there are no more blocks following it.
                // Also this makes sure it can only add one new block, so it
                // can't accidentally go into any infinite loops.
                _kheap_add_block(pmm_alloc());
                added_new_block = 1;
            }

            continue;
        }
    }

    return NULL;
}

void kfree(void *ptr) {
    struct block_meta_t *current_block;

    for (current_block = _kheap_first; current_block != NULL; 
            current_block = current_block->next) {
        if (ptr > (void*)current_block && 
                ptr < (void*)current_block+PMM_BLOCKSIZE) {
            // Found the block that the pointer to free is in.
            unsigned int block_offset = ptr - (void*)current_block;
            unsigned int subblock = block_offset / HEAP_SUBBLOCKSIZE;
            uint8_t uid = current_block->subblocks[subblock];
            for (unsigned int i = subblock; 
                    current_block->subblocks[i] == uid; i++) {
                current_block->subblocks[i] = 0;
                current_block->free++;
            }
        }
    }
}