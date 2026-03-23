/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: openvg api file
 * Author: @CompanyNameTag
 * Create: 2021-07-20
 */

#include "VG/vgu.h"
#include <stdlib.h>
#include <securec.h>
#include "VG/openvg.h"
#include "vg_common.h"
#include "vg_math.h"
#include "vau_arc.h"

VGUErrorCode vguLine(VGPath path, VGfloat x0, VGfloat y0,  VGfloat x1, VGfloat y1)
{
    VGErrorCode error = vgGetError();
    static const VGubyte segments[2] = {VG_MOVE_TO_ABS, VG_LINE_TO_ABS};
    const VGfloat data[4] = {x0, y0, x1, y1}; // number of coordinates

    vgAppendPathData(path, 2, segments, data); // 2 : segments number

    error = vgGetError();
    return (VGUErrorCode)error;
}

VGUErrorCode vguPolygon(VGPath path, const VGfloat *points, VGint count, VGboolean closed)
{
    VGErrorCode error = vgGetError();
    if ((points == NULL) || (count <= 0)) {
        return VGU_ILLEGAL_ARGUMENT_ERROR;
    }

    int segment_num = count + ((bool)closed ? 1 : 0);
    VGubyte *segments = (VGubyte *)malloc(sizeof(VGubyte) * segment_num);
    if (segments == NULL) {
        vg_err("malloc failed");
        return VGU_OUT_OF_MEMORY_ERROR;
    }
    segments[0] = VG_MOVE_TO_ABS;

    for (int i = 1; i < count; i++) {
        segments[i] = VG_LINE_TO_ABS;
    }
    if (closed != VG_FALSE) {
        segments[count] = VG_CLOSE_PATH;
    }
    vgAppendPathData(path, segment_num, segments, points);
    free(segments);
    error = vgGetError();
    return (VGUErrorCode)error;
}

VGUErrorCode vguRect(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height)
{
    VGErrorCode error = vgGetError();
    if (width <= 0 || height <= 0) {
        vg_err("invalid para, width:%f, height:%f", width, height);
        return VGU_ILLEGAL_ARGUMENT_ERROR;
    }
    static const VGubyte segments[5] = {VG_MOVE_TO_ABS,  // segments number
                                        VG_HLINE_TO_REL,
                                        VG_VLINE_TO_REL,
                                        VG_HLINE_TO_REL,
                                        VG_CLOSE_PATH};
    const VGfloat data[5] = {x, y, width, height, -width}; // 5 : data length
    vgAppendPathData(path, 5, segments, data);     // 5 : segments number
    error = vgGetError();
    return (VGUErrorCode)error;
}

VGUErrorCode vguRoundRect(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height,
    VGfloat arcWidth, VGfloat arcHeight)
{
    VGfloat tmp_arc_width = arcWidth;
    VGfloat tmp_arc_height = arcHeight;
    VGErrorCode error = vgGetError();   // clear the error state
    uapi_unused(error);
    if (width <= 0 || height <= 0) {
        vg_err("invalid para, width:%f, height:%f", width, height);
        return VGU_ILLEGAL_ARGUMENT_ERROR;
    }
    // only support circle arc
    if (!vg_float_is_equal(tmp_arc_width, tmp_arc_height)) {
        vg_err("only support circle arc, arcWidth:%f, arcHeight:%f", tmp_arc_width, tmp_arc_height);
        return VGU_ILLEGAL_ARGUMENT_ERROR;
    }
    tmp_arc_width = vg_float_clamp(tmp_arc_width, 0.0f, width);
    tmp_arc_height = vg_float_clamp(tmp_arc_height, 0.0f, height);

    // only support circle arc
    VGfloat r = tmp_arc_width < tmp_arc_height ? tmp_arc_width / 2.0f : tmp_arc_height / 2.0f;

    static const VGubyte cmds[] = {
        VG_MOVE_TO_ABS,
        VG_HLINE_TO_ABS, VG_CUBIC_TO_ABS, VG_VLINE_TO_ABS, VG_CUBIC_TO_ABS,
        VG_HLINE_TO_ABS, VG_CUBIC_TO_ABS, VG_VLINE_TO_ABS, VG_CUBIC_TO_ABS
    };

    VGfloat h = r * CIRCLE_TO_BEZIER_H_90_DEG;
    VGfloat dh = r - h;

    const VGfloat datas[] = {
        /* VG_MOVE_TO_ABS */
        x + r, y,
        /* VG_HLINE_TO_ABS */
        x + width - r,
        /* VG_CUBIC_TO_ABS */
        x + width - dh, y,
        x + width, y + dh,
        x + width, y + r,
        /* VG_VLINE_TO_ABS */
        y + height - r,
        /* VG_CUBIC_TO_ABS */
        x + width, y + height - dh,
        x + width - dh, y + height,
        x + width - r, y + height,
        /* VG_HLINE_TO_ABS */
        x + r,
        /* VG_CUBIC_TO_ABS */
        x + dh, y + height,
        x, y + height - dh,
        x, y + height - r,
        /* VG_VLINE_TO_ABS */
        y + r,
        /* VG_CUBIC_TO_ABS */
        x, y + dh,
        x + dh, y,
        x + r, y
    };

    vgAppendPathData(path, sizeof(cmds) / sizeof(cmds[0]), cmds, datas);
    return (VGUErrorCode)vgGetError();
}

