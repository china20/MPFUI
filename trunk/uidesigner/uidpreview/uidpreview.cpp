// uidpreview.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "src/MainWindow.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    String strCmd(lpCmdLine);
    StringArray vecCmd;

    //DebugBreak();

    vecCmd.SplitString(strCmd, _U("="));

    if (vecCmd.Length() == 4)
    {
        suic::InitUIWgx();

        suic::ThemeOp::AddDir(vecCmd[0], vecCmd[1]);
        suic::Builder::BuildUri(NULL, vecCmd[2].c_str());

        if (suic::Application::Current())
        {
            suic::WindowPtr mainWnd = new MainWindow();
            if (suic::Builder::BuildUri(mainWnd.get(), vecCmd[3].c_str()).get() != NULL)
            {
                suic::Application::Current()->Run(mainWnd.get());
            }
        }

        suic::ExitUIWgx();
    }

    return 0;
}
