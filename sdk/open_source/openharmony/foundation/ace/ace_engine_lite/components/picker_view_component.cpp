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

#include "picker_view_component.h"
#include "ace_log.h"
#include "ace_mem_base.h"
#include "key_parser.h"
#include "keys.h"
#include "product_adapter.h"
#include "ui_font.h"

namespace OHOS {
namespace ACELite {
#ifdef FEATURE_COMPONENT_DATE_PICKER
void DatePickerListener::OnDatePickerStoped(UIDatePicker &picker)
{
    if (!jerry_value_is_function(dateCallback_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "DatePickerListener: callback type invalid!");
        return;
    }
    uint32_t year = (picker.GetSelectYear() == nullptr) ? 0 : strtol(picker.GetSelectYear(), nullptr, DEC);
    uint32_t month = (picker.GetSelectMonth() == nullptr) ? 0 : strtol(picker.GetSelectMonth(), nullptr, DEC);
    uint32_t day = (picker.GetSelectDay() == nullptr) ? 0 : strtol(picker.GetSelectDay(), nullptr, DEC);

    jerry_value_t arg = jerry_create_object();
    JerrySetNumberProperty(arg, DATE_PICKER_YEAR, year);
    JerrySetNumberProperty(arg, DATE_PICKER_MONTH, month);
    JerrySetNumberProperty(arg, DATE_PICKER_DAY, day);
    CallJSFunctionAutoRelease(dateCallback_, UNDEFINED, &arg, 1);
    jerry_release_value(arg);
}
#endif // FEATURE_COMPONENT_DATE_PICKER

void TextPickerListener::OnPickerStoped(UIPicker &picker)
{
    if (!jerry_value_is_function(textCallback_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "TextPickerListener: callback type invalid!");
        return;
    }
    uint16_t index = picker.GetSelected();
    jerry_value_t arg = jerry_create_object();
    JerrySetStringProperty(arg, TEXT_PICKER_NEW_VALUE, pTextArray_[index]);
    JerrySetNumberProperty(arg, TEXT_PICKER_NEW_SELECTED, index);
    CallJSFunctionAutoRelease(textCallback_, UNDEFINED, &arg, 1);
    jerry_release_value(arg);
}

#ifdef FEATURE_COMPONENT_IMAGE_PICKER
void ImagePickerListener::OnPickerStoped(UIImagePicker& picker)
{
    if (!jerry_value_is_function(imageCallback_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "imagePickerListener: callback type invalid!");
        return;
    }
    uint16_t index = picker.GetSelected();
    jerry_value_t arg = jerry_create_object();
    JerrySetStringProperty(arg, IMAGE_PICKER_NEW_VALUE, unselectedArray_[index]);
    JerrySetNumberProperty(arg, IMAGE_PICKER_NEW_SELECTED, index);
    CallJSFunctionAutoRelease(imageCallback_, UNDEFINED, &arg, 1);
    jerry_release_value(arg);
}
#endif // FEATURE_COMPONENT_IMAGE_PICKER

void TimePickerListener::OnTimePickerStoped(UITimePicker &picker)
{
    if (!jerry_value_is_function(timeCallback_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "TimePickerListener: callback type invalid!");
        return;
    }
    uint32_t hour = (picker.GetSelectHour() == nullptr) ? 0 : strtol(picker.GetSelectHour(), nullptr, DEC);
    uint32_t minute = (picker.GetSelectMinute() == nullptr) ? 0 : strtol(picker.GetSelectMinute(), nullptr, DEC);

    jerry_value_t arg = jerry_create_object();
    JerrySetNumberProperty(arg, TIME_PICKER_HOUR, hour);
    JerrySetNumberProperty(arg, TIME_PICKER_MINUTE, minute);
    CallJSFunctionAutoRelease(timeCallback_, UNDEFINED, &arg, 1);
    jerry_release_value(arg);
}

PickerViewComponent::PickerViewComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager),
      options_(options),
      pickerType_(PickerType::TEXT),
      pickerView_(nullptr),
      color_(Color::Gray()),
      fontSize_(DEFAULT_FONT_SIZE),
      fontFamily_(nullptr),
      selectedColor_(Color::White()),
      selectedFontSize_(PICKER_SELECTED_FONT_SIZE),
      selectedFontFamily_(nullptr),
      pickerItemHeight_(PICKER_ITEM_HEIGHT),
      loopState_(true),
      textSelected_(0),
      pTextArray_(nullptr),
      textArraySize_(0),
      textPickerListener_(nullptr),
#ifdef FEATURE_COMPONENT_DATE_PICKER
      dateSelected_(nullptr),
      dateStart_(nullptr),
      dateEnd_(nullptr),
      datePickerListener_(nullptr),
#endif // FEATURE_COMPONENT_DATE_PICKER
#ifdef FEATURE_COMPONENT_IMAGE_PICKER
      selectedArray_(nullptr),
      unselectedArray_(nullptr),
      selectedArraySize_(0),
      unselectedArraySize_(0),
      imageSelected_(0),
      imagePickerListener_(nullptr),
#endif // FEATURE_COMPONENT_IMAGE_PICKER
      timePickerListener_(nullptr),
      timeSelected_(nullptr)
{
    SetComponentName(K_PICKER_VIEW);
    fontSize_ = ProductAdapter::GetDefaultFontSize();
    RegisterNamedFunction(OHOS::ACELite::Component::FUNC_SCROLL, PickerViewScroll);
#ifdef FEATURE_ROTATION_API
    RegisterNamedFunction(FUNC_ROTATION_NAME, HandleRotationRequest);
#endif // FEATURE_ROTATION_API
}

bool PickerViewComponent::CreateNativeViews()
{
    bool result = false;
    pickerType_ = GetPickerType();
    SetPickerLoopState();
    switch (pickerType_) {
        case PickerType::TEXT: {
            result = CreateTextPicker();
            break;
        }
        case PickerType::TIME: {
            result = CreateTimePicker();
            break;
        }
#ifdef FEATURE_COMPONENT_DATE_PICKER
        case PickerType::DATE: {
            result = CreateDatePicker();
            break;
        }
#endif // FEATURE_COMPONENT_DATE_PICKER
#ifdef FEATURE_COMPONENT_IMAGE_PICKER
        case PickerType::IMAGE: {
            result = CreateImagePicker();
            break;
        }
#endif // FEATURE_COMPONENT_IMAGE_PICKER
        case PickerType::UNKNOWN: {
            HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: picker view type invalid!");
            break;
        }
        default:
            break;
    }

    if (pickerView_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: create native view failed!");
        result = false;
    }

    pickerView_->SetIntercept(true);
    if (result) {
        result = (CopyFontFamily(fontFamily_, ProductAdapter::GetDefaultFontFamilyName())) &&
                 (CopyFontFamily(selectedFontFamily_, ProductAdapter::GetDefaultFontFamilyName()));
    }
    return result;
}

bool PickerViewComponent::CreateTextPicker()
{
    UIPicker *uiPicker = new UIPicker();
    if (uiPicker == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: create text picker failed!");
        return false;
    }
    uiPicker->SetLoopState(loopState_);
    textPickerListener_ = new TextPickerListener();
    if (textPickerListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: create text picker listener failed!");
        delete uiPicker;
        uiPicker = nullptr;
        return false;
    }
    uiPicker->SetItemHeight(pickerItemHeight_);
    pickerView_ = static_cast<UIView *>(uiPicker);
    return true;
}

bool PickerViewComponent::CreateTimePicker()
{
    UITimePicker *uiTimePicker = new UITimePicker();
    if (uiTimePicker == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: create time picker failed");
        return false;
    }
    uiTimePicker->SetLoopState(uiTimePicker->PICKER_HOUR, loopState_);
    uiTimePicker->SetLoopState(uiTimePicker->PICKER_MIN, loopState_);
    uiTimePicker->SetLoopState(uiTimePicker->PICKER_SEC, loopState_);
    timePickerListener_ = new TimePickerListener();
    if (timePickerListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: create time picker listener failed");
        delete uiTimePicker;
        uiTimePicker = nullptr;
        return false;
    }
    uiTimePicker->SetItemHeight(pickerItemHeight_);
    pickerView_ = static_cast<UIView *>(uiTimePicker);
    return true;
}

#ifdef FEATURE_COMPONENT_IMAGE_PICKER
bool PickerViewComponent::CreateImagePicker()
{
    UIImagePicker* uiImagePicker = new UIImagePicker();
    if (uiImagePicker == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: create image picker failed");
        return false;
    }
    imagePickerListener_ = new ImagePickerListener();
    if (imagePickerListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: create image picker listener failed");
        delete uiImagePicker;
        uiImagePicker = nullptr;
        return false;
    }
    uiImagePicker->SetItemHeight(pickerItemHeight_);
    uiImagePicker->SetItemWidth(PICKER_ITEM_WIDTH);
    pickerView_ = uiImagePicker;
    return true;
}
#endif

#ifdef FEATURE_COMPONENT_DATE_PICKER
bool PickerViewComponent::CreateDatePicker()
{
    UIDatePicker *uiDatePicker = new UIDatePicker();
    if (uiDatePicker == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: create date picker failed");
        return false;
    }
    datePickerListener_ = new DatePickerListener();
    if (datePickerListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: create date picker failed");
        delete uiDatePicker;
        uiDatePicker = nullptr;
        return false;
    }
    uiDatePicker->SetItemHeight(pickerItemHeight_);
    pickerView_ = static_cast<UIView *>(uiDatePicker);
    return true;
}
#endif // FEATURE_COMPONENT_DATE_PICKER

void PickerViewComponent::ReleaseTextRange()
{
    // free text picker resources
    if ((textArraySize_ > 0) && pTextArray_) {
        for (uint16_t index = 0; index < textArraySize_; index++) {
            if (pTextArray_[index]) {
                ace_free(pTextArray_[index]);
                pTextArray_[index] = nullptr;
            }
        }
        delete[] pTextArray_;
        pTextArray_ = nullptr;
    }
}

#ifdef FEATURE_COMPONENT_IMAGE_PICKER
void PickerViewComponent::ReleaseImageRange()
{
    // freee image picker resources
    if ((unselectedArraySize_ > 0) && (unselectedArray_ != nullptr)) {
        for (uint16_t index = 0; index < unselectedArraySize_; index++) {
            if (unselectedArray_[index] != nullptr) {
                ace_free(unselectedArray_[index]);
                unselectedArray_[index] = nullptr;
            }
        }
        delete[] unselectedArray_;
        unselectedArray_ = nullptr;
    }
}

void PickerViewComponent::ReleaseSelectedImageRange()
{
    if ((selectedArraySize_ > 0) && (selectedArray_ != nullptr)) {
        for (uint16_t index = 0; index < selectedArraySize_; index++) {
            if (selectedArray_[index] != nullptr) {
                ace_free(selectedArray_[index]);
                selectedArray_[index] = nullptr;
            }
        }
        delete[] selectedArray_;
        selectedArray_ = nullptr;
    }
}
#endif // FEATURE_COMPONENT_IMAGE_PICKER

void PickerViewComponent::ReleaseNativeViews()
{
    if (pickerView_ != nullptr) {
        delete pickerView_;
        pickerView_ = nullptr;
    }

    ReleaseTextRange();

    if (textPickerListener_ != nullptr) {
        delete textPickerListener_;
        textPickerListener_ = nullptr;
    }

    // free time picker resources
    if (timePickerListener_ != nullptr) {
        delete timePickerListener_;
        timePickerListener_ = nullptr;
    }
    ACE_FREE(timeSelected_);
    ACE_FREE(fontFamily_);
    ACE_FREE(selectedFontFamily_);

#ifdef FEATURE_COMPONENT_IMAGE_PICKER
    if (imagePickerListener_ != nullptr) {
        delete imagePickerListener_;
        imagePickerListener_ = nullptr;
    }
    ReleaseSelectedImageRange();
    ReleaseImageRange();
#endif // FEATURE_COMPONENT_IMAGE_PICKER
#ifdef FEATURE_COMPONENT_DATE_PICKER
    // free date picker resources
    if (datePickerListener_ != nullptr) {
        delete datePickerListener_;
        datePickerListener_ = nullptr;
    }
    ACE_FREE(dateSelected_);
    ACE_FREE(dateStart_);
    ACE_FREE(dateEnd_);
#endif // FEATURE_COMPONENT_DATE_PICKER
}

inline UIView *PickerViewComponent::GetComponentRootView() const
{
    return pickerView_;
}

void PickerViewComponent::PostRender()
{
    if (pickerView_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: no native view for post render!");
        return;
    }

    UpdatePickerStyles();
    UpdatePickerAttrs();
}

void PickerViewComponent::PostUpdate(uint16_t attrKeyId)
{
    if (pickerView_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: post update failed!");
        return;
    }

    switch (attrKeyId) {
        case K_SELECTED:
        case K_LOOP:
#ifdef FEATURE_COMPONENT_DATE_PICKER
        case K_START:
        case K_END:
#endif // FEATURE_COMPONENT_DATE_PICKER
            UpdatePickerAttrs();
            break;
        case K_COLOR:
        case K_FONT_SIZE:
        case K_FONT_FAMILY:
        case K_SELECTED_COLOR:
        case K_SELECTED_FONT_SIZE:
        case K_SELECTED_FONT_FAMILY:
            UpdatePickerStyles();
            break;
        default:
            break;
    }
}

void PickerViewComponent::UpdatePickerStyles() const
{
    if (pickerView_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: update picker styles failed!");
        return;
    }
    if ((fontFamily_ == nullptr) || (fontSize_ == 0) || (selectedFontFamily_ == nullptr) || (selectedFontSize_ == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: font family or size invalid!");
        return;
    }
    UIFont *font = UIFont::GetInstance();
    if (font == nullptr) {
        return;
    }

    switch (pickerType_) {
        case PickerType::TEXT: {
            UIPicker *textPicker = reinterpret_cast<UIPicker *>(pickerView_);
            textPicker->SetBackgroundFont(fontFamily_, fontSize_);
            textPicker->SetHighlightFont(selectedFontFamily_, selectedFontSize_);
            textPicker->SetTextColor(color_, selectedColor_);
            textPicker->SetItemHeight(pickerItemHeight_);
            break;
        }
        case PickerType::TIME: {
            UITimePicker *timePicker = reinterpret_cast<UITimePicker *>(pickerView_);
            timePicker->SetBackgroundFont(fontFamily_, fontSize_);
            timePicker->SetHighlightFont(selectedFontFamily_, selectedFontSize_);
            timePicker->SetTextColor(color_, selectedColor_);
            timePicker->SetItemHeight(pickerItemHeight_);
            break;
        }
#ifdef FEATURE_COMPONENT_DATE_PICKER
        case PickerType::DATE: {
            uint8_t fontId = font->GetFontId(fontFamily_, fontSize_);
            uint8_t selectedFontId = font->GetFontId(selectedFontFamily_, selectedFontSize_);
            UIDatePicker *datePicker = reinterpret_cast<UIDatePicker *>(pickerView_);
            datePicker->SetTextStyle(fontId, selectedFontId, color_, selectedColor_);
            break;
        }
#endif // FEATURE_COMPONENT_DATE_PICKER
        default:
            break;
    }
}

void PickerViewComponent::UpdatePickerAttrs() const
{
    if (pickerView_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: update picker attributes failed!");
        return;
    }
    switch (pickerType_) {
        case PickerType::TEXT: {
            UIPicker *textPicker = reinterpret_cast<UIPicker *>(pickerView_);
            textPicker->SetSelected(textSelected_);
            textPicker->SetLoopState(loopState_);
            break;
        }
        case PickerType::TIME: {
            UITimePicker *timePicker = reinterpret_cast<UITimePicker *>(pickerView_);
            if (timeSelected_ != nullptr) {
                timePicker->SetSelected(timeSelected_);
            }
            timePicker->SetLoopState(timePicker->PICKER_HOUR, loopState_);
            timePicker->SetLoopState(timePicker->PICKER_MIN, loopState_);
            timePicker->SetLoopState(timePicker->PICKER_SEC, loopState_);
            break;
        }
#ifdef FEATURE_COMPONENT_IMAGE_PICKER
        case PickerType::IMAGE: {
            UIImagePicker *imagePicker = reinterpret_cast<UIImagePicker *>(pickerView_);
            imagePicker->SetSelected(imageSelected_);
            break;
        }
#endif // FEATURE_COMPONENT_IMAGE_PICKER
#ifdef FEATURE_COMPONENT_DATE_PICKER
        case PickerType::DATE: {
            UIDatePicker *datePicker = reinterpret_cast<UIDatePicker *>(pickerView_);
            if (dateStart_ != nullptr) {
                datePicker->SetStart(dateStart_);
            }
            if (dateEnd_ != nullptr) {
                datePicker->SetEnd(dateEnd_);
            }
            if (dateSelected_ != nullptr) {
                datePicker->SetSelected(dateSelected_);
            }
            break;
        }
#endif // FEATURE_COMPONENT_DATE_PICKER
        default:
            break;
    }
}

bool PickerViewComponent::SetPickerLoop(jerry_value_t loopValue)
{
    char* loopStateValue = MallocStringOf(loopValue);
    loopState_ = strcmp(loopStateValue, "false") == 0 ? false : true;
    ace_free(loopStateValue);
    return true;
}

bool PickerViewComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    bool result = true;
    switch (attrKeyId) {
        case K_SELECTED: {
            result = SetPickerSelected(attrValue);
            break;
        }
        case K_RANGE: {
            if (pickerType_ == PickerType::TEXT) {
                result = SetTextPickerRange(attrValue);
            }
            break;
        }
        case K_LOOP: {
            result = SetPickerLoop(attrValue);
            break;
        }
#ifdef FEATURE_COMPONENT_IMAGE_PICKER
        case K_IMAGE_RANGE: {
            if (pickerType_ == PickerType::IMAGE) {
                result = SetImagePickerRange(attrValue);
            }
            break;
        }
        case K_IMAGE_SELECTED_RANGE: {
            if (pickerType_ == PickerType::IMAGE) {
                result = SetImagePickerSelectedRange(attrValue);
            }
        }
#endif // FEATURE_COMPONENT_IMAGE_PICKER
#ifdef FEATURE_COMPONENT_DATE_PICKER
        case K_START: {
            if (pickerType_ == PickerType::DATE) {
                ACE_FREE(dateStart_);
                dateStart_ = MallocStringOf(attrValue);
            }
            break;
        }
        case K_END: {
            if (pickerType_ == PickerType::DATE) {
                ACE_FREE(dateEnd_);
                dateEnd_ = MallocStringOf(attrValue);
            }
            break;
        }
#endif // FEATURE_COMPONENT_DATE_PICKER
        default:
            result = false;
            break;
    }
    return result;
}

bool PickerViewComponent::ApplyPrivateStyle(const AppStyleItem *styleItem)
{
    uint16_t styleNameId = GetStylePropNameId(styleItem);
    if (!KeyParser::IsKeyValid(styleNameId)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: apply private style failed!");
        return false;
    }
    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    switch (styleNameId) {
        case K_COLOR:
            if (!GetStyleColorValue(styleItem, color, alpha)) {
                return false;
            }
            color_ = GetRGBColor(color);
            break;
        case K_FONT_SIZE:
            fontSize_ = GetStylePixelValue(styleItem);
            // reset picker item height if fontSize is larger than PICKER_SELECTED_FONT_SIZE.
            if (fontSize_ > PICKER_SELECTED_FONT_SIZE) {
                pickerItemHeight_ = fontSize_ + PICKER_ITEM_HEIGHT_GAP;
            }
            break;
        case K_FONT_FAMILY:
            CopyFontFamily(fontFamily_, GetStyleStrValue(styleItem));
            break;
        case K_SELECTED_COLOR:
            if (!GetStyleColorValue(styleItem, color, alpha)) {
                return false;
            }
            selectedColor_ = GetRGBColor(color);
            break;
        case K_SELECTED_FONT_SIZE:
            selectedFontSize_ = GetStylePixelValue(styleItem);
            // reset picker item height if selectedFontSize is larger than PICKER_SELECTED_FONT_SIZE.
            if (selectedFontSize_ > PICKER_SELECTED_FONT_SIZE) {
                pickerItemHeight_ = selectedFontSize_ + PICKER_ITEM_HEIGHT_GAP;
            }
            break;
        case K_SELECTED_FONT_FAMILY:
            CopyFontFamily(selectedFontFamily_, GetStyleStrValue(styleItem));
            break;
        default:
            return false;
    }
    return true;
}

bool PickerViewComponent::RegisterPrivateEventListener(uint16_t eventTypeId,
                                                       jerry_value_t funcValue,
                                                       bool isStopPropagation)
{
    bool result = false;
    switch (eventTypeId) {
        case K_CHANGE: {
            SetPickerListener(funcValue);
            result = true;
            break;
        }
        default:
            break;
    }
    return result;
}

char *PickerViewComponent::GetStringAttrByName(const char * const name, uint16_t &length) const
{
    if (jerry_value_is_undefined(options_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: options undefined!");
        return nullptr;
    }

    char *strValue = nullptr;
    jerry_value_t attrsPropValue = jerryx_get_property_str(options_, ATTR_ATTRS);
    if (!jerry_value_is_undefined(attrsPropValue)) {
        strValue = JerryMallocStringProperty(attrsPropValue, name, length);
    }
    jerry_release_value(attrsPropValue);
    return strValue;
}

void PickerViewComponent::SetPickerLoopState()
{
    uint16_t length = 0;
    char *typeStr = GetStringAttrByName(ATTR_LOOP, length);
    loopState_ = strcmp(typeStr, "false") == 0 ? false : true;
}

PickerViewComponent::PickerType PickerViewComponent::GetPickerType() const
{
    uint16_t length = 0;
    char *typeStr = GetStringAttrByName(ATTR_TYPE, length);
    if (typeStr == nullptr) {
        return PickerType::TEXT;
    }

    PickerType type = PickerType::UNKNOWN;
    uint16_t typeId = KeyParser::ParseKeyId(typeStr, length);
    switch (typeId) {
        case K_TEXT:
            type = PickerType::TEXT;
            break;
        case K_TIME:
            type = PickerType::TIME;
            break;
#ifdef FEATURE_COMPONENT_IMAGE_PICKER
        case K_IMAGE:
            type = PickerType::IMAGE;
            break;
#endif
#ifdef FEATURE_COMPONENT_DATE_PICKER
        case K_DATE:
            type = PickerType::DATE;
            break;
#endif // FEATURE_COMPONENT_DATE_PICKER
        default:
            type = PickerType::UNKNOWN;
            break;
    }
    ace_free(typeStr);
    typeStr = nullptr;
    return type;
}

bool PickerViewComponent::SetPickerSelected(jerry_value_t selectedValue)
{
    switch (pickerType_) {
        case PickerType::TEXT: {
            textSelected_ = IntegerOf(selectedValue);
            break;
        }
        case PickerType::TIME: {
            ACE_FREE(timeSelected_);
            timeSelected_ = MallocStringOf(selectedValue);
            break;
        }
#ifdef FEATURE_COMPONENT_IMAGE_PICKER
        case PickerType::IMAGE: {
            imageSelected_ = IntegerOf(selectedValue);
            break;
        }
#endif // FEATURE_COMPONENT_IMAGE_PICKER
#ifdef FEATURE_COMPONENT_DATE_PICKER
        case PickerType::DATE: {
            ACE_FREE(dateSelected_);
            dateSelected_ = MallocStringOf(selectedValue);
            break;
        }
#endif // FEATURE_COMPONENT_DATE_PICKER
        default:
            break;
    }
    return true;
}

void PickerViewComponent::SetPickerListener(jerry_value_t funcValue) const
{
    switch (pickerType_) {
        case PickerType::TEXT: {
            if (textPickerListener_ != nullptr) {
                UIPicker *textPicker = reinterpret_cast<UIPicker *>(pickerView_);
                textPickerListener_->SetTextCallback(funcValue);
                textPicker->RegisterSelectedListener(textPickerListener_);
            }
            break;
        }
        case PickerType::TIME: {
            if (timePickerListener_ != nullptr) {
                UITimePicker *timePicker = reinterpret_cast<UITimePicker *>(pickerView_);
                timePickerListener_->SetTimeCallback(funcValue);
                timePicker->RegisterSelectedListener(timePickerListener_);
            }
        }
#ifdef FEATURE_COMPONENT_IMAGE_PICKER
        case PickerType::IMAGE: {
            if (imagePickerListener_ != nullptr) {
                UIImagePicker *imagePicker = reinterpret_cast<UIImagePicker *>(pickerView_);
                imagePickerListener_->SetImageCallback(funcValue);
                imagePicker->RegisterSelectedListener(imagePickerListener_);
            }
            break;
        }
#endif // FEATURE_COMPONENT_IMAGE_PICKER
#ifdef FEATURE_COMPONENT_DATE_PICKER
        case PickerType::DATE: {
            if (datePickerListener_ != nullptr) {
                UIDatePicker *datePicker = reinterpret_cast<UIDatePicker *>(pickerView_);
                datePickerListener_->SetDateCallback(funcValue);
                datePicker->RegisterSelectedListener(datePickerListener_);
            }
            break;
        }
#endif // FEATURE_COMPONENT_DATE_PICKER
        default:
            break;
    }
}

bool PickerViewComponent::SetTextPickerRange(jerry_value_t rangeValue)
{
    textArraySize_ = jerry_get_array_length(rangeValue);
    if (textArraySize_ == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: text picker range invalid!");
        return false;
    }

    ReleaseTextRange();

    pTextArray_ = new char *[textArraySize_];
    if (pTextArray_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: allocate memory for dynamic array failed!");
        return false;
    }

    for (uint16_t index = 0; index < textArraySize_; index++) {
        jerry_value_t element = jerry_get_property_by_index(rangeValue, index);
        char *elementStr = MallocStringOf(element);
        jerry_release_value(element);
        if (elementStr == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: text picker range contents invalid!");
            textArraySize_ = index;
            return false;
        }
        pTextArray_[index] = elementStr;
    }
    UIPicker *textPicker = reinterpret_cast<UIPicker *>(pickerView_);
    if (textPickerListener_ != nullptr) {
        textPickerListener_->SetTextRange(pTextArray_); // call SetTextRange before SetValues!
    }
    textPicker->SetValues(const_cast<const char **>(pTextArray_), textArraySize_);
    // can not loop when pickerview has one child
    if (textArraySize_ == 1) {
        textPicker->SetLoopState(false);
    }
    return true;
}

#ifdef FEATURE_COMPONENT_IMAGE_PICKER
bool PickerViewComponent::SetImagePickerRange(jerry_value_t rangeValue)
{
    unselectedArraySize_ = jerry_get_array_length(rangeValue);
    if (unselectedArraySize_ == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: image picker range invalid!");
        return false;
    }

    ReleaseImageRange();

    unselectedArray_ = new char *[unselectedArraySize_];
    if (unselectedArray_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: allocate memory for dynamic array failed!");
        return false;
    }

    for (uint16_t index = 0; index < unselectedArraySize_; index++) {
        jerry_value_t element = jerry_get_property_by_index(rangeValue, index);
        char *elementStr = const_cast<char *>(ParseImageSrc(element));
        jerry_release_value(element);
        if (elementStr == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: image picker range contents invalid!");
            unselectedArraySize_ = index;
            return false;
        }
        unselectedArray_[index] = elementStr;
    }
    return true;
}

bool PickerViewComponent::SetImagePickerSelectedRange(jerry_value_t rangeValue)
{
    selectedArraySize_ = jerry_get_array_length(rangeValue);
    if (selectedArraySize_ == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: selected image picker range invalid!");
        return false;
    }

    ReleaseSelectedImageRange();

    selectedArray_ = new char *[selectedArraySize_];
    if (selectedArray_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: allocate memory for dynamic array failed!");
        return false;
    }

    for (uint16_t index = 0; index < selectedArraySize_; index++) {
        jerry_value_t element = jerry_get_property_by_index(rangeValue, index);
        char *elementStr = const_cast<char *>(ParseImageSrc(element));
        jerry_release_value(element);
        if (elementStr == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewComponent: selected image picker range contents invalid!");
            selectedArraySize_ = index;
            return false;
        }
        selectedArray_[index] = elementStr;
    }
    return true;
}
#endif // FEATURE_COMPONENT_IMAGE_PICKER

bool PickerViewComponent::RefreshRectCallback()
{
#ifdef FEATURE_COMPONENT_IMAGE_PICKER
   if (pickerType_ == PickerType::IMAGE) {
        UIImagePicker* imagePicker = reinterpret_cast<UIImagePicker *>(pickerView_);
        if (unselectedArray_ != nullptr) {
            if (imagePickerListener_ != nullptr) {
                imagePickerListener_->SetImageRange(unselectedArray_);
            }
            imagePicker->SetValues(const_cast<const char **>(unselectedArray_), unselectedArraySize_);
        }
        if (selectedArray_ != nullptr) {
            if (imagePickerListener_ != nullptr) {
                imagePickerListener_->SetSelectedImageRange(selectedArray_);
            }
            imagePicker->SetSelectedValues(const_cast<const char **>(selectedArray_), selectedArraySize_);
        }
        imagePicker->Refresh();
        return true;
    }
#endif
    return false;
}

JSValue PickerViewComponent::PickerViewScroll(
    const JSValue func, const JSValue dom, const JSValue args[], const jerry_length_t size)
{
    UIView *uiview = GetScrollView(dom, size);
    if (uiview == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewScroll uiview is nullptr");
        return UNDEFINED;
    }

    UIPicker *picker = reinterpret_cast<UIPicker *>(uiview);
    if (picker == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewScroll picker is nullptr");
        return UNDEFINED;
    }

    UIList *list = picker->GetList();
    if (list == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PickerViewScroll list_ is nullptr");
        return UNDEFINED;
    }

    int16_t rotate = JerryGetIntegerProperty(args[0], OHOS::ACELite::Component::ROTATE_SCROLL);
    list->OnRotateStartEvent(rotate);
    list->OnRotateEvent(rotate);
    list->OnRotateEndEvent(0);

    return UNDEFINED;
}

} // namespace ACELite
} // namespace OHOS
