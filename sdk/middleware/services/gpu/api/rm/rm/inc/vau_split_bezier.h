/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: render path basic
 */

#ifndef VAU_SPLIT_BEZIER_H
#define VAU_SPLIT_BEZIER_H

#include "vau_stroke_dash.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_u32 cubic_split_agg(const vau_point_float *cubic, vau_point_float *line_points, td_u32 point_num);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* VAU_SPLIT_BEZIER_H */
