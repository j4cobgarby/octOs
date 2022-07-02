#include "klib.h"
#include "pmm.h"
#include "kio.h"

struct block_meta_t *_kheap_first = NULL;

void _kheap_add_block(void *addr, uint32_t blocks) {
    // Create block metadata at beginning of the block
    struct block_meta_t *meta = (struct block_meta_t *)addr;
    meta->blocks = blocks;

    uint8_t *bitmap = addr + HEAP_SUBBLOCKSIZE;
    unsigned int subblocks_count = HEAP_SBLK_PER_BLOCK*blocks;

    if (_kheap_first == NULL) {
        // Set beginning of the heap linked list
        _kheap_first = meta;
    } else {
        // Add the new block to the beginning of the linked list.
        meta->next = _kheap_first;
        _kheap_first = meta;
    }

    meta->free = subblocks_count;

    kio_printf("[HEAP] Adding new block to heap.\n%d blocks long.\nAddress: %x\n\
%d subblocks long.\n", blocks, (uint32_t)addr, subblocks_count);

    for (unsigned int i = 0; i < subblocks_count; i++) {
        bitmap[i] = 0; // Set all subblocks to free
    }

    kio_printf("[HEAP] Reserving %d subblocks for metadata.\n", 
        1+N_SUBBLOCKS_CONTAINING(subblocks_count));
    for (unsigned int i = 0; i < 1+N_SUBBLOCKS_CONTAINING(subblocks_count); i++) {
        bitmap[i] = 1; // Reserve subblocks for metadata and the bitmap itself
        meta->free--;
    }
}

inline uint8_t _kheap_make_uid(uint8_t left, uint8_t right) {
    uint8_t ret;
    for (ret = left+1; ret == left || ret == right || ret == 0; ret++);
    return ret;
}

// Calculate the index of the first subblock (after metadata) in a given block
int first_user_subblock(struct block_meta_t *meta) {
    return 1 + N_SUBBLOCKS_CONTAINING(HEAP_SBLK_PER_BLOCK * meta->blocks);
}

void *kmalloc(size_t size) {
    struct block_meta_t *current_block;
    unsigned int subblocks = N_SUBBLOCKS_CONTAINING(size);
    unsigned int found = 0;
    unsigned int start;
    int added_new_block = 0;

    if (_kheap_first == NULL) {
        _kheap_add_block(pmm_alloc(), 1);
    }

    for (current_block = _kheap_first; current_block != NULL; 
            current_block = current_block->next) {
        if (current_block->free < subblocks) continue;

        uint8_t *bitmap = (uint8_t*)((uint32_t)current_block + HEAP_SUBBLOCKSIZE);

        // Look for a run of size subblocks
        start = 0;
        for (unsigned int i = 0; i < HEAP_SBLK_PER_BLOCK * current_block -> blocks; i++) {
            if (bitmap[i] != 0) {
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
                bitmap[start-1];
            uint8_t right_uid = start == HEAP_SBLK_PER_BLOCK * current_block->blocks - 1 ?
                0 : bitmap[start]+subblocks;
            uint8_t uid = _kheap_make_uid(left_uid, right_uid);

            // Return a pointer to the allocated memory
            for (unsigned int i = 0; i < subblocks; i++) {
                uint8_t *bitmap = (uint8_t*)((uint32_t)current_block + HEAP_SUBBLOCKSIZE);
                bitmap[start+i] = uid;
            }

            return ((uint8_t*)current_block) + start * HEAP_SUBBLOCKSIZE;
        }
    }

    // At this point, none of the blocks were suitable, so we must add a new one
    // The amount of subblocks we need is:
    // subblocks containing(`size`), for the actual requested data storage
    // + 1 for the metadata
    // + a varying amount for the bitmap, which depends on the amount of blocks

    for (unsigned int blocks = 1; ; blocks++) {
        unsigned int subs = N_SUBBLOCKS_CONTAINING(size) + 1 +
            N_SUBBLOCKS_CONTAINING(HEAP_SBLK_PER_BLOCK*blocks);
        if (subs <= HEAP_SBLK_PER_BLOCK*blocks) {
            // This amount of blocks will suffice
            kio_printf("[HEAP] Adding new block of size %d blocks\n", blocks);
            void *addr = pmm_allocs(blocks);
            if (addr != NULL) {
                _kheap_add_block(addr, blocks);
            } else {
                kio_printf("[HEAP] We appear to have run out of memory :(");
                return NULL;
            }

            uint8_t *bitmap = (uint8_t*)((uint32_t)addr + HEAP_SUBBLOCKSIZE);
            unsigned int user_sblk = first_user_subblock((struct block_meta_t*)addr);

            for (unsigned int i = 0; i < subblocks; i++) {
                // '2' is a bitmap value that won't yet have been used in this block
                // since it's a new block
                bitmap[user_sblk + i] = 2;
            }

            return addr + user_sblk * HEAP_SUBBLOCKSIZE;
        }
    }

    return NULL;
}

void kfree(void *ptr) {
    struct block_meta_t *current_block;

    for (current_block = _kheap_first; current_block != NULL; 
            current_block = current_block->next) {
        if (ptr > (void*)current_block && 
                ptr < (void*)current_block + PMM_BLOCKSIZE*current_block->blocks) {
            // Found the block that the pointer to free is in.
            uint8_t *bitmap = (uint8_t*)((uint32_t)current_block + HEAP_SUBBLOCKSIZE);
            unsigned int block_offset = ptr - (void*)current_block;
            unsigned int subblock = block_offset / HEAP_SUBBLOCKSIZE;
            uint8_t uid = bitmap[subblock];
            for (unsigned int i = subblock; 
                    bitmap[i] == uid; i++) {
                bitmap[i] = 0;
                current_block->free++;
            }
        }
    }
}