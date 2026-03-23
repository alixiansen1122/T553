/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: vau path utility api
 */

#ifndef RM_UTILITY_H
#define RM_UTILITY_H

#include "soc_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 uapi_vau_path_create_line(ext_vau_path *path, const ext_vau_line *line);
td_s32 uapi_vau_path_create_polygon(ext_vau_path *path, const ext_vau_point *points, td_u32 count, td_bool closed);
td_s32 uapi_vau_path_create_rect(ext_vau_path *path, const ext_vau_rect *rect);
td_s32 uapi_vau_path_create_round_rect(ext_vau_path *path, const ext_vau_round_rect *rect);
td_s32 uapi_vau_path_create_ellipse(ext_vau_path *path, const ext_vau_ellipse *ellipse);
td_s32 uapi_vau_path_create_arc(ext_vau_path *path, const ext_vau_arc *arc);
td_void uapi_vau_path_destroy(ext_vau_path *path);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* RM_UTILITY_H */
