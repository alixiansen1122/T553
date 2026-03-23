/*
 * Copyright (c) 2024 CompanyNameMagicTag.
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

#ifndef TRIANGULAR_PLANE_H
#define TRIANGULAR_PLANE_H

#include "gfx_utils/graphic_math.h"
namespace OHOS {
class TriangularPlane : public HeapBase {
public:
    TriangularPlane() {}
    virtual ~TriangularPlane() {}

    void SetVertex(Vector3<float>vertex1, Vector3<float>vertex2, Vector3<float>vertex3)
    {
        vertexes_[0] = vertex1;
        vertexes_[1] = vertex2;
        vertexes_[2] = vertex3; // 2:index
    }

    void SetMatrix(Matrix4<float> matrix)
    {
        matrix_ = matrix;
    }

    Matrix4<float> GetMatrix()
    {
        return matrix_;
    }

    const Vector4<float> GetNormal()
    {
        float nx = (vertexes_[1].y_ - vertexes_[0].y_) * (vertexes_[2].z_ - vertexes_[0].z_) -  // 2: vertex index
            (vertexes_[1].z_ - vertexes_[0].z_) * (vertexes_[2].y_ - vertexes_[0].y_);          // 2: vertex index
        float ny = (vertexes_[1].z_ - vertexes_[0].z_) * (vertexes_[2].x_ - vertexes_[0].x_) -  // 2: vertex index
            (vertexes_[1].x_ - vertexes_[0].x_) * (vertexes_[2].z_ - vertexes_[0].z_);          // 2: vertex index
        float nz = (vertexes_[1].x_ - vertexes_[0].x_) * (vertexes_[2].y_ - vertexes_[0].y_) -  // 2: vertex index
            (vertexes_[1].y_ - vertexes_[0].y_) * (vertexes_[2].x_ - vertexes_[0].x_);          // 2: vertex index
        return Vector4<float>(nx, ny, nz, 0);
    }

    Vector3<float>& operator[](uint8_t index)
    {
        return vertexes_[index];
    }

    const Vector3<float> operator[](uint8_t index) const
    {
        return vertexes_[index];
    }

private:
    static constexpr uint8_t MAX_VERTEX_NUM = 3;    // 3: triangular vertex number
    Vector3<float> vertexes_[MAX_VERTEX_NUM];       // the vertexes of polygon
    Matrix4<float> matrix_;
};
}
#endif // TRIANGULAR_PLANE_H