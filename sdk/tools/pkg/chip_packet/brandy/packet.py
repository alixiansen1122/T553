#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    packet files
# ============================================================================

import os
import sys

PY_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PY_PATH)
PKG_DIR = os.path.dirname(PY_PATH)
PKG_DIR = os.path.dirname(PKG_DIR)
from packet_create import create_sha_file
from packet_create import packet_bin

TOOLS_DIR = os.path.dirname(PKG_DIR)
SDK_DIR = os.path.dirname(TOOLS_DIR)
sys.path.append(os.path.join(SDK_DIR, "build", "script"))
sys.path.append(os.path.join(SDK_DIR, 'build', 'config'))
sys.path.append(os.path.join(SDK_DIR, "build", "script", "nv"))
from enviroment import TargetEnvironment
from param_packet import gen_flash_part_bin
from nv_binary import nv_begin

# brandy packet
def build_brandy_lfs_image(pack_style_str):
    ''' generate LittleFS file.bin from lfs_root/ into the target output dir '''
    chip_dir = os.path.join(SDK_DIR, "build", "config", "target_config", "brandy")
    lfs_script = os.path.join(chip_dir, "mk_littlefs_image", "generate_lfs_image.py")
    lfs_root   = os.path.join(chip_dir, "mk_littlefs_image", "lfs_root")
    output_dir = os.path.join(SDK_DIR, "output", "brandy", "acore", pack_style_str)
    file_bin   = os.path.join(output_dir, "file.bin")

    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)

    print("====== Generating LittleFS image ======")
    import subprocess
    ret = subprocess.call([sys.executable, lfs_script, lfs_root, file_bin])
    if ret != 0:
        print("ERROR: LittleFS image generation failed!")
        sys.exit(1)
    print("file.bin -> %s" % file_bin)


def build_brandy_other_bin(pack_style_str, flash_size, no_dsp):
    ''' build brandy partition and nv '''
    chip_dir = os.path.join(SDK_DIR, "build", "config", "target_config", "brandy")
    # 分区配置文件路径
    partition_cfg = os.path.join(chip_dir, "param_sector")
    if flash_size == "0x1000000":
        partition_cfg = os.path.join(partition_cfg, "param_sector_16M.json")
    elif flash_size == "0x400000":
        partition_cfg = os.path.join(partition_cfg, "param_sector_4M.json")
    else:
        if (no_dsp == "True"):
            partition_cfg = os.path.join(partition_cfg, "param_sector_no_dsp.json")
        else:
            partition_cfg = os.path.join(partition_cfg, "param_sector.json")
    
    partition_bin = os.path.join(SDK_DIR, "output", "brandy", "partition_table.bin")
    # nv配置文件路径
    nv_config_json = os.path.join(chip_dir, "nv_bin_cfg")
    if pack_style_str.endswith("-user"):
        nv_config_json = os.path.join(nv_config_json, "mk_nv_bin_cfg_user.json")
    else:
        nv_config_json = os.path.join(nv_config_json, "mk_nv_bin_cfg_eng.json")
    targets = ["acore"]
    # 输出路径
    nv_output_path = os.path.join(SDK_DIR, "output", "brandy", "nv_bin")

    if not os.path.exists(nv_output_path):
        os.makedirs(nv_output_path)

    gen_flash_part_bin(partition_cfg, partition_bin)
    nv_begin(nv_config_json, targets, 0)

