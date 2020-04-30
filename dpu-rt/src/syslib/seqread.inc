/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <seqread.h>
#include <mram.h>
#include <dpuconst.h>
#include <dpuruntime.h>
#include <atomic_bit.h>

#if PAGE_SIZE_LOG2 == 5

#define PAGE_SIZE 32
#define PAGE_ALLOC_SIZE 64

#elif PAGE_SIZE_LOG2 == 6

#define PAGE_SIZE 64
#define PAGE_ALLOC_SIZE 128

#elif PAGE_SIZE_LOG2 == 7

#define PAGE_SIZE 128
#define PAGE_ALLOC_SIZE 256

#elif PAGE_SIZE_LOG2 == 8

#define PAGE_SIZE 256
#define PAGE_ALLOC_SIZE 512

#else

#error "Invalid PAGE_SIZE_LOG2"

#endif

#define PAGE_OFF_MASK                                   (PAGE_SIZE - 1)
#define PAGE_IDX_MASK                                   (~PAGE_OFF_MASK)
#define WRAM_SEGMENT_SHIFT                              (__DPU_MRAM_SIZE_LOG2 - PAGE_SIZE_LOG2)
#define WRAM_SEGMENT_MASK                               ((1 << WRAM_SEGMENT_SHIFT) - 1)

#define JOIN(x, y) JOIN_AGAIN(x, y)
#define JOIN_AGAIN(x, y) x ## y

#define SEQREAD_ALLOC JOIN(JOIN(seqread, PAGE_SIZE), _alloc)
#define SEQREAD_INIT  JOIN(JOIN(seqread, PAGE_SIZE), _init)
#define SEQREAD_GET   JOIN(JOIN(seqread, PAGE_SIZE), _get)

#define MRAM_READ_PAGE(from, to)     mram_read(from, to, PAGE_ALLOC_SIZE)

ATOMIC_BIT_EXTERN(__heap_pointer);

void* SEQREAD_ALLOC(void)
{
    ATOMIC_BIT_ACQUIRE(__heap_pointer);

    unsigned int pointer = (__HEAP_POINTER + PAGE_OFF_MASK) & PAGE_IDX_MASK;
    unsigned int new_heap_pointer = pointer + PAGE_ALLOC_SIZE;
    unsigned int dummy;

    __asm__ volatile("lbu %[dumb], %[nhp], -1" : [dumb] "=r" (dummy) : [nhp] "r" (new_heap_pointer));

    __HEAP_POINTER = new_heap_pointer;

    ATOMIC_BIT_RELEASE(__heap_pointer);

    return (void *) pointer;
}

seqreader_t SEQREAD_INIT(void *cache, __mram_ptr void *mram_addr, void **ptr) {
    unsigned int wram_cache = ((unsigned int) cache);
    __mram_ptr void *aligned_mram_addr = (__mram_ptr void *)((uintptr_t)mram_addr & PAGE_IDX_MASK);

    MRAM_READ_PAGE(aligned_mram_addr, cache);

    *((unsigned int *) ptr) = wram_cache + ((uintptr_t)mram_addr & PAGE_OFF_MASK);

    return ((wram_cache >> PAGE_SIZE_LOG2) << WRAM_SEGMENT_SHIFT) | ((uintptr_t)aligned_mram_addr >> PAGE_SIZE_LOG2);;
}

void* SEQREAD_GET(void *ptr, uint8_t inc, seqreader_t *reader) {
    void* result;
    unsigned int still_in_buffer = 1;

    __asm__ volatile("add %[last], %[p], %[i], nc" __STR(PAGE_SIZE_LOG2) ", . + 2 ; move %[b], 0" : [last] "=r" (result), [b] "+r" (still_in_buffer) : [p] "r" (ptr), [i] "r" (inc));

    if (still_in_buffer == 0) {
        seqreader_t seqreader = *reader + 1;
        *reader = seqreader;

        __mram_ptr void *aligned_mram_addr = (__mram_ptr void *)((seqreader & WRAM_SEGMENT_MASK) << PAGE_SIZE_LOG2);
        void* wram_cache = (void *) ((seqreader >> WRAM_SEGMENT_SHIFT) << PAGE_SIZE_LOG2);

        MRAM_READ_PAGE(aligned_mram_addr, wram_cache);
        result = wram_cache + (((unsigned int) result) & PAGE_OFF_MASK);
    }

    return result;
}
