/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stddef.h>
#include <stdint.h>

void *
memcpy(void *dest, const void *src, size_t len)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    uint32_t *dw;
    const uint32_t *sw;
    uint8_t *head;
    uint8_t *const tail = (uint8_t *)dest + len;
    /* Set 'body' to the last word boundary */
    uint32_t *const body = (uint32_t *)((uintptr_t)tail & ~3);

    if (((uintptr_t)dest & 3) != ((uintptr_t)src & 3)) {
        /* Misaligned. no body, no tail. */
        head = tail;
    } else {
        /* Aligned */
        if ((uintptr_t)tail < (((uintptr_t)d + 3) & ~3))
            /* len is shorter than the first word boundary */
            head = tail;
        else
            /* Set 'head' to the first word boundary */
            head = (uint8_t *)(((uintptr_t)d + 3) & ~3);
    }

    /* Copy head */
    uint32_t head_len = head - d;
    if (head_len != 0) {
        for (uint32_t i = 0; i < head_len; ++i)
            d[i] = s[i];
    }

    /* Copy body */
    dw = (uint32_t *)(d + head_len);
    sw = (uint32_t *)(s + head_len);

    uint32_t body_len = (body < dw) ? 0 : body - dw;
    if (body_len != 0) {
        for (uint32_t i = 0; i < body_len; ++i)
            dw[i] = sw[i];
    }

    /* Copy tail */
    d = (uint8_t *)(dw + body_len);
    s = (const uint8_t *)(sw + body_len);
    uint32_t tail_len = tail - d;
    if (tail_len != 0) {
        for (uint32_t i = 0; i < tail_len; ++i)
            d[i] = s[i];
    }

    return dest;
}
