/*
 * Copyright (c) CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: source code file for overlay module
 * Author: audio
 */

#include "core_overlay.h"
#include "component.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static td_u32 align_up(td_u32 size)
{
    size += 0x10;
    size -= (size & 0xf);
    return size;
}

typedef struct {
    td_void *addr;
    size_t size;
} heap_buffer;

#define HEAP_IN_OVERLAY

td_s32 __errno = 0;
static td_char g_overlay_tag[OVERLAY_NAME_SIZE] = {0};
static overlay_func *g_overlay_func = TD_NULL;

extern td_u8 _end[];
extern td_u8 _heap_sentry[];

extern td_u8 _bss_start[];
extern td_u8 _bss_end[];

static td_u8 *g_heap_offset = TD_NULL;

static td_void clear_bss(td_void)
{
    td_u8 *c = TD_NULL;

    for (c = _bss_start; c < _bss_end; c++) {
        *c = 0;
    }
}

static inline td_void heap_init(td_void)
{
    td_u32 off;

    off = (td_u32)(uintptr_t)_end;
    off = align_up(off);
    g_heap_offset = (td_u8 *)(uintptr_t)off;
}

static td_void core_overlay_init(overlay_func *func, td_char *lib)
{
    g_overlay_func = func;
    g_overlay_func->memcpy_s(g_overlay_tag, OVERLAY_NAME_SIZE, lib, OVERLAY_NAME_SIZE);
    g_overlay_tag[OVERLAY_NAME_SIZE - 1] = '\0';

    heap_init();
}

int vsnprintf_s(char *str, size_t dest_max, size_t size, const char *format, va_list arg)
{
    return g_overlay_func->vsnprintf_s(str, dest_max, size, format, arg);
}

int printf(const char *fmt, ...)
{
    int ret;
    va_list args;
    char str[1024] = {'\0'}; /* maximum 1024 characters */

    va_start(args, fmt);
    ret = vsnprintf_s(str, sizeof(str) - 1, sizeof(str) - 1, fmt, args);
    va_end(args);
    if (ret < 0) {
        return ret;
    }

    return g_overlay_func->printf(str);
}

int memcpy_s(void *dest, size_t dest_max, const void *src, size_t count)
{
    return g_overlay_func->memcpy_s(dest, dest_max, src, count);
}

int memset_s(void *dest, size_t dest_max, int c, size_t count)
{
    return g_overlay_func->memset_s(dest, dest_max, c, count);
}

int memmove_s(void *dest, size_t dest_max, const void *src, size_t count)
{
    return g_overlay_func->memmove_s(dest, dest_max, src, count);
}

int memcmp(const void *p1, const void *p2, size_t size)
{
    return g_overlay_func->memcmp(p1, p2, size);
}

void *memcpy(void *to, const void *from, size_t size)
{
    return g_overlay_func->memcpy(to, from, size);
}

void *memmove(void *to, const void *from, size_t size)
{
    return g_overlay_func->memmove(to, from, size);
}

void *memset(void *dest, int c, size_t size)
{
    return g_overlay_func->memset(dest, c, size);
}

void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))
{
    return g_overlay_func->qsort(base, nitems, size, compar);
}

void *memchr(const void *buf, int ch, size_t count)
{
    return g_overlay_func->memchr(buf, ch, count);
}

#ifdef SAP_MEM_STATISTIC
static void *overlay_malloc(size_t size)
{
    return g_overlay_func->malloc(size, g_overlay_tag);
}

void *overlay_calloc(size_t size, size_t count)
{
    return g_overlay_func->calloc(size * count, g_overlay_tag);
}

static void *overlay_realloc(void *p, size_t size)
{
    return g_overlay_func->realloc(p, size, g_overlay_tag);
}
#else
static void *overlay_malloc(size_t size)
{
    return g_overlay_func->malloc(size);
}

void *overlay_calloc(size_t size, size_t count)
{
    return g_overlay_func->calloc(size, count);
}

static void *overlay_realloc(void *p, size_t size)
{
    return g_overlay_func->realloc(p, size);
}
#endif

#ifdef HEAP_IN_OVERLAY
#define HEAP_BUFFER_NUM_MAX 16
static heap_buffer g_heap_buffer_tab[HEAP_BUFFER_NUM_MAX];

