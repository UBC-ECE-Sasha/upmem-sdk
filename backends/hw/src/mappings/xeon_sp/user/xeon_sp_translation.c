/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define _GNU_SOURCE
#include <stdint.h>

#include "dpu_region_address_translation.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <x86intrin.h>
#include <immintrin.h>

#include "static_verbose.h"

static struct verbose_control *this_vc;
static struct verbose_control *
__vc()
{
    if (this_vc == NULL) {
        this_vc = get_verbose_control_for("hw");
    }
    return this_vc;
}

#define NB_ELEM_MATRIX 8
#define NB_WRQ_FIFO_ENTRIES 100 // ??
#define WRQ_FIFO_ENTRY_SIZE 64 // TODO check that, cache line is 128B (?!)

#define for_each_dpu_in_rank(idx, ci, dpu, nb_cis, nb_dpus_per_ci)                                                               \
    for (dpu = 0, idx = 0; dpu < nb_dpus_per_ci; ++dpu)                                                                          \
        for (ci = 0; ci < nb_cis; ++ci, ++idx)

#define NB_THREADS 8
#define THREAD_MRAM_READ 0
#define THREAD_MRAM_WRITE 1

struct xeon_sp_private {
    struct dpu_region_address_translation *tr;

    void *base_region_addr;
    uint8_t direction;
    struct dpu_transfer_mram *xfer_matrix;

    pthread_t threads[NB_THREADS];

    pthread_barrier_t barrier_threads;
    pthread_mutex_t mutex_threads;
    pthread_cond_t cond_threads;

    bool threads_shall_exit;
    bool work_to_do;

    uint8_t nb_threads_awoken;
    uint8_t nb_dpus_per_thread;
    uint8_t dpu_id_thread;
};

/* Write NB_WRQ_FIFO_ENTRIES of 0 right after the CI */
void
flush_mc_fifo(void *base_ci_address)
{
    uint64_t *next_ci_address = (uint64_t *)base_ci_address;

    for (unsigned int i = 0; i < NB_WRQ_FIFO_ENTRIES; ++i) {
        for (unsigned int j = 0; j < WRQ_FIFO_ENTRY_SIZE / sizeof(uint64_t); ++j)
            next_ci_address[j] = (uint64_t)0ULL;

        __builtin_ia32_mfence();
        __builtin_ia32_clflush(next_ci_address);
        __builtin_ia32_mfence();

        next_ci_address += 8;
    }
}

void
byte_interleave(uint64_t *input, uint64_t *output)
{
    unsigned int i, j;

    for (i = 0; i < NB_ELEM_MATRIX; ++i)
        for (j = 0; j < sizeof(uint64_t); ++j)
            ((uint8_t *)&output[i])[j] = ((uint8_t *)&input[j])[i];
}

/* SSE4.1 and AVX2 implementations come from:
 * https://stackoverflow.com/questions/42162270/a-better-8x8-bytes-matrix-transpose-with-sse
 */
