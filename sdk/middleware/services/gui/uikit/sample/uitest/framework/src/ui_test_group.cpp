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

#include "ui_test_group.h"
#include "graphic_config.h"
#if ENABLE_BITMAP_FONT
#include "ui_test_bitmap_font/ui_test_bitmap_font.h"
#endif
#include "ui_test_chart/ui_test_chart_polyline.h"
#include "ui_test_chart/ui_test_chart_pillar.h"
#include "ui_test_draw/ui_test_draw.h"
#include "ui_test_draw_rect/ui_test_draw_rect.h"
#include "ui_test_image/ui_test_image.h"
#include "ui_test_image_fps/ui_test_image_fps.h"
#include "ui_test_key/ui_test_key.h"
#include "ui_test_list/ui_test_list.h"
#include "ui_test_list_ext/ui_test_list_ext.h"
#include "ui_test_qrcode/ui_test_qrcode.h"
#if ENABLE_DEBUG && ENABLE_ROTATE_INPUT
#include "ui_test_rotate_injector/ui_test_rotate_injector.h"
#endif
#include "ui_test_slider/ui_test_slider.h"
#if ENABLE_VECTOR_FONT
#include "ui_test_font/ui_test_font.h"
#endif
#if (ENABLE_FONT_VECTOR_GLOBAL && ENABLE_FONT_GLOBAL_TEST)
#include "ui_test_font/ui_test_font_global.h"
#endif

#if (ENABLE_IMAGE_PACKER && ENABLE_IMAGE_PACKER_TEST)
#include "ui_test_image/ui_test_image_packer.h"
#endif
#ifndef _WIN32
#include "ui_test_video_play/ui_test_video_play.h"
#endif
#include "ui_test_capture/ui_test_capture.h"
#if ENABLE_SOFT_KEYBOARD
#include "ui_test_keyboard/ui_test_keyboard.h"
#endif
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
#include "ui_test_draw/ui_test_canvas_ext.h"
#include "ui_test_countdown/ui_test_countdown.h"
#include "ui_test_running_ball/ui_test_running_ball.h"
#ifndef _WIN32
#include "ui_test_svg/ui_test_svg.h"
#endif
#endif
#if CAPABILITY_HW_JPEG_DECODE
#include "ui_test_image/ui_test_jpeg.h"
#endif
#if ENABLE_JPEG_AND_PNG
#include "ui_test_image/ui_test_png.h"
#endif
#if ENABLE_ICU && ENABLE_MULTI_FONT && ENABLE_SHAPING
#include "ui_test_font/ui_test_multi_font.h"
#endif
#include "ui_test_icosahedron/ui_test_icosahedron.h"
#include "ui_test_texture_mapper/ui_test_texture_mapper.h"
#include "ui_test_label/ui_test_arc_label.h"
#include "ui_test_clock_dial/ui_test_clock_dial.h"
#include "ui_test_compass/ui_test_compass.h"
#include "ui_test_sweepclock/ui_test_sweepclock.h"
#include "ui_test_text_fps/ui_test_text_fps.h"
#include "ui_test_tiger/ui_test_tiger.h"
#include "ui_test_hexagons_list/ui_test_hexagons_list.h"
#include "ui_test_waterfall_list/ui_test_waterfall_list.h"
#include "ui_test_list_fps/ui_test_list_fps.h"
#include "ui_test_card_switch/ui_test_card_switch.h"
#include "ui_test_message/ui_test_message.h"
#include "ui_test_breath/ui_test_breath.h"
#include "ui_test_cube_rotate/ui_test_cube_rotate.h"
#include "ui_test_cross/ui_test_cross.h"
#include "ui_test_map/ui_test_map.h"
#include "ui_test_coverflow/ui_test_coverflow.h"
#include "ui_test_coverflow/ui_test_coverflow2.h"
#include "ui_test_kaleidoscope/ui_test_kaleidoscope.h"
#include "ui_test_roller_view/ui_test_roller_view.h"
#include "ui_test_emoji/ui_test_emoji.h"
#include "ui_test_particle/ui_test_particle.h"
#include "ui_test_lottie/ui_test_lottie.h"
#include "ui_test_lottie/ui_test_trim_path.h"
#include "ui_test_button/ui_test_button.h"
#include "ui_test_picker/ui_test_ui_picker.h"
#include "ui_test_progress_bar/ui_test_box_progress.h"
#include "ui_test_progress_bar/ui_test_circle_progress.h"
#include "ui_test_ui_list/ui_test_ui_list.h"
#include "ui_test_ui_list/ui_test_ui_list.h"
#include "ui_test_ui_list_view/ui_test_list_layout.h"
#include "ui_test_ui_scroll_view/ui_test_ui_scroll_view.h"
#include "ui_test_ui_swipe_view/ui_test_ui_swipe_view.h"
#include "ui_test_canvas/ui_test_canvas.h"
#include "ui_test_scroll_bar/ui_test_scroll_bar.h"
#include "ui_test_input_event/ui_test_input_event.h"
#include "ui_test_view_group/ui_test_view_group.h"
#include "ui_test_animator/ui_test_animator.h"
#include "ui_test_border_margin_padding/ui_test_border_margin_padding.h"
#include "ui_test_digital_clock/ui_test_digital_clock.h"
#include "ui_test_draw_line/ui_test_draw_line.h"
#include "ui_test_event_injector/ui_test_event_injector.h"
#include "ui_test_focus_manager/ui_test_focus_manager.h"
#include "ui_test_image_animator/ui_test_image_animator.h"
#include "ui_test_opacity/ui_test_opacity.h"
#include "ui_test_render/ui_test_render.h"
#if ENABLE_VECTOR_FONT
#include "ui_test_vector_font/ui_test_vector_font.h"
#endif
#include "ui_test_rotate_input/ui_test_rotate_input.h"
#include "ui_test_analog_clock/ui_test_analog_clock.h"
#include "ui_test_view_bounds/ui_test_view_bounds.h"
#include "ui_test_view_percent/ui_test_view_percent.h"
#include "ui_test_view_scale_rotate/ui_test_view_scale_rotate.h"
#include "ui_test_dump_dom_tree/ui_test_dump_dom.h"
#include "ui_test_view_bitmap/ui_test_view_bitmap.h"
#include "ui_test_view_alignment/ui_test_alignment_external.h"
#include "ui_test_view_alignment/ui_test_alignment_internal.h"
#include "ui_test_view_alignment/ui_test_alignment_dynamic.h"
#include "ui_test_simple_list/ui_test_simple_list.h"
#include "ui_test_simple_list/ui_test_simple_transform_list.h"
#include "ui_test_float_box/ui_test_float_box.h"
#include "ui_test_chart/ui_test_scatter_plot.h"

