// 华勤科技版权所有 2010-2011
// 
// 文件名：ExpressionWindow.h
// 功  能：表达式界面窗口。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _EXPRESSIONWINDOW_H_
# define _EXPRESSIONWINDOW_H_

using namespace suic;

class ExpressionWindow : public suic::Window
{
public:

    BeginRoutedEvent(ExpressionWindow, suic::Window)
        MemberRouted(OnOkButtonClick)
    EndRoutedEvent()

    ExpressionWindow();
    ExpressionWindow(bool fromTemp);
    ~ExpressionWindow();

    String GetValue() const;
    void SetPreviousValue(const String& val);

    void OnOkButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

protected:

    bool _fromTemplate;
    suic::String _value;
};

#endif

