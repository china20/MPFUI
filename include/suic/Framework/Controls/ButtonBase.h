// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ButtonBase.h
// ��  �ܣ���ť����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIBUTTONBASE_H_
#define _UIBUTTONBASE_H_

#include <Framework/Controls/Content.h>

namespace suic
{

/// <summary>
/// ButtonBase��
/// </summary>
class SUICORE_API ButtonBase : public ContentControl
{
public:

    static RoutedEvent* ClickEvent;

    static DpProperty* IsPressedProperty;
    
    static bool StaticInit();

    ButtonBase();
    virtual ~ButtonBase();

    RTTIOfAbstractClass(ButtonBase);

    bool IsPressed();

    void AddClick(ClickEventHandler* handler);
    void RemoveClick(ClickEventHandler* handler);

    ICommandSource* GetCommandSource();

    void OnCheckTimer(Object* sender, EventArg* e);

public:

    static void OnIsPressedPropChanged(DpObject* d, DpPropChangedEventArg* e);

    virtual void OnIsPressedChanged(DpPropChangedEventArg* e);

protected:

    void OnKeyDown(KeyboardEventArg* e);
    void OnKeyUp(KeyboardEventArg* e);
    void OnTextInput(suic::KeyboardEventArg* e);

    void OnMouseEnter(MouseButtonEventArg* e);
    void OnMouseLeave(MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);

    void SetIsPressed(bool pressed);
    bool HandleIsMouseOverChanged();

    virtual void OnClick();

protected:

    int _animationType;
    AssignerTimer* _timer;
    ICommandSource* _commandSource;
};

inline bool ButtonBase::IsPressed()
{
    return GetValue(IsPressedProperty)->ToInt() != 0;
}

typedef shared<ButtonBase> ButtonBasePtr;

}

#endif
