// 华勤科技版权所有 2010-2022
// 
// 文件名：MainWindow.h
// 功  能：主界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2017-09-02
// 
// ============================================================================

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "DriverItem.h"

class MainWindow : public suic::Window
{
public:

    BeginRoutedEvent(MainWindow, suic::Window)
        MemberRouted(OnClickPrevImage)
        MemberRouted(OnClickNextImage)
        MemberRouted(OnTreeViewSelectedItemChanged)
    EndRoutedEvent()

    MainWindow();
    ~MainWindow();

    void ShowImage(DriverItem* pItem);

    void OnLoaded(suic::LoadedEventArg* e);

protected:

    void OnClickPrevImage(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickNextImage(suic::DpObject* sender, suic::RoutedEventArg* e);

    void OnTreeViewSelectedItemChanged(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void OnRefleshTimer(suic::Object* sender, suic::EventArg* e);

private:

    DriverGroup* _driverGrp;
    suic::TreeView* _tvDrivers;

    // DriverLevel* _driverLevel;

    // 定时器，用来刷新进程列表
    suic::AssignerTimer* _timer;
};

#endif
