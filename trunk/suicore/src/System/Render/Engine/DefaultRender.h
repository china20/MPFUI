// 华勤科技版权所有 2008-2022
// 
// 文件名：defaultrender.h
// 功  能：默认绘制方式（串行）
// 
// 作  者：MPF开发组
// 时  间：2010-07-12
// 
// ============================================================================

#ifndef _DEFAULTRENDER_H_
#define _DEFAULTRENDER_H_

#include <System/Tools/Thread.h>
#include <System/Tools/Signal.h>
#include <System/Windows/HwndObject.h>
#include <System/Windows/RenderTarget.h>

namespace suic
{

struct RenderUnit
{
    // 如果画布为空，表示需要进行布局
    Bitmap* canvas;
    fRect clip;
    Handle h;
    bool debugLine;
    FrameworkElement* root;
};

typedef Array<RenderUnit, false> RenderArray;

class RenderPools
{
public:

    RenderPools();

    int GetCount() const;

    bool PopRenderUnit(RenderUnit& ru);
    void PushRenderUnit(const RenderUnit& ru);
    
    void Clear();

    bool Wait(int iTimeout);
    void InitRenderPools(int iCount);

    bool WaitGlobalDone(int iTimeout);
    void DoneOne();
    void InitGlobalCount(int iCount);

private:

    int _doneCount;
    int _globalCount;
    RenderArray _units;
    Mutex _mutex;
    UISignal _signal;
    UISignal _donesig;
};

class RenderThread : public Thread
{
public:

    static RenderPools ThrPools;

    RenderThread();

    void Run();

    static void InitThreadPoos(int iCount);
    static void CloseThreads();

private:

    bool _exit;
    static Array<Thread*, false> _threads;
};

class DefRender
{
public:

    static void RenderCanvas(FrameworkElement* root, Bitmap* canvas, fRect clip, Point pt, bool bDebugLine);

protected:

    static void DrawDebugLine(FrameworkElement* child, Drawing * drawing);
};

}

#endif
