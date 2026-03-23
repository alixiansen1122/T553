/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_DFX_CMD_H
#define LV_DFX_CMD_H

#include "lvgl.h"
#include "lv_conf.h"
#include "draw/lv_img_buf.h"

#ifdef __cplusplus
extern "C" {
#endif

bool ExecLvglDfxCmd(int argc, char* argv[]);

#if LV_USE_DFX_CMD
#if LV_USE_PERF_MONITOR_EXT
bool PrintPerf(int argc, char* argv[]);

void ShowPerf(int argc, char* argv[]);

void HidePerf(int argc, char* argv[]);
#endif

#if LV_USE_SNAPSHOT_EXT
bool Screencap(int argc, char* argv[]);

bool ScreencapToFile(void);
#endif

bool InjectClick(int argc, char* argv[]);

bool InjectDrag(int argc, char* argv[]);

bool InjectLongPress(int argc, char* argv[]);

bool InjectKey(int argc, char* argv[]);

bool InjectKeyLongPress(int argc, char* argv[]);

bool InjectRotate(int argc, char* argv[]);

bool OutputDscToFile(lv_img_dsc_t* dsc, const char* filePath);
#endif

#if LV_USE_DFX_TRACE
bool ShowTime(int argc, char *argv[]);
#endif

void DumpMemory(int argc, char *argv[]);

bool SetMemMonitor(int argc, char *argv[]);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // LV_DFX_CMD_H
