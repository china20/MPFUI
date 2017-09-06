/////////////////////////////////////////////////////////////////////////
// uienv.cpp

#include <System/Windows/Environment.h>
#include <System/Windows/SystemParameters.h>

#include <time.h>

namespace suic
{

static OSType g_s_ostype = OSType::osUnknown;

OSType Environment::GetOSType()
{
    if (g_s_ostype != OSType::osUnknown)
    {
        return g_s_ostype;
    }

    OSVERSIONINFO osver;
    osver.dwOSVersionInfoSize = sizeof(osver);

    if (GetVersionEx(&osver))
    {
        switch (osver.dwPlatformId)
        {
        case VER_PLATFORM_WIN32s:
            break;

        case VER_PLATFORM_WIN32_WINDOWS:
            break;

        case VER_PLATFORM_WIN32_NT:
            {
                if (osver.dwMajorVersion == 5)
                {
                    if (osver.dwMinorVersion == 1)
                    {
                        g_s_ostype = OSType::osXP;
                    }
                    else if (osver.dwMinorVersion == 1)
                    {
                        g_s_ostype = OSType::osWin2003;
                    }
                }
                else if (osver.dwMajorVersion == 6)
                {
                    if (osver.dwMinorVersion == 0)
                    {
                        g_s_ostype = OSType::osVista;
                    }
                    else if (osver.dwMinorVersion == 1)
                    {
                        g_s_ostype = OSType::osWin7;
                    }
                }
            }
            break;
        }
    }

    return g_s_ostype;
}

Size Environment::GetScreenClient()
{
    Size size;
    size.cx = GetSystemMetrics(SM_CXFULLSCREEN);
    size.cy = GetSystemMetrics(SM_CYFULLSCREEN);
    return size;
}

Size Environment::GetScreenBound()
{
    Size size;
    size.cx = GetSystemMetrics(SM_CXSCREEN);
    size.cy = GetSystemMetrics(SM_CYSCREEN);
    return size;
}

Time_t Environment::GetSystemTick()
{
    return (Time_t)::GetTickCount();
}

Time_t Environment::GetSystemTime()
{
    return (Time_t)time(NULL);
}

MonitorInfo Environment::GetMonitorBoundByPoint(suic::Point* pt)
{
    MonitorInfo mi;
    suic::Point pos;

    if (NULL == pt)
    {
        ::GetCursorPos(&pos);
    }
    else
    {
        pos = *pt;
    }

#if WINVER < 0x500
    mi.rcWork = Rect(Point(), Environment::GetScreenClient());
    mi.rcMonitor = Rect(Point(), Environment::GetScreenBound());
#else
    MONITORINFO minfo;
    HMONITOR hMonitor = ::MonitorFromPoint(pos, MONITOR_DEFAULTTOPRIMARY);
    minfo.cbSize = sizeof(MONITORINFO);
    ::GetMonitorInfo(hMonitor, &minfo);
    mi.rcWork = minfo.rcWork;
    mi.rcMonitor = minfo.rcMonitor;
#endif

    return mi;
}

MonitorInfo Environment::GetMonitorBoundByWindow(HWND hwnd)
{
    MonitorInfo mi;

#if WINVER < 0x500
    mi.rcWork = Rect(Point(), Environment::GetScreenClient());
    mi.rcMonitor = Rect(Point(), Environment::GetScreenBound());
#else
    MONITORINFO minfo;
    HMONITOR hMonitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
    minfo.cbSize = sizeof(MONITORINFO);
    ::GetMonitorInfo(hMonitor, &minfo);
    mi.rcWork = minfo.rcWork;
    mi.rcMonitor = minfo.rcMonitor;
#endif

    return mi;
}

}
