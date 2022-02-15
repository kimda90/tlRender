// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#pragma once

#include <tlCore/AVIO.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

} // extern "C"

namespace tl
{
    //! FFmpeg I/O
    namespace ffmpeg
    {
        //! Profiles.
        enum class Profile
        {
            H264,
            ProRes,
            ProRes_Proxy,
            ProRes_LT,
            ProRes_HQ,
            ProRes_4444,
            ProRes_XQ,

            Count
        };
        TLRENDER_ENUM(Profile);
        TLRENDER_ENUM_SERIALIZE(Profile);

        //! Number of threads.
        const size_t threadCount = 4;

        //! Timeout for requests.
        const std::chrono::milliseconds requestTimeout(1);

        //! Software scaler flags.
        const int swsScaleFlags = SWS_FAST_BILINEAR;

        //! Swap the numerator and denominator.
        AVRational swap(AVRational);

        //! Convert to FFmpeg channel layout.
        int64_t fromChannelCount(uint8_t);

        //! Convert from FFmpeg.
        audio::DataType toAudioType(AVSampleFormat);

        //! Convert to FFmpeg.
        AVSampleFormat fromAudioType(audio::DataType);

        //! Get a label for a FFmpeg error code.
        std::string getErrorLabel(int);

        //! FFmpeg reader
        class Read : public avio::IRead
        {
        protected:
            void _init(
                const file::Path&,
                const avio::Options&,
                const std::weak_ptr<core::LogSystem>&);
            Read();

        public:
            ~Read() override;

            //! Create a new reader.
            static std::shared_ptr<Read> create(
                const file::Path&,
                const avio::Options&,
                const std::weak_ptr<core::LogSystem>&);

            std::future<avio::Info> getInfo() override;
            std::future<avio::VideoData> readVideo(const otime::RationalTime&, uint16_t layer = 0) override;
            std::future<avio::AudioData> readAudio(const otime::TimeRange&) override;
            bool hasRequests() override;
            void cancelRequests() override;
            void stop() override;
            bool hasStopped() const override;

        private:
            void _open(const std::string& fileName);
            void _run();
            void _close();

            TLRENDER_PRIVATE();
        };

        //! FFmpeg writer.
        class Write : public avio::IWrite
        {
        protected:
            void _init(
                const file::Path&,
                const avio::Info&,
                const avio::Options&,
                const std::weak_ptr<core::LogSystem>&);
            Write();

        public:
            ~Write() override;

            //! Create a new writer.
            static std::shared_ptr<Write> create(
                const file::Path&,
                const avio::Info&,
                const avio::Options&,
                const std::weak_ptr<core::LogSystem>&);

            void writeVideo(
                const otime::RationalTime&,
                const std::shared_ptr<imaging::Image>&) override;

        private:
            void _encodeVideo(AVFrame*);

            TLRENDER_PRIVATE();
        };

        //! FFmpeg Plugin
        class Plugin : public avio::IPlugin
        {
        protected:
            void _init(const std::weak_ptr<core::LogSystem>&);
            Plugin();

        public:
            //! Create a new plugin.
            static std::shared_ptr<Plugin> create(const std::weak_ptr<core::LogSystem>&);

            std::shared_ptr<avio::IRead> read(
                const file::Path&,
                const avio::Options& = avio::Options()) override;
            imaging::Info getWriteInfo(
                const imaging::Info&,
                const avio::Options& = avio::Options()) const override;
            std::shared_ptr<avio::IWrite> write(
                const file::Path&,
                const avio::Info&,
                const avio::Options& = avio::Options()) override;

        private:
            static void _logCallback(void*, int, const char*, va_list);

            static std::weak_ptr<core::LogSystem> _logSystemWeak;
        };
    }
}
