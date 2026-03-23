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
#include "gfx_utils/trans_affine.h"
#if ENABLE_CMATH
#include <cmath>
#endif

namespace OHOS {
const TransAffine& TransAffine::RectToParl(float x1, float y1,
                                           float x2, float y2,
                                           const float* parl)
{
    data_[0] = x2 - x1;
    data_[3] = 0; // 3 : index
    data_[1] = x2 - x1;
    data_[4] = y2 - y1; // 4 : index
    data_[2] = x1; // 2 : index
    data_[5] = y1; // 5 : index
    Invert();
    Multiply(TransAffine(parl[2] - parl[0], parl[3] - parl[1], // 2 3 : index
                         parl[4] - parl[0], parl[5] - parl[1], // 4 5 : index
                         parl[0], parl[1]));
    return *this;
}

const TransAffine& TransAffine::Multiply(const TransAffine& metrix)
{
    float t0 = data_[0] * metrix.data_[0] + data_[3] * metrix.data_[1]; // 3 : index
    float t2 = data_[1] * metrix.data_[0] + data_[4] * metrix.data_[1]; // 4 : index
    float t4 = data_[2] * metrix.data_[0] + data_[5] * metrix.data_[1] + metrix.data_[2]; // 2 5 : index
    data_[3] = data_[0] * metrix.data_[3] + data_[3] * metrix.data_[4]; // 3 4 : index
    data_[4] = data_[1] * metrix.data_[3] + data_[4] * metrix.data_[4]; // 3 4 5: index
    data_[5] = data_[2] * metrix.data_[3] + data_[5] * metrix.data_[4] + metrix.data_[5]; // 2 3 4 5 : index
    data_[0] = t0;
    data_[1] = t2;
    data_[2] = t4; // 2 : index
    return *this;
}

const TransAffine& TransAffine::Invert()
{
    float d = DeterminantReciprocal();

    float t0 = data_[4] * d; // 4 : index
    data_[4] = data_[0] * d; // 4 : index
    data_[3] = -data_[3] * d; // 3 : index
    data_[1] = -data_[1] * d;

    float t4 = -data_[2] * t0 - data_[5] * data_[1]; // 2 5 : index
    data_[5] = -data_[2] * data_[3] - data_[5] * data_[4]; // 2 3 4 5 : index

    data_[0] = t0;
    data_[2] = t4; // 2 : index
    return *this;
}

const TransAffine& TransAffine::Reset()
{
    data_[1] = 0;
    data_[2] = 0; // 2 : index
    data_[3] = 0; // 3 : index
    data_[5] = 0; // 5 : index
    data_[6] = 0; // 6 : index
    data_[7] = 0; // 7 : index
    data_[0] = 1;
    data_[4] = 1; // 4 : index
    data_[8] = 1; // 8 : index
    return *this;
}

bool TransAffine::IsIdentity() const
{
    return MATH_FLT_EQUAL(data_[0], 1.0f) &&
           MATH_FLT_EQUAL(data_[3], 0.0f) && // 3 : index
           MATH_FLT_EQUAL(data_[1], 0.0f) &&
           MATH_FLT_EQUAL(data_[4], 1.0f) && // 4 : index
           MATH_FLT_EQUAL(data_[2], 0.0f) && // 2 : index
           MATH_FLT_EQUAL(data_[5], 0.0f); // 5 : index
}

bool TransAffine::IsValid(float epsilon) const
{
    return (MATH_ABS(data_[0]) > epsilon) && (MATH_ABS(data_[4]) > epsilon); // 4 : index
}

TransAffine TransAffine::TransAffineRotation(float angle)
{
#if ENABLE_CMATH
        return TransAffine(std::cos(angle), std::sin(angle), -std::sin(angle), std::cos(angle), 0.0f, 0.0f);
#else
        return TransAffine(Sin(QUARTER_IN_DEGREE - angle), Sin(angle), -Sin(angle), Sin(QUARTER_IN_DEGREE - angle),
            0.0f, 0.0f);
#endif
}

const TransAffine& TransAffine::Rotate(float angle)
{
#if ENABLE_CMATH
    float scaleXTemp = data_[0] * std::cos(angle) - data_[3] * std::sin(angle); // 3 : index
    float shearXTemp = data_[1] * std::cos(angle) - data_[4] * std::sin(angle); // 4 : index
    float translateXTemp = data_[2] * std::cos(angle) - data_[5] * std::sin(angle); // 5 : index
    data_[3] = data_[0] * std::sin(angle) + data_[3] * std::cos(angle); // 3 : index
    data_[4] = data_[1] * std::sin(angle) + data_[4] * std::cos(angle); // 4 : index
    data_[5] = data_[2] * std::sin(angle) + data_[5] * std::cos(angle); // 5 : index
#else
    float scaleXTemp = data_[0] * Sin(QUARTER_IN_DEGREE - angle) - data_[3] * Sin(angle); // 3 : index
    float shearXTemp = data_[1] * Sin(QUARTER_IN_DEGREE - angle) - data_[4] * Sin(angle); // 4 : index
    float translateXTemp = data_[2] * Sin(QUARTER_IN_DEGREE - angle) - data_[5] * Sin(angle); // 2 5 : index
    data_[3] = data_[0] * Sin(angle) + data_[3] * Sin(QUARTER_IN_DEGREE - angle); // 3 : index
    data_[4] = data_[1] * Sin(angle) + data_[4] * Sin(QUARTER_IN_DEGREE - angle); // 4 : index
    data_[5] = data_[2] * Sin(angle) + data_[5] * Sin(QUARTER_IN_DEGREE - angle); // 2 5 : index
#endif
    data_[0] = scaleXTemp;
    data_[1] = shearXTemp;
    data_[2] = translateXTemp; // 2 : index
    return *this;
}
} // namespace OHOS