VGUErrorCode vguEllipse(VGPath path, VGfloat cx, VGfloat cy, VGfloat width, VGfloat height)
{
    VGErrorCode error = vgGetError();    // clear the error state
    uapi_unused(error);
    if (width <= 0 || height <= 0) {
        vg_err("invalid para, width:%f, height:%f", width, height);
        return VGU_ILLEGAL_ARGUMENT_ERROR;
    }
    // only support circle
    if (!vg_float_is_equal(width, height)) {
        vg_err("only support circle arc, width:%f, height:%f", width, height);
        return VGU_ILLEGAL_ARGUMENT_ERROR;
    }

    VGfloat r = width / 2.0f;
    VGfloat h = r * CIRCLE_TO_BEZIER_H_90_DEG;

    static const VGubyte cmds[] = {
        VG_MOVE_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS };
    const VGfloat datas[] = {
        cx + r, cy,
        cx + r, cy + h, cx + h, cy + r, cx, cy + r,
        cx - h, cy + r, cx - r, cy + h, cx - r, cy,
        cx - r, cy - h, cx - h, cy - r, cx, cy - r,
        cx + h, cy - r, cx + r, cy - h, cx + r, cy };

    vgAppendPathData(path, sizeof(cmds) / sizeof(cmds[0]), cmds, datas);
    return (VGUErrorCode)vgGetError();
}

static VGUErrorCode vgu_cubic(VGPath path, VGUArcType arc_type, float_point *center, float_point *cubic, int num)
{
    VGint cmd_num = num / 3 + 1; /* cubic content 3 point + 1 start point */
    VGint data_num = num * 2; /* one point has 2 coords */
    if (arc_type == VGU_ARC_CHORD) {
        cmd_num += 1; /* 2: close */
    } else if (arc_type == VGU_ARC_PIE) {
        cmd_num += 2; /* 2: line to + close */
        data_num += 2; /* 2: line to */
    }

    VGubyte *cmds = (VGubyte *)malloc(sizeof(VGubyte) * cmd_num);
    if (cmds == NULL) {
        vg_err("malloc failed");
        return VGU_OUT_OF_MEMORY_ERROR;
    }
    cmds[0] = VG_MOVE_TO_ABS;
    for (int i = 1; i < cmd_num; i++) {
        cmds[i] = VG_CUBIC_TO_ABS;
    }
    VGfloat *datas = (VGfloat *)malloc(sizeof(VGfloat) * data_num);
    if (datas == NULL) {
        free(cmds);
        vg_err("malloc failed");
        return VGU_OUT_OF_MEMORY_ERROR;
    }
    datas[0] = cubic[0].x;
    datas[1] = cubic[0].y;
    for (int i = 0; i < num / 3; i++) { /* cubic content 3 point */
        datas[6 * i + 2] = cubic[3 * i + 1].x; /* cubic content 3 point, for 6 coords, index 2 for point 1 */
        datas[6 * i + 3] = cubic[3 * i + 1].y; /* cubic content 3 point, for 6 coords, index 3 for point 1 */
        datas[6 * i + 4] = cubic[3 * i + 2].x; /* cubic content 3 point, for 6 coords, index 4 for point 2 */
        datas[6 * i + 5] = cubic[3 * i + 2].y; /* cubic content 3 point, for 6 coords, index 5 for point 2 */
        datas[6 * i + 6] = cubic[3 * i + 3].x; /* cubic content 3 point, for 6 coords, index 6 for point 3 */
        datas[6 * i + 7] = cubic[3 * i + 3].y; /* cubic content 3 point, for 6 coords, index 7 for point 3 */
    }

    if (arc_type == VGU_ARC_CHORD) {
        cmds[cmd_num - 1] = VG_CLOSE_PATH;
    } else if (arc_type == VGU_ARC_PIE) {
        cmds[cmd_num - 2] = VG_LINE_TO_ABS; /* last -1 (index is cmd_num - 2) cmd */
        cmds[cmd_num - 1] = VG_CLOSE_PATH; /* last cmd */
        datas[data_num - 2] = center->x; /* last -1 (index is data_num - 2) data */
        datas[data_num - 1] = center->y; /* last data */
    }

    vgAppendPathData(path, cmd_num, cmds, datas);

    free(cmds);
    free(datas);
    return (VGUErrorCode)vgGetError();
}

