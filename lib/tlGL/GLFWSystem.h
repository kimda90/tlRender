// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2023 Darby Johnston
// All rights reserved.

#pragma once

#include <tlCore/ISystem.h>

namespace tl
{
    namespace gl
    {
        //! GLFW system.
        class GLFWSystem : public system::ISystem
        {
            TLRENDER_NON_COPYABLE(GLFWSystem);

        protected:
            void _init(const std::shared_ptr<system::Context>&);
            
            GLFWSystem();

        public:
            ~GLFWSystem() override;

            //! Create a new system.
            static std::shared_ptr<GLFWSystem> create(const std::shared_ptr<system::Context>&);
        
        private:
            TLRENDER_PRIVATE();
        };
    }
}
