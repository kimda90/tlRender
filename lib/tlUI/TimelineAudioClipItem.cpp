// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2023 Darby Johnston
// All rights reserved.

#include <tlUI/TimelineAudioClipItem.h>

#include <tlUI/DrawUtil.h>

#include <tlTimeline/Util.h>

#include <tlIO/IOSystem.h>

#include <tlCore/AudioConvert.h>

#include <sstream>

namespace tl
{
    namespace ui
    {
        void TimelineAudioClipItem::_init(
            const otio::Clip* clip,
            const TimelineItemData& itemData,
            const std::shared_ptr<system::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            ITimelineItem::_init("TimelineAudioClipItem", itemData, context, parent);

            _clip = clip;
            _track = dynamic_cast<otio::Track*>(clip->parent());

            _path = timeline::getPath(
                _clip->media_reference(),
                itemData.directory,
                itemData.pathOptions);
            _memoryRead = timeline::getMemoryRead(
                _clip->media_reference());

            auto rangeOpt = clip->trimmed_range_in_parent();
            if (rangeOpt.has_value())
            {
                _timeRange = rangeOpt.value();
            }

            _label = _path.get(-1, false);
            _textUpdate();

            _cancelObserver = observer::ValueObserver<bool>::create(
                _data.ioManager->observeCancelRequests(),
                [this](bool)
                {
                    _audioDataFutures.clear();
                });
        }

        TimelineAudioClipItem::~TimelineAudioClipItem()
        {}

        std::shared_ptr<TimelineAudioClipItem> TimelineAudioClipItem::create(
            const otio::Clip* clip,
            const TimelineItemData& itemData,
            const std::shared_ptr<system::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<TimelineAudioClipItem>(new TimelineAudioClipItem);
            out->_init(clip, itemData, context, parent);
            return out;
        }

        void TimelineAudioClipItem::setOptions(const TimelineItemOptions& value)
        {
            ITimelineItem::setOptions(value);
            if (_updates & ui::Update::Size)
            {
                _textUpdate();
                _data.ioManager->cancelRequests();
                _audioData.clear();
            }
        }

        void TimelineAudioClipItem::setViewport(const math::BBox2i& value)
        {
            ITimelineItem::setViewport(value);
            if (_updates & ui::Update::Size)
            {
                _data.ioManager->cancelRequests();
            }
        }

        namespace
        {
            std::shared_ptr<geom::TriangleMesh2> audioMesh(
                const std::shared_ptr<audio::Audio>& audio,
                const math::Vector2i& size)
            {
                auto out = std::shared_ptr<geom::TriangleMesh2>(new geom::TriangleMesh2);
                const auto& info = audio->getInfo();
                const size_t sampleCount = audio->getSampleCount();
                if (sampleCount > 0)
                {
                    switch (info.dataType)
                    {
                    case audio::DataType::F32:
                    {
                        const audio::F32_T* data = reinterpret_cast<const audio::F32_T*>(
                            audio->getData());
                        for (int x = 0; x < size.x; ++x)
                        {
                            const int x0 = std::min(
                                static_cast<size_t>((x + 0) / static_cast<double>(size.x - 1) * (sampleCount - 1)),
                                sampleCount - 1);
                            const int x1 = std::min(
                                static_cast<size_t>((x + 1) / static_cast<double>(size.x - 1) * (sampleCount - 1)),
                                sampleCount - 1);
                            //std::cout << x << ": " << x0 << " " << x1 << std::endl;
                            audio::F32_T min = 0.F;
                            audio::F32_T max = 0.F;
                            if (x0 < x1)
                            {
                                min = audio::F32Range.getMax();
                                max = audio::F32Range.getMin();
                                for (int i = x0; i < x1; ++i)
                                {
                                    const audio::F32_T v = *(data + i * info.channelCount);
                                    min = std::min(min, v);
                                    max = std::max(max, v);
                                }
                            }
                            const int h2 = size.y / 2;
                            const math::BBox2i bbox(
                                math::Vector2i(
                                    x,
                                    h2 - h2 * max),
                                math::Vector2i(
                                    x + 1,
                                    h2 - h2 * min));
                            if (bbox.isValid())
                            {
                                const size_t j = 1 + out->v.size();
                                out->v.push_back(math::Vector2f(bbox.x(), bbox.y()));
                                out->v.push_back(math::Vector2f(bbox.x() + bbox.w(), bbox.y()));
                                out->v.push_back(math::Vector2f(bbox.x() + bbox.w(), bbox.y() + bbox.h()));
                                out->v.push_back(math::Vector2f(bbox.x(), bbox.y() + bbox.h()));
                                out->triangles.push_back(geom::Triangle2({ j + 0, j + 1, j + 2 }));
                                out->triangles.push_back(geom::Triangle2({ j + 2, j + 3, j + 0 }));
                            }
                        }
                        break;
                    }
                    }
                }
                return out;
            }
        }

