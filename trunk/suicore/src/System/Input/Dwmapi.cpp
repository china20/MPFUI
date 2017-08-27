/////////////////////////////////////////////////////////////////////////
// uidwmapi.cpp

#include <System/Input/Dwmapi.h>
#include <System/Windows/Environment.h>

//#include <dwmapi.h>

//#ifndef DWMAPI
//#define DWMAPI EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
//#endif

#ifndef DWM_BB_ENABLE
#define DWM_BB_ENABLE                 0x00000001  // fEnable has been specified
#endif
#ifndef DWM_BB_BLURREGION
#define DWM_BB_BLURREGION             0x00000002  // hRgnBlur has been specified
#endif
#ifndef DWM_BB_TRANSITIONONMAXIMIZED
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004  // fTransitionOnMaximized has been specified
#endif

enum DWMWINDOWATTRIBUTE
{
    DWMWA_NCRENDERING_ENABLED = 1,      // [get] Is non-client rendering enabled/disabled
    DWMWA_NCRENDERING_POLICY,           // [set] Non-client rendering policy
    DWMWA_TRANSITIONS_FORCEDISABLED,    // [set] Potentially enable/forcibly disable transitions
    DWMWA_ALLOW_NCPAINT,                // [set] Allow contents rendered in the non-client area to be visible on the DWM-drawn frame.
    DWMWA_CAPTION_BUTTON_BOUNDS,        // [get] Bounds of the caption button area in window-relative space.
    DWMWA_NONCLIENT_RTL_LAYOUT,         // [set] Is non-client content RTL mirrored
    DWMWA_FORCE_ICONIC_REPRESENTATION,  // [set] Force this window to display iconic thumbnails.
    DWMWA_FLIP3D_POLICY,                // [set] Designates how Flip3D will treat the window.
    DWMWA_EXTENDED_FRAME_BOUNDS,        // [get] Gets the extended frame bounds rectangle in screen space
    DWMWA_HAS_ICONIC_BITMAP,            // [set] Indicates an available bitmap when there is no better thumbnail representation.
    DWMWA_DISALLOW_PEEK,                // [set] Don't invoke Peek on the window.
    DWMWA_EXCLUDED_FROM_PEEK,           // [set] LivePreview exclusion information
    DWMWA_LAST
};

typedef struct _MARGINS
{
    int cxLeftWidth;      // width of left border that retains its size
    int cxRightWidth;     // width of right border that retains its size
    int cyTopHeight;      // height of top border that retains its size
    int cyBottomHeight;   // height of bottom border that retains its size
} MARGINS, *PMARGINS;

enum DWMNCRENDERINGPOLICY
{
    DWMNCRP_USEWINDOWSTYLE, // Enable/disable non-client rendering based on window style
    DWMNCRP_DISABLED,       // Disabled non-client rendering; window style is ignored
    DWMNCRP_ENABLED,        // Enabled non-client rendering; window style is ignored
    DWMNCRP_LAST
};

