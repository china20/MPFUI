// 华勤科技版权所有 2010-2011
// 
// 文件名：CreateStyleWindow.h
// 功  能：编辑样式或控件模版选择界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _CREATESTYLEWINDOW_H_
# define _CREATESTYLEWINDOW_H_

#include <main/CommonInterface.h>

using namespace suic;

class CreateStyleWindow : public suic::Window
{
public:

    BeginRoutedEvent(CreateStyleWindow, suic::Window)
        MemberRouted(OnOkButtonClick)
        MemberRouted(OnCancelButtonClick)
    EndRoutedEvent()

    CreateStyleWindow(bool fromStyle);
    CreateStyleWindow(bool fromStyle, const suic::String& strTitle);
    ~CreateStyleWindow();

    bool IsFromBlank() const
    {
        return _fromBlank;
    }

protected:

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnOkButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnCancelButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);

private:

    bool _fromStyle;
    bool _fromBlank;

    suic::String _title;
    suic::RadioButton* _rBlank;
    suic::RadioButton* _rBackup;
};

#endif
