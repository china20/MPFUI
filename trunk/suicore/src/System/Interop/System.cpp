
#include <resource.h>

#include <System/Interop/System.h>

#include <System/Tools/CoreHelper.h>
#include <System/Tools/ThemeOp.h>
#include <System/Windows/BuildFactory.h>
#include <System/Windows/HwndSubclass.h>

extern HMODULE s_g_handle;

namespace suic
{

POINT __GetCusorPoint(HWND h);

void DoInitResourceByEvent(Element* pElem)
{
    FrameworkElementPtr ctrlPtr(pElem);

    if (ctrlPtr)
    {
        ctrlPtr->UpdateVisualState(true);
    }
}

/////////////////////////////////////////////////////////////////
// UISystem

static CoreSystem* g_system = NULL;

CoreSystem::CoreSystem()
{
    _isSizeMode = false;
    _drawDebugLine = false;
    _setMouseDown = false;

    _scaleRatio = 1.0f;

    _scaleDpi = false;
    _bInit = true;
    _bInitRes = false;
    _decrypt = NULL;
    _defResInfo = NULL;
    _defaultRes = new ResourceDictionary();
    _loglevel = Log::eNone;

    _defaultRes->ref();

    XNULL = new OString(_U("x:Null"));
    XNULL->ref();

    _defUIFont = GetStockObject(DEFAULT_GUI_FONT);
}

CoreSystem::~CoreSystem()
{
    _defaultRes->unref();
    _defaultRes = NULL;

    if (NULL != _defResInfo)
    {
        _defResInfo->unref();
        _defResInfo = NULL;
    }

    XNULL->unref();
    g_system = NULL;
}

bool CoreSystem::IsInitialized()
{
    return _bInit;
}

CoreSystem* CoreSystem::Ins()
{
    if (NULL == g_system)
    {
        g_system = new CoreSystem();
    }

    if (!g_system->IsInitialized())
    {
        g_system->CoreSystem::CoreSystem();
    }

    return g_system;
}

int CoreSystem::ReadDefRes(ResContext& resCtx, const String& key, suic::ISStream* buff)
{
    int iSucc = 0;
    IThemeInfo* pResInfo = InitDefaultResInfo();
    if (pResInfo != NULL)
    {
        iSucc = pResInfo->ReadRes(key, buff);
        resCtx.rftType = ResReadFrom::rrfDef;
        resCtx.resInfo = _defResInfo;
    }
    return iSucc;
}

void CoreSystem::SetWndPos(HWND hwnd, HWND hAfter, int x, int y, int cx, int cy, UINT uFlags)
{
    x = SystemParameters::TransformXToDevice(x);
    cx = SystemParameters::TransformXToDevice(cx);

    y = SystemParameters::TransformYToDevice(y);
    cy = SystemParameters::TransformYToDevice(cy);


    ::SetWindowPos(hwnd, hAfter, x, y, cx, cy, uFlags);
}

int CoreSystem::ReadFileData(const String& strPath, suic::ISStream* data)
{
    String strFile(FileDir::CalculatePath(strPath));
    int iSize = 0;
    FileStream fStrm;

    if (fStrm.Open(strFile, false))
    {
        StreamOp::WriteToStream(data, &fStrm);
        iSize = data->GetSize();
    }

    /*HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD readBytes = 0;
        iSize = (int)GetFileSize(hFile, NULL);

        while ()
        {
            ;
        }

        data.Resize(iSize);

        ReadFile(hFile, (LPVOID)data.GetBuffer(), iSize, &readBytes, NULL);

        if (iSize != readBytes)
        {
            data.Clear();
            iSize = 0;
        }

        CloseHandle(hFile);
    }*/

    return iSize;
}

void CoreSystem::ReadDefaultThemeData(suic::ISStream* data)
{
    IThemeInfo* pResInfo = InitDefaultResInfo();
    if (pResInfo != NULL)
    {
        pResInfo->ReadRes(_U("theme/default.xaml"), data);
    }
}

void CoreSystem::ReadDefaultThemeInfo(suic::ISStream* data)
{
    suic::String resPath = FileDir::CalculatePath(_U("mpf.mres"));
    if (FileDir::FileExist(resPath))
    {
        CoreSystem::ReadFileData(resPath, data);
    }
    else
    {
        CoreSystem::ReadObjFromRes(s_g_handle, MAKEINTRESOURCE(DEFRES), data);
    }
}

IThemeInfo* CoreSystem::InitDefaultResInfo()
{
    if (!_bInitRes && _defResInfo == NULL)
    {
        MemThemeInfo* pRes = NULL;
        ByteStream data;

        _bInitRes = true;
        ReadDefaultThemeInfo(&data);

        if (data.GetSize() > 16)
        {
            pRes = new MemThemeInfo(NULL);
            pRes->ref();

            if (pRes->InitRes(data, _U("_mpf_res_")))
            {
                _defResInfo = pRes;
                InitDefaultRes();
            }
            else
            {
                pRes->unref();
            }
        }
    }
    return _defResInfo;
}

void CoreSystem::InitDefaultRes()
{
    IThemeInfo* pResInfo = _defResInfo;

    if (pResInfo != NULL)
    {
        ByteStream data;

        if (pResInfo->ReadRes(_U("theme/default.xaml"), &data) > 0)
        {
            XamlReader reader;
            suic::Uint16 iMask = ThemeOp::GetReadResourceMask();

            ThemeOp::SetReadResourceMask(suic::ResReadMask::ReadDefMask);

            AutoObj dicObj = reader.LoadMemory(NULL, NULL, data.GetBuffer(), data.GetSize());
            ResourceDictionary* resDic = RTTICast<ResourceDictionary>(dicObj.get());
            if (NULL != resDic)
            {
                SETREFOBJ(_defaultRes, resDic);
            }

            ThemeOp::SetReadResourceMask(iMask);
        }
    }
}

ResourceDictionary* CoreSystem::GetDefaultTheme()
{
    InitDefaultResInfo();
    return _defaultRes;
}

int CoreSystem::ReadBytesFromRes(HMODULE h, HRSRC hres, suic::ISStream* data)
{
    if (hres == NULL)
    {
        return 0;
    }

    int iSize = (int)::SizeofResource(h, hres);
    HGLOBAL hMem= ::LoadResource(h, hres);
    if (!hMem)
    {
        return 0;
    }

    data->Write((Byte*)LockResource(hMem), iSize);
    return iSize;
}

int CoreSystem::ReadObjFromRes(HMODULE h, LPCTSTR name, suic::ISStream* data)
{
    HRSRC hRsc = ::FindResource(h, name, _T("BYTEDATA"));
    return ReadBytesFromRes(h, hRsc, data);
}

Object* CoreSystem::FindRes(const String& strName)
{
    ResourceDictionary* resDic = GetDefaultTheme();
    if (NULL != resDic)
    {
        return resDic->Lookup(strName);
    }
    else
    {
        return DpProperty::UnsetValue();
    }
}

HBITMAP CoreSystem::CreateAlphaBitmap(HDC hdc, int iWid, int iHei)
{
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iWid;
    bmi.bmiHeader.biHeight = iHei;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 4 * iHei * iWid;
    RGBQUAD* rq = NULL;
    HBITMAP bmp = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (LPVOID*)&rq, NULL, 0);

