// uidrun.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <uidcore/uidframe.h>

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    //
    // 初始化MPF环境
    //
    suic::InitUIWgxs(true);
    suic::InitUILogLevel(suic::LogLevel::Trace);

    UIDFramePtr frame;

    //
    // 获取设计器核心接口
    //
    UIDGetFrame(frame);

    //
    // 运行
    //
    frame->RunFrame();
    frame = NULL;

    //
    // MPF环境清理
    //
    suic::ExitUIWgx();

	return 0;
}
