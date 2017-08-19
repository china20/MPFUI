// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Button.h
// ��  �ܣ���ť����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIBUTTON_H_
#define _UIBUTTON_H_

#include <Framework/Controls/ButtonBase.h>

namespace suic
{

/// <summary>
/// Button��
/// </summary>
class SUICORE_API Button : public ButtonBase
{
public:

    static DpProperty* IsCancelProperty;
    static DpProperty* IsDefaultedProperty;
    static DpProperty* IsDefaultProperty;

    static bool StaticInit();

    static void OnIsDefaultedPropChanged(DpObject* d, DpPropChangedEventArg* e);

    Button();
    virtual ~Button();

    RTTIOfClass(Button)

protected:

    void OnDataContextChanged(DpPropChangedEventArg* e);
    void OnFocusChanged(Element* sender, KeyboardEventArg* e);
    
    void OnClick();
};

typedef shared<Button> ButtonPtr;

class SUICORE_API SysButton : public ButtonBase
{
public:

    static bool StaticInit();

    SysButton();
    ~SysButton();

    RTTIOfClass(SysButton)
};

/// <summary>
/// ϵͳ��С����ť��ʵ�ֵ�����С������
/// </summary>
class SUICORE_API MinimizeButton : public SysButton
{
public:

    MinimizeButton();
    virtual ~MinimizeButton();

    RTTIOfClass(MinimizeButton)

    void OnClick();
};

/// <summary>
/// ϵͳ��󻯺ͻ�ԭ��ť�������ǿ���󻯻�ԭ����
/// </summary>
class SUICORE_API MaximizeButton : public SysButton
{
public:

    static DpProperty* WindowStateProperty;
    static bool StaticInit();

    MaximizeButton();
    ~MaximizeButton();

    RTTIOfClass(MaximizeButton)

protected:

    void OnClick();
    void OnInitialized(EventArg* e);
};

/// <summary>
/// ϵͳ�رհ�ť��ʵ�ֵ����رմ���
/// </summary>
class SUICORE_API CloseButton : public SysButton
{
public:

    CloseButton();
    virtual ~CloseButton();

    RTTIOfClass(CloseButton)

    void OnClick();
};

}

#endif