def make_all_in_one_packet_8M(pack_style_str):
    # make all in one packet
    target_env = TargetEnvironment(pack_style_str)
    bin_dir = os.path.join(PKG_DIR, "bin", "brandy")
    application_dir = os.path.join(SDK_DIR,
                                   "output",
                                   "brandy",
                                   "acore",
                                   pack_style_str)
    recovery_target = "%s-r" % pack_style_str
    recovery_target = recovery_target.replace('-user', '')
    recovery_bin = os.path.join(SDK_DIR,
                                "output",
                                "brandy",
                                "acore",
                                recovery_target,
                                "recovery.bin")
    recovery_bx = recovery_bin + "|0x70050000|0x40000|100"

    upload_app_fwpkg = os.path.join(PKG_DIR, "fwpkg", "brandy", "upload_app.fwpkg")
    ssb = os.path.join(SDK_DIR, "output", "brandy", "acore", target_env.get("ssb_version"), "ssb.bin")
    if not os.path.exists(ssb):
        ssb = os.path.join(bin_dir, "ssb.bin")
    print("ssb_path: ", ssb)
    ssb_bx = ssb + "|0x70000000|0x20000|0"

    partition = os.path.join(SDK_DIR, "output", "brandy", "partition_table.bin")
    partition_bx = partition + "|0x7004F000|0x1000|100"
    nv = os.path.join(SDK_DIR, "output", "brandy", "nv_bin", "brandy_all_nv.bin")
    nv_bx = nv + "|0x707F9000|0x4000|100"

    app = os.path.join(application_dir, "application.bin")
    app_bx = app + "|0x702B0000|0x549000|18"

    bt = os.path.join(bin_dir, "bt.bin")
    bt_bx = bt + "|0x70090000|0x70000|20"

    dsp_version = target_env.get("dsp_version")
    dsp_main = os.path.join(bin_dir, "dsp", dsp_version, "dsp_main.bin")
    dsp_main_bx = dsp_main + "|0x70100000|0x70000|22"
    dsp_overlay = os.path.join(bin_dir, "dsp", dsp_version, "dsp_overlay.bin")
    dsp_overlay_bx = dsp_overlay + "|0x70170000|0x140000|26"

    fs = os.path.join(application_dir, "file.bin")
    fs_bx = fs + "|0xFFFFFFFF|0xFFFFFFFF|101"

    try:
        packet_post_agvs = list()
        packet_post_agvs.append(ssb_bx)
        packet_post_agvs.append(partition_bx)
        packet_post_agvs.append(app_bx)
        packet_post_agvs.append(recovery_bx)
        packet_post_agvs.append(bt_bx)
        packet_post_agvs.append(dsp_main_bx)
        packet_post_agvs.append(dsp_overlay_bx)
        packet_post_agvs.append(nv_bx)
        packet_post_agvs.append(fs_bx)
        packet_bin(upload_app_fwpkg, packet_post_agvs)
    except Exception as e:
        print(e)
        exit(-1)

def make_all_in_one_packet_no_dsp(pack_style_str):
    # make all in one packet
    target_env = TargetEnvironment(pack_style_str)
    bin_dir = os.path.join(PKG_DIR, "bin", "brandy")
    application_dir = os.path.join(SDK_DIR,
                                   "output",
                                   "brandy",
                                   "acore",
                                   pack_style_str)
    recovery_target = "%s-r" % pack_style_str
    recovery_target = recovery_target.replace('-user', '')
    recovery_bin = os.path.join(SDK_DIR,
                                "output",
                                "brandy",
                                "acore",
                                recovery_target,
                                "recovery.bin")
    recovery_bx = recovery_bin + "|0x70050000|0x40000|100"

    upload_app_fwpkg = os.path.join(PKG_DIR, "fwpkg", "brandy", "upload_app.fwpkg")

    ssb = os.path.join(SDK_DIR, "output", "brandy", "acore", target_env.get("ssb_version"), "ssb.bin")
    if not os.path.exists(ssb):
        ssb = os.path.join(bin_dir, "ssb.bin")
    print("ssb_path: ", ssb)
    ssb_bx = ssb + "|0x70000000|0x20000|0"

    partition = os.path.join(SDK_DIR, "output", "brandy", "partition_table.bin")
    partition_bx = partition + "|0x7004F000|0x1000|100"
    nv = os.path.join(SDK_DIR, "output", "brandy", "nv_bin", "brandy_all_nv.bin")
    nv_bx = nv + "|0x707F9000|0x4000|100"

    app = os.path.join(application_dir, "application.bin")
    app_bx = app + "|0x70100000|0x6F9000|18"

    bt = os.path.join(bin_dir, "bt.bin")
    bt_bx = bt + "|0x70090000|0x70000|20"

    fs = os.path.join(application_dir, "file.bin")
    fs_bx = fs + "|0xFFFFFFFF|0xFFFFFFFF|101"

    try:
        packet_post_agvs = list()
        packet_post_agvs.append(ssb_bx)
        packet_post_agvs.append(partition_bx)
        packet_post_agvs.append(app_bx)
        packet_post_agvs.append(recovery_bx)
        packet_post_agvs.append(bt_bx)
        packet_post_agvs.append(nv_bx)
        packet_post_agvs.append(fs_bx)
        packet_bin(upload_app_fwpkg, packet_post_agvs)
    except Exception as e:
        print(e)
        exit(-1)

