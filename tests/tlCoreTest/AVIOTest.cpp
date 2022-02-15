// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include <tlCoreTest/AVIOTest.h>

#include <tlCore/AVIOSystem.h>
#include <tlCore/Assert.h>
#include <tlCore/String.h>
#include <tlCore/StringFormat.h>

#include <sstream>

using namespace tl::avio;

namespace tl
{
    namespace CoreTest
    {
        AVIOTest::AVIOTest(const std::shared_ptr<core::Context>& context) :
            ITest("CoreTest::AVIOTest", context)
        {}

        std::shared_ptr<AVIOTest> AVIOTest::create(const std::shared_ptr<core::Context>& context)
        {
            return std::shared_ptr<AVIOTest>(new AVIOTest(context));
        }

        void AVIOTest::run()
        {
            _enums();
            _videoData();
            _ioSystem();
        }

        void AVIOTest::_enums()
        {
            _enum<VideoType>("VideoType", getVideoTypeEnums);
        }

        void AVIOTest::_videoData()
        {
            {
                const VideoData v;
                TLRENDER_ASSERT(time::invalidTime == v.time);
                TLRENDER_ASSERT(!v.image);
            }
            {
                const auto time = otime::RationalTime(1.0, 24.0);
                const uint16_t layer = 1;
                const auto image = imaging::Image::create(imaging::Info(160, 80, imaging::PixelType::L_U8));
                const VideoData v(time, layer, image);
                TLRENDER_ASSERT(time == v.time);
                TLRENDER_ASSERT(layer == v.layer);
                TLRENDER_ASSERT(image == v.image);
            }
            {
                const auto time = otime::RationalTime(1.0, 24.0);
                const uint16_t layer = 1;
                const auto image = imaging::Image::create(imaging::Info(16, 16, imaging::PixelType::L_U8));
                const VideoData a(time, layer, image);
                VideoData b(time, layer, image);
                TLRENDER_ASSERT(a == b);
                b.time = otime::RationalTime(2.0, 24.0);
                TLRENDER_ASSERT(a != b);
                TLRENDER_ASSERT(a < b);
            }
        }
        
        namespace
        {
            class DummyPlugin : public IPlugin
            {
            public:
                std::shared_ptr<IRead> read(
                    const file::Path&,
                    const Options& = Options()) override
                {
                    return nullptr;
                }

                imaging::Info getWriteInfo(
                    const imaging::Info&,
                    const avio::Options& = avio::Options()) const override
                {
                    return imaging::Info();
                }

                std::shared_ptr<IWrite> write(
                    const file::Path&,
                    const Info&,
                    const Options & = Options()) override
                {
                    return nullptr;
                }
            };
        }

        void AVIOTest::_ioSystem()
        {
            auto system = _context->getSystem<System>();
            {
                std::vector<std::string> plugins;
                for (const auto& plugin : system->getPlugins())
                {
                    plugins.push_back(plugin->getName());
                }
                std::stringstream ss;
                ss << "Plugins: " << string::join(plugins, ", ");
                _print(ss.str());
            }
            {
                std::map<std::string, std::shared_ptr<IPlugin> > plugins;
                for (const auto& plugin : system->getPlugins())
                {
                    const auto& extensions = plugin->getExtensions();
                    if (!extensions.empty())
                    {
                        plugins[*(extensions.begin())] = plugin;
                    }
                }
                for (const auto& plugin : plugins)
                {
                    TLRENDER_ASSERT(system->getPlugin(file::Path("test" + plugin.first)) == plugin.second);
                }
                TLRENDER_ASSERT(!system->getPlugin(file::Path()));
                TLRENDER_ASSERT(!system->getPlugin<DummyPlugin>());
            }
            {
                std::vector<std::string> extensions;
                for (const auto& extension : system->getExtensions())
                {
                    extensions.push_back(extension);
                }
                std::stringstream ss;
                ss << "Extensions: " << string::join(extensions, ", ");
                _print(ss.str());
            }
            TLRENDER_ASSERT(!system->read(file::Path()));
            TLRENDER_ASSERT(!system->write(file::Path(), Info()));
        }
    }
}
