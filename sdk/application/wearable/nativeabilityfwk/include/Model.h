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

#ifndef APPFWK_MODEL_H
#define APPFWK_MODEL_H

#include "heap_base.h"

namespace OHOS {
template<class P>
class Model : public HeapBase {
public:
    Model() : presenter_(nullptr) {}
    virtual ~Model() {}

    /**
     * @brief This function will attach presenter to this model
     *
     * @param presenter presenter pointer defined by developer
     */
    void Attach(P* presenter)
    {
        presenter_ = presenter;
    }

protected:
    P* presenter_;

private:
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(Model&&) = delete;
};
}
#endif
