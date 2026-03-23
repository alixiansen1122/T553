/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: hifi component define
 * Author: audio
 */

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "td_type.h"

enum {
    ADEC,
    AENC,
    SEA,
    HAID,
};

typedef struct {
    td_u32 id;
    td_void *entry;
} component;

#define define_component(_id, _entry) \
    static component __component_entry_##_entry \
    __attribute__((__used__)) \
    __attribute__((__section__(".component"))) = { \
        .id = (_id), \
        .entry = &(_entry), \
    };

td_void *adec_get_component(td_u32 codec_id);
td_void *aenc_get_component(td_u32 codec_id);
td_void *sea_get_component(td_u32 sea_id);
td_void *haid_get_component(td_void);

#endif /* __COMPONENT_H__ */
