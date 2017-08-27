// 华勤科技版权所有 2008-2022
// 
// 文件名：uidwmapi.h
// 功  能：封装win7的areo效果。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIDWMAPI_H_
#define _UIDWMAPI_H_

#include <System/Windows/Object.h>

namespace suic
{

class UIDwmApi
{
public:

    static void EnabledComposition(bool enable);
    static bool IsCompositionEnabled();
    static void ExtendFrameIntoClientArea(HWND hwnd, Rect rect);

    static bool GetRenderNcWindow(HWND hwnd);
    static void SetRenderNcWindow(HWND hwnd, bool enable);

    static void EnableBlurBehindWindow(HWND hwnd, bool enable);
    static void EnableWindowThumbnail(HWND hwnd, bool enable);

    static bool IsValid();

private:

    UIDwmApi();
};

}

#endif
