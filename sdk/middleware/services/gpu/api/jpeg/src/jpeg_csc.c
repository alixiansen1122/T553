/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg csc
 */

#include "jpeg_csc.h"

/*-------------------------------------- macro define -------------------------------------*/

/*-------------------------------------- struct info --------------------------------------*/

typedef struct {
    int *cr_r_tab;          /* => table for Cr to R conversion */
    int *cb_b_tab;          /* => table for Cb to B conversion */
    long *cr_g_tab;         /* => table for Cr to G conversion */
    long *cb_g_tab;         /* => table for Cb to G conversion */
} my_color_converter, *my_cconvert_ptr;

/*-------------------------------------- global data --------------------------------------*/


/*-------------------------------------- func define --------------------------------------*/


/*-------------------------------------- func release -------------------------------------*/

/*
 * R = Y                + 1.40200 * Cr
 * G = Y - 0.34414 * Cb - 0.71414 * Cr
 * B = Y + 1.77200 * Cb
*/

td_s32 jpeg_csc_convert(const jpeg_decompress_par_ptr par)
{
    if ((par->output_color_space != MY_JPEG_COLOR_SPACE_ARGB8888) &&
        (par->output_color_space != MY_JPEG_COLOR_SPACE_RGB888)) {
        return EXT_SUCCESS;
    }

    return EXT_SUCCESS;
}
