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

#include "ui_test_text_fps.h"

namespace OHOS {
UITestTextFPS::UITestTextFPS() {}

void UITestTextFPS::ScrollTextCallback::Callback(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    UIList* list = static_cast<UIList*>(view);
    if (list == nullptr) {
        return;
    }
    list->ScrollBy(-20); // -20: scroll distance
}

bool UITestTextFPS::TextList::OnPressEvent(const PressEvent& event)
{
    if (animator_ != nullptr) {
        animator_->Stop();
    }
    return UIList::OnPressEvent(event);
}

bool UITestTextFPS::TextList::OnLongPressEvent(const LongPressEvent& event)
{
    if (animator_ != nullptr) {
        animator_->Start();
    }
    return true;
}

void UITestTextFPS::TextList::SetAnimator(Animator* animator)
{
    animator_ = animator;
}

void UITestTextFPS::SetUp()
{
    if (adapterData_ == nullptr) {
        adapterData_ = new List<const char*>();
    }

    adapterData_->PushBack("36号全屏字体测试用例。请欣赏现代诗人汪国真发表的我微笑着走向生活。"
        "我微笑着走向生活，无论生活以什么方式回敬我。报我以平坦吗？我是一条欢乐奔流的小河。"
        "报我以崎岖吗？我是一座庄严思索的大山。报我以幸福吗？我是一只凌空飞翔的燕子。"
        "报我以不幸吗？我是一根劲竹经得起千击万磨。生活里不能没有笑声，没有笑声的世界该是多么寂寞。"
        "什么也改变不了我对生活的热爱，我微笑着走向火热的生活！"
        "现代诗词已全部展示完毕。");

    if (adapter_ == nullptr) {
        adapter_ = new TextAdapter();
    }
    adapter_->SetWidth(Screen::GetInstance().GetWidth());
    adapter_->SetLineBreakMode(UILabel::LineBreakMode::LINE_BREAK_WRAP);
    adapter_->SetData(adapterData_);
    adapter_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 36); // 36: fontSize

    if (list_ == nullptr) {
        list_ = new TextList();
    }
    list_->SetIntercept(true);
    list_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    list_->SetAdapter(adapter_);
    list_->SetLoopState(true);
    list_->SetThrowDrag(true);
    list_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    list_->SetMaxScrollDistance(1000); // 1000: max scroll distance
    list_->SetSwipeACCLevel(200); // 200: compensation distance
    list_->SetDragACCLevel(5); // 5: drag acceleration
    if (animator_ == nullptr) {
        animator_ = new Animator(&callback_, list_, DEFAULT_TASK_PERIOD, true);
    }
    list_->SetAnimator(animator_);
}

void UITestTextFPS::TearDown()
{
    if (animator_ != nullptr) {
        animator_->Stop();
        delete animator_;
        animator_ = nullptr;
    }

    if (adapterData_ != nullptr) {
        adapterData_->Clear();
        delete adapterData_;
        adapterData_ = nullptr;
    }

    if (adapter_ != nullptr) {
        delete adapter_;
        adapter_ = nullptr;
    }

    delete list_;
    list_ = nullptr;
}

const UIView* UITestTextFPS::GetTestView()
{
    return list_;
}
} // namespace OHOS
