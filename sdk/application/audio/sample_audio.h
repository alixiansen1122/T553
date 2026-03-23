/*
 * Copyright (c) CompanyNameMagicTag 2020-2021. All rights reserved.
 * Description: source file for module operation
 */

#ifndef _SAMPLE_AUDIO_H_
#define _SAMPLE_AUDIO_H_

#include "td_type.h"

#define TEST_SUITE_MAX_ARGC 16
#define TEST_SUITE_MAX_ARGV 16

td_s32 audio_init(td_void);
td_s32 audio_deinit(td_void);
td_void audio_execute_function(td_s32 argc, td_char *argv[]);

#endif
