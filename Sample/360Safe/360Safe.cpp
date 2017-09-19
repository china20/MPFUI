// 360Safe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "./src/MainWindow.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    suic::InitUIWgxs(true);
    suic::InitUILogLevel(suic::LogLevel::Trace);

    suic::WindowPtr wnd(new MainWindow());

    try
    {
        // 加载资源文件
        suic::ThemeOp::AddDir(_U("360Safe"), suic::FileDir::CalculatePath(_U("360Safe")));
        suic::ThemeOp::SetSystemTheme("default.sres", "wr");
        suic::String strApp = _T("/360Safe;Application.xml");

        // 载入布局资源并运行系统
        suic::Application::RunApp(wnd.get(), strApp.c_str());
    }
    catch(suic::Exception& e)
    {
        suic::String err = e.GetErrorMsg();
    }

    wnd = NULL;
    
    suic::ExitUIWgx();

    return 0;
}

