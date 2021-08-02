// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021 Darby Johnston
// All rights reserved.

#include "App.h"

#include "MainWindow.h"

#include <tlrCore/Math.h>
#include <tlrCore/StringFormat.h>
#include <tlrCore/Time.h>

void qtInitResources()
{
    Q_INIT_RESOURCE(tlrQt);
}

namespace tlr
{
    App::App(int& argc, char** argv) :
        QApplication(argc, argv)
    {
        IApp::_init(
            argc,
            argv,
            "tlrfilmstrip-qwidget",
            "View a timeline as a series of thumbnail images.",
            {
                app::CmdLineValueArg<std::string>::create(
                    _input,
                    "input",
                    "The input timeline.",
                    true)
            });
        const int exitCode = getExit();
        if (exitCode != 0)
        {
            exit(exitCode);
            return;
        }

        qtInitResources();

        QCoreApplication::setOrganizationName("tlRender");
        QCoreApplication::setApplicationName("tlrfilmstrip-qwidget");

        setStyle("Fusion");

        auto mainWindow = new MainWindow(_input, _context);
        mainWindow->show();
    }
}
