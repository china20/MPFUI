// 华勤科技版权所有 2008-2022
// 
// 文件名：Window.h
// 功  能：实现标准的窗口样式。
// 
// 作  者：MPF开发组
// 时  间：2010-12-12
// 
// ============================================================

#ifndef _UIWINDOW_H_
#define _UIWINDOW_H_

#include <Framework/Controls/Content.h>

namespace suic
{

enum SizeToContent
{
    stcManual,
    stcWidth,
    stcHeight,
    stcWidthAndHeight
};

class SUICORE_API ResizeGrip : public FrameworkElement
{
public:

    RTTIOfClass(ResizeGrip)

public:

    ResizeGrip();
    ~ResizeGrip();

protected:

    Size OnMeasure(const Size& availableSize);
    void OnHitTest(HitResultEventArg* e);
    void OnMouseMove(MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);
};

class Window;

class WindowInvoker : public AssignerInvoker
{
public:

    WindowInvoker(Window* sender, int type);
    ~WindowInvoker();

    static int Show(Window* wnd, Element* owner);
    static int ShowDialog(Window* wnd, Handle owner, int timeout);

    static int Show(Window* wnd, const String& uri, Element* owner);
    static int ShowDialog(Window* wnd, const String& uri, int timeout);

    static void CloseWindow(Window* wnd, bool bAsync);

private:

    void Invoke();

    static int ShowUri(Window* wnd, const String& uri, int type, Handle h);
    static int ShowOnThread(Window* wnd, Handle owner, const String& uri, int type, int timeout);

private:

    int _type;
    int _result;
    String _uri;
    Handle _holder;
    Window* _target;
};

class SUICORE_API Window : public ContentControl
{
public:

    enum WndFlag
    {
        wfDisposed = 0x01,
        wfFullScreen = 0x02,
        wfIsActivate = 0x04,
        wfShowingAsDialog = 0x08,
        wfShowingAsInfo = 0x10,
        wfAppShuttingDown = 0x20,
        wfIsClosing = 0x40,
        wfIsPending = 0x80,
    };

    EventHandler Closed;
    EventHandler Closing;

    EventHandler StateChanged;
    EventHandler LocationChanged;

    static DpProperty* IsActiveProperty;
    static DpProperty* ShowActivatedProperty;

    // Window的图标
    static DpProperty* IconProperty;
    // Window的标题
    static DpProperty* TitleProperty;
    // Window是否在最顶上
    static DpProperty* TopmostProperty;
    // Window的左上角坐标
    static DpProperty* LocationProperty;
    // Window的圆角大小
    static DpProperty* RoundSizeProperty;
    // Window是否可以移动
    static DpProperty* CanMovingProperty;
    // Window的拉伸模式
    static DpProperty* ResizeModeProperty;
    // Window的状态（正常、最小化、最大化）
    static DpProperty* WindowStateProperty;
    // Window的样式
    static DpProperty* WindowStyleProperty;
    // Window的标题栏高度
    static DpProperty* CaptionHeightProperty;
    // 是否显示在任务栏
    static DpProperty* ShowInTaskbarProperty;
    // Window司否拉伸到其内容大小
    static DpProperty* SizeToContentProperty;
    // 是否运行窗口半透明
    static DpProperty* AllowsTransparencyProperty;

    static void StaticInit();

    static void OnAllowDropPropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnIconPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnWidthPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnHeightPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMinWidthPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMinHeightPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMaxWidthPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMaxHeightPropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnWindowStatePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnWindowStylePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnVisibilityPropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void OnLocationPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnTopMostPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnShowInTaskbarPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnTitlePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnResizeModePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnSizeToContentPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnAllowsTransparencyPropChanged(DpObject* d, DpPropChangedEventArg* e);

    RTTIOfClass(Window)

    Window();
    virtual ~Window();

    void Dispose();
    bool IsDisposed();

    void SetPos(int x, int y);
    void SetMove(int x, int y, int cx, int cy);

    Rect GetWndRect();

    void Hide();
    void Close();
    void AsyncClose();

    void CenterWindow(Handle hActive = 0);
    bool IsTopMost();
    void SetTopMost(bool topmost);

    bool IsClosing();
    bool IsClosed();
    bool IsValid();

    void SetCloseTime(int elapse);

    Size GetSizeFrame();
    void SetSizeFrame(Size rect);

    Point GetLocation();
    void SetLocation(Point val);

    WindowStyle GetWindowStyle();
    void SetWindowStyle(WindowStyle val);

    WindowState GetWindowState();
    void SetWindowState(WindowState val);

    Handle GetOwnerHandle();
    void SetOwnerHandle(Handle val);

    bool AllowsTransparency();
    void SetAllowsTransparency(bool val);

    int GetSizeToContent();
    void SetSizeToContent(int val);

    Size UpdateSizeOnContent();

    ImageSource* GetIcon();
    void SetIcon(ImageSource* val);

    String GetTitle();
    void SetTitle(String val);

    int GetCaptionHeight();
    void SetCaptionHeight(int val);

