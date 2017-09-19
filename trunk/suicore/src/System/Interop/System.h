// 华勤科技版权所有 2008-2022
// 
// 文件名：system.h
// 功  能：负责绘制层叠窗口(带有alpha通道)
// 
// 作  者：MPF开发组
// 时  间：2010-07-12
// 
// ============================================================================

#ifndef _UISYSTEM_H_
#define _UISYSTEM_H_

#include <System/Tools/Collection.h>
#include <System/Tools/Log.h>
#include <System/Tools/Array.h>
#include <System/Tools/XRUnzip.h>
#include <System/Tools/TreeBuilder.h>

#include <System/Types/Interface.h>

#include <System/Interop/ResourceInfo.h>
#include <System/Interop/PlugManager.h>
#include <System/Interop/ColorManager.h>

#include <Skia/core/SkTypeface.h>
#include <Skia/core/SkPath.h>
#include <Skia/core/SkMatrix.h>
#include <Skia/core/SkBitmap.h>

namespace suic
{

struct HwndItems
{
    Array<HWND, false> hwnds;
};

void InitializeSystemParameters();

struct BitmapInfo
{
    SkBitmap bmp;

    BitmapInfo()
    {
    }

    ~BitmapInfo()
    {
    }
};

struct PathInfo
{
    SkPath path;

    void Reset()
    {
        path.reset();
    }
};

struct MatrixInfo
{
    SkMatrix m;
};

class CoreSystem
{
public:

    static CoreSystem* Ins();

    CoreSystem();
    ~CoreSystem();

    static int ReadObjFromRes(HMODULE h, LPCTSTR name, suic::ISStream* data);
    static int ReadBytesFromRes(HMODULE h, HRSRC hres, suic::ISStream* data);
    static int ReadFileData(const String& strPath, suic::ISStream* data);

    static void SetWndPos(HWND hwnd, HWND hAfter, int x, int y, int cx, int cy, UINT uFlags);

    bool IsInitialized();

    ResourceDictionary* GetDefaultTheme();
    IThemeInfo* InitDefaultResInfo();
    void ReadDefaultThemeInfo(suic::ISStream* data);
    void ReadDefaultThemeData(suic::ISStream* data);

    int ReadDefRes(ResContext& resCtx, const String& key, suic::ISStream* buff);

    bool GetDrawDebugLine() const
    {
        return _drawDebugLine;
    }

    void SetDrawDebugLine()
    {
        _drawDebugLine = !_drawDebugLine;
    }

    bool IsSetMouseDown() const
    {
        return _setMouseDown;
    }

    void SetMouseDown(bool val)
    {
        _setMouseDown = val;
    }

    void SetResourceDecryption(IResDecryption* pDecrypt)
    {
        SETREFOBJ(_decrypt, pDecrypt);
    }

    IResDecryption* GetResDecryption() const
    {
        return _decrypt;
    }

    HGDIOBJ GetDefUIFont() { return _defUIFont; }

    String GetMouseupFlag();
    void SetMouseupFlag(String val);
    String GetRMouseupFlag();
    String SetRMouseupFlag(String val);

    void BuildElement(const String& strType, ObjectPtr& obj);

    void SetSizeMode(bool bMode);
    bool IsSizeMode();

    void GetDefContent(Object* val, ObjectPtr& obj);
    Object* FindRes(const String& strName);

    HBITMAP CreateAlphaBitmap(HDC hdc, int iWid, int iHei);

    void Clear();

    HwndItems* SearchCurrentThreadWindow();
    void AddhwndItem(HWND hwnd, HwndItems* pItem);
    void EnableThreadWindowAndErase(HWND hwnd);

    HwndItems* FindHwndItem(HWND hwnd);
    Log* GetSystemLog();

    void SetLogLevel(int level);

    void SetDpiScale(bool bScale);
    bool GetDpiScale();
    void SetScaleRatio(Float fScale);
    Float GetScaleRatio();

    OString* GetXNULL();

private:

    void InitDefaultRes();

protected:

    IResDecryption* _decrypt;
    IThemeInfo* _defResInfo;
    ResourceDictionary* _defaultRes;

    PointerDic<HWND, HwndItems*> _threadWindowHandles;

    Log _systemLog;
    HGDIOBJ _defUIFont;

    String _resPath;
    String _activeStyle;
    String _mouseupflag;
    String _rmouseupflag;

    int _loglevel;

    Float _scaleRatio;

    bool _scaleDpi;

    bool _bInit;
    bool _bInitRes;
    bool _isSizeMode;
    bool _drawDebugLine;
    bool _setMouseDown;

    OString* XNULL;
};

inline OString* CoreSystem::GetXNULL()
{
    return XNULL;
}

inline Log* CoreSystem::GetSystemLog()
{
    if (_loglevel > 0 && !_systemLog.IsValid())
    {
        String logPath;
        Char cpPath[256] = {0};

        GetModuleFileName(NULL, cpPath, 256);
        logPath = cpPath;
        logPath += _U(".log");
        _systemLog.Init(logPath, _loglevel);
    }

    return &_systemLog;
}

inline void CoreSystem::SetLogLevel(int level)
{
    _loglevel = level;
}

inline void CoreSystem::SetDpiScale(bool bScale)
{
    _scaleDpi = bScale;
    InitializeSystemParameters();
}

inline bool CoreSystem::GetDpiScale()
{
    return _scaleDpi;
}

inline void CoreSystem::SetScaleRatio(Float fScale)
{
    _scaleRatio = fScale;
    InitializeSystemParameters();
}

inline Float CoreSystem::GetScaleRatio()
{
    return _scaleRatio;
}

inline String CoreSystem::GetMouseupFlag()
{
    return _mouseupflag;
}

inline void CoreSystem::SetMouseupFlag(String val)
{
    _mouseupflag = val;
}

inline String CoreSystem::GetRMouseupFlag()
{
    return _rmouseupflag;
}

inline String CoreSystem::SetRMouseupFlag(String flag)
{
    static int i = 0;

    if (flag.Empty())
    {
        _rmouseupflag = flag;
    }
    else
    {
        i = (++i) % 1000000;
        _rmouseupflag.Format(_T("%s:%d"), flag.c_str(), i);
    }

    return _rmouseupflag;
}

inline void CoreSystem::SetSizeMode(bool bMode)
{
    _isSizeMode = bMode;
}

inline bool CoreSystem::IsSizeMode()
{
    return _isSizeMode;
}

}

#define SystemLog() suic::CoreSystem::Ins()->GetSystemLog()
#define DefUIFont() suic::CoreSystem::Ins()->GetDefUIFont()

#endif
