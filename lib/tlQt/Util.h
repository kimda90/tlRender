// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#pragma once

#include <tlGlad/gl.h>

#include <QString>

namespace tl
{
    //! Qt support.
    namespace qt
    {
        //! Initialize the library. This needs to be called before the Qt
        //! application is instantiated.
        void init();
    }
}
