// 华勤科技版权所有 2010-2011
// 
// 文件名：VSWindow.h
// 功  能：VS工程向导界面窗口。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _VSWINDOW_H_
# define _VSWINDOW_H_

#include <uidcore/uidframe.h>

using namespace suic;

class VSWindow : public suic::Window
{
public:

    BeginRoutedEvent(VSWindow, suic::Window)
        MemberRouted(OnButtonClick)
        MemberRouted(OnVSButtonClick)
        MemberRouted(OnOpenDirButtonClick)
    EndRoutedEvent()

    VSWindow();
    ~VSWindow();

    void SetVSVersion(const suic::String& strVer);
    void InitVSType(const suic::String strVer);

    static void StartVSCreator(CreateVSInfo& info);

protected:

    void OnButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnVSButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnOpenDirButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    CreateVSInfo _createVsInfo;
    suic::String _checkedVs;
};

#endif

