// 华勤科技版权所有 2010-2011
// 
// 文件名：UIConst.h
// 功  能：定义核心库用到的常量。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UICONST_H_
#define _UICONST_H_

#include <System/Types/Types.h>

namespace suic
{

struct InvTags
{
    enum
    {
        All = 0,
        Measure = 1,
        Arrange = 2,
        Render = 4,
    };
};

enum AxisDirection
{
    xyAxis = 0,
    xAxis = 1,
    yAxis = 2,
};

enum FocusNavDirection
{
    fndNext,
    fndPrevious,
    fndFirst,
    fndLast,
    fndLeft,
    fndRight,
    fndUp,
    fndDown
};

enum KNavMode
{
    knNone,
    knOnce,
    knContinue,
    knCycle,
    knContained,
    knLocal,
};

enum Key
{
    kNone = 0,

    kLeftButton = 0x01,
    kRightButton = 0x02,
    kCancel = 0x03,
    kMidButton = 0x04,

    kXButton1 = 0x05,
    kXButton2 = 0x06,

    kBack = 0x08,
    kTab = 0x09,

    kClear = 0x0C,
    kReturn = 0x0D,

    kShift = 0x10,
    kControl = 0x11,
    kMenu = 0x12,
    kPause = 0x13,
    kCapital = 0x14,

    kKana = 0x15,
    kMangeul = 0x15,
    kHangul = 0x15,
    kJunja = 0x17,
    kFinal = 0x18,
    kHanja = 0x19,
    kKanji = 0x19,

    kEscape = 0x1B,

    kConvert = 0x1C,
    kNonConvert = 0x1D,
    kAccept = 0x1E,
    kModeChange = 0x1F,

    kSpace = 0x20,
    kPrior = 0x21,
    kNext = 0x22,
    kEnd = 0x23,
    kHome = 0x24,
    kLeft = 0x25,
    kUp = 0x26,
    kRight = 0x27,
    kDown = 0x28,
    kSelect = 0x29,
    kPrint = 0x2A,
    kExecute = 0x2B,
    kPrintScreen = 0x2C,
    kInsert = 0x2D,
    kDelete = 0x2E,
    kHelp = 0x2F,

    kLeftWin = 0x5B,
    kRightWin = 0x5C,
    kApps = 0x5D,

    kSleep = 0x5F,

    kNumPad0 = 0x60,
    kNumPad1 = 0x61,
    kNumPad2 = 0x62,
    kNumPad3 = 0x63,
    kNumPad4 = 0x64,
    kNumPad5 = 0x65,
    kNumPad6 = 0x66,
    kNumPad7 = 0x67,
    kNumPad8 = 0x68,
    kNumPad9 = 0x69,
    kMultiply = 0x6A,
    kAdd =  0x6B,
    kSeparator =  0x6C,
    kSubtract =  0x6D,
    kDecimal =  0x6E,
    kDivide =  0x6F,
    kF1 =  0x70,
    kF2 =  0x71,
    kF3 =  0x72,
    kF4 =  0x73,
    kF5 =  0x74,
    kF6 =  0x75,
    kF7 =  0x76,
    kF8 =  0x77,
    kF9 =  0x78,
    kF10 = 0x79,
    kF11 = 0x7A,
    kF12 = 0x7B,
    kF13 = 0x7C,
    kF14 = 0x7D,
    kF15 = 0x7E,
    kF16 = 0x7F,
    kF17 = 0x80,
    kF18 = 0x81,
    kF19 = 0x82,
    kF20 = 0x83,
    kF21 = 0x84,
    kF22 = 0x85,
    kF23 = 0x86,
    kF24 = 0x87,

    kNumLock = 0x90,
    kScroll = 0x91,
    kLeftShift = 0xA0,
    kRightShift = 0xA1,
    kLeftCtrl = 0xA2,
    kRightCtrl = 0xA3,
    kLeftAlt = 0xA4,
    kRightAlt = 0xA5,
};

enum ModifierKey
{
    mkNone = 0,
    mkAlt = 1,
    mkControl = 2,
    mkShift = 4,
    mkWindows = 8,
};

enum KeyFlags
{
    kfRepeat = 1,
    kfExtended = 2,
};

enum MouseButtonState
{
    mbMouseRelease,
    mbMousePress,
};

enum MouseButton
{
    mbUnknown = 0,
    mbLeftButton = 1,
    mbRightButton = 2,
    mbMiddleButton = 4,
    mbXBUTTON1 = 8,
    mbXBUTTON2 = 16,
};

enum OSType
{
    osUnknown = 0,
    osWin95 = 1,
    osXP = 2,
    osWin2003 = 3,
    osVista = 4,
    osWin7 = 5,
    osWin8 = 6,
    osWin81 = 7,
    osWin100 = 8,
};

enum Dock
{
    dockLeft,
    dockTop,
    dockRight,
    dockBottom,
};

enum ElementAlign
{
    eaLeftTop,
    eaLeftBottom,
    eaRightTop,
    eaRightBottom,
};

enum Visibility : int
{
    Collapsed = 2,
    Hidden = 1,
    Visible = 0
};

enum Orientation
{
    Horizontal = 0,
    Vertical = 1,
};

enum FlowDirection
{
    LeftToRight,
    RightToLeft,
};

struct CoreFlags
{
    enum eVertState
    {
        Top = 0,
        Bottom = 1,
        Center = 2,
        Stretch = 3,
        VerticalOrientation = 0x100,
    };

    enum eHorzState
    {
        Left = 0,
        Right = 1,
    };

    enum eHitFlag
    {
        HitValid = HTERROR,
        HitCaption = HTCAPTION,
        HitClient = HTCLIENT,
    };

