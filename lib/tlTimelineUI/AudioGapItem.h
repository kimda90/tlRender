// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2023 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimelineUI/IBasicItem.h>

#include <opentimelineio/gap.h>

namespace tl
{
    namespace timelineui
    {
        //! Audio gap item.
        class AudioGapItem : public IBasicItem
        {
        protected:
            void _init(
                const otio::Gap*,
                const ItemData&,
                const std::shared_ptr<system::Context>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            AudioGapItem();

        public:
            ~AudioGapItem() override;

            //! Create a new item.
            static std::shared_ptr<AudioGapItem> create(
                const otio::Gap*,
                const ItemData&,
                const std::shared_ptr<system::Context>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