void
byte_interleave_sse4_1(uint64_t *input, uint64_t *output)
{
    char *A = (char *)input;
    char *B = (char *)output;

    __m128i pshufbcnst_0 = _mm_set_epi8(15, 7, 11, 3, 13, 5, 9, 1, 14, 6, 10, 2, 12, 4, 8, 0);

    __m128i pshufbcnst_1 = _mm_set_epi8(13, 5, 9, 1, 15, 7, 11, 3, 12, 4, 8, 0, 14, 6, 10, 2);

    __m128i pshufbcnst_2 = _mm_set_epi8(11, 3, 15, 7, 9, 1, 13, 5, 10, 2, 14, 6, 8, 0, 12, 4);

    __m128i pshufbcnst_3 = _mm_set_epi8(9, 1, 13, 5, 11, 3, 15, 7, 8, 0, 12, 4, 10, 2, 14, 6);
    __m128 B0, B1, B2, B3, T0, T1, T2, T3;

    B0 = _mm_loadu_ps((float *)&A[0]);
    B1 = _mm_loadu_ps((float *)&A[16]);
    B2 = _mm_loadu_ps((float *)&A[32]);
    B3 = _mm_loadu_ps((float *)&A[48]);

    B1 = _mm_shuffle_ps(B1, B1, 0b10110001);
    B3 = _mm_shuffle_ps(B3, B3, 0b10110001);
    T0 = _mm_blend_ps(B0, B1, 0b1010);
    T1 = _mm_blend_ps(B2, B3, 0b1010);
    T2 = _mm_blend_ps(B0, B1, 0b0101);
    T3 = _mm_blend_ps(B2, B3, 0b0101);

    B0 = _mm_castsi128_ps(_mm_shuffle_epi8(_mm_castps_si128(T0), pshufbcnst_0));
    B1 = _mm_castsi128_ps(_mm_shuffle_epi8(_mm_castps_si128(T1), pshufbcnst_1));
    B2 = _mm_castsi128_ps(_mm_shuffle_epi8(_mm_castps_si128(T2), pshufbcnst_2));
    B3 = _mm_castsi128_ps(_mm_shuffle_epi8(_mm_castps_si128(T3), pshufbcnst_3));

    T0 = _mm_blend_ps(B0, B1, 0b1010);
    T1 = _mm_blend_ps(B0, B1, 0b0101);
    T2 = _mm_blend_ps(B2, B3, 0b1010);
    T3 = _mm_blend_ps(B2, B3, 0b0101);
    T1 = _mm_shuffle_ps(T1, T1, 0b10110001);
    T3 = _mm_shuffle_ps(T3, T3, 0b10110001);

    _mm_storeu_ps((float *)&B[0], T0);
    _mm_storeu_ps((float *)&B[16], T1);
    _mm_storeu_ps((float *)&B[32], T2);
    _mm_storeu_ps((float *)&B[48], T3);
}

void
byte_interleave_avx2(uint64_t *input, uint64_t *output)
{
    __m256i tm = _mm256_set_epi8(15,
        11,
        7,
        3,
        14,
        10,
        6,
        2,
        13,
        9,
        5,
        1,
        12,
        8,
        4,
        0,

        15,
        11,
        7,
        3,
        14,
        10,
        6,
        2,
        13,
        9,
        5,
        1,
        12,
        8,
        4,
        0);
    char *src1 = (char *)input, *dst1 = (char *)output;

    __m256i vindex = _mm256_setr_epi32(0, 8, 16, 24, 32, 40, 48, 56);
    __m256i perm = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);

    __m256i load0 = _mm256_i32gather_epi32((int *)src1, vindex, 1);
    __m256i load1 = _mm256_i32gather_epi32((int *)(src1 + 4), vindex, 1);

    __m256i transpose0 = _mm256_shuffle_epi8(load0, tm);
    __m256i transpose1 = _mm256_shuffle_epi8(load1, tm);

    __m256i final0 = _mm256_permutevar8x32_epi32(transpose0, perm);
    __m256i final1 = _mm256_permutevar8x32_epi32(transpose1, perm);

    _mm256_storeu_si256((__m256i *)&dst1[0], final0);
    _mm256_storeu_si256((__m256i *)&dst1[32], final1);
}

void
byte_interleave_avx512(uint64_t *input, uint64_t *output, bool use_stream)
{
    __m512i mask;

    mask = _mm512_set_epi64(0x0f0b07030e0a0602ULL,
        0x0d0905010c080400ULL,

        0x0f0b07030e0a0602ULL,
        0x0d0905010c080400ULL,

        0x0f0b07030e0a0602ULL,
        0x0d0905010c080400ULL,

        0x0f0b07030e0a0602ULL,
        0x0d0905010c080400ULL);

    __m512i vindex = _mm512_setr_epi32(0, 8, 16, 24, 32, 40, 48, 56, 4, 12, 20, 28, 36, 44, 52, 60);
    __m512i perm = _mm512_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7, 8, 12, 9, 13, 10, 14, 11, 15);

    __m512i load = _mm512_i32gather_epi32(vindex, input, 1);
    __m512i transpose = _mm512_shuffle_epi8(load, mask);
    __m512i final = _mm512_permutexvar_epi32(perm, transpose);

    if (use_stream) {
        _mm512_stream_si512((void *)output, final);
        return;
    }

    _mm512_storeu_si512((void *)output, final);
}

