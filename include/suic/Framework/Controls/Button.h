// 华勤科技版权所有 2008-2022
// 
// 文件名：Button.h
// 功  能：按钮基类
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIBUTTON_H_
#define _UIBUTTON_H_

#include <Framework/Controls/ButtonBase.h>

namespace suic
{

/// <summary>
/// Button。
/// </summary>
class SUICORE_API Button : public ButtonBase
{
public:

    static DpProperty* IsCancelProperty;
    static DpProperty* IsDefaultedProperty;
    static DpProperty* IsDefaultProperty;

    static void StaticInit();

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

    static void StaticInit();

    SysButton();
    ~SysButton();

    RTTIOfClass(SysButton)
};

/// <summary>
/// 系统最小化按钮，实现单击最小化窗口
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
/// 系统最大化和还原按钮，单击是可最大化或还原窗口
/// </summary>
class SUICORE_API MaximizeButton : public SysButton
{
public:

    static DpProperty* WindowStateProperty;
    static void StaticInit();

    MaximizeButton();
    ~MaximizeButton();

    RTTIOfClass(MaximizeButton)

protected:

    void OnClick();
    void OnInitialized(EventArg* e);
};

/// <summary>
/// 系统关闭按钮，实现单击关闭窗口
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
