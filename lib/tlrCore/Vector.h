// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021 Darby Johnston
// All rights reserved.

#pragma once

namespace tlr
{
    namespace math
    {
        //! Two-Dimensional Vector
        template<typename T>
        struct Vector2
        {
            Vector2();
            Vector2(T x, T y);
            
            T x, y;

            bool operator == (const Vector2<T>&) const;
            bool operator != (const Vector2<T>&) const;
        };

        //! Three-Dimensional Vector
        template<typename T>
        struct Vector3
        {
            Vector3();
            Vector3(T x, T y, T z);

            T x, y, z;

            bool operator == (const Vector3<T>&) const;
            bool operator != (const Vector3<T>&) const;
        };

        //! Four-Dimensional Vector
        template<typename T>
        struct Vector4
        {
            Vector4();
            Vector4(T x, T y, T z, T w);

            T x, y, z, w;

            bool operator == (const Vector4<T>&) const;
            bool operator != (const Vector4<T>&) const;
        };

        template<typename T>
        Vector2<T> operator + (const Vector2<T>&, T);
        template<typename T>
        Vector3<T> operator + (const Vector3<T>&, T);
        template<typename T>
        Vector4<T> operator + (const Vector4<T>&, T);

        //! This typedef provides a two-dimensional integer vector.
        typedef Vector2<int> Vector2i;

        //! This typedef provides a two-dimensional floating point vector.
        typedef Vector2<float> Vector2f;

        //! This typedef provides a three-dimensional integer vector.
        typedef Vector3<int> Vector3i;

        //! This typedef provides a three-dimensional floating point vector.
        typedef Vector3<float> Vector3f;

        //! This typedef provides a four-dimensional integer vector.
        typedef Vector4<int> Vector4i;

        //! This typedef provides a four-dimensional floating point vector.
        typedef Vector4<float> Vector4f;
    }
}

#include <tlrCore/VectorInline.h>