def make_all_in_one_packet_16M(pack_style_str):
    # make all in one packet
    target_env = TargetEnvironment(pack_style_str)
    bin_dir = os.path.join(PKG_DIR, "bin", "brandy")
    application_dir = os.path.join(SDK_DIR,
                                   "output",
                                   "brandy",
                                   "acore",
                                   pack_style_str)
    recovery_target = "%s-r" % pack_style_str
    recovery_target = recovery_target.replace('-user', '')
    recovery_bin = os.path.join(SDK_DIR,
                                "output",
                                "brandy",
                                "acore",
                                recovery_target,
                                "recovery.bin")
    recovery_bx = recovery_bin + "|0x70B80000|0x100000|100"

    upload_app_fwpkg = os.path.join(PKG_DIR, "fwpkg", "brandy", "upload_app.fwpkg")

    ssb = os.path.join(SDK_DIR, "output", "brandy", "acore", target_env.get("ssb_version"), "ssb.bin")
    if not os.path.exists(ssb):
        ssb = os.path.join(bin_dir, "ssb.bin")
    print("ssb_path: ", ssb)
    ssb_bx = ssb + "|0x70000000|0x20000|0"

    partition = os.path.join(SDK_DIR, "output", "brandy", "partition_table.bin")
    partition_bx = partition + "|0x7004F000|0x1000|100"
    nv = os.path.join(SDK_DIR, "output", "brandy", "nv_bin", "brandy_all_nv.bin")
    nv_bx = nv + "|0x70C80000|0x4000|100"

    app = os.path.join(application_dir, "application.bin")
    app_bx = app + "|0x70380000|0x800000|18"

    bt = os.path.join(bin_dir, "bt.bin")
    bt_bx = bt + "|0x70090000|0x70000|20"

    dsp_version = target_env.get("dsp_version")
    dsp_main = os.path.join(bin_dir, "dsp", dsp_version, "dsp_main.bin")
    dsp_main_bx = dsp_main + "|0x70100000|0xC0000|22"
    dsp_overlay = os.path.join(bin_dir, "dsp", dsp_version, "dsp_overlay.bin")
    dsp_overlay_bx = dsp_overlay + "|0x701C0000|0x1C0000|26"

    fs = os.path.join(application_dir, "file.bin")
    fs_bx = fs + "|0xFFFFFFFF|0xFFFFFFFF|101"

    try:
        packet_post_agvs = list()
        packet_post_agvs.append(ssb_bx)
        packet_post_agvs.append(partition_bx)
        packet_post_agvs.append(app_bx)
        packet_post_agvs.append(recovery_bx)
        packet_post_agvs.append(bt_bx)
        packet_post_agvs.append(dsp_main_bx)
        packet_post_agvs.append(dsp_overlay_bx)
        packet_post_agvs.append(nv_bx)
        packet_post_agvs.append(fs_bx)
        packet_bin(upload_app_fwpkg, packet_post_agvs)
    except Exception as e:
        print(e)
        exit(-1)