    enum eCoreFlags
    {
        None = 0x00000000,

        MeasureInProgress = 0x00000001,
        ArrangeInProgress = 0x00000002,
        IsInitialized = 0x00000004,
        IsInitPending = 0x00000008,

        IsMouseOverWithinCache = 0x00000010,
        IsMouseOverWithinChanged = 0x00000020,
        IsMouseCapturedWithinCache = 0x00000040,
        IsMouseCapturedWithinChanged = 0x00000080,

        IsKeyboardFocusWithinCache = 0x00000100,
        IsKeyboardFocusWithinChanged = 0x00000200,
        IsVisibleCache = 0x00000400,
        IsHitTestHidden = 0x00000800,
        
        IsHidden = 0x0001000,
        IsCollapsed = 0x00002000,
        IsTopMost = 0x00004000,
        IsSysPending = 0x00008000,

        IsMeasureDirty = 0x00010000,
        IsArrangeDirty = 0x00020000,
        IsRenderDirty = 0x00040000,
        IsForbidInvalidateDC = 0x00080000,

        IsSizeModing = 0x00100000,
        IsInvalidateLocked = 0x00200000,
        IsTabStop = 0x00400000,
        IsLoaded = 0x00800000,
 
        IsCachedStyle = 0x02000000,
        IsNotRender = 0x04000000,
        IsUseLayoutPolicy = 0x08000000,

        IsInnerHidden = 0x10000000,
        IsInnerDisabled = 0x20000000,
        IsBlockMeasure = 0x40000000,
        IsMouseDownHidden = 0x80000000,
    };
};

enum InnerFlags : Uint32
{
    IsLeftMouseDown = 0x00000010,
    IsVirtualizing = 0x00000020,
    IsScrollArrange = 0x00000040,
    IsScrolling = 0x00000080,

    IsLazyGenerate = 0x00000100,
    IsConnectedGenerator = 0x00000200,
    IsRightToLeft = 0x00000400,

    IsCanSkipMeasure = 0x00001000,
    IsCanSkipArrange = 0x00002000,
    IsNeedsClipBounds = 0x00004000,

    HasTemplateChanged = 0x00010000,
    HasStyleInvalidated = 0x00020000,
    IsStyleUpdateInProgress = 0x00040000,
    IsStyleSetFromGenerator = 0x00080000,

    IsTemplateGenerating = 0x00100000,
    IsTemplateGenerated = 0x00400000,

    ContentIsItem = 0x01000000,
    ContentIsElement = 0x02000000,
    TemplateIsCurrent = 0x04000000,
    HasLocalStyle = 0x08000000,
};

enum ViewFlags : suic::Uint32
{
    IsSubtreeDirtyForPrecompute = 0x00000010,
    IsLayoutIslandRootVF = 0x00000020,
    IsLayoutSuspendedVF = 0x00000040,
    IsObjectSealedVF = 0x00000080,

    IsClipToBoundsVF = 0x00000100,
    OperationPendingVF = 0x00000200,
    IsShouldHideColumn = 0x00000400,
    IsNotCalcMaxWidth = 0x00000800,

    VisibilityCache_TakesSpace = 0x00001000,
    VisibilityCache_Visible = 0x00002000,
    IsNotInVisibileRange = 0x00008000,

    InheritanceContextCached = 0x00010000,

    IsGroupItemVF = 0x00100000,
    IsContainerItemVF = 0x00200000,
};

enum HoriAlign : suic::Byte
{
    haLeft = 0,
    haRight,
    haCenter,
    haStretch,
};

enum VertAlign : int
{
    vaTop = 0,
    vaBottom,
    vaCenter,
    vaStretch,
};

enum TileMode : suic::Byte
{
    tmNone = 0,
    tmFlipX = 1,
    tmFlipY = 2,
    tmFlipXY = 3,
    tmTile = 4,
};

enum AlignmentX : suic::Byte
{
    axLeft = 0,
    axCenter = 1,
    axRight = 2,
};

enum AlignmentY : suic::Byte
{
    ayTop = 0,
    ayCenter = 1,
    ayBottom = 2,
};

enum BrushMappingMode : suic::Byte
{
    bmmAbsolute,
    bmmRelativeToBoundingBox,
};

enum Stretch : suic::Byte
{
    stNone = 0,
    stFill = 1,
    stUniform = 2,
    stUniformToFill = 3,
};

enum StretchDirection : suic::Byte
{
    sdUpOnly = 0,
    sdDownOnly,
    sdBoth
};

enum ResizeMode
{
    NoResize,
    CanMinimize,
    CanResize,
    CanResizeWithGrip
};

enum WindowState
{
    wsNormal,
    wsMinimized,
    wsMaximized,
};

enum WindowStyle
{
    wsNone,
    wsSingleBorderWindow,
    wsThreeDBorderWindow,
    wsToolWindow,
};

}

#ifdef __NOBMPDEF

typedef struct tagBITMAPINFOHEADER{
    Uint32      biSize;
    Int32       biWidth;
    Int32       biHeight;
    Uint16      biPlanes;
    Uint16      biBitCount;
    Uint32      biCompression;
    Uint32      biSizeImage;
    Int32       biXPelsPerMeter;
    Int32       biYPelsPerMeter;
    Uint32      biClrUsed;
    Uint32      biClrImportant;
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
    Byte    rgbBlue;
    Byte    rgbGreen;
    Byte    rgbRed;
    Byte    rgbReserved;
}RGBQUAD;

typedef struct tagDIBInfo
{
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
}DIBInfo;

#else

typedef BITMAPINFO DIBInfo;

#endif

#endif

