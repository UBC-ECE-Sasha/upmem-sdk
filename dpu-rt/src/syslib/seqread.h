/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPUSYSCORE_SEQREAD_H
#define DPUSYSCORE_SEQREAD_H

/**
 * @file seqread.h
 * @brief Sequential reading of items in MRAM.
 *
 * A sequential reader allows to parse a contiguous area in MRAM in sequence.
 * For example, if the MRAM contains an array of N structures, a sequential
 * reader on this array will automatically fetch the data into WRAM, thus
 * simplify the iterative loop on the elements.
 *
 * The size of cached area is defined by default but can be overriden by
 * defining this value in SEQREAD_CACHE_SIZE.
 *
 * The use of a sequential reader implies:
 *
 *  - first, to allocate some storage in WRAM to cache the items, using seqread_alloc.
 *  - then to initialize a reader on the MRAM area, via seqread_init
 *  - finally to iterate on the elements, invoking seqread_get whenever a new item is accessed.
 *
 */

#include <stdint.h>
#include <mram.h>

#ifndef SEQREAD_CACHE_SIZE
/**
 * @def SEQREAD_CACHE_SIZE
 * @hideinitializer
 * @brief Size of caches used by seqread.
 */
#define SEQREAD_CACHE_SIZE 256
#endif

/// @cond SEQREADINTERNAL
#if (SEQREAD_CACHE_SIZE == 32)
#define __SEQREAD_ALLOC seqread32_alloc
#define __SEQREAD_INIT seqread32_init
#define __SEQREAD_GET seqread32_get
#elif (SEQREAD_CACHE_SIZE == 64)
#define __SEQREAD_ALLOC seqread64_alloc
#define __SEQREAD_INIT seqread64_init
#define __SEQREAD_GET seqread64_get
#elif (SEQREAD_CACHE_SIZE == 128)
#define __SEQREAD_ALLOC seqread128_alloc
#define __SEQREAD_INIT seqread128_init
#define __SEQREAD_GET seqread128_get
#elif (SEQREAD_CACHE_SIZE == 256)
#define __SEQREAD_ALLOC seqread256_alloc
#define __SEQREAD_INIT seqread256_init
#define __SEQREAD_GET seqread256_get
#else
#error "seqread error: invalid cache size defined"
#endif /* SEQREAD_CACHE_SIZE ==... */
/// @endcond

/**
 * @typedef seqreader_t
 * @brief An object used to perform sequential reading of MRAM.
 */
typedef uint32_t seqreader_t;

void *
__SEQREAD_ALLOC();

/**
 * @fn seqread_alloc
 * @brief Initializes an area in WRAM to cache the read buffers.
 *
 * Notice that this buffer can be re-used for different sequential reads,
 * as long as it is initialized each time to a new buffer in MRAM.
 *
 * @return A pointer to the allocated cache base address.
 */
#define seqread_alloc __SEQREAD_ALLOC

seqreader_t
__SEQREAD_INIT(void *cache, __mram_ptr void *mram_addr, void **ptr);

/**
 * @fn seqread_init
 * @brief Creates a sequential reader.
 *
 * The reader is associated to an existing cache in WRAM, created with
 * seqread_alloc and a contiguous area of data in MRAM. The function
 * loads the first pages of data into the cache and provides a pointer
 * to the first byte in cache actually mapping the expected data.
 *
 * Notice that the provided MRAM address does not need to be aligned on
 * any constraint: the routine does the alignment automatically.
 *
 * @param cache the reader's cache in WRAM
 * @param mram_addr the buffer address in MRAM
 * @param ptr set to the address of the first byte in cache
 * @return A new sequential reader to map the supplied MRAM address.
 */
#define seqread_init __SEQREAD_INIT

void *
__SEQREAD_GET(void *ptr, uint8_t inc, seqreader_t *reader);

/**
 * @fn seqread_get
 * @brief Fetches the next item in a sequence.
 *
 * This operation basically consists in incrementing the pointer that goes
 * through the mapped area of memory. The function automatically reloads
 * data from cache if necessary.
 *
 * As a result, the provided pointer to the cache area is set to its new value.
 *
 * The provided increment must be less than SEQREAD_CACHE_SIZE. The reader's
 * behavior is undefined if the increment exceeds this value.
 *
 * @param ptr the incremented pointer
 * @param inc the number of bytes added to this pointer
 * @param reader a pointer to the sequential reader
 * @return The updated pointer value.
 */
#define seqread_get __SEQREAD_GET

#endif /* DPUSYSCORE_SEQREAD_H */