typedef struct _DWM_BLURBEHIND
{
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;

HMODULE g_dwm_dll = NULL;

// 定义导出函数
typedef HRESULT (__stdcall *lpfnDwmEnableComposition)(UINT uCompositionAction);
typedef HRESULT (__stdcall *lpfnDwmExtendFrameIntoClientArea)(HWND hWnd, __in const MARGINS* pMarInset);
typedef HRESULT (__stdcall *lpfnDwmIsCompositionEnabled)(__out BOOL* pfEnabled);
typedef HRESULT (__stdcall *lpfnDwmFlush)();
typedef HRESULT (__stdcall *lpfnDwmSetWindowAttribute)(HWND ,DWORD,LPCVOID, DWORD);
typedef HRESULT (__stdcall *lpfnDwmEnableBlurBehindWindow)(HWND, __in const DWM_BLURBEHIND* pBlurBehind);

lpfnDwmEnableComposition fnDwmEnableComposition = NULL;
lpfnDwmExtendFrameIntoClientArea fnDwmExtendFrameIntoClientArea = NULL;
lpfnDwmIsCompositionEnabled fnDwmIsCompositionEnabled = NULL;
lpfnDwmFlush fnDwmFlush = NULL;
lpfnDwmSetWindowAttribute fnDwmSetWindowAttribute = NULL;
lpfnDwmEnableBlurBehindWindow fnDwmEnableBlurBehindWindow = NULL;

bool suic::UIDwmApi::IsValid()
{
    if (Environment::GetOSType() != OSType::osWin7)
    {
        return false;
    }
    else if (g_dwm_dll != NULL)
    {
        return true;
    }
    else
    {
        g_dwm_dll = ::LoadLibrary(_U("dwmapi.dll"));
        if (!g_dwm_dll)
        {
            return false;
        }
        else
        {
            fnDwmEnableComposition = (lpfnDwmEnableComposition)::GetProcAddress(g_dwm_dll, "DwmEnableComposition");
            fnDwmExtendFrameIntoClientArea = (lpfnDwmExtendFrameIntoClientArea)::GetProcAddress(g_dwm_dll, "DwmExtendFrameIntoClientArea");
            fnDwmIsCompositionEnabled = (lpfnDwmIsCompositionEnabled)::GetProcAddress(g_dwm_dll, "DwmIsCompositionEnabled");
            fnDwmFlush = (lpfnDwmFlush)::GetProcAddress(g_dwm_dll, "DwmFlush");
            fnDwmSetWindowAttribute = (lpfnDwmSetWindowAttribute)::GetProcAddress(g_dwm_dll, "DwmSetWindowAttribute");
            fnDwmEnableBlurBehindWindow = (lpfnDwmEnableBlurBehindWindow)::GetProcAddress(g_dwm_dll, "DwmEnableBlurBehindWindow");

            return true;
        }
    }
}

void suic::UIDwmApi::EnabledComposition(bool enable)
{
    if (IsCompositionEnabled() != enable && fnDwmEnableComposition)
    {
        fnDwmEnableComposition(enable);
    }
}

bool suic::UIDwmApi::IsCompositionEnabled()
{
    if (IsValid() && fnDwmIsCompositionEnabled)
    {
        BOOL enable = FALSE;
        fnDwmIsCompositionEnabled(&enable);
        return (enable ? true : false);
    }
    else
    {
        return false;
    }
}

void suic::UIDwmApi::ExtendFrameIntoClientArea(HWND hwnd, Rect rect)
{
    if (IsValid() && fnDwmExtendFrameIntoClientArea)
    {
        MARGINS marinset;
        marinset.cxLeftWidth = rect.left;
        marinset.cyTopHeight = rect.top;
        marinset.cxRightWidth = rect.right;
        marinset.cyBottomHeight = rect.bottom;

        fnDwmExtendFrameIntoClientArea(hwnd, &marinset);
    }
}


bool suic::UIDwmApi::GetRenderNcWindow(HWND hwnd)
{
    return false;
}

void suic::UIDwmApi::SetRenderNcWindow(HWND hwnd, bool enable)
{
    if (IsValid() && fnDwmSetWindowAttribute)
    {
        DWMNCRENDERINGPOLICY ncrp = DWMNCRP_DISABLED;
        if (enable)
        {
            ncrp = DWMNCRP_ENABLED;
        }
        fnDwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
    }
}

void suic::UIDwmApi::EnableBlurBehindWindow(HWND hwnd, bool enable)
{
    if (IsValid() && fnDwmEnableBlurBehindWindow)
    {
        DWM_BLURBEHIND bb = {0};
        bb.dwFlags = DWM_BB_BLURREGION | DWM_BB_ENABLE;
        if (enable)
        {
            bb.fEnable = TRUE;
        }
        else
        {
            bb.fEnable = FALSE;
        }
        bb.hRgnBlur = NULL;

        fnDwmEnableBlurBehindWindow(hwnd, &bb);
    }
}

void suic::UIDwmApi::EnableWindowThumbnail(HWND hwnd, bool enable)
{
    /*HRESULT hr = S_OK;
    HTHUMBNAIL thumbnail = NULL;
    
    hr = DwmRegisterThumbnail(hwnd, hwnd, &thumbnail);
    if (SUCCEEDED(hr))
    {
        RECT rc;
        GetClientRect(hwnd, &rc);
        DWM_THUMBNAIL_PROPERTIES dskThumbProps;
        dskThumbProps.dwFlags = DWM_TNP_RECTDESTINATION|DWM_TNP_VISIBLE|DWM_TNP_OPACITY;
        dskThumbProps.fVisible = TRUE;
        dskThumbProps.opacity = 200;
        dskThumbProps.rcDestination = rc;

        hr = DwmUpdateThumbnailProperties(thumbnail, &dskThumbProps);
    }*/
}

