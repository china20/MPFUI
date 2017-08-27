// 华勤科技版权所有 2010-2022
// 
// 文件名：SettingWindow.h
// 功  能：主界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _SETTINGWINDOW_H_
#define _SETTINGWINDOW_H_

#include "ChatWindow.h"
#include <Framework/Controls/Menu.h>

using namespace suic;

class SettingWindow : public OSWindow
{
public:

    SettingWindow();
    ~SettingWindow();

    void OnLoaded(suic::LoadedEventArg* e);

    void OnScrollViewChanged(suic::Element*, suic::ScrollChangedEventArg*);
    void OnLeftSwitchClick(suic::Element* sender, suic::RoutedEventArg* e);

protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    suic::Array<String, true> _labels;
};

#endif