    return bmp;
}

void CoreSystem::Clear()
{
    _threadWindowHandles.Clear();
    _bInit = false;

    ThemeOp::_reses.Clear();

    delete this;
}

BOOL WINAPI ThreadWindowsCallback(HWND hWnd, LPARAM lparam)
{ 
    HwndItems* item = (HwndItems*)lparam;

    if (IsWindowVisible(hWnd) && 
        IsWindowEnabled(hWnd))
    { 
        item->hwnds.Add(hWnd);
        EnableWindow(hWnd, FALSE);
    }

    return TRUE;
}

HwndItems* CoreSystem::FindHwndItem(HWND hwnd)
{
    HwndItems* pItem = NULL;
    _threadWindowHandles.TryGetValue(hwnd, pItem);
    return pItem;
}

HwndItems* CoreSystem::SearchCurrentThreadWindow()
{
    HwndItems* pVec(new HwndItems());

    //
    // 找到线程内所有的窗口对象
    //
    EnumThreadWindows(GetCurrentThreadId(), ThreadWindowsCallback, LPARAM(pVec));

    return pVec;
}

void CoreSystem::AddhwndItem(HWND hwnd, HwndItems* pItem)
{
    _threadWindowHandles.Add(hwnd, pItem);
}

void CoreSystem::EnableThreadWindowAndErase(HWND hwnd)
{
    HwndItems* pItem = NULL;

    if (_threadWindowHandles.TryGetValue(hwnd, pItem))
    {
        for (int i = 0; i < (int)pItem->hwnds.GetCount(); ++i)
        {
            EnableWindow(pItem->hwnds[i], TRUE);
        }

        delete pItem;

        _threadWindowHandles.Remove(hwnd);
    }
}

//------------------------------------------------------------

static void __EnableElementMouseCapture(Element* element)
{
    if (element)
    {
        element->WriteFlag(CoreFlags::IsMouseCapturedWithinCache, true);
        __EnableElementMouseCapture(element->GetUIParent());
    }
}

static void __RemoveElementMouseCapture(Element* element)
{
    if (element)
    {
        element->WriteFlag(CoreFlags::IsMouseCapturedWithinCache, false);
        __RemoveElementMouseCapture(element->GetUIParent());
    }
}

void CoreSystem::BuildElement(const String& strType, ObjectPtr& obj)
{
    RTTIOfInfo* cls = BuildFactory::Ins()->Lookup(strType);
    if (cls)
    {
        obj = cls->Create();
    }
}

}