        void TimelineAudioClipItem::tickEvent(const ui::TickEvent& event)
        {
            auto i = _audioDataFutures.begin();
            while (i != _audioDataFutures.end())
            {
                if (i->second.future.valid() &&
                    i->second.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    const auto audio = i->second.future.get();
                    const auto size = i->second.size;
                    AudioData audioData;
                    audioData.audio = audio;
                    audioData.size = size;
                    if (audio.audio)
                    {
                        audioData.meshFuture = std::async(
                            std::launch::async,
                            [audio, size]
                            {
                                auto convert = audio::AudioConvert::create(
                                    audio.audio->getInfo(),
                                    audio::Info(1, audio::DataType::F32, audio.audio->getSampleRate()));
                            const auto convertedAudio = convert->convert(audio.audio);
                            return audioMesh(convertedAudio, size);
                            });
                    }
                    _audioData[i->first] = std::move(audioData);
                    i = _audioDataFutures.erase(i);
                    continue;
                }
                ++i;
            }

            auto j = _audioData.begin();
            while (j != _audioData.end())
            {
                if (j->second.meshFuture.valid() &&
                    j->second.meshFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    const auto mesh = j->second.meshFuture.get();
                    j->second.mesh = mesh;
                    _updates |= ui::Update::Draw;
                }
                ++j;
            }
        }

        void TimelineAudioClipItem::sizeEvent(const ui::SizeEvent& event)
        {
            ITimelineItem::sizeEvent(event);

            _margin = event.style->getSizeRole(ui::SizeRole::MarginSmall) * event.contentScale;
            _spacing = event.style->getSizeRole(ui::SizeRole::SpacingSmall) * event.contentScale;
            const auto fontMetrics = event.getFontMetrics(_fontRole);

            const int waveformWidth =
                otime::RationalTime(1.0, 1.0).value() * _options.scale;
            if (waveformWidth != _waveformWidth)
            {
                _waveformWidth = waveformWidth;
                _data.ioManager->cancelRequests();
                _audioData.clear();
            }

            _sizeHint = math::Vector2i(
                _timeRange.duration().rescaled_to(1.0).value() * _options.scale,
                _margin +
                fontMetrics.lineHeight +
                _spacing +
                _options.waveformHeight +
                _margin);
        }

        void TimelineAudioClipItem::drawEvent(const ui::DrawEvent& event)
        {
            ITimelineItem::drawEvent(event);
            if (_geometry.isValid() && _insideViewport())
            {
                const int b = event.style->getSizeRole(ui::SizeRole::Border) * event.contentScale;
                math::BBox2i g = _geometry;

                //event.render->drawMesh(
                //    ui::border(g, b, _margin / 2),
                //    event.style->getColorRole(ui::ColorRole::Border));

                event.render->drawRect(
                    g.margin(-b),
                    imaging::Color4f(.3F, .25F, .4F));

                _drawInfo(event);
                _drawWaveforms(event);
            }
        }

        void TimelineAudioClipItem::_textUpdate()
        {
            _durationLabel = ITimelineItem::_durationLabel(
                _timeRange.duration(),
                _options.timeUnits);
        }

