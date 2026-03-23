/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Generic type adapter for view management
 * Author: Hisi Team
 * Created: 2025-07
 */
#ifndef GRAPHIC_CUSTOM_ADPTER_H
#define GRAPHIC_CUSTOM_ADPTER_H

#include "components/abstract_adapter.h"
#include "gfx_utils/vector.h"

namespace OHOS {
class Contents;
/**
 * Custom adapter implementation for view management
 * Inherits from AbstractAdapter base class
 */
class CommonAdapter : public AbstractAdapter {
public:
    CommonAdapter();
    ~CommonAdapter();

    /**
     * Gets total view count
     * @return Number of views managed by this adapter
     */
    uint16_t GetCount();

    /**
     * Retrieves a view at specified index
     * @param inView Recyclable view container
     * @param index Index of view to retrieve
     * @return View instance or new view when inView is null
     */
    UIView* GetView(UIView* inView, int16_t index);

    /**
     * Adds new view content configuration
     * @param data Contents structure with callbacks and data
     */
    void AddContent(Contents& data);

    /**
     * Updates view content at specific index
     * @param index Target view index to update
     * @param data New content configuration to apply
     * @return True if update successful
     */
    bool UpdateContent(int16_t index, Contents& data);

    /**
    * Removes the content at the specified index from the adapter.
    * @param index The zero-based index of the content to remove.
    *              Must be between 0 and (GetCount() - 1).
    * @return True if the content was successfully removed,
    *         false if the index is out of bounds.
    * @note This method clears the corresponding element in the contents_ vector.
    *       If the index is invalid (e.g., negative or exceeds current size), the operation fails.
    *       The memory of any pointers (like data in the Contents struct) is not managed here.
    */
    bool RemoveContent(int16_t index);

    /** Removes all view content from the adapter */
    void ClearAll();

    /** Retrieve the type value of the current index data */
    uint8_t GetViewType(int16_t index);
private:
    Graphic::Vector<Contents> contents_;
};
}
#endif
