// 华勤科技版权所有 2010-2011
// 
// 文件名：ActiveXControl.h
// 功  能：实现ActiveXHost控件的封装
// 
// 作  者：汪荣
// 时  间：2010-08-31
// 
// ============================================================================

# ifndef _UIACTIVEX_H_
# define _UIACTIVEX_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/HwndHost.h>

namespace suic
{

/// <summary>
///  ActiveXHost，封装ActiveXHost控件。
/// </summary>
class SUIWGX_API ActiveXHost : public HwndHost
{
public:

    ActiveXHost();
    virtual ~ActiveXHost();

    RTTIOfClass(ActiveXHost)

    Handle GetControlHandle() const;

protected:

    bool DoVerb(int verb);

protected:

    virtual void OnInitialized();

    virtual void OnRender(DrawingContext * drawing);

    virtual void OnTextInput(KeyboardEventArg* e);
    virtual void OnKeyDown(KeyboardEventArg* e);

    virtual void OnGotFocus(FocusEventArg& e);
    virtual void OnLostFocus(FocusEventArg& e);
    virtual void OnSetCursor(CursorEventArg* e);

    virtual void OnMouseEnter(MouseEventArg& e);
    virtual void OnMouseMove(MouseEventArg& e);
    virtual void OnMouseLeave(MouseEventArg& e);
    virtual void OnMouseLeftButtonDown(MouseEventArg& e);
    virtual void OnMouseLeftButtonDbclk(MouseEventArg& e);
    virtual void OnMouseLeftButtonUp(MouseEventArg& e);

protected:

    // 组件class id
    //CLSID _clsid;

    Handle _hwndParent;
    Handle _axWindow;
    Rect _boundRect;

    // 控件对象
    /*IOleObject* _axOleObject;
    IOleInPlaceObject* _axOleInPlaceObject;
    IOleInPlaceActiveObject* _axOleInPlaceActiveObject;*/
};

typedef shared<ActiveXHost> ActiveXPtr;

};

# endif
