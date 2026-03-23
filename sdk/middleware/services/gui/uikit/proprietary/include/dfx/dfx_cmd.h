/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup UI_Dfx_Cmd
 * @{
 *
 * @brief Provides test and analysis capabilities, such as stimulating input events and viewing information about a
 *        Document Object Model (DOM) tree.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef GRAPHIC_LITE_DFX_CMD_H
#define GRAPHIC_LITE_DFX_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Init dfx cmd if needed.
 *
 */
void InitDfxCmd(void);

/**
 * @brief Save screen capture to file.
 *
 * @return true
 * @return false
 */
bool SaveScreencapToFile(void);

/**
 * @brief Main entrance to execute dfx command.
 *
 */
bool ExecUikitDfxByCmdType(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif
#endif // GRAPHIC_LITE_DFX_CMD_H
/**
 * @}
 */