#ifdef __AVX512VBMI__
void
byte_interleave_avx512vbmi(uint64_t *src, uint64_t *dst, bool use_stream)
{
    const __m512i trans8x8shuf = _mm512_set_epi64(0x0f0b07030e0a0602ULL,
        0x0d0905010c080400ULL,

        0x0f0b07030e0a0602ULL,
        0x0d0905010c080400ULL,

        0x0f0b07030e0a0602ULL,
        0x0d0905010c080400ULL,

        0x0f0b07030e0a0602ULL,
        0x0d0905010c080400ULL);

    __m512i vsrc = _mm512_loadu_si512(src);
    __m512i shuffled = _mm512_permutexvar_epi8(trans8x8shuf, vsrc);

    if (use_stream) {
        _mm512_stream_si512((void *)dst, shuffled);
        return;
    }

    _mm512_storeu_si512(dst, shuffled);
}
#endif

void
write_block_sse4_1(uint8_t *ci_address, uint64_t *data)
{
    __m128i v0 = _mm_set_epi64x(data[0], data[1]);
    __m128i v1 = _mm_set_epi64x(data[2], data[3]);
    __m128i v2 = _mm_set_epi64x(data[4], data[5]);
    __m128i v3 = _mm_set_epi64x(data[6], data[7]);

    _mm_stream_si128((__m128i *)&ci_address[0], v0);
    _mm_stream_si128((__m128i *)&ci_address[16], v1);
    _mm_stream_si128((__m128i *)&ci_address[32], v2);
    _mm_stream_si128((__m128i *)&ci_address[48], v3);
}

