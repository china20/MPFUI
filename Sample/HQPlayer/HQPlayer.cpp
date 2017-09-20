// HQPlayer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "./src/MainWindow.h"

typedef void(*lpfn_av_register_all)(void);

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include <libavutil/time.h>
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
    #include "libswresample/swresample.h"
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    suic::InitUIWgx();

    suic::WindowPtr wnd(new MainWindow());

    av_register_all();

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        fprintf(stderr, "Count not initialize SDL - %s\n", SDL_GetError());
        exit(1);
    }

    try
    {
        // 加载资源文件
        //suic::ThemeOp::SetSystemTheme("default.sres", "wr");
        suic::String strApp = _T("HQPlayer/Layout/Application.xml");

        // 载入布局资源并运行系统
        suic::Application::RunApp(wnd.get(), strApp.c_str());
    }
    catch(suic::Exception& e)
    {
        suic::String err = e.GetErrorMsg();
    }

    SDL_Quit();

    wnd = NULL;
    
    suic::ExitUIWgx();

    return 0;
}

