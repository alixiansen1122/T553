/*
 * Copyright (c) CompanyNameMagicTag.
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

#ifndef APPFWK_VIEW_H
#define APPFWK_VIEW_H

#include "components/ui_view_group.h"
#include "BackDragListener.h"

namespace OHOS {
class PresenterBase;
class ViewBase {
public:
    ViewBase() : rootContainer_(nullptr), backDragListener_(nullptr) {}
    virtual ~ViewBase() = default;

    /**
     * @brief Get container view
     * @returns rootContainer_
     */
    UIViewGroup* GetRootContainer()
    {
        return rootContainer_;
    }

    /**
     * @brief Get back drag listener
     *        This method can be used in AbilitySlice.cpp, and also can be used in XXXView.cpp
     *        If used in XXXView.cpp, you can use it to check whether default OnDragListener is configured.
     * @returns backDragListener_
     */
    BackDragListener* GetBackDragListener()
    {
        return backDragListener_;
    }

    /**
     * @deprecated 推荐使用方法OnStart(void* data)
     */
    virtual void OnStart() {}

    /**
     * @brief Scheduled by slice after funtion SetupView and before transition animator starts if exits.
     *
     *        You should override it to prepare your own views and add them into root container
     *        A simple examle is as follow：
     *        void OnStart() override
     *        {
     *            label_ = new UILabel();
     *            ....
     *            AddViewToRootContainer(label_); // add label_ into container
     *        }
     */
    virtual void OnStart(void* data) {}

    /**
     * @brief Scheduled by slice after function TearDownView.
     *
     *        You can override it or override function of Destructor function to destroy ui views
     *        A simple examle is as follow：
     *        void OnStop() override
     *        {
     *            DELETE_IF_NOT_NULL(label_);
     *            ....
     *        }
     */
    virtual void OnStop() {}

    /**
     * @brief draw container
     *
     */
    void Draw();

    /**
     * @brief This function will attach presenter to this view
     *
     * @param presenter presenter pointer defined by developer
     */
    virtual void Attach(PresenterBase* presenter) = 0;

    void AddViewToRootContainer(UIView* view);

    ViewBase(const ViewBase&) = delete;
    ViewBase& operator=(const ViewBase&) = delete;
    ViewBase(ViewBase&&) = delete;
    ViewBase& operator=(ViewBase&&) = delete;

private:
    friend class AbilitySlice;
    void SetupView();
    void TearDownView();
    UIViewGroup* rootContainer_;
    BackDragListener* backDragListener_;
};

template<class P>
class View : public ViewBase {
public:
    View() : presenter_(nullptr) {}

    /**
     * @brief This function will attach presenter to this view
     *
     * @param presenter presenter pointer defined by developer
     */
    void Attach(PresenterBase* presenter) override
    {
        presenter_ = dynamic_cast<P*>(presenter);
    }

protected:
    P* presenter_;
};
}
#endif
