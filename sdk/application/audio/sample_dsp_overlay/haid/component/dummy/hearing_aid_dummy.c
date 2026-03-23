/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: dummy haid
 * Author: audio
 */

#include "hearing_aid_dummy.h"

void *hearing_aid_open(const hearing_aid_dummy_cfg *config)
{
    (void)config;
    return (void *)config; /* return a dummy inst */
}

void hearing_aid_process(void *ins, void *in, void *out)
{
    int i;
    hearing_aid_data_buffer *in_buf = (hearing_aid_data_buffer *)in;
    hearing_aid_data_buffer *out_buf = (hearing_aid_data_buffer *)out;

    (void)ins;

    for (i = 0; i < HEARINGAIDS_FRAME_LENGTH_96K; i++) {
        out_buf->out_data[i] = in_buf->mic1_data[i];
    }
}

void hearing_aid_close(void *ins)
{
    (void)ins;
}

void hearing_aid_reset(void *ins)
{
    (void)ins;
}

int hearing_aid_set_config(void *ins, const void *value, int *length)
{
    (void)ins;
    (void)value;
    (void)length;
    return 0;
}

int hearing_aid_get_config(void *ins, void *value, int *length)
{
    (void)ins;
    (void)value;

    *length = 512; /* 512 is the length */
    return 0;
}
