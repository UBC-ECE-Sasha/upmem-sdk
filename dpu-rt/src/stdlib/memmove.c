/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <string.h>
#include <stdint.h>

void *
memmove(void *dest, const void *src, size_t len)
{
    if ((uintptr_t)dest <= (uintptr_t)src || (uintptr_t)dest >= (uintptr_t)src + len) {
        /* Start of destination doesn't overlap source, so just use
         * memcpy(). */
        return memcpy(dest, src, len);
    } else {
        /* Need to copy from tail because there is overlap. */
        char *d = (char *)dest + len;
        const char *s = (const char *)src + len;
        uint32_t *dw;
        const uint32_t *sw;
        char *head;
        char *const tail = (char *)dest;
        /* Set 'body' to the last word boundary */
        uint32_t *const body = (uint32_t *)(((uintptr_t)tail + 3) & ~3);

        if (((uintptr_t)dest & 3) != ((uintptr_t)src & 3)) {
            /* Misaligned. no body, no tail. */
            head = tail;
        } else {
            /* Aligned */
            if ((uintptr_t)tail > ((uintptr_t)d & ~3))
                /* Shorter than the first word boundary */
                head = tail;
            else
                /* Set 'head' to the first word boundary */
                head = (char *)((uintptr_t)d & ~3);
        }

        /* Copy head */
        uint32_t head_len = d - head;
        for (int32_t i = head_len - 1; i >= 0; --i)
            d[i - head_len] = s[i - head_len];

        /* Copy body */
        dw = (uint32_t *)(d - head_len);
        sw = (uint32_t *)(s - head_len);

        uint32_t body_len = (dw < body) ? 0 : dw - body;
        for (int32_t i = body_len - 1; i >= 0; --i)
            dw[i - body_len] = sw[i - body_len];

        /* Copy tail */
        d = (char *)(dw - body_len);
        s = (const char *)(sw - body_len);

        uint32_t tail_len = d - tail;
        for (int32_t i = tail_len - 1; i >= 0; --i)
            d[i - tail_len] = s[i - tail_len];

        return dest;
    }
}
