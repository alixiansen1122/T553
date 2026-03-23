/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : vau matrix
 */

#ifndef API_VAU_MATRIX_H
#define API_VAU_MATRIX_H

#include "soc_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_void uapi_vau_matrix_identity(ext_vau_matrix *m);
td_void uapi_vau_matrix_mult(ext_vau_matrix *m, const ext_vau_matrix *n);
td_void uapi_vau_matrix_trans(ext_vau_matrix *m, td_float tx, td_float ty);
td_void uapi_vau_matrix_scale(ext_vau_matrix *m, td_float sx, td_float sy);
td_void uapi_vau_matrix_shear(ext_vau_matrix *m, td_float shx, td_float shy);
td_void uapi_vau_matrix_rotate(ext_vau_matrix *m, td_float angle);
td_void uapi_vau_matrix_reflect_x(ext_vau_matrix *m);
td_void uapi_vau_matrix_reflect_y(ext_vau_matrix *m);
td_s32 uapi_vau_compute_warp_quad_to_square(ext_vau_matrix *m, const ext_vau_point points[4]);
td_s32 uapi_vau_compute_warp_square_to_quad(ext_vau_matrix *m, const ext_vau_point points[4]);
td_s32 uapi_vau_compute_warp_quad_to_quad(ext_vau_matrix *m, const ext_vau_point src_points[4],
                                          const ext_vau_point dst_points[4]);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_VAU_MATRIX_H */
