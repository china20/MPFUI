// TextBlock.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "src/MainWindow.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    suic::InitUIWgx();
    suic::InitUILogLevel(suic::LogLevel::Trace);

    suic::WindowPtr wnd(new MainWindow());
    //suic::ResourceHelper::AddDir(_U("demon"), _U("Z:\\Work\\UI\\sharpui\\trunk\\bin\\debug_vc90\\jianfeng\\"));

    try
    {
        suic::String strApp = _T("ControlDemo/ListView/Application.xml");
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
