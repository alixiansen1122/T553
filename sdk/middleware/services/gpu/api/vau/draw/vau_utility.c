/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: openvg api file
 * Author: @CompanyNameTag
 * Create: 2022-03-21
 */

#include <stdlib.h>
#include <securec.h>
#include "soc_vau_api.h"
#include "vau_debug.h"
#include "vau_math.h"
#include "vau_arc.h"
#include "vau_utility.h"

static td_s32 copy_path_cmd(ext_vau_path *path, const td_u8 *cmds, td_u32 num)
{
    path->cmd_num = num;
    path->cmds = (td_u8 *)malloc(sizeof(td_u8) * path->cmd_num);
    if (path->cmds == TD_NULL) {
        vau_err("malloc %u for cmd failed", sizeof(td_u8) * path->cmd_num);
        return EXT_ERR_GFX_NO_MEM;
    }
    errno_t ret = memcpy_s(path->cmds, sizeof(td_u8) * path->cmd_num, cmds, sizeof(td_u8) * num);
    if (ret != EOK) {
        vau_err("memcpy_s failed");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 copy_path_data(ext_vau_path *path, const td_float *datas, td_u32 num)
{
    path->data_num = num;
    path->datas = (td_float *)malloc(sizeof(td_float) * path->data_num);
    if (path->datas == TD_NULL) {
        vau_err("malloc %u for data failed", sizeof(td_float) * path->data_num);
        return EXT_ERR_GFX_NO_MEM;
    }
    errno_t ret = memcpy_s(path->datas, sizeof(td_float) * path->data_num, datas, sizeof(td_float) * num);
    if (ret != EOK) {
        vau_err("memcpy_s %u %u failed", sizeof(td_float) * path->data_num, sizeof(td_float) * num);
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

td_void uapi_vau_path_destroy(ext_vau_path *path)
{
    if (path == TD_NULL) {
        return;
    }
    if (path->cmds != TD_NULL) {
        free(path->cmds);
        path->cmds = TD_NULL;
        path->cmd_num = 0;
    }
    if (path->datas != TD_NULL) {
        free(path->datas);
        path->datas = TD_NULL;
        path->data_num = 0;
    }
}

td_s32 uapi_vau_path_create_line(ext_vau_path *path, const ext_vau_line *line)
{
    if (path == TD_NULL) {
        vau_err("path is null");
        return EXT_ERR_GFX_NULL_POINTER;
    }
    path->cmd_num = 2; /* 2: move to + line to */
    path->cmds = (td_u8 *)malloc(sizeof(td_u8) * path->cmd_num);
    if (path->cmds == TD_NULL) {
        vau_err("malloc %u for cmd failed", sizeof(td_u8) * path->cmd_num);
        return EXT_ERR_GFX_NO_MEM;
    }
    path->data_num = 4; /* 4: move to + line to */
    path->datas = (td_float *)malloc(sizeof(td_float) * path->data_num);
    if (path->datas == TD_NULL) {
        vau_err("malloc %u for data failed", sizeof(td_float) * path->data_num);
        uapi_vau_path_destroy(path);
        return EXT_ERR_GFX_NO_MEM;
    }
    path->cmds[0] = VAU_PATH_MOVE_TO_ABS; /* index 0 */
    path->cmds[1] = VAU_PATH_LINE_TO_ABS; /* index 1 */
    path->datas[0] = line->x0; /* index 0 */
    path->datas[1] = line->y0; /* index 1 */
    path->datas[2] = line->x1; /* index 2 */
    path->datas[3] = line->y1; /* index 3 */
    return EXT_SUCCESS;
}

td_s32 uapi_vau_path_create_polygon(ext_vau_path *path, const ext_vau_point *points, td_u32 count, td_bool closed)
{
    if ((path == TD_NULL) || (points == TD_NULL) || (count == 0)) {
        vau_err("invalid input");
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    path->cmd_num = count + (closed ? 1 : 0);
    path->cmds = (td_u8 *)malloc(sizeof(td_u8) * path->cmd_num);
    if (path->cmds == TD_NULL) {
        vau_err("malloc %u for cmd failed", sizeof(td_u8) * path->cmd_num);
        return EXT_ERR_GFX_NO_MEM;
    }
    path->data_num = count * 2; /* 2: x + y */
    path->datas = (td_float *)malloc(sizeof(td_float) * path->data_num);
    if (path->datas == TD_NULL) {
        vau_err("malloc %u for data failed", sizeof(td_float) * path->data_num);
        uapi_vau_path_destroy(path);
        return EXT_ERR_GFX_NO_MEM;
    }

    for (td_u32 i = 0; i < count; i++) {
        path->cmds[i] = VAU_PATH_LINE_TO_ABS;
        path->datas[i * 2] = points[i].x; /* * 2: x */
        path->datas[i * 2 + 1] = points[i].y; /* * 2 + 1: x */
    }
    if (closed) {
        path->cmds[count] = VAU_PATH_CLOSE_PATH;
    }
    path->cmds[0] = VAU_PATH_MOVE_TO_ABS;
    return EXT_SUCCESS;
}

td_s32 uapi_vau_path_create_rect(ext_vau_path *path, const ext_vau_rect *rect)
{
    if ((path == TD_NULL) || (rect == TD_NULL) || (rect->width <= 0) || (rect->height <= 0)) {
        vau_err("invalid input");
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    path->cmd_num = 5; /* 5 cmds */
    path->cmds = (td_u8 *)malloc(sizeof(td_u8) * path->cmd_num);
    if (path->cmds == TD_NULL) {
        vau_err("malloc %u for cmd failed", sizeof(td_u8) * path->cmd_num);
        return EXT_ERR_GFX_NO_MEM;
    }
    path->data_num = 5; /* 5 datas */
    path->datas = (td_float *)malloc(sizeof(td_float) * path->data_num);
    if (path->datas == TD_NULL) {
        vau_err("malloc %u for data failed", sizeof(td_float) * path->data_num);
        uapi_vau_path_destroy(path);
        return EXT_ERR_GFX_NO_MEM;
    }
    path->cmds[0] = VAU_PATH_MOVE_TO_ABS; /* index 0 */
    path->cmds[1] = VAU_PATH_HLINE_TO_REL; /* index 1 */
    path->cmds[2] = VAU_PATH_VLINE_TO_REL; /* index 2 */
    path->cmds[3] = VAU_PATH_HLINE_TO_REL; /* index 3 */
    path->cmds[4] = VAU_PATH_CLOSE_PATH; /* index 4 */

    path->datas[0] = rect->x; /* index 0 */
    path->datas[1] = rect->y; /* index 1 */
    path->datas[2] = rect->width; /* index 2 */
    path->datas[3] = rect->height; /* index 3 */
    path->datas[4] = -rect->width; /* index 4 */
    return EXT_SUCCESS;
}

td_s32 uapi_vau_path_create_round_rect(ext_vau_path *path, const ext_vau_round_rect *rect)
{
    if ((path == TD_NULL) || (rect == TD_NULL) || (rect->width <= 0) || (rect->height <= 0)) {
        vau_err("invalid input");
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    // only support circle arc
    if (!vau_math_float_is_equal(rect->arc_width, rect->arc_height)) {
        vau_err("only support circle arc, arc_width:%f, arc_height:%f", rect->arc_width, rect->arc_height);
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    td_float arc_width = vau_math_clamp(rect->arc_width, 0.0f, rect->width);
    td_float arc_height = vau_math_clamp(rect->arc_height, 0.0f, rect->height);

    // only support circle arc
    td_float r = arc_width < arc_height ? arc_width / 2.0f : arc_height / 2.0f; /* 2.0: radius is half */
    td_float h = r * CIRCLE_TO_BEZIER_H_90_DEG;
    td_float dh = r - h;

    static const td_u8 cmds[] = { VAU_PATH_MOVE_TO_ABS,
        VAU_PATH_HLINE_TO_ABS, VAU_PATH_CUBIC_TO_ABS, VAU_PATH_VLINE_TO_ABS, VAU_PATH_CUBIC_TO_ABS,
        VAU_PATH_HLINE_TO_ABS, VAU_PATH_CUBIC_TO_ABS, VAU_PATH_VLINE_TO_ABS, VAU_PATH_CUBIC_TO_ABS };

    const td_float datas[] = { rect->x + r, rect->y, rect->x + rect->width - r, /* MOVE + HLINE */
        /* VAU_PATH_CUBIC_TO_ABS */
        rect->x + rect->width - dh, rect->y, rect->x + rect->width, rect->y + dh, rect->x + rect->width, rect->y + r,
        /* VAU_PATH_VLINE_TO_ABS */
        rect->y + rect->height - r,
        /* VAU_PATH_CUBIC_TO_ABS */
        rect->x + rect->width, rect->y + rect->height - dh, rect->x + rect->width - dh, rect->y + rect->height,
        rect->x + rect->width - r, rect->y + rect->height,
        /* VAU_PATH_HLINE_TO_ABS */
        rect->x + r,
        /* VAU_PATH_CUBIC_TO_ABS */
        rect->x + dh, rect->y + rect->height, rect->x, rect->y + rect->height - dh, rect->x, rect->y + rect->height - r,
        /* VAU_PATH_VLINE_TO_ABS */
        rect->y + r,
        /* VAU_PATH_CUBIC_TO_ABS */
        rect->x, rect->y + dh, rect->x + dh, rect->y, rect->x + r, rect->y
    };
    td_s32 ret = copy_path_cmd(path, cmds, sizeof(cmds) / sizeof(cmds[0]));
    if (ret != EXT_SUCCESS) {
        vau_err("copy_path_cmd failed, ret:0x%x", ret);
        return ret;
    }
    ret = copy_path_data(path, datas, sizeof(datas) / sizeof(datas[0]));
    if (ret != EXT_SUCCESS) {
        vau_err("copy_path_data failed, ret:0x%x", ret);
        uapi_vau_path_destroy(path);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_path_create_ellipse(ext_vau_path *path, const ext_vau_ellipse *ellipse)
{
    if ((path == TD_NULL) || (ellipse == TD_NULL) || (ellipse->width <= 0) || (ellipse->height <= 0)) {
        vau_err("invalid input");
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    // only support circle arc
    if (!vau_math_float_is_equal(ellipse->width, ellipse->height)) {
        vau_err("only support circle arc, width:%f, height:%f", ellipse->width, ellipse->height);
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    td_float r = ellipse->width / 2.0f; /* 2.0: radius */
    td_float h = r * CIRCLE_TO_BEZIER_H_90_DEG;

    vau_dbg("input rect [%f %f %f %f]", ellipse->x, ellipse->y, ellipse->width, ellipse->height);

    static const td_u8 cmds[] = {
        VAU_PATH_MOVE_TO_ABS,
        VAU_PATH_CUBIC_TO_ABS, VAU_PATH_CUBIC_TO_ABS, VAU_PATH_CUBIC_TO_ABS, VAU_PATH_CUBIC_TO_ABS };
    const td_float datas[] = {
        ellipse->x + r, ellipse->y,
        ellipse->x + r, ellipse->y + h, ellipse->x + h, ellipse->y + r, ellipse->x, ellipse->y + r,
        ellipse->x - h, ellipse->y + r, ellipse->x - r, ellipse->y + h, ellipse->x - r, ellipse->y,
        ellipse->x - r, ellipse->y - h, ellipse->x - h, ellipse->y - r, ellipse->x, ellipse->y - r,
        ellipse->x + h, ellipse->y - r, ellipse->x + r, ellipse->y - h, ellipse->x + r, ellipse->y };

    path->cmd_num = (td_u32)(sizeof(cmds) / sizeof(cmds[0]));
    path->cmds = (td_u8 *)malloc(sizeof(td_u8) * path->cmd_num);
    if (path->cmds == TD_NULL) {
        vau_err("malloc %u for cmd failed", sizeof(td_u8) * path->cmd_num);
        return EXT_ERR_GFX_NO_MEM;
    }
    path->data_num = (td_u32)(sizeof(datas) / sizeof(datas[0]));
    path->datas = (td_float *)malloc(sizeof(td_float) * path->data_num);
    if (path->datas == TD_NULL) {
        vau_err("malloc %u for data failed", sizeof(td_float) * path->data_num);
        uapi_vau_path_destroy(path);
        return EXT_ERR_GFX_NO_MEM;
    }
    errno_t ret = memcpy_s(path->cmds, sizeof(td_u8) * path->cmd_num, cmds, sizeof(cmds));
    if (ret != EOK) {
        vau_err("memcpy_s failed");
        uapi_vau_path_destroy(path);
        return EXT_FAILURE;
    }
    ret = memcpy_s(path->datas, sizeof(td_float) * path->data_num, datas, sizeof(datas));
    if (ret != EOK) {
        vau_err("memcpy_s failed");
        uapi_vau_path_destroy(path);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 append_cubic(ext_vau_path *path, ext_vau_arc_type arc_type, const float_point *center,
    float_point *cubic, int num)
{
    path->cmd_num = (td_u32)(num / 3 + 1); /* cubic content 3 point + 1 start point */
    path->data_num = (td_u32)(num * 2); /* one point has 2 coords */
    if (arc_type == EXT_VAU_ARC_TYPE_CHORD) {
        path->cmd_num += 1; /* 2: close */
    } else if (arc_type == EXT_VAU_ARC_TYPE_PIE) {
        path->cmd_num += 2; /* 2: line to + close */
        path->data_num += 2; /* 2: line to */
    }

    path->cmds = (td_u8 *)malloc(sizeof(td_u8) * path->cmd_num);
    if (path->cmds == TD_NULL) {
        vau_err("malloc %u for cmd failed", sizeof(td_u8) * path->cmd_num);
        return EXT_ERR_GFX_NO_MEM;
    }

    path->cmds[0] = VAU_PATH_MOVE_TO_ABS;
    for (td_u32 i = 1; i < path->cmd_num; i++) {
        path->cmds[i] = VAU_PATH_CUBIC_TO_ABS;
    }
    path->datas = (td_float *)malloc(sizeof(td_float) * path->data_num);
    if (path->datas == TD_NULL) {
        uapi_vau_path_destroy(path);
        vau_err("malloc %u for data failed", sizeof(td_float) * path->data_num);
        return EXT_ERR_GFX_NO_MEM;
    }
    path->datas[0] = cubic[0].x;
    path->datas[1] = cubic[0].y;
    for (int i = 0; i < num / 3; i++) { /* cubic content 3 point */
        path->datas[6 * i + 2] = cubic[3 * i + 1].x; /* cubic content 3 point, for 6 coords, index 2 for point 1 */
        path->datas[6 * i + 3] = cubic[3 * i + 1].y; /* cubic content 3 point, for 6 coords, index 3 for point 1 */
        path->datas[6 * i + 4] = cubic[3 * i + 2].x; /* cubic content 3 point, for 6 coords, index 4 for point 2 */
        path->datas[6 * i + 5] = cubic[3 * i + 2].y; /* cubic content 3 point, for 6 coords, index 5 for point 2 */
        path->datas[6 * i + 6] = cubic[3 * i + 3].x; /* cubic content 3 point, for 6 coords, index 6 for point 3 */
        path->datas[6 * i + 7] = cubic[3 * i + 3].y; /* cubic content 3 point, for 6 coords, index 7 for point 3 */
    }

    if (arc_type == EXT_VAU_ARC_TYPE_CHORD) {
        path->cmds[path->cmd_num - 1] = VAU_PATH_CLOSE_PATH;
    } else if (arc_type == EXT_VAU_ARC_TYPE_PIE) {
        path->cmds[path->cmd_num - 2] = VAU_PATH_LINE_TO_ABS; /* last -1 (index is cmd_num - 2) cmd */
        path->cmds[path->cmd_num - 1] = VAU_PATH_CLOSE_PATH; /* last cmd */
        path->datas[path->data_num - 2] = center->x; /* last -1 (index is data_num - 2) data */
        path->datas[path->data_num - 1] = center->y; /* last data */
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_path_create_arc(ext_vau_path *path, const ext_vau_arc *arc)
{
    if ((path == TD_NULL) || (arc == TD_NULL) || (arc->type != EXT_VAU_ARC_TYPE_OPEN &&
        arc->type != EXT_VAU_ARC_TYPE_CHORD && arc->type != EXT_VAU_ARC_TYPE_PIE) ||
        arc->width <= 0.0f || arc->height <= 0.0f) {
        vau_err("invalid para");
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    // only support circle arc
    if (!vau_math_float_is_equal(arc->width, arc->height)) {
        vau_err("only support circle arc, width:%f, height:%f", arc->width, arc->height);
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    float_point center = { arc->x, arc->y };
    td_float r = arc->width / 2; /* 2: r is half of width */
    int num = 0;
    float_point *cubic = create_arc_utility(&center, r, arc->start_angle, arc->angle_extend, &num);
    if (cubic == NULL || (num / 3) == 0) { /* cubic content 3 point */
        vau_err("call create_arc_utility failure");
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    td_s32 ret = append_cubic(path, arc->type, &center, cubic, num);
    if (ret != EXT_SUCCESS) {
        vau_err("call arc_to_cubic failure");
        return ret;
    }

    destroy_arc_path(cubic);
    return ret;
}