VGUErrorCode vguArc(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height,
                    VGfloat startAngle, VGfloat angleExtent, VGUArcType arcType)
{
    VGUErrorCode error = (VGUErrorCode)vgGetError();
    if ((arcType != VGU_ARC_OPEN && arcType != VGU_ARC_CHORD && arcType != VGU_ARC_PIE) ||
        width <= 0.0f || height <= 0.0f) {
        vg_err("invalid para, width:%f, height:%f", width, height);
        return VGU_ILLEGAL_ARGUMENT_ERROR;
    }
    // currenty only support circle arc
    if (!vg_float_is_equal(width, height)) {
        vg_err("only support circle arc, width:%f, height:%f", width, height);
        return VGU_ILLEGAL_ARGUMENT_ERROR;
    }
    float_point center = { x, y };
    float r = width / 2; /* 2: r is half of width */
    int num = 0;
    float_point *cubic = create_arc_utility(&center, r, startAngle, angleExtent, &num);
    if (cubic == NULL || (num / 3) == 0) { /* cubic content 3 point */
        vg_err("call create_arc_utility failure");
        return VGU_ILLEGAL_ARGUMENT_ERROR;
    }

    error = vgu_cubic(path, arcType, &center, cubic, num);

    destroy_arc_path(cubic);
    return error;
}

static VGboolean vg_matrix_is_affine(ext_vau_matrix *matrix)
{
    // 2: row 2
    if (vg_float_is_equal(matrix->matrix[2][0], 0.0) && vg_float_is_equal(matrix->matrix[2][1], 0.0) && /* 2:index */
        vg_float_is_equal(matrix->matrix[2][2], 1.0)) { /* 2: row 2 */
        return VG_TRUE;
    }
    return VG_FALSE;
}

