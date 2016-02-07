/*
 * Copyright (c) 2015 WinT 3794 <http://wint3794.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//=============================================================================
// System includes
//=============================================================================

#include <QDir>
#include <QApplication>

//=============================================================================
// Application includes
//=============================================================================

#include "Global/Global.h"
#include "Global/Settings.h"
#include "InfoWindow/InfoWindow.h"
#include "Dashboards/Dashboards.h"

//=============================================================================
// Use "Program Files" or "Program Files (x86)"?
//=============================================================================

#if defined Q_OS_WIN
#include <windows.h>
#define IS_64_BIT true
#if _WIN32
#undef  IS_64_BIT
#define IS_64_BIT GetProcAddress (GetModuleHandle (TEXT ("kernel32")), "IsWow64Process")
#endif
#endif

//=============================================================================
// Dashboard::Dashboard
//=============================================================================

Dashboards::Dashboards()
{
    connect (qApp, &QApplication::aboutToQuit,
             this, &Dashboards::closeDashboard);
    openDashboard();
}

//=============================================================================
// Dashboard::getInstance
//=============================================================================

Dashboards* Dashboards::getInstance()
{
    static Dashboards instance;
    return &instance;
}

//=============================================================================
// Dashboard::currentDashboard
//=============================================================================

Dashboards::DashboardTypes Dashboards::currentDashboard()
{
    return m_current;
}

//=============================================================================
// Dashboard::dashboardList
//=============================================================================

QStringList Dashboards::dashboardList()
{
    QStringList list;
    list.append (tr ("None"));
    list.append (tr ("Built-in Dashboard"));
    list.append (tr ("SmartDashboard"));

#if defined _WIN32 || defined _WIN64
    list.append (tr ("LabVIEW Dashboard"));
#endif

    return list;
}

//=============================================================================
// Dashboard::openDashboard
//=============================================================================

void Dashboards::openDashboard()
{
    QString path;
    m_current = (DashboardTypes) Settings::get ("Dashboard", kNone).toInt();

    /* Open the 'internal' dashboard */
    if (m_current == kBuiltin)
        INFORMATION_WINDOW()->show();

    /* Open an external dashboard */
    else
        {
            /* Open the SmartDashboard, easy as cake */
            if (m_current == kSmartDashboard)
                {
                    path = QString ("java -jar \"%1/wpilib/tools/SmartDashboard.jar\"")
                           .arg (QDir::homePath());
                }

            /* Open the LabVIEW Dashboard */
#if defined _WIN32 || defined _WIN64
            else if (m_current == kLabVIEW)
                {
                    QString pF = IS_64_BIT ? "C:/Program Files (x86)" : "C:/Program Files";
                    path = QString ("%1/FRC Dashboard/Dashboard.exe").arg (pF);
                    path = "\"" + path + "\"";
                }
#endif

            m_process.start (path);
        }

    emit dashboardChanged();
}

//=============================================================================
// Dashboard::closeDashboard
//=============================================================================

void Dashboards::closeDashboard()
{
    if (m_current != kBuiltin)
        m_process.close();

    else
        INFORMATION_WINDOW()->hide();
}

//=============================================================================
// Dashboard::reloadDashboard
//=============================================================================

void Dashboards::reloadDashboard()
{
    closeDashboard();
    openDashboard();
}
