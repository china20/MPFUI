
#include <System/Interop/InternalOper.h>
#include <Framework/Controls/Window.h>

/*--------内部使用头文件--------*/
#include <System/Render/RenderManager.h>
#include <System/Render/ImageResourcePool.h>
#include <System/Interop/System.h>
#include <System/Interop/InternalWindowOper.h>
#include <System/Windows/HwndSubclass.h>
#include <System/Windows/EventManager.h>

DWORD g_bmp_time = 0;
int g_bmp_num = 0;

#ifndef SUICORE_LIB
extern HMODULE s_g_handle;
#endif

namespace suic
{

static bool gs_exitui = false;

void suiInternalClear()
{
    if (!gs_exitui)
    {
        gs_exitui = true;
        ImageSourcePool::Close();
        CoreSystem::Ins()->Clear();
    }
}

ElementPtr INT_GetLogicalRootElement(ElementPtr element)
{
    if (!element->GetUIParent())
    {
        return element;
    }
    else
    {
        return INT_GetLogicalRootElement(element->GetUIParent());
    }
}

}


