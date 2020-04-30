/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stddef.h>
#include <stdint.h>

void *
memset(void *dest, int c, size_t len)
{
    char *d = (char *)dest;
    uint32_t cccc;
    uint32_t *dw;
    char *head;
    char *const tail = (char *)dest + len;
    /* Set 'body' to the last word boundary */
    uint32_t *const body = (uint32_t *)((uintptr_t)tail & ~3);

    c &= 0xff; /* Clear upper bits before ORing below */
    cccc = c | (c << 8) | (c << 16) | (c << 24);

    if ((uintptr_t)tail < (((uintptr_t)d + 3) & ~3))
        /* len is shorter than the first word boundary */
        head = tail;
    else
        /* Set 'head' to the first word boundary */
        head = (char *)(((uintptr_t)d + 3) & ~3);

    /* Copy head */
    uint32_t head_len = head - d;
    if (head_len != 0) {
        for (uint32_t i = 0; i < head_len; ++i)
            d[i] = c;
    }

    /* Copy body */
    dw = (uint32_t *)(d + head_len);

    uint32_t body_len = (body < dw) ? 0 : body - dw;
    if (body_len != 0) {
        for (uint32_t i = 0; i < body_len; ++i)
            dw[i] = cccc;
    }

    /* Copy tail */
    d = (char *)(dw + body_len);

    uint32_t tail_len = tail - d;
    if (tail_len != 0) {
        for (uint32_t i = 0; i < tail_len; ++i)
            d[i] = c;
    }

    return dest;
}