def make_all_in_one_packet_4M(pack_style_str):
    # make all in one packet
    target_env = TargetEnvironment(pack_style_str)
    bin_dir = os.path.join(PKG_DIR, "bin", "brandy")
    application_dir = os.path.join(SDK_DIR,
                                   "output",
                                   "brandy",
                                   "acore",
                                   pack_style_str)
    upload_app_fwpkg = os.path.join(PKG_DIR, "fwpkg", "brandy", "upload_app.fwpkg")
    ssb = os.path.join(SDK_DIR, "output", "brandy", "acore", target_env.get("ssb_version"), "ssb.bin")
    if not os.path.exists(ssb):
        ssb = os.path.join(bin_dir, "ssb.bin")
    print("ssb_path: ", ssb)
    ssb_bx = ssb + "|0x70000000|0x20000|0"

    partition = os.path.join(SDK_DIR, "output", "brandy", "partition_table.bin")
    partition_bx = partition + "|0x7004F000|0x1000|100"
    nv = os.path.join(SDK_DIR, "output", "brandy", "nv_bin", "brandy_all_nv.bin")
    nv_bx = nv + "|0x703B0000|0x4000|100"

    app = os.path.join(application_dir, "application.bin")
    app_bx = app + "|0x701B0000|0x200000|18"

    bt = os.path.join(bin_dir, "bt.bin")
    bt_bx = bt + "|0x70090000|0x70000|20"

    dsp_version = target_env.get("dsp_version")
    dsp_main = os.path.join(bin_dir, "dsp", dsp_version, "dsp_main.bin")
    dsp_main_bx = dsp_main + "|0x70100000|0x70000|22"
    dsp_overlay = os.path.join(bin_dir, "dsp", dsp_version, "dsp_overlay.bin")
    dsp_overlay_bx = dsp_overlay + "|0x70170000|0x40000|26"

    try:
        packet_post_agvs = list()
        packet_post_agvs.append(ssb_bx)
        packet_post_agvs.append(partition_bx)
        packet_post_agvs.append(app_bx)
        packet_post_agvs.append(bt_bx)
        packet_post_agvs.append(dsp_main_bx)
        packet_post_agvs.append(dsp_overlay_bx)
        packet_post_agvs.append(nv_bx)
        packet_bin(upload_app_fwpkg, packet_post_agvs)
    except Exception as e:
        print(e)
        exit(-1)


def make_all_in_one_packet(pack_style_str, flash_size, no_dsp):
    # make all in one packet
    build_brandy_other_bin(pack_style_str, flash_size, no_dsp)
    build_brandy_lfs_image(pack_style_str)   # generate file.bin from lfs_root/
    if flash_size == "0x1000000":
        make_all_in_one_packet_16M(pack_style_str)
    elif flash_size == "0x400000":
        make_all_in_one_packet_4M(pack_style_str)
    else:
        if no_dsp == "True":
            make_all_in_one_packet_no_dsp(pack_style_str)
        else:
            make_all_in_one_packet_8M(pack_style_str)

def is_packing_files_exist(soc, pack_style_str):
    """
    判断打包文件是否存在
    :return:
    """
    packing_files = get_packing_files(soc, pack_style_str)
    lost_files = list()
    for f_path in packing_files:
        if not os.path.isfile(f_path):
            lost_files.append(f_path)
    return lost_files

def get_packing_files(soc, pack_style_str):
    """
    直接添加需要打包的文件路径
    :return:
    """
    packing_files = list()
    packing_files.append(os.path.join(PKG_DIR, "bin", "brandy", "ssb.bin"))
    application_path = os.path.join(SDK_DIR,
                                    "output",
                                    "brandy",
                                    "acore",
                                    pack_style_str,
                                    "application.bin")
    packing_files.append(application_path)
    packing_files.append(os.path.join(PKG_DIR, "bin", "brandy", "bt.bin"))

    target_env = TargetEnvironment(pack_style_str)
    dsp_version = target_env.get("dsp_version")
    packing_files.append(os.path.join(PKG_DIR, "bin", "brandy", "dsp", dsp_version, "dsp_main.bin"))
    packing_files.append(os.path.join(PKG_DIR, "bin", "brandy", "dsp", dsp_version, "dsp_overlay.bin"))
    return packing_files
