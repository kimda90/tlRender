// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021 Darby Johnston
// All rights reserved.

#pragma once

#include <tlrCore/AVIO.h>
#include <tlrCore/Context.h>

#include <opentimelineio/composable.h>
#include <opentimelineio/item.h>
#include <opentimelineio/timeline.h>

#include <future>

namespace tlr
{
    //! Timelines.
    namespace timeline
    {
        //! Get the timeline file extensions.
        std::vector<std::string> getExtensions(
            int types,
            const std::shared_ptr<core::Context>&);

        //! Convert frames to ranges.
        std::vector<otime::TimeRange> toRanges(std::vector<otime::RationalTime>);

        //! Get the root (highest parent).
        const otio::Composable* getRoot(const otio::Composable*);

        //! Get the parent of the given type.
        template<typename T>
        const T* getParent(const otio::Item*);

        //! Get the duration of all tracks of the same kind.
        otio::optional<otime::RationalTime> getDuration(const otio::Timeline*, const std::string& kind);

        //! Timeline options.
        struct Options
        {
            size_t                    videoRequestCount = 16;
            size_t                    audioRequestCount = 16;
            std::chrono::milliseconds requestTimeout    = std::chrono::milliseconds(1);
            avio::Options             avioOptions;

            bool operator == (const Options&) const;
            bool operator != (const Options&) const;
        };

        //! Transitions.
        enum class Transition
        {
            None,
            Dissolve,

            Count,
            First = None
        };
        TLR_ENUM(Transition);
        TLR_ENUM_SERIALIZE(Transition);

        //! Convert to a transition.
        Transition toTransition(const std::string&);

        //! Video layer.
        struct VideoLayer
        {
            std::shared_ptr<imaging::Image> image;
            std::shared_ptr<imaging::Image> imageB;
            Transition transition = Transition::None;
            float transitionValue = 0.F;

            bool operator == (const VideoLayer&) const;
            bool operator != (const VideoLayer&) const;
        };

        //! Video data.
        struct VideoData
        {
            otime::RationalTime time = time::invalidTime;
            std::vector<VideoLayer> layers;

            bool operator == (const VideoData&) const;
            bool operator != (const VideoData&) const;
        };

        //! Compare the time values of video data.
        bool isTimeEqual(const VideoData&, const VideoData&);

        //! Audio layer.
        struct AudioLayer
        {
            std::shared_ptr<audio::Audio> audio;

            bool operator == (const AudioLayer&) const;
            bool operator != (const AudioLayer&) const;
        };

        //! Audio data.
        struct AudioData
        {
            int64_t seconds = -1;
            std::vector<AudioLayer> layers;

            bool operator == (const AudioData&) const;
            bool operator != (const AudioData&) const;
        };

        //! Compare the time values of audio data.
        bool isTimeEqual(const AudioData&, const AudioData&);

        //! Timeline.
        class Timeline : public std::enable_shared_from_this<Timeline>
        {
            TLR_NON_COPYABLE(Timeline);

        protected:
            void _init(
                const otio::SerializableObject::Retainer<otio::Timeline>&,
                const std::shared_ptr<core::Context>&,
                const Options&);
            Timeline();

        public:
            ~Timeline();

            //! Create a new timeline.
            static std::shared_ptr<Timeline> create(
                const otio::SerializableObject::Retainer<otio::Timeline>&,
                const std::shared_ptr<core::Context>&,
                const Options& = Options());

            //! Create a new timeline from a file path. The file path can
            //! point to an .otio file, movie file, or image sequence.
            static std::shared_ptr<Timeline> create(
                const file::Path&,
                const std::shared_ptr<core::Context>&,
                const Options & = Options());

            //! Create a new timeline from a file path and audio file path.
            //! The file path can point to an .otio file, movie file, or
            //! image sequence.
            static std::shared_ptr<Timeline> create(
                const file::Path& path,
                const file::Path& audioPath,
                const std::shared_ptr<core::Context>&,
                const Options & = Options());

            //! Get the context.
            const std::weak_ptr<core::Context>& getContext() const;

            //! Get the timeline.
            const otio::SerializableObject::Retainer<otio::Timeline>& getTimeline() const;

            //! Get the file path.
            const file::Path& getPath() const;

            //! Get the timeline options.
            const Options& getOptions() const;

            //! \name Information
            ///@{

            //! Get the duration.
            const otime::RationalTime& getDuration() const;

            //! Get the global start time.
            const otime::RationalTime& getGlobalStartTime() const;

            //! Get the A/V information. This information is retreived from
            //! the first clip in the timeline.
            const avio::Info& getAVInfo() const;

            ///@}

            //! \name Video and Audio Data
            ///@{

            //! Set the active time ranges. This informs the timeline which
            //! I/O readers to keep active.
            void setActiveRanges(const std::vector<otime::TimeRange>&);

            //! Get video data.
            std::future<VideoData> getVideo(const otime::RationalTime&, uint16_t layer = 0);

            //! Get audio data.
            std::future<AudioData> getAudio(int64_t seconds);

            //! Cancel requests.
            void cancelRequests();

            ///@}

        private:
            TLR_PRIVATE();
        };
    }
}

#include <tlrCore/TimelineInline.h>
