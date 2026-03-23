/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: sample utils
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef SAMPLE_UI_H
#define SAMPLE_UI_H

#ifdef __cplusplus
extern "C" {
#endif

int SampleUi(void);

/*
  功能描述	:  启动指定用例
  参数内容	:  参数为用例名称
*/
int StartCaseUi(const char* testCaseId);

#ifdef __cplusplus
}
#endif
#endif