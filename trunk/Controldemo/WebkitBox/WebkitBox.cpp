// Button.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "src/MainWindow.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    suic::InitUIWgx();

    suic::WindowPtr wnd(new MainWindow());

    try
    {
        // 加载资源文件
        suic::ThemeOp::SetSystemTheme("default.sres", "wr");
        suic::String strApp = _T("ControlDemo/WebkitBox/Application.xml");

        // 载入布局资源并运行系统
        suic::Application::RunApp(wnd.get(), strApp.c_str());
        wnd->Dispose();
    }
    catch(suic::Exception& e)
    {
        suic::String err = e.GetErrorMsg();
    }

    suic::ExitUIWgx();

    return 0;
}
