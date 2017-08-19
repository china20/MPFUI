// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ActiveXControl.h
// ��  �ܣ�ʵ��ActiveXHost�ؼ��ķ�װ
// 
// ��  �ߣ�����
// ʱ  �䣺2010-08-31
// 
// ============================================================================

# ifndef _UIACTIVEX_H_
# define _UIACTIVEX_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/HwndHost.h>

namespace suic
{

/// <summary>
///  ActiveXHost����װActiveXHost�ؼ���
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

    // ���class id
    //CLSID _clsid;

    Handle _hwndParent;
    Handle _axWindow;
    Rect _boundRect;

    // �ؼ�����
    /*IOleObject* _axOleObject;
    IOleInPlaceObject* _axOleInPlaceObject;
    IOleInPlaceActiveObject* _axOleInPlaceActiveObject;*/
};

typedef shared<ActiveXHost> ActiveXPtr;

};

# endif