static VGboolean vg_matrix_invert(ext_vau_matrix *matrix)
{
    VGboolean affine = vg_matrix_is_affine(matrix);
    float det00 = matrix->matrix[1][1] * matrix->matrix[2][2] - matrix->matrix[2][1] * matrix->matrix[1][2];
    float det01 = matrix->matrix[2][0] * matrix->matrix[1][2] - matrix->matrix[1][0] * matrix->matrix[2][2];
    float det02 = matrix->matrix[1][0] * matrix->matrix[2][1] - matrix->matrix[2][0] * matrix->matrix[1][1];
    float d = matrix->matrix[0][0] * det00 + matrix->matrix[0][1] * det01 + matrix->matrix[0][2] * det02;
    if (vg_float_is_equal(d, 0)) {
        vg_err("matrix can not invert");
        return VG_FALSE; // singular, leave the matrix unmodified and return false
    }
    d = 1.0f / d;
    ext_vau_matrix t;
    t.matrix[0][0] = d * det00;
    t.matrix[1][0] = d * det01;
    // [2][]: row 2; [][2]: col 2
    t.matrix[2][0] = d * det02;
    // [2][]: row 2; [][2]: col 2
    t.matrix[0][1] = d * (matrix->matrix[2][1] * matrix->matrix[0][2] - matrix->matrix[0][1] * matrix->matrix[2][2]);
    // [2][]: row 2; [][2]: col 2
    t.matrix[1][1] = d * (matrix->matrix[0][0] * matrix->matrix[2][2] - matrix->matrix[2][0] * matrix->matrix[0][2]);
    // [2][]: row 2; [][2]: col 2
    t.matrix[2][1] = d * (matrix->matrix[2][0] * matrix->matrix[0][1] - matrix->matrix[0][0] * matrix->matrix[2][1]);
    // [2][]: row 2; [][2]: col 2
    t.matrix[0][2] = d * (matrix->matrix[0][1] * matrix->matrix[1][2] - matrix->matrix[1][1] * matrix->matrix[0][2]);
    // [2][]: row 2; [][2]: col 2
    t.matrix[1][2] = d * (matrix->matrix[1][0] * matrix->matrix[0][2] - matrix->matrix[0][0] * matrix->matrix[1][2]);
    // [2][]: row 2; [][2]: col 2
    t.matrix[2][2] = d * (matrix->matrix[0][0] * matrix->matrix[1][1] - matrix->matrix[1][0] * matrix->matrix[0][1]);
    if (affine != VG_FALSE) {
        // [2][]: row 2; [][2]: col 2
        t.matrix[2][0] = 0;
        // [2][]: row 2; [][2]: col 2
        t.matrix[2][1] = 0;
        // [2][]: row 2; [][2]: col 2
        t.matrix[2][2] = 1;
    }
    errno_t ret = memcpy_s(matrix->matrix, sizeof(ext_vau_matrix), t.matrix, sizeof(ext_vau_matrix));
    if (ret != EOK) {
        return VG_FALSE;
    }
    return VG_TRUE;
}

VGUErrorCode vguComputeWarpSquareToQuad(VGfloat dx0, VGfloat dy0, VGfloat dx1, VGfloat dy1,
                                        VGfloat dx2, VGfloat dy2, VGfloat dx3, VGfloat dy3,
                                        VGfloat *matrix)
{
    float diffx1 = dx1 - dx3;
    float diffy1 = dy1 - dy3;
    float diffx2 = dx2 - dx3;
    float diffy2 = dy2 - dy3;
    float det = diffx1 * diffy2 - diffx2 * diffy1;
    if (vg_float_is_equal(det, 0)) {
        vg_err("det is 0");
        return VGU_BAD_WARP_ERROR;
    }
    float sumx = dx0 - dx1 + dx3 - dx2;
    float sumy = dy0 - dy1 + dy3 - dy2;
    if (vg_float_is_equal(sumx, 0.0f) && vg_float_is_equal(sumy, 0.0f)) { // affine mapping
        // matrix index 0
        matrix[0] = dx1 - dx0;
        // matrix index 1
        matrix[1] = dy1 - dy0;
        // matrix index 2
        matrix[2] = 0.0f;
        // matrix index 3
        matrix[3] = dx3 - dx1;
        // matrix index 4
        matrix[4] = dy3 - dy1;
        // matrix index 5
        matrix[5] = 0.0f;
        // matrix index 6
        matrix[6] = dx0;
        // matrix index 7
        matrix[7] = dy0;
        // matrix index 8
        matrix[8] = 1.0f;
        return VGU_NO_ERROR;
    }
    float oodet = 1.0f / det;
    float g = (sumx * diffy2 - diffx2 * sumy) * oodet;
    float h = (diffx1 * sumy - sumx * diffy1) * oodet;
    // matrix index 0
    matrix[0] = dx1 - dx0 + g * dx1;
    // matrix index 1
    matrix[1] = dy1 - dy0 + g * dy1;
    // matrix index 2
    matrix[2] = g;
    // matrix index 3
    matrix[3] = dx2 - dx0 + h * dx2;
    // matrix index 4
    matrix[4] = dy2 - dy0 + h * dy2;
    // matrix index 5
    matrix[5] = h;
    // matrix index 6
    matrix[6] = dx0;
    // matrix index 7
    matrix[7] = dy0;
    // matrix index 8
    matrix[8] = 1.0f;
    return VGU_NO_ERROR;
}