static heap_buffer *heap_find_buf(td_void *addr)
{
    td_u32 i;

    if (addr == TD_NULL) {
        return TD_NULL;
    }

    for (i = 0; i < HEAP_BUFFER_NUM_MAX; i++) {
        if (g_heap_buffer_tab[i].addr == addr) {
            return &(g_heap_buffer_tab[i]);
        }
    }

    return TD_NULL;
}

static td_s32 heap_store_buf(td_void *addr, size_t size)
{
    td_u32 i;

    for (i = 0; i < HEAP_BUFFER_NUM_MAX; i++) {
        if (g_heap_buffer_tab[i].addr == TD_NULL) {
            g_heap_buffer_tab[i].addr = addr;
            g_heap_buffer_tab[i].size = size;
            return 0;
        }
    }

    return -1;
}
#endif

td_void *malloc(size_t size)
{
#ifdef HEAP_IN_OVERLAY
    td_void *addr = TD_NULL;
    td_u8 *safe = _heap_sentry;

    if (g_heap_offset + size >= safe) {
        return overlay_malloc(size);
    }

    addr = (td_void *)g_heap_offset;
    if (heap_store_buf(addr, size)) {
        return TD_NULL;
    }

    g_heap_offset += align_up(size);
    return addr;
#else
    return overlay_malloc(size);
#endif
}

void *calloc(size_t size, size_t count)
{
#ifdef HEAP_IN_OVERLAY
    td_void *addr = TD_NULL;

    size *= count;

    addr = malloc(size);
    if (addr == TD_NULL) {
        return TD_NULL;
    }

    g_overlay_func->memset(addr, 0, size);
    return addr;
#else
    return overlay_calloc(size, count);
#endif
}

void *realloc(void *p, size_t size)
{
#ifdef HEAP_IN_OVERLAY
    td_void *addr = TD_NULL;
    heap_buffer *heap = TD_NULL;
    td_u8 *safe = _heap_sentry;

    if (p == TD_NULL) {
        return malloc(size);
    }

    heap = heap_find_buf(p);
    if (heap == TD_NULL) {
        return overlay_realloc(p, size);
    }

    if (heap->size >= size) {
        return heap->addr;
    }

    if (g_heap_offset + size >= safe) {
        return TD_NULL;
    }

    addr = (td_void *)g_heap_offset;
    (td_void)memcpy_s(addr, size, heap->addr, heap->size);

    heap->addr = addr;
    heap->size = size;

    g_heap_offset += align_up(size);
    return addr;
#else
    return overlay_realloc(p, size);
#endif
}

void free(void *addr)
{
#ifdef HEAP_IN_OVERLAY
    heap_buffer *heap = TD_NULL;

    if (addr == TD_NULL) {
        return;
    }

    heap = heap_find_buf(addr);
    if (heap == TD_NULL) {
        g_overlay_func->free(addr);
    } else {
        heap->addr = TD_NULL;
        heap->size = 0;
        heap_init();
    }
#else
    g_overlay_func->free(addr);
#endif
}

int vsprintf(char *str, const char *format, va_list arg)
{
    return g_overlay_func->vsprintf(str, format, arg);
}

long lrintf(float arg)
{
    return g_overlay_func->lrintf(arg);
}

long lrint(double arg)
{
    return g_overlay_func->lrint(arg);
}

double floor(double arg)
{
    return g_overlay_func->floor(arg);
}

double log(double arg)
{
    return g_overlay_func->log(arg);
}

double log10(double arg)
{
    return g_overlay_func->log10(arg);
}

double pow(double arg1, double arg2)
{
    return g_overlay_func->pow(arg1, arg2);
}

double sqrt(double arg)
{
    return g_overlay_func->sqrt(arg);
}

int rand(void)
{
    return g_overlay_func->rand();
}

static overlay_contex g_overlay_ctx = {
    .overlay_init = core_overlay_init,
    .ops = TD_NULL,
};

extern component _component_start[];
extern component _component_end[];

overlay_contex *_start(td_void)
{
    clear_bss();

    if (_component_start == _component_end) {
        return TD_NULL;
    }

    g_overlay_ctx.ops = _component_start->entry;
    return &g_overlay_ctx;
}

static overlay_start_call g_overlay_start_call \
    __attribute__((__used__)) \
    __attribute__((__section__(".start"))) = _start;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
