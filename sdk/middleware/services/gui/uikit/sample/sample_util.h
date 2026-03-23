/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: sample utils
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef SAMPLE_UTIL_H
#define SAMPLE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
  功能描述	:  计算当前画面截图后文件的hash值，并打印出来。
  参数内容	:  参数包含一个参数(key)，打印在时一同输出。用于关联定位。
*/
int CalcSampleUiHash(const char* viewId);


#ifdef __cplusplus
}
#endif
#endif