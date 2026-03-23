/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: List with built-in adapter
 * Author: Hisi Team
 * Created: 2025-07
 */
#ifndef GRAPHIC_SIMPLE_LIST_H
#define GRAPHIC_SIMPLE_LIST_H

#include "common/graphic_hardware_types.h"
#include "components/ui_list_nested.h"
#include "components/ui_transform_list.h"
#include "components/ui_view.h"
#include "components/common_adapter.h"

namespace OHOS {
/**
 * Create common views for special type, and set non-updating property for common views.
 * This callback requires the user to implement the control combination for the item.
 * And configure the control's non-updating property.
 * If the controls within an item are identical,
 * the same type value can be used to achieve memory reuse for that type of item.
 * The type can't be 0.
 * @param Type Current creation item type
 * @return UIView* A pointer to the newly created view.
 */
typedef UIView* (*CreateViewCallback)(uint8_t type);

/**
 * Update differentiation properties for common views based on special data and type.
 * The callback requires the user to update the control properties based on the item type.
 * @param view The target view to update
 * @param data Associated data for the update
 * @param type The type of the item being updated.
 */
typedef void (*UpdateViewCallback)(UIView* view, void* data, uint8_t type);

/**
 * @brief Metadata structure for managing view content information.
 * Contains callbacks and associated data for each view item.
 */
class Contents {
public:
    CreateViewCallback createFunc;
    UpdateViewCallback updateFunc;
    uint8_t type;                     // Unique view type identifier
    void* data;                      // View-specific data payload
};

/**
 * @brief Base class providing common adapter functionality.
 * Wraps a custom adapter and provides methods for managing items.
 */
class AdapterWrapper : public HeapBase {
public:
    AdapterWrapper() {}
    virtual ~AdapterWrapper() {}

    /**
     * @brief Adds a new view content configuration.
     * Call RefreshList() after adding to update the list.
     * @param metadata The metadata containing view creation and update callbacks.
     */
    void AddContent(Contents& metadata)
    {
        adapter_.AddContent(metadata);
    }

    /**
     * @brief Updates an existing view at a specific index.
     * Call RefreshList() after updating to reflect changes.
     * @param index Index of the view to update.
     * @param metadata New metadata to apply.
     * @return true if the update was successful, false otherwise.
     */
    bool UpdateContent(int16_t index, Contents& metadata)
    {
        return adapter_.UpdateContent(index, metadata);
    }

    /**
    * Removes the content at the specified index from the adapter.call the RefreshList() interface after remove
    * @param index The zero-based index of the content to remove.
    *              Must be between 0 and (GetCount() - 1).
    * @return True if the content was successfully removed,
    *         false if the index is out of bounds.
    * @note This method clears the corresponding element in the contents vector.
    *       If the index is invalid (e.g., negative or exceeds current size), the operation fails.
    *       The memory of any pointers (like data in the Contents struct) is not managed here.
    */
    void RemoveContent(int16_t index)
    {
        adapter_.RemoveContent(index);
    }

    /**
     * @brief Removes all view content from the adapter.
     */
    void ClearAll()
    {
        adapter_.ClearAll();
    }

protected:
    CommonAdapter adapter_;
};

/**
 * @brief Simple list component using a base adapter.
 * Provides basic list functionality with a custom adapter.
 */
class UISimpleList : public AdapterWrapper, public UIListNested {
public:
    explicit UISimpleList()
    {
        SetAdapter(&adapter_);
    }

    ~UISimpleList() override = default;
};

/**
 * @brief Transformable simple list component.
 * Extends the simple list with transform capabilities.
 */
class UISimpleTransformList : public AdapterWrapper, public UITransformList {
public:
    explicit UISimpleTransformList()
    {
        SetAdapter(&adapter_);
    }

    ~UISimpleTransformList() override = default;
};
}
#endif