VGUErrorCode vguComputeWarpQuadToSquare(VGfloat sx0, VGfloat sy0, VGfloat sx1, VGfloat sy1,
                                        VGfloat sx2, VGfloat sy2, VGfloat sx3, VGfloat sy3,
                                        VGfloat *matrix)
{
    float mat[9];
    VGint ret = vguComputeWarpSquareToQuad(sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3, mat);
    if (ret != VGU_NO_ERROR) {
        return ret;
    }
    ext_vau_matrix m = { .matrix = {
        { mat[0], mat[3], mat[6] },
        { mat[1], mat[4], mat[7] },
        { mat[2], mat[5], mat[8] } }
    };
    VGboolean nonsingular = vg_matrix_invert(&m);
    if (!nonsingular) {
        vg_err("matrix can not invert");
        return VGU_BAD_WARP_ERROR;
    }
    // index 0; [2][]: row 2; [][2]: col 2
    matrix[0] = m.matrix[0][0];
    // index 1; [2][]: row 2; [][2]: col 2
    matrix[1] = m.matrix[1][0];
    // index 2; [2][]: row 2; [][2]: col 2
    matrix[2] = m.matrix[2][0];
    // index 3; [2][]: row 2; [][2]: col 2
    matrix[3] = m.matrix[0][1];
    // index 4; [2][]: row 2; [][2]: col 2
    matrix[4] = m.matrix[1][1];
    // index 5; [2][]: row 2; [][2]: col 2
    matrix[5] = m.matrix[2][1];
    // index 6; [2][]: row 2; [][2]: col 2
    matrix[6] = m.matrix[0][2];
    // index 7; [2][]: row 2; [][2]: col 2
    matrix[7] = m.matrix[1][2];
    // index 8; [2][]: row 2; [][2]: col 2
    matrix[8] = m.matrix[2][2];
    return VGU_NO_ERROR;
}

VGUErrorCode vguComputeWarpQuadToQuad(VGfloat dx0, VGfloat dy0, VGfloat dx1, VGfloat dy1,
                                      VGfloat dx2, VGfloat dy2, VGfloat dx3, VGfloat dy3,
                                      VGfloat sx0, VGfloat sy0, VGfloat sx1, VGfloat sy1,
                                      VGfloat sx2, VGfloat sy2, VGfloat sx3, VGfloat sy3,
                                      VGfloat *matrix)
{
    float qtos[9];
    VGUErrorCode ret = vguComputeWarpQuadToSquare(sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3, qtos);
    if (ret != VGU_NO_ERROR) {
        return ret;
    }
    float stoq[9];
    ret = vguComputeWarpSquareToQuad(dx0, dy0, dx1, dy1, dx2, dy2, dx3, dy3, stoq);
    if (ret != VGU_NO_ERROR) {
        return ret;
    }
    ext_vau_matrix m1 = { .matrix = {
        { qtos[0], qtos[3], qtos[6] },
        { qtos[1], qtos[4], qtos[7] },
        { qtos[2], qtos[5], qtos[8] } }
    };
    ext_vau_matrix m2 = { .matrix = {
        { stoq[0], stoq[3], stoq[6] },
        { stoq[1], stoq[4], stoq[7] },
        { stoq[2], stoq[5], stoq[8] } }
    };
    vg_matrix_mult(&m2, &m1);
    // index 0; [2][]: row 2; [][2]: col 2
    matrix[0] = m2.matrix[0][0];
    // index 1; [2][]: row 2; [][2]: col 2
    matrix[1] = m2.matrix[1][0];
    // index 2; [2][]: row 2; [][2]: col 2
    matrix[2] = m2.matrix[2][0];
    // index 3; [2][]: row 2; [][2]: col 2
    matrix[3] = m2.matrix[0][1];
    // index 4; [2][]: row 2; [][2]: col 2
    matrix[4] = m2.matrix[1][1];
    // index 5; [2][]: row 2; [][2]: col 2
    matrix[5] = m2.matrix[2][1];
    // index 6; [2][]: row 2; [][2]: col 2
    matrix[6] = m2.matrix[0][2];
    // index 7; [2][]: row 2; [][2]: col 2
    matrix[7] = m2.matrix[1][2];
    // index 8; [2][]: row 2; [][2]: col 2
    matrix[8] = m2.matrix[2][2];
    return VGU_NO_ERROR;
}