namespace OHOS {
void UITestGroup::AddTestCase(TestCaseInfo testCaseInfo)
{
    if (testCaseList_ == nullptr) {
        return;
    }
    testCaseList_->PushBack(testCaseInfo);
}

void UITestGroup::SetUpTestCase()
{
    testCaseList_ = new List<TestCaseInfo>();
    if (testCaseList_ == nullptr) {
        return;
    }
    testCaseList_->Clear();
    testCaseList_->PushBack(TestCaseInfo{"散点图", new UITestScatterPlot()});
    testCaseList_->PushBack(TestCaseInfo{"悬浮框", new UITestFloatBox()});
    testCaseList_->PushBack(TestCaseInfo{"Lottie", new UITestLottie()});
    testCaseList_->PushBack(TestCaseInfo{"路径裁剪", new UITestTrimPath()});
    testCaseList_->PushBack(TestCaseInfo{"粒子效果", new UITestParticle()});
    testCaseList_->PushBack(TestCaseInfo{"瀑布列表", new UITestWaterfallList()});
    testCaseList_->PushBack(TestCaseInfo{"老虎头", new UITestTiger()});
#ifndef QSPI_DISPLAY
    testCaseList_->PushBack(TestCaseInfo{"蜂窝表盘", new UITestHexagonsList()});
#endif
    testCaseList_->PushBack(TestCaseInfo{"无变换滑动列表", new UITestListFPS()});
    testCaseList_->PushBack(TestCaseInfo{"带缩放滑动列表", new UITestListExt()});
    testCaseList_->PushBack(TestCaseInfo{"简易列表", new UITestSimpleList()});
    testCaseList_->PushBack(TestCaseInfo{"简易变换列表", new UITestSimpleTransformList()});
    testCaseList_->PushBack(TestCaseInfo{"扫秒", new UITestSweepClock()});
    testCaseList_->PushBack(TestCaseInfo{"指南针", new UITestCompass()});
    testCaseList_->PushBack(TestCaseInfo{"全屏文字", new UITestTextFPS()});
    testCaseList_->PushBack(TestCaseInfo{"立方体旋转", new UITestCubeRotate()});
    testCaseList_->PushBack(TestCaseInfo{"卡片切换", new UITestCardSwitch()});
    testCaseList_->PushBack(TestCaseInfo{"消息", new UITestMessage()});
    testCaseList_->PushBack(TestCaseInfo{"呼吸效果", new UITestBreath()});
#ifndef QSPI_DISPLAY
    testCaseList_->PushBack(TestCaseInfo{"地图", new UITestMap()});
#if !defined(_WIN32)
    testCaseList_->PushBack(TestCaseInfo{"视频播放", new UITestVideoPlay()});
#endif
    testCaseList_->PushBack(TestCaseInfo{"翻转动效", new UITestCross()});
#if !defined(_WIN32)
    testCaseList_->PushBack(TestCaseInfo{"万花筒", new UITestKaleidoscope()});
#endif
    testCaseList_->PushBack(TestCaseInfo{"容器覆盖流", new UITestCoverflow2()});
    testCaseList_->PushBack(TestCaseInfo{"覆盖流", new UITestCoverflow()});
#endif
    testCaseList_->PushBack(TestCaseInfo{"滚筒", new UITestRollerView()});
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    testCaseList_->PushBack(TestCaseInfo{"倒计时", new UITestCountdown()});
    testCaseList_->PushBack(TestCaseInfo{"动画曲线", new UITestRunningBall()});
    testCaseList_->PushBack(TestCaseInfo{"扩展画布", new UITestCanvasExt()});
#ifndef _WIN32
    testCaseList_->PushBack(TestCaseInfo{"绘制SVG", new UITestSVG()});
#endif
#endif
    testCaseList_->PushBack(TestCaseInfo{"足球效果", new UITestIcosahedron()});
#if ENABLE_ICU && ENABLE_MULTI_FONT && ENABLE_SHAPING
    testCaseList_->PushBack(TestCaseInfo{"多语言混排", new UITestMultiFont()});
#endif
#if ENABLE_SOFT_KEYBOARD
    testCaseList_->PushBack(TestCaseInfo{"软键盘", new UITestKeyboard()});
#endif
#if CAPABILITY_HW_JPEG_DECODE
    testCaseList_->PushBack(TestCaseInfo{"JPEG显示", new UITestJPEG()});
#endif
#if ENABLE_JPEG_AND_PNG
    testCaseList_->PushBack(TestCaseInfo{"PNG显示", new UITestPNG()});
#endif
    testCaseList_->PushBack(TestCaseInfo{"弧形文字", new UITestArcLabel()});
    testCaseList_->PushBack(TestCaseInfo{"图片旋转缩放动画", new UITestTextureMapper()});
    testCaseList_->PushBack(TestCaseInfo{"截图", new UITestCapture()});
    testCaseList_->PushBack(TestCaseInfo{"全屏高斯模糊", new UITestKey()});
#if ENABLE_DEBUG && ENABLE_ROTATE_INPUT
    testCaseList_->PushBack(TestCaseInfo{"模拟表冠", new UITestRotateInjector()});
#endif
    testCaseList_->PushBack(TestCaseInfo{"二维码", new UITestQrcode()});
    testCaseList_->PushBack(TestCaseInfo{"折线图", new UITestChartPolyline()});
    testCaseList_->PushBack(TestCaseInfo{"柱状图", new UITestChartPillar()});
#if ENABLE_BITMAP_FONT
    testCaseList_->PushBack(TestCaseInfo{"点阵文字", new UITestBitmapFont()});
#endif
#if ENABLE_VECTOR_FONT
    testCaseList_->PushBack(TestCaseInfo{"矢量文字", new UITestFont()});
#endif
#if (ENABLE_FONT_VECTOR_GLOBAL && ENABLE_FONT_GLOBAL_TEST)
    testCaseList_->PushBack(TestCaseInfo{"多语言切换", new UITestFontGlobal()});
#endif
    testCaseList_->PushBack(TestCaseInfo{"画布", new UITestDraw()});
    testCaseList_->PushBack(TestCaseInfo{"图片显示", new UITestImage()});
    testCaseList_->PushBack(TestCaseInfo{"矩形绘制", new UITestDrawRect()});
#if (ENABLE_IMAGE_PACKER && ENABLE_IMAGE_PACKER_TEST)
    testCaseList_->PushBack(TestCaseInfo{"图片打包", new UITestImageBin()});
#endif
    testCaseList_->PushBack(TestCaseInfo{"列表", new UITestList()});
    testCaseList_->PushBack(TestCaseInfo{"滑动条", new UITestSlider()});
    testCaseList_->PushBack(TestCaseInfo{"图片动画", new UITestImageFPS()});
    testCaseList_->PushBack(TestCaseInfo{"切换动效", new UITestClockDial()});
    testCaseList_->PushBack(TestCaseInfo{"透明度", new UITestOpacity()});
    testCaseList_->PushBack(TestCaseInfo{"播放图片动画", new UITestImageAnimator()});
    testCaseList_->PushBack(TestCaseInfo{"焦点管理", new UITestFocusManager()});
    testCaseList_->PushBack(TestCaseInfo{"绘制线条", new UITestDrawLine()});
    testCaseList_->PushBack(TestCaseInfo{"数字时钟", new UITestDigitalClock()});
    testCaseList_->PushBack(TestCaseInfo{"边界间隔", new UITestBorderMarginPadding()});
    testCaseList_->PushBack(TestCaseInfo{"动效", new UITestAnimator()});
    testCaseList_->PushBack(TestCaseInfo{"视图组", new UITestViewGroup()});
    testCaseList_->PushBack(TestCaseInfo{"事件测试", new UITestInputEvent()});
    testCaseList_->PushBack(TestCaseInfo{"滚动条", new UITestScrollBar()});
    testCaseList_->PushBack(TestCaseInfo{"画布", new UITestCanvas()});
    testCaseList_->PushBack(TestCaseInfo{"Swipe视图", new UITestUISwipeView()});
    testCaseList_->PushBack(TestCaseInfo{"滑动视图", new UITestUIScrollView()});
    testCaseList_->PushBack(TestCaseInfo{"列表布局", new UITestListLayout()});
    testCaseList_->PushBack(TestCaseInfo{"列表", new UITestUIList()});
    testCaseList_->PushBack(TestCaseInfo{"按钮", new UITestButton()});
    testCaseList_->PushBack(TestCaseInfo{"选择", new UITestUIPicker()});
    testCaseList_->PushBack(TestCaseInfo{"圆形进度条", new UITestCircleProgress()});
    testCaseList_->PushBack(TestCaseInfo{"盒状进度条", new UITestBoxProgress()});
    testCaseList_->PushBack(TestCaseInfo{"事件模拟", new UITestEventInjector()});
    testCaseList_->PushBack(TestCaseInfo{"DOM转储", new UITestDumpDomTree()});
    testCaseList_->PushBack(TestCaseInfo{"旋转", new UITestViewScaleRotate()});
    testCaseList_->PushBack(TestCaseInfo{"视图比例", new UITestViewPercent()});
    testCaseList_->PushBack(TestCaseInfo{"视图边界", new UITestViewBounds()});
#if ENABLE_VECTOR_FONT
    testCaseList_->PushBack(TestCaseInfo{"矢量文字", new UITestVectorFont()});
#endif
    testCaseList_->PushBack(TestCaseInfo{"模拟时钟", new UITestAnalogClock()});
    testCaseList_->PushBack(TestCaseInfo{"旋转输入", new UITestRotateInput()});
    testCaseList_->PushBack(TestCaseInfo{"渲染", new UITestRender()});
    testCaseList_->PushBack(TestCaseInfo{"外部对齐", new UITestExternalAlignment()});
    testCaseList_->PushBack(TestCaseInfo{"内部对齐", new UITestInternalAlignment()});
    testCaseList_->PushBack(TestCaseInfo{"动态对齐", new UITestDynamicAlignment()});
}

List<TestCaseInfo>& UITestGroup::GetTestCase()
{
    return *testCaseList_;
}

UITest* UITestGroup::GetTestCase(const char* id)
{
    if (id == nullptr) {
        return nullptr;
    }
    ListNode<TestCaseInfo>* node = testCaseList_->Begin();
    while (node != testCaseList_->End()) {
        if (!strcmp(id, node->data_.sliceId)) {
            return node->data_.testObj;
        }
        node = node->next_;
    }
    return nullptr;
}

void UITestGroup::TearDownTestCase()
{
    ListNode<TestCaseInfo>* node = testCaseList_->Begin();
    while (node != testCaseList_->End()) {
        delete node->data_.testObj;
        node->data_.testObj = nullptr;
        node = node->next_;
    }
    testCaseList_->Clear();
}
} // namespace OHOS
