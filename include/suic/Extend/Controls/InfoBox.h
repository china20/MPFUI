// 华勤科技版权所有 2010-2011
// 
// 文件名：InfoBox.h
// 功  能：消息控件。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UIINFOBOX_H_
#define _UIINFOBOX_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/Window.h>
#include <System/Windows/AssignerTimer.h>

namespace suic
{

class SUIWGX_API InfoBox : public Window
{
public:

    enum Result
    {
        None = 0,
        OK = 1,
        Cancel = 2,
        Yes = 6,
        No = 7,
    };

    enum ButtonType
    {
        IB_OK = 0,
        IB_OKCancel = 1,
        IB_YesNoCancel = 2,
        IB_YesNo = 3,
    };

    enum InfoBoxOptions
    {
        Error = 0x10,
        Stop = 0x20,
        Question = 0x40,
        Warning = 0x80,
        Information = 0x100,

        Def_Button1 = 0,
        Def_Button2 = 0x1000,
        Def_Button3 = 0x2000,
    };

    static DpProperty* ButtonTypeProperty;

    static bool StaticInit();

    ~InfoBox();

    RTTIOfAbstractClass(InfoBox)

    static InfoBox::Result Show(const String& info, const String& caption);
    static InfoBox::Result Show(const String& info, const String& caption, ButtonType bt);
    static InfoBox::Result Show(const String& info, const String& caption, ButtonType bt, Uint32 options);

    static InfoBox::Result Show(Window* main, const String& info, const String& caption);
    static InfoBox::Result Show(Window* main, const String& info, const String& caption, ButtonType bt);
    static InfoBox::Result Show(Window* main, const String& info, const String& caption, ButtonType bt, Uint32 options);

    static InfoBox::Result Show(Object* info, const String& caption);
    static InfoBox::Result Show(Object* info, const String& caption, ButtonType bt);
    static InfoBox::Result Show(Object* info, const String& caption, ButtonType bt, Uint32 options);

    static InfoBox::Result Show(Window* main, Object* info, const String& caption);
    static InfoBox::Result Show(Window* main, Object* info, const String& caption, ButtonType bt);
    static InfoBox::Result Show(Window* main, Object* info, const String& caption, ButtonType bt, Uint32 options);

public:

    void OnLoaded(LoadedEventArg* e);
    void OnButtonClick(Element* sender, RoutedEventArg* e);

private:

    InfoBox();

    InfoBox* Clone();
    String ToString();

    void InitializeButtons(ButtonType bt);
    void SetButttonVisibilty(Element* elem, bool val);

    static InfoBox::Result ShowInfo(Window* main, Object* info, const String& caption, ButtonType bt, Uint32 options);

private:

    Uint32 _options;
};

class SUIWGX_API Toast : public Window
{
public:

    enum DelayClose
    {
        LongTime = -1,
        ShortTime = -2,
    };

    static void Show(suic::Object* info, suic::Uint32 iDelayClose);
    static void Show(const suic::String& info, suic::Uint32 iDelayClose);

    RTTIOfAbstractClass(Toast)

    Toast();
    ~Toast();

protected:

    void OnLoaded(LoadedEventArg* e);
    void OnTimer(Object* sender, EventArg* e);
    void CloseCompleted(Object* sender, EventArg* e);

private:

    void Remove();

private:

    suic::AssignerTimer* _timer;

    static Array<Toast*> _showToasts;
};

}

#endif
