/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: dtcf qsort
* Author: Media Software Group
* Create: 2025-09-20
*/

#ifndef DTCF_QSORT_H_
#define DTCF_QSORT_H_

#include <stddef.h>

#define MIN(A, B) (A) < (B) ? (A) : (B)

static inline void Swap(char *a, char *b, int n)
{
    long i = n;
    char *pi = a, *pj = b;
    if (a < b) {
        pi = a + n - 1, pj = b + n - 1;
    }
    do {
        char t = *pi;
        *pi-- = *pj, *pj-- = t;
    } while (--i > 0);
}

static inline void VecSwap(char *a, char *b, int n)
{
    if (n > 0) {
        Swap(a, b, n);
    }
}

static inline char *Med3(char *a, char *b, char *c, int (*cmp)(const void *a, const void *b))
{
    return cmp(a, b) < 0 ? (cmp(b, c) < 0 ? b : (cmp(a, c) < 0 ? c : a)) :
                           (cmp(b, c) > 0 ? b : (cmp(a, c) < 0 ? a : c));
}

static void SubSwap(char *a, size_t n, size_t es, int (*cmp)(const void *a, const void *b))
{
    char *pm = a + (n / 2) * es; // 2:get mid pointer
    if (n > 7) { // 7: sort list big than 7
        char *pl;
        char *pn;
        pl = a;
        pn = a + (n - 1) * es;
        if (n > 40) { // 40: sort list big than 40
            int d = (n / 8) * es; // 8: 1/8 of n as distance
            pl = Med3(pl, pl + d, pl + 2 * d, cmp); // 2: double distance
            pm = Med3(pm - d, pm, pm + d, cmp);
            pn = Med3(pn - 2 * d, pn - d, pn, cmp); // 2:doubule distance
        }
        pm = Med3(pl, pm, pn, cmp);
    }
    Swap(a, pm, es);
}

void Qsort(void *a, size_t n, size_t es, int (*cmp)(const void *a, const void *b))
{
    char *pa, *pb, *pc, *pd, *pn;
    int ret;

    SubSwap(a, n, es, cmp);

    pa = pb = a;
    pc = pd = (char *)a + (n - 1) * es;
    for (;;) {
        while (pb <= pc && (ret = cmp(pb, a)) <= 0) {
            if (ret == 0) {
                Swap(pa, pb, es);
                pa += es;
            }
            pb += es;
        }
        while (pc >= pb && (ret = cmp(pc, a)) >= 0) {
            if (ret == 0) {
                Swap(pc, pd, es);
                pd -= es;
            }
            pc -= es;
        }

        if (pb > pc) {
            break;
        }
        Swap(pb, pc, es);
        pb += es;
        pc -= es;
    }

    pn = (char *)a + n * es;
    ret = MIN(pa - (char *)a, pb - pa);
    VecSwap(a, pb - ret, ret);
    ret = MIN(pd - pc, pn - pd - (int)es);
    VecSwap(pb, pn - ret, ret);
    if ((ret = pb - pa) > (int)es) {
        qsort(a, ret / es, es, cmp);
    }

    if ((ret = pd - pc) > (int)es) {
        qsort(pn - ret, ret / es, es, cmp);
    }
}

#endif /* DTCF_QSORT_H_ */
