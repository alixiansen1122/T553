/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "common/graphic_startup.h"
#include "animator/animator_manager.h"
#include "common/input_device_manager.h"
#include "common/task_manager.h"
#include "core/render_manager.h"
#include "dfx/performance_task.h"
#include "font/ui_font.h"
#if ENABLE_ICU
#include "font/ui_line_break.h"
#endif
#if ENABLE_SHAPING
#include "font/ui_text_shaping.h"
#endif
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "imgdecode/cache_manager.h"
#ifdef VERSION_STANDARD
#include "dock/ohos/ohos_input_device.h"
#endif
#if ENABLE_WINDOW
#include "iwindows_manager.h"
#endif
#if ENABLE_GFX_ENGINES
#include "hals/gfx_engines.h"
#include "hals/gralloc_engines.h"
#endif
#include "securec.h"
#if ENABLE_MEMORY_CHECK
#include "gfx_utils/mem_check.h"
#endif
#ifdef VERSION_IOT
#include "default_resource/check_box_res.h"
#include "dfx/dfx_cmd.h"
#include "dock/input_device.h"
#include "dock/lite_touch_device.h"
#include "dock/lite_key_device.h"
#include "hals/lite_input_hal.h"
#include "hals/display_dev.h"
#include "hals/gralloc_engines.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "font/ui_multi_font_manager.h"
#include "font/ui_text_shaping_ext.h"
#endif
#if ENABLE_SOFT_KEYBOARD
#include "common/input_method_manager.h"
#include "input_method/ui_input_method.h"
#endif
namespace OHOS {
void GraphicStartUp::InitFontEngine(uintptr_t cacheMemAddr,
                                    uint32_t cacheMemLen,
                                    const char* dPath,
                                    const char* ttfName)
{
    UIFont* uiFont = UIFont::GetInstance();
    if (uiFont == nullptr) {
        GRAPHIC_LOGE("Get UIFont error");
        return;
    }
    uiFont->SetPsramMemory(cacheMemAddr, cacheMemLen);
    int8_t ret = uiFont->SetFontPath(const_cast<char*>(dPath), nullptr);
    if (ret == INVALID_RET_VALUE) {
        GRAPHIC_LOGW("SetFontPath failed");
    }
    if (uiFont->IsVectorFont()) {
        if (ttfName != nullptr) {
            uint8_t ret2 = uiFont->RegisterFontInfo(ttfName);
            if (ret2 == INVALID_UCHAR_ID) {
                GRAPHIC_LOGW("SetTtfName failed");
            }
        }
    } else {
        (void)uiFont->SetCurrentLangId(0); // set language
    }

#if ENABLE_SHAPING
    uint8_t *g_shapingMemAddr = static_cast<uint8_t*>(UIMalloc(MIN_SHAPING_PSRAM_LENGTH));
    if (!UITextShapingExt::GetInstance()->SetPsramMemory(reinterpret_cast<uintptr_t>(g_shapingMemAddr),
        MIN_SHAPING_PSRAM_LENGTH)) {
        GRAPHIC_LOGE("UITextShapingExt SetPsram failed.");
        return;
    }
#endif
#if ENABLE_ICU
    uint8_t *g_icuMemBaseAddr = static_cast<uint8_t*>(UIMalloc(SHAPING_WORD_DICT_LENGTH));
    InitLineBreakEngine(reinterpret_cast<uintptr_t>(g_icuMemBaseAddr), SHAPING_WORD_DICT_LENGTH,
        VECTOR_FONT_DIR, DEFAULT_LINE_BREAK_RULE_FILENAME);
#endif
}

void GraphicStartUp::InitLineBreakEngine(uintptr_t cacheMemAddr, uint32_t cacheMemLen, const char* path,
                                         const char* fileName)
{
#if ENABLE_ICU
    if ((path == nullptr) || (fileName == nullptr) || cacheMemLen < OHOS::SHAPING_WORD_DICT_LENGTH) {
        return;
    }
    uint32_t len = static_cast<uint32_t>(strlen(path) + strlen(fileName) + 1);
    char* lineBreakRuleFile = reinterpret_cast<char*>(UIMalloc(len));
    if (lineBreakRuleFile == nullptr) {
        GRAPHIC_LOGW("UIMalloc failed");
        return;
    }
    if (strcpy_s(lineBreakRuleFile, len, path) != EOK) {
        UIFree(reinterpret_cast<void*>(lineBreakRuleFile));
        lineBreakRuleFile = nullptr;
        return;
    }
    if (strcat_s(lineBreakRuleFile, len, fileName) != EOK) {
        UIFree(reinterpret_cast<void*>(lineBreakRuleFile));
        lineBreakRuleFile = nullptr;
        return;
    }
    int32_t fp;
#ifdef _WIN32
    fp = open(reinterpret_cast<const char*>(lineBreakRuleFile), O_RDONLY | O_BINARY);
#else
    fp = open(reinterpret_cast<const char*>(lineBreakRuleFile), O_RDONLY);
#endif
    if (fp < 0) {
        UIFree(reinterpret_cast<void*>(lineBreakRuleFile));
        lineBreakRuleFile = nullptr;
        GRAPHIC_LOGW("Open lineBreak rule file failed");
        return;
    }
    int32_t lineBreakSize = lseek(fp, 0, SEEK_END);
    if (lineBreakSize < 0) {
        UIFree(reinterpret_cast<void*>(lineBreakRuleFile));
        lineBreakRuleFile = nullptr;
        close(fp);
        return;
    }
    lseek(fp, 0, SEEK_SET);
    UILineBreakEngine& lbEngine = UILineBreakEngine::GetInstance();
    lbEngine.SetRuleBinInfo(fp, 0, lineBreakSize);
    lbEngine.SetRuleFileLoadAddr(reinterpret_cast<char*>(cacheMemAddr));
    lbEngine.Init();
    UIFree(reinterpret_cast<void*>(lineBreakRuleFile));
    lineBreakRuleFile = nullptr;
    close(fp);
#endif
}

void GraphicStartUp::Init()
{
#if ENABLE_MEMORY_CHECK
    OHOS::MemCheck::GetInstance()->MemCheckInit();
#endif

#ifdef VERSION_IOT
    GrallocEngines::GetInstance()->Init();
    DisplayDev::GetInstance()->DisplayDevInit();
#endif

    TaskManager::GetInstance()->SetTaskRun(true);
    DEBUG_PERFORMANCE_TASK_INIT();
    RenderManager::GetInstance().Init();
    AnimatorManager::GetInstance()->Init();
    if (INDEV_READ_PERIOD > 0) {
        InputDeviceManager::GetInstance()->Init();
    }

    StyleDefault::Init();
    CacheManager::GetInstance().Init(IMG_CACHE_SIZE);
#ifdef VERSION_IOT
    InitCheckBoxRes();
#if ENABLE_VGU_ENGINE
    LiteMGfxEngine::GetInstance()->InitDriver();
#endif
#if ENABLE_DFX_CMD
    InitDfxCmd();
#endif
    BaseGfxEngine::GetInstance()->InitGfxEngine(LiteMGfxEngine::GetInstance());
#elif defined VERSION_STANDARD
    OHOSInputDevice* input = new OHOSInputDevice();
    if (input == nullptr) {
        GRAPHIC_LOGE("new OHOSInputDevice fail");
        return;
    }
    InputDeviceManager::GetInstance()->Add(input);
#endif

#if ENABLE_WINDOW
    IWindowsManager::GetInstance()->Init();
#endif
#if ENABLE_GFX_ENGINES
    GfxEngines::GetInstance()->InitDriver();
    GrallocEngines::GetInstance()->Init();
#endif
#if ENABLE_SOFT_KEYBOARD
    InputMethodManager::GetInstance().SetInputMethodListener(UIInputMethod::GetInstance());
#endif
}
} // namespace OHOS