void
write_block_avx512(uint64_t *ci_address, uint64_t *data)
{
    volatile __m512i zmm;

    zmm = _mm512_setr_epi64(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    _mm512_stream_si512((void *)ci_address, zmm);
}

void
read_block_avx512(uint64_t *ci_address, uint64_t *output)
{
    volatile __m512i zmm;
    uint64_t *o = (uint64_t *)&zmm;

    zmm = _mm512_stream_load_si512((void *)ci_address);

    output[0] = o[0];
    output[1] = o[1];
    output[2] = o[2];
    output[3] = o[3];
    output[4] = o[4];
    output[5] = o[5];
    output[6] = o[6];
    output[7] = o[7];
}

static bool one_read = false;

void
xeon_sp_write_to_cis(__attribute__((unused)) struct dpu_region_address_translation *tr,
    void *base_region_addr,
    __attribute__((unused)) uint8_t channel_id,
    __attribute__((unused)) uint8_t rank_id,
    void *block_data,
    __attribute__((unused)) uint32_t block_size)
{
    uint64_t *ci_address;

    ci_address = (uint64_t *)((uint8_t *)base_region_addr + 0x20000);

    byte_interleave_avx512(block_data, ci_address, true);

    one_read = false;
}

void
xeon_sp_read_from_cis(__attribute__((unused)) struct dpu_region_address_translation *tr,
    void *base_region_addr,
    __attribute__((unused)) uint8_t channel_id,
    __attribute__((unused)) uint8_t rank_id,
    void *block_data,
    __attribute__((unused)) uint32_t block_size)
{
#define NB_READS 3
    uint64_t input[NB_ELEM_MATRIX];
    uint64_t *ci_address;
    int i;
    uint8_t nb_reads = one_read ? NB_READS - 1 : NB_READS;

    ci_address = (uint64_t *)((uint8_t *)base_region_addr + 0x20000 + 32 * 1024);

    for (i = 0; i < nb_reads; ++i) {
        /* FWIW: "data can be speculatively loaded into a cache line just
         * before, during, or after the execution of a CLFLUSH instruction that
         * references the cache line", Volume 2 of the Intel Architectures SW
         * Developer's Manual.
         */
        __builtin_ia32_clflushopt((uint8_t *)ci_address);
        __builtin_ia32_mfence();

        ((volatile uint64_t *)input)[0] = *(ci_address + 0);
        ((volatile uint64_t *)input)[1] = *(ci_address + 1);
        ((volatile uint64_t *)input)[2] = *(ci_address + 2);
        ((volatile uint64_t *)input)[3] = *(ci_address + 3);
        ((volatile uint64_t *)input)[4] = *(ci_address + 4);
        ((volatile uint64_t *)input)[5] = *(ci_address + 5);
        ((volatile uint64_t *)input)[6] = *(ci_address + 6);
        ((volatile uint64_t *)input)[7] = *(ci_address + 7);

        // printf("0x%" PRIx64 "\n", ((uint64_t *)block_data)[0]);
    }

    /* Do not use streaming instructions here because I observed that
     * dpu_planner is quite slowed down when it reads packet->data if
     * packet->data is not cached by this access./
     */
    byte_interleave_avx512(input, block_data, false);

    one_read = true;
}

#define BANK_START(dpu_id) (0x40000 * ((dpu_id) % 4) + ((dpu_id >= 4) ? 0x40 : 0))
#define BANK_OFFSET_NEXT_DATA(i) (i * 16) // For each 64bit word, you must jump 16 * 64bit (2 cache lines)
#define BANK_CHUNK_SIZE 0x20000
#define BANK_NEXT_CHUNK_OFFSET 0x100000

static uint32_t
apply_address_translation_on_mram_offset(uint32_t byte_offset)
{
    /* We have observed that, within the 26 address bits of the MRAM address, we need to apply an address translation:
     *
     * virtual[13: 0] = physical[13: 0]
     * virtual[20:14] = physical[21:15]
     * virtual[   21] = physical[   14]
     * virtual[25:22] = physical[25:22]
     *
     * This function computes the "virtual" mram address based on the given "physical" mram address.
     */

    uint32_t mask_21_to_15 = ((1 << (21 - 15 + 1)) - 1) << 15;
    uint32_t mask_21_to_14 = ((1 << (21 - 14 + 1)) - 1) << 14;
    uint32_t bits_21_to_15 = (byte_offset & mask_21_to_15) >> 15;
    uint32_t bit_14 = (byte_offset >> 14) & 1;
    uint32_t unchanged_bits = byte_offset & ~mask_21_to_14;

    return unchanged_bits | (bits_21_to_15 << 14) | (bit_14 << 21);
}

void
xeon_sp_write_to_rank(struct dpu_region_address_translation *tr,
    void *base_region_addr,
    __attribute__((unused)) uint8_t channel_id,
    __attribute__((unused)) uint8_t rank_id,
    struct dpu_transfer_mram *xfer_matrix)
{
    struct xeon_sp_private *xeon_sp_priv = tr->private;

    pthread_mutex_lock(&xeon_sp_priv->mutex_threads);
    /* Init transfer */
    xeon_sp_priv->direction = THREAD_MRAM_WRITE;
    xeon_sp_priv->xfer_matrix = xfer_matrix;
    xeon_sp_priv->base_region_addr = base_region_addr;
    xeon_sp_priv->work_to_do = true;
    /* Signal every thread */
    pthread_cond_broadcast(&xeon_sp_priv->cond_threads);
    pthread_mutex_unlock(&xeon_sp_priv->mutex_threads);

    pthread_barrier_wait(&xeon_sp_priv->barrier_threads);

    pthread_mutex_lock(&xeon_sp_priv->mutex_threads);
    xeon_sp_priv->dpu_id_thread = 0;
    pthread_mutex_unlock(&xeon_sp_priv->mutex_threads);
}

void
xeon_sp_read_from_rank(struct dpu_region_address_translation *tr,
    void *base_region_addr,
    __attribute__((unused)) uint8_t channel_id,
    __attribute__((unused)) uint8_t rank_id,
    struct dpu_transfer_mram *xfer_matrix)
{
    struct xeon_sp_private *xeon_sp_priv = tr->private;

    pthread_mutex_lock(&xeon_sp_priv->mutex_threads);
    /* Init transfer */
    xeon_sp_priv->direction = THREAD_MRAM_READ;
    xeon_sp_priv->xfer_matrix = xfer_matrix;
    xeon_sp_priv->base_region_addr = base_region_addr;
    xeon_sp_priv->work_to_do = true;
    /* Signal every thread */
    pthread_cond_broadcast(&xeon_sp_priv->cond_threads);
    pthread_mutex_unlock(&xeon_sp_priv->mutex_threads);

    pthread_barrier_wait(&xeon_sp_priv->barrier_threads);

    pthread_mutex_lock(&xeon_sp_priv->mutex_threads);
    xeon_sp_priv->dpu_id_thread = 0;
    pthread_mutex_unlock(&xeon_sp_priv->mutex_threads);
}

void
threads_write_to_rank(struct xeon_sp_private *xeon_sp_priv, uint8_t dpu_id_thread)
{
    struct dpu_transfer_mram *xfer_matrix = xeon_sp_priv->xfer_matrix;
    uint64_t cache_line[8];
    uint8_t idx, ci_id, dpu_id, nb_cis;

    nb_cis = xeon_sp_priv->tr->interleave->nb_real_ci;

    /* Works only for transfers:
     * - of same size and same offset on the same line
     * - size and offset are aligned on 8B
     */
    for (dpu_id = dpu_id_thread, idx = dpu_id_thread * 8; dpu_id < dpu_id_thread + xeon_sp_priv->nb_dpus_per_thread;
         ++dpu_id, idx += 8) {
        uint8_t *ptr_dest = (uint8_t *)xeon_sp_priv->base_region_addr + BANK_START(dpu_id);
        uint32_t size_transfer = 0, i;
        uint32_t offset_in_mram = 0;

        for (ci_id = 0; ci_id < nb_cis; ++ci_id) {
            if (xfer_matrix[idx + ci_id].ptr) {
                if (!size_transfer && !offset_in_mram) {
                    /* Check that access is aligned on 8B */
                    if (xfer_matrix[idx + ci_id].size & 0x7 || xfer_matrix[idx + ci_id].offset_in_mram & 0x7) {
                        LOGW(__vc(), "ERROR: MRAM transfer not aligned on 8B is not supported.");
                        return;
                    }
                    size_transfer = xfer_matrix[idx + ci_id].size;
                    offset_in_mram = xfer_matrix[idx + ci_id].offset_in_mram;
                } else {
                    /* Check that accesses are indeed all aligned with each other */
                    if (xfer_matrix[idx + ci_id].size != size_transfer
                        || xfer_matrix[idx + ci_id].offset_in_mram != offset_in_mram) {
                        LOGW(__vc(), "ERROR: MRAM transfers not aligned with each other is not supported.");
                        return;
                    }
                }
            }
        }

        if (!size_transfer)
            continue;

        for (i = 0; i < size_transfer / sizeof(uint64_t); ++i) {
            uint32_t mram_64_bit_word_offset = apply_address_translation_on_mram_offset(i * 8 + offset_in_mram) / 8;
            uint64_t next_data = BANK_OFFSET_NEXT_DATA(mram_64_bit_word_offset * sizeof(uint64_t));
            uint64_t offset = (next_data % BANK_CHUNK_SIZE) + (next_data / BANK_CHUNK_SIZE) * BANK_NEXT_CHUNK_OFFSET;

            for (ci_id = 0; ci_id < nb_cis; ++ci_id) {
                if (xfer_matrix[idx + ci_id].ptr)
                    cache_line[ci_id] = *((uint64_t *)xfer_matrix[idx + ci_id].ptr + i);
            }

            byte_interleave_avx512(cache_line, (uint64_t *)((uint8_t *)ptr_dest + offset), true);
        }

        __builtin_ia32_mfence();

        /* This is not efficient at all since:
         * "If a cache line is dirty, no other cache in the system is allowed to
         * have a copy of the cache line.  In this case, the hardware knows that it
         * only needs to initiate a writeback of the line and invalidate it in the
         * local cache, and it does not need to send any messages to any other caches
         * (or wait for responses to those messages).  After the first flush, the
         * data is not in the local cache, so the hardware has to do a global cache
         * invalidation and wait for the responses from all the other caches."
         * https://software.intel.com/en-us/forums/software-tuning-performance-optimization-platform-monitoring/topic/699950
         */
        for (i = 0; i < size_transfer / sizeof(uint64_t); ++i) {
            uint32_t mram_64_bit_word_offset = apply_address_translation_on_mram_offset(i * 8 + offset_in_mram) / 8;
            uint64_t next_data = BANK_OFFSET_NEXT_DATA(mram_64_bit_word_offset * sizeof(uint64_t));
            uint64_t offset = (next_data % BANK_CHUNK_SIZE) + (next_data / BANK_CHUNK_SIZE) * BANK_NEXT_CHUNK_OFFSET;

            __builtin_ia32_clflushopt((uint8_t *)ptr_dest + offset);
        }

        __builtin_ia32_mfence();
    }

    flush_mc_fifo((uint8_t *)xeon_sp_priv->base_region_addr + 0x20000);
}

void
threads_read_from_rank(struct xeon_sp_private *xeon_sp_priv, uint8_t dpu_id_thread)
{
    struct dpu_transfer_mram *xfer_matrix = xeon_sp_priv->xfer_matrix;
    uint64_t cache_line[8], cache_line_interleave[8];
    uint8_t idx, ci_id, dpu_id, nb_cis;

    nb_cis = xeon_sp_priv->tr->interleave->nb_real_ci;

    /* Works only for transfers of same size and same offset on the
     * same line
     */
    for (dpu_id = dpu_id_thread, idx = dpu_id_thread * 8; dpu_id < dpu_id_thread + xeon_sp_priv->nb_dpus_per_thread;
         ++dpu_id, idx += 8) {
        uint8_t *ptr_dest = (uint8_t *)xeon_sp_priv->base_region_addr + BANK_START(dpu_id);
        uint32_t size_transfer = 0, i;
        uint32_t offset_in_mram = 0;

        for (ci_id = 0; ci_id < nb_cis; ++ci_id) {
            if (xfer_matrix[idx + ci_id].ptr) {
                if (!size_transfer && !offset_in_mram) {
                    /* Check that access is aligned on 8B */
                    if (xfer_matrix[idx + ci_id].size & 0x7 || xfer_matrix[idx + ci_id].offset_in_mram & 0x7) {
                        LOGW(__vc(), "ERROR: MRAM transfer not aligned on 8B is not supported.");
                        return;
                    }
                    size_transfer = xfer_matrix[idx + ci_id].size;
                    offset_in_mram = xfer_matrix[idx + ci_id].offset_in_mram;
                } else {
                    /* Check that accesses are indeed all aligned with each other */
                    if (xfer_matrix[idx + ci_id].size != size_transfer
                        || xfer_matrix[idx + ci_id].offset_in_mram != offset_in_mram) {
                        LOGW(__vc(), "ERROR: MRAM transfers not aligned with each other is not supported.");
                        return;
                    }
                }
            }
        }

        if (!size_transfer)
            continue;

        __builtin_ia32_mfence();

        for (i = 0; i < size_transfer / sizeof(uint64_t); ++i) {
            uint32_t mram_64_bit_word_offset = apply_address_translation_on_mram_offset(i * 8 + offset_in_mram) / 8;
            uint64_t next_data = BANK_OFFSET_NEXT_DATA(mram_64_bit_word_offset * sizeof(uint64_t));
            uint64_t offset = (next_data % BANK_CHUNK_SIZE) + (next_data / BANK_CHUNK_SIZE) * BANK_NEXT_CHUNK_OFFSET;

            /* Invalidates possible prefetched cache line or old cache line */
            __builtin_ia32_clflushopt((uint8_t *)ptr_dest + offset);
        }

        __builtin_ia32_mfence();

        for (i = 0; i < size_transfer / sizeof(uint64_t); ++i) {
            uint32_t mram_64_bit_word_offset = apply_address_translation_on_mram_offset(i * 8 + offset_in_mram) / 8;
            uint64_t next_data = BANK_OFFSET_NEXT_DATA(mram_64_bit_word_offset * sizeof(uint64_t));
            uint64_t offset = (next_data % BANK_CHUNK_SIZE) + (next_data / BANK_CHUNK_SIZE) * BANK_NEXT_CHUNK_OFFSET;

            cache_line[0] = *((volatile uint64_t *)((uint8_t *)ptr_dest + offset + 0 * sizeof(uint64_t)));
            cache_line[1] = *((volatile uint64_t *)((uint8_t *)ptr_dest + offset + 1 * sizeof(uint64_t)));
            cache_line[2] = *((volatile uint64_t *)((uint8_t *)ptr_dest + offset + 2 * sizeof(uint64_t)));
            cache_line[3] = *((volatile uint64_t *)((uint8_t *)ptr_dest + offset + 3 * sizeof(uint64_t)));
            cache_line[4] = *((volatile uint64_t *)((uint8_t *)ptr_dest + offset + 4 * sizeof(uint64_t)));
            cache_line[5] = *((volatile uint64_t *)((uint8_t *)ptr_dest + offset + 5 * sizeof(uint64_t)));
            cache_line[6] = *((volatile uint64_t *)((uint8_t *)ptr_dest + offset + 6 * sizeof(uint64_t)));
            cache_line[7] = *((volatile uint64_t *)((uint8_t *)ptr_dest + offset + 7 * sizeof(uint64_t)));

            byte_interleave_avx2(cache_line, cache_line_interleave);

            for (ci_id = 0; ci_id < nb_cis; ++ci_id) {
                if (xfer_matrix[idx + ci_id].ptr) {
                    *((uint64_t *)xfer_matrix[idx + ci_id].ptr + i) = cache_line_interleave[ci_id];
                }
            }
        }
    }
}

void *
thread_mram(void *arg)
{
    struct xeon_sp_private *xeon_sp_priv = arg;
    uint8_t cur_dpu_id;

    while (1) {
        pthread_mutex_lock(&xeon_sp_priv->mutex_threads);

        if (!xeon_sp_priv->work_to_do)
            pthread_cond_wait(&xeon_sp_priv->cond_threads, &xeon_sp_priv->mutex_threads);

        xeon_sp_priv->nb_threads_awoken++;

        /* Once everyone is awoken, the last thread clears work_to_do: we must
         * clear work_to_do here, since it can't be done by main thread before
         * the barrier wait since some threads might not be awoken already.
         */
        if (xeon_sp_priv->nb_threads_awoken == NB_THREADS) {
            xeon_sp_priv->work_to_do = false;
            xeon_sp_priv->nb_threads_awoken = 0;
        }

        if (xeon_sp_priv->threads_shall_exit)
            break;

        cur_dpu_id = xeon_sp_priv->dpu_id_thread;
        xeon_sp_priv->dpu_id_thread += xeon_sp_priv->nb_dpus_per_thread;

        pthread_mutex_unlock(&xeon_sp_priv->mutex_threads);

        if (xeon_sp_priv->direction == THREAD_MRAM_READ)
            threads_read_from_rank(xeon_sp_priv, cur_dpu_id);
        else
            threads_write_to_rank(xeon_sp_priv, cur_dpu_id);

        pthread_barrier_wait(&xeon_sp_priv->barrier_threads);
    }

    pthread_mutex_unlock(&xeon_sp_priv->mutex_threads);

    return NULL;
}

// TODO retrieve environment variable that decides which byte_interleave function
// to use.
int
xeon_sp_init_region(struct dpu_region_address_translation *tr)
{
    struct xeon_sp_private *xeon_sp_priv;
    int i, ret;
    uint8_t nb_dpus_per_ci;

    nb_dpus_per_ci = tr->interleave->nb_dpus_per_ci;

    xeon_sp_priv = calloc(1, sizeof(struct xeon_sp_private));
    if (xeon_sp_priv == NULL)
        return -ENOMEM;

    tr->private = xeon_sp_priv;

    xeon_sp_priv->threads_shall_exit = false;
    xeon_sp_priv->work_to_do = false;
    pthread_mutex_init(&xeon_sp_priv->mutex_threads, NULL);
    pthread_cond_init(&xeon_sp_priv->cond_threads, NULL);

    ret = pthread_barrier_init(&xeon_sp_priv->barrier_threads, NULL, NB_THREADS + 1);
    if (ret)
        goto err;

    xeon_sp_priv->tr = tr;
    xeon_sp_priv->dpu_id_thread = 0;
    xeon_sp_priv->nb_dpus_per_thread = nb_dpus_per_ci / NB_THREADS;
    xeon_sp_priv->nb_threads_awoken = 0;

    for (i = 0; i < NB_THREADS; ++i) {
        ret = pthread_create(&xeon_sp_priv->threads[i], NULL, thread_mram, xeon_sp_priv);
        if (ret)
            goto kill_threads;
    }

    return 0;

kill_threads:
    pthread_mutex_lock(&xeon_sp_priv->mutex_threads);
    xeon_sp_priv->threads_shall_exit = true;
    pthread_cond_broadcast(&xeon_sp_priv->cond_threads);
    pthread_mutex_unlock(&xeon_sp_priv->mutex_threads);
err:
    free(xeon_sp_priv);

    return ret;
}

void
xeon_sp_destroy_region(struct dpu_region_address_translation *tr)
{
    struct xeon_sp_private *xeon_sp_priv;
    int i;

    xeon_sp_priv = tr->private;

    pthread_mutex_lock(&xeon_sp_priv->mutex_threads);
    xeon_sp_priv->threads_shall_exit = true;
    pthread_cond_broadcast(&xeon_sp_priv->cond_threads);
    pthread_mutex_unlock(&xeon_sp_priv->mutex_threads);

    for (i = 0; i < NB_THREADS; ++i) {
        pthread_join(xeon_sp_priv->threads[i], NULL);
    }

    pthread_barrier_destroy(&xeon_sp_priv->barrier_threads);

    free(xeon_sp_priv);
}

struct dpu_region_interleaving xeon_sp_interleave = {
    .nb_channels = 1,
    .nb_dimms_per_channel = 1,
    .nb_ranks_per_dimm = 1,
    .nb_ci = 1,
    .nb_real_ci = 8,
    .nb_dpus_per_ci = 8,
    .mram_size = 64 * 1024 * 1024,
    .channel_line_size = 128,
    .rank_line_size = 64,
};

struct dpu_region_address_translation xeon_sp_translate = {
    .interleave = &xeon_sp_interleave,
    .backend_id = DPU_BACKEND_XEON_SP,
    .capabilities = CAP_PERF | CAP_SAFE,
    .init_region = xeon_sp_init_region,
    .destroy_region = xeon_sp_destroy_region,
    //.init_rank	        = xeon_sp_init_rank,
    //.destroy_rank         = xeon_sp_destroy_rank,
    .write_to_rank = xeon_sp_write_to_rank,
    .read_from_rank = xeon_sp_read_from_rank,
    .write_to_cis = xeon_sp_write_to_cis,
    .read_from_cis = xeon_sp_read_from_cis,
};
