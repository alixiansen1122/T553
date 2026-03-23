/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dfx manage
 */

#include "vau_dfx.h"
#include "drv_gfx_port.h"
#include "soc_osal.h"
#include "vau_list.h"
#include "vau_mem.h"
#include "drv_vau.h"
#include "vau_hal_apb.h"

/*-------------------------------- macro define -------------------------------------------*/

#ifdef CONFIG_GFX_PROC_SUPPORT
#define VAU_CMD_MAX_LEN 32
#define VAU_CMD_HELP_MAX_LEN 32
#define VAU_CMD_MAX_ARG_CNT 16

/*-------------------------------- struct define ------------------------------------------*/

typedef td_s32 (*vau_cmd_func)(int argc, const char *argv[]);

typedef struct {
    td_char *cmd_str;
    td_char *cmd_help;
    vau_cmd_func cmd;
} vau_dfx_cmd;

/*-------------------------------- func declares ------------------------------------------*/

static td_s32 vau_dfx_help(int argc, const char *argv[]);
static inline td_void vau_dfx_print_func_info(void);
static td_void vau_dfx_print_func_time_info(void);
static td_void vau_dfx_print_func_cnt_info(void);

/*-------------------------------- par define ---------------------------------------------*/

static td_u32 g_vau_debug_mask = 0x0;
vau_dfx_info g_vau_dfx_info = {0};

/*-------------------------------- func release -------------------------------------------*/
td_s32 vau_dfx_print(int argc, const char *argv[])
{
    if (argc < 1) {
        vau_print("usage: vau_print 0/1\n");
        return EXT_FAILURE;
    }

    g_vau_dfx_info.print_en = (td_bool)strtoul(argv[0], TD_NULL, 10); /* 10: index */

    return EXT_SUCCESS;
}

static td_void vau_dfx_print_node_info(void)
{
    td_u32 num = (td_u32)(sizeof(g_vau_dfx_info.mem_info.handle_busy) / sizeof(td_bool));

    vau_print("-----------node info-----------\n");
    vau_print("[%u][%u][%u]\n",
              g_vau_dfx_info.mem_info.handle_num, g_vau_dfx_info.mem_info.handle_size,
              g_vau_dfx_info.mem_info.handle_num * g_vau_dfx_info.mem_info.handle_size);
    for (td_u32 i = 0; (i < g_vau_dfx_info.mem_info.handle_num) && (i < num); i++) {
        if (g_vau_dfx_info.mem_info.handle_busy[i] == TD_TRUE) {
            vau_print("busy: [node_%u]", i);
        } else {
            vau_print("free: [node_%u]", i);
        }
    }
    vau_print("\n");
    return;
}

td_s32 vau_dfx_proc_read(int argc, const char *argv[])
{
    uapi_unused(argc);
    uapi_unused(argv);

    vau_print("\n-----------VAU proc----------------------\n");
    vau_dfx_print_node_info();
    vau_dfx_print_func_info();
    vau_osi_list_proc();

    if (g_vau_dfx_info.print_en != TD_TRUE) {
        return EXT_SUCCESS;
    }

    vau_hal_apb_print_all_reg();

    return EXT_SUCCESS;
}

static inline td_void vau_dfx_print_func_info(td_void)
{
    vau_print("beg func info: ------------------------------------\n");
    vau_dfx_print_func_time_info();
    vau_dfx_print_func_cnt_info();
    vau_print("end func info: ------------------------------------\n");
}