        void TimelineAudioClipItem::_drawInfo(const ui::DrawEvent& event)
        {
            const auto fontInfo = event.getFontInfo(_fontRole);
            const auto fontMetrics = event.getFontMetrics(_fontRole);
            math::BBox2i g = _geometry;

            event.render->drawText(
                event.fontSystem->getGlyphs(_label, fontInfo),
                math::Vector2i(
                    g.min.x +
                    _margin,
                    g.min.y +
                    _margin +
                    fontMetrics.ascender),
                event.style->getColorRole(ui::ColorRole::Text));

            math::Vector2i textSize = event.fontSystem->measure(_durationLabel, fontInfo);
            event.render->drawText(
                event.fontSystem->getGlyphs(_durationLabel, fontInfo),
                math::Vector2i(
                    g.max.x -
                    _margin -
                    textSize.x,
                    g.min.y +
                    _margin +
                    fontMetrics.ascender),
                event.style->getColorRole(ui::ColorRole::Text));
        }

        void TimelineAudioClipItem::_drawWaveforms(const ui::DrawEvent& event)
        {
            const auto fontMetrics = event.getFontMetrics(_fontRole);
            const math::BBox2i vp(0, 0, _viewport.w(), _viewport.h());
            math::BBox2i g = _geometry;

            const math::BBox2i bbox(
                g.min.x +
                _margin,
                g.min.y +
                _margin +
                fontMetrics.lineHeight +
                _spacing,
                _sizeHint.x - _margin * 2,
                _options.waveformHeight);
            event.render->drawRect(
                bbox,
                imaging::Color4f(0.F, 0.F, 0.F));
            event.render->setClipRectEnabled(true);
            event.render->setClipRect(bbox);

            std::set<otime::RationalTime> audioDataDelete;
            for (const auto& audioData : _audioData)
            {
                audioDataDelete.insert(audioData.first);
            }

            if (g.intersects(vp))
            {
                if (_ioInfoInit)
                {
                    _ioInfoInit = false;
                    _ioInfo = _data.ioManager->getInfo(_path).get();
                    _updates |= ui::Update::Size;
                    _updates |= ui::Update::Draw;
                }
            }

            if (_waveformWidth > 0)
            {
                for (int x = _margin; x < _sizeHint.x - _margin; x += _waveformWidth)
                {
                    math::BBox2i bbox(
                        g.min.x +
                        x,
                        g.min.y +
                        _margin +
                        fontMetrics.lineHeight +
                        _spacing,
                        _waveformWidth,
                        _options.waveformHeight);
                    if (bbox.intersects(vp))
                    {
                        const int w = _sizeHint.x - _margin * 2;
                        const otime::RationalTime time = time::round(otime::RationalTime(
                            _timeRange.start_time().value() +
                            (w > 0 ? ((x - _margin) / static_cast<double>(w)) : 0) *
                            _timeRange.duration().value(),
                            _timeRange.duration().rate()));
                        auto i = _audioData.find(time);
                        if (i != _audioData.end())
                        {
                            if (i->second.mesh)
                            {
                                event.render->drawMesh(
                                    *i->second.mesh,
                                    bbox.min,
                                    imaging::Color4f(1.F, 1.F, 1.F));
                            }
                            audioDataDelete.erase(time);
                        }
                        else if (_ioInfo.audio.isValid())
                        {
                            const auto j = _audioDataFutures.find(time);
                            if (j == _audioDataFutures.end())
                            {
                                const otime::RationalTime mediaTime = timeline::mediaTime(
                                    time,
                                    _track,
                                    _clip,
                                    _ioInfo.audioTime.duration().rate());
                                const otime::TimeRange mediaTimeRange(
                                    mediaTime,
                                    otime::RationalTime(
                                        _ioInfo.audioTime.duration().rate(),
                                        _ioInfo.audioTime.duration().rate()));
                                _audioDataFutures[time].future = _data.ioManager->readAudio(_path, mediaTimeRange);
                                _audioDataFutures[time].size = bbox.getSize();
                            }
                        }
                    }
                }
            }

            for (auto i : audioDataDelete)
            {
                const auto j = _audioData.find(i);
                if (j != _audioData.end())
                {
                    _audioData.erase(j);
                }
            }

            event.render->setClipRectEnabled(false);
        }
    }
}