    ResizeMode GetResizeMode();
    void SetResizeMode(ResizeMode val);

    void SetPopupCapture();
    void ReleasePopupCapture();

    FrameworkElement* GetOwner();
    void SetOwner(FrameworkElement* owner);

    AssignerFrame* GetAssignerFrame();

    void InvalidateForce();

    void Activate();

    //--- 下面的显示函数是线程安全的

    int Show();
    int Show(Element* owner);
    int ShowDialog();

    int Show(const String& uri, Element* owner);
    int ShowDialog(const String& uri);

    Handle ShowChild(Handle parent);
    Handle ShowChild(const String& uri, Handle parent);

    //--- 线程安全函数结束

    bool IsCanMoving();
    void SetCanMoving(bool val);

    bool IsShowInTaskBar();
    void ShowInTaskBar(bool val);
    
    Int32 GetDialogResult();
    void SetDialogResult(Int32 val);

    bool StartDragMove();
    void DragMove();
    void EndDragMove();

    void LoadWindow(const String& uri, ObjectPtr& obj);

public:

    void UpdateWindowSize();
    bool AllowsFullScreen();
    void SetAllowsFullScreen(bool val);

    bool OnFilterMessage(Object* sender, MessageParam* mp, bool& interrupt);

    virtual void OnActivated(EventArg* e);
    virtual void OnDeactivated(EventArg* e);

    virtual bool OnEvent(Object* sender, MessageParam* mp);
    virtual bool OnBuild(IXamlNode* pNode, ObjectPtr& obj);
    virtual void OnConnect(IXamlNode* pNode, Object* target);

protected:

    virtual void OnClosed(EventArg* e);
    virtual void OnClosing(CancelEventArg* e);

    virtual bool OnShowingWindow();
    virtual void OnShowWindow();
    virtual void OnHideWindow();

    virtual void OnPreviewGotKeyboardFocus(KeyboardFocusEventArg* e);

    virtual void OnAllowsTransparencyChanged(bool oldVal, bool newVal);
    virtual void OnIconChanged(ImageSource* oldVal, ImageSource* newVal);
    virtual void OnWidthChanged(int oldVal, int newVal);
    virtual void OnHeightChanged(int oldVal, int newVal);
    virtual void OnMinWidthChanged(int oldVal, int newVal);
    virtual void OnMinHeightChanged(int oldVal, int newVal);
    virtual void OnMaxWidthChanged(int oldVal, int newVal);
    virtual void OnMaxHeightChanged(int oldVal, int newVal);
    virtual void OnWindowStateChanged(Uint32 oldVal, Uint32 newVal);
    virtual void OnAllowDropChanged(bool oldVal, bool newVal);
    virtual void OnVisibilityChanged(Visibility oldVal, Visibility newVal);
    virtual void OnWindowStyleChanged(Uint32 oldVal, Uint32 newVal);
    virtual void OnLocationChanged(Point oldVal, Point newVal);
    virtual void OnTopMostChanged(bool oldVal, bool newVal);
    virtual void OnShowInTaskbarChanged(bool oldVal, bool newVal);
    virtual void OnTitleChanged(String oldTitle, String newTitle);
    virtual void OnResizeModeChanged(ResizeMode oldVal, ResizeMode newVal);
    virtual void OnSizeToContentChanged(int oldVal, int newVal);

protected:
    
    void UpdateIcon();
    void Resize(int cx, int cy);
    void CreateWindowStyle(Uint32& iStyle, Uint32& iStyleEx);
    bool HitTestCore(HitTestCtx* hitResult, bool onlySelf);

    void OnLoaded(LoadedEventArg* e);
    void OnArrange(const Size& arrangeBounds);
    Size OnMeasure(const Size& availableSize);

    void UpdateMeasureSize(const Size& availableSize, Size& measureSize);

    void OnRenderSizeChanged(SizeChangedInfo& e);
    void OnRenderSizeModeChanged(SizeModeChangedEventArg* e);

    void OnHitTest(HitResultEventArg* e);
    void OnKeyDown(KeyboardEventArg* e);

    void OnMouseMove(MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);

protected:

    void SetOwnerParent();
    void InternalClose(bool async);

    bool CreateWindowDuringShow(suic::Handle hParent);

    bool ContainFlag(Uint32 key);
    void SetWndFlag(Uint32 key, bool bAdd);

    int ShowInternal(Handle owner, Handle parent);
    int ShowDialogInternal(Handle owner);

protected:

    Rect _startpos;
    Rect _prevPos;
    Point _startdragpos;
    Size _sizeFrame;
    Point _location;

    int _closeTimes;
    int _closeTimerId;

    int _sizeToContent;

    Uint32 _windowFlag;
    Int32 _dialogResult;

    Handle _bigIcon;
    Handle _smallIcon;
    Handle _ownerHandle;

    WeakRef _owner;
    AssignerFrame* _frame;
    
    Handle _dialogPreviousActiveHandle;

    friend class WindowInvoker;
};

typedef shared<Window> WindowPtr;

}

#endif