static td_void vau_dfx_print_func_time_info(td_void)
{
    vau_print("-----------time info-----------\n");
    vau_print("create_cost(us)      :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.create_cost));
    vau_print("destroy_cost(us)     :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.destroy_cost));
    vau_print("blit_cost(us)        :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.blit_cost));
    vau_print("compose_cost(us)     :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.compose_cost));
    vau_print("submit_cost(us)      :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.submit_cost));
    vau_print("wait done cose(us)   :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.wait_for_done_cost));
    vau_print("suspend/resume       :%u/%u\n", g_vau_dfx_info.suspend_cnt, g_vau_dfx_info.resume_cnt);

    if (drv_vau_get_suspend_flag() == TD_FALSE) {
        uapi_reg_read(HAL_VAU_LIST_PFCNT, g_vau_dfx_info.func_info.time_info.list_pfcnt);
    } else {
        vau_print("vau is in lowpower state, list pfcnt is invalid!");
    }
    vau_print("list pfcnt/time_us   :%u/%u\n", g_vau_dfx_info.func_info.time_info.list_pfcnt,
              g_vau_dfx_info.func_info.time_info.list_pfcnt / 176); /* 0.9V AXI 176M */
}

static td_void vau_dfx_print_func_cnt_info(td_void)
{
    td_u32 cnt = g_vau_dfx_info.func_info.func_cnt.submit_cnt - g_vau_dfx_info.func_info.func_cnt.submit_usr_cnt;

    vau_print("-----------call count info-----------\n");
    vau_print("create_cnt       :%u\n", g_vau_dfx_info.func_info.func_cnt.create_cnt);
    vau_print("cancel_cnt       :%u\n", g_vau_dfx_info.func_info.func_cnt.cancel_cnt);
    vau_print("destroy_cnt      :%u\n", g_vau_dfx_info.func_info.func_cnt.destroy_cnt);
    vau_print("blit_cnt         :%u\n", g_vau_dfx_info.func_info.func_cnt.blit_cnt);
    vau_print("compose_cnt      :%u\n", g_vau_dfx_info.func_info.func_cnt.compose_cnt);
    vau_print("submit_cnt       :%u ", g_vau_dfx_info.func_info.func_cnt.submit_cnt);
    vau_print("(usr             :%u", g_vau_dfx_info.func_info.func_cnt.submit_usr_cnt);
    vau_print(" drv             :%u", cnt);
    vau_print(" submit from int :%u", g_vau_dfx_info.func_info.func_cnt.submit_from_int);
    vau_print(" submit from user:%u)\n", g_vau_dfx_info.func_info.func_cnt.submit_from_user);
    vau_print("wait_done_cnt    :%u\n", g_vau_dfx_info.func_info.func_cnt.wait_for_done_cnt);
    vau_print("isr_cnt          :%u ", g_vau_dfx_info.func_info.func_cnt.isr_cnt);
    vau_print("(node_end        :%u", g_vau_dfx_info.func_info.isr_cnt.isr_node_end);
    vau_print(" tunl_done       :%u", g_vau_dfx_info.func_info.isr_cnt.isr_tunl_done);
    vau_print(" timeout         :%u", g_vau_dfx_info.func_info.isr_cnt.isr_timeout);
    vau_print(" bus_err         :%u", g_vau_dfx_info.func_info.isr_cnt.isr_bus_err);
    vau_print(" list_end        :%u", g_vau_dfx_info.func_info.isr_cnt.isr_list_end);
    vau_print(" disp_done       :%u", g_vau_dfx_info.func_info.isr_cnt.isr_disp_done);
    vau_print(" conflict        :%u", g_vau_dfx_info.func_info.isr_cnt.isr_conflict);
    vau_print(" isr zero value  :%u)\n", g_vau_dfx_info.func_info.isr_cnt.isr_status_zero_value_cnt);
    vau_print("tasklet_func_cnt :%u\n", g_vau_dfx_info.func_info.func_cnt.tasklet_func_cnt);
    vau_print("execute_node_cnt :%u\n", g_vau_dfx_info.func_info.func_cnt.execute_node_cnt);

    td_u32 hard_start_cnt = 0;
    td_u32 hard_list_finish_cnt = 0;
    td_u32 hard_int_cnt = 0;
    if (drv_vau_get_suspend_flag() == TD_FALSE) {
        uapi_reg_read(HAL_VAU_START_CNT, hard_start_cnt);
        uapi_reg_read(HAL_VAU_LIST_FINISH_CNT, hard_list_finish_cnt);
        uapi_reg_read(HAL_VAU_INT_CNT, hard_int_cnt);
    } else {
        vau_print("vau is in lowpower state, hard_int_cnt/hard_start_cnt/hard_list_finish_cnt is invalid!");
    }

    // hard int count
    vau_print("hard int cnt     :%u ", hard_int_cnt);
    vau_print("(hard start cnt  :%u", hard_start_cnt);
    vau_print(" hard list finish cnt:%u)\n", hard_list_finish_cnt);
}

static td_s32 vau_dfx_set_timeoutreset(int argc, const char *argv[])
{
    vau_job_dbg *dbg = TD_NULL;

    dbg = vau_list_get_dbg();
    if ((dbg == TD_NULL) || (argc < 2)) { /* min cnt is 2 */
        return EXT_FAILURE;
    }

    dbg->no_reset = strtol(argv[1], TD_NULL, 0);
    vau_print("reset(%d)\n", (dbg->no_reset == TD_TRUE) ? 1 : 0);
    return EXT_SUCCESS;
}

td_bool vau_dfx_check_mask(td_u32 mask)
{
    return ((g_vau_debug_mask & mask) == 0) ? TD_FALSE : TD_TRUE;
}

static td_s32 vau_dfx_set_mask(int argc, const char *argv[])
{
    if (argc < 2) { /* 2 : parameter number */
        return EXT_FAILURE;
    }

    g_vau_debug_mask = (td_u32)strtol(argv[1], TD_NULL, 0);
    vau_print("set mask %#x\n", g_vau_debug_mask);
    return EXT_SUCCESS;
}

const static vau_dfx_cmd g_vau_dfx_cmd_array[] = {
    {"no_reset", "[0,1]; 0 reset,1 no reset", vau_dfx_set_timeoutreset},
    {"debug_mask", "[bits mask]; 0 nothing,1 node_nozero,2 node all,4 reg,...", vau_dfx_set_mask},
    {"help", "help", vau_dfx_help}
};

static td_s32 vau_dfx_help(int argc, const char *argv[])
{
    uapi_unused(argc);
    uapi_unused(argv);

    for (td_u32 i = 0; i < sizeof(g_vau_dfx_cmd_array) / sizeof(g_vau_dfx_cmd_array[0]); i++) {
        if (g_vau_dfx_cmd_array[i].cmd != TD_NULL) {
            vau_print("cmd(%u) | vau_echo %-16s ", i, g_vau_dfx_cmd_array[i].cmd_str);
        }
        if (g_vau_dfx_cmd_array[i].cmd_help != TD_NULL) {
            vau_print("%s", g_vau_dfx_cmd_array[i].cmd_help);
        }
        vau_print("\n");
    }

    return EXT_SUCCESS;
}

td_s32 vau_dfx_proc_write(int argc, const char *argv[])
{
    const td_u32 cmd_cnt = (td_u32)(sizeof(g_vau_dfx_cmd_array) / sizeof(g_vau_dfx_cmd_array[0]));
    if ((argv == TD_NULL) || (argc > VAU_CMD_MAX_ARG_CNT) || (argc <= 0)) {
        vau_err("invalid cmd! argc %d\n", argc);
        vau_dfx_help(0, TD_NULL);
        return EXT_FAILURE;
    }

    for (td_u32 i = 0; i < (td_u32)argc; i++) {
        if (argv[i] == TD_NULL) {
            vau_err("invalid cmd! argv index %u\n", i);
            vau_dfx_help(0, TD_NULL);
            return EXT_FAILURE;
        }
    }

    for (td_u32 i = 0; i < cmd_cnt; i++) {
        if ((g_vau_dfx_cmd_array[i].cmd_str == TD_NULL) ||
            (g_vau_dfx_cmd_array[i].cmd == TD_NULL) ||
            (osal_strcasecmp(g_vau_dfx_cmd_array[i].cmd_str, argv[0]) != 0)) {
            continue;
        }
        return g_vau_dfx_cmd_array[i].cmd(argc, argv);
    }

    vau_err("invalid cmd! argc %d\n", argc);
    return EXT_FAILURE;
}
#endif

td_void vau_dfx_print_path(const td_u8 *cmds, const td_float *datas, td_u16 cmd_num, td_u16 data_num)
{
    if (cmd_num != 0) {
        vau_print("cmd[%u] = { ", cmd_num);
        for (td_u32 i = 0; i < cmd_num; i++) {
            vau_print("%u, ", (td_u32)cmds[i]);
        }
        vau_print("};\n");
    }
    if (data_num != 0) {
        vau_print("data[%u] = { ", data_num);
        for (td_u32 i = 0; i < data_num; i++) {
            vau_print("%d.%u, ", (td_s32)datas[i], fractional_part(datas[i]));
        }
        vau_print("};\n");
    }
}

vau_dfx_info *vau_dfx_get_info(td_void)
{
    return &g_vau_dfx_info;
}

td_s32 vau_dfx_enable_api_print(int argc, const char *argv[])
{
    if (argc < 1) {
        vau_print("should input enable switch 0/1\n");
        return EXT_FAILURE;
    }

    g_vau_dfx_info.is_print_vau_api = (td_bool)strtoul(argv[0x0], TD_NULL, 10); /* 10: index */
    if (g_vau_dfx_info.is_print_vau_api) {
        vau_print("open vau_api_print\n");
    } else {
        vau_print("close vau_api_print\n");
    }

    return EXT_SUCCESS;
}
