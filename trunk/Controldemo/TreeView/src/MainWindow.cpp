/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

MainWindow::MainWindow()
{
}

MainWindow::~MainWindow()
{
}

bool MainWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void MainWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void MainWindow::OnInitialized(suic::EventArg* e)
{
    suic::Window::OnInitialized(e);
}

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    //
    // 窗口居中
    //
    CenterWindow();

    //
    // 注册按钮事件处理
    //
    RegisterButtonEvent();
}

void MainWindow::OnButtonClicked(suic::Element* sender, suic::RoutedEventArg* e)
{
}

void MainWindow::RegisterButtonEvent()
{
    suic::TreeView* treeView = FindElem<suic::TreeView>("bindTree");
    if (NULL != treeView)
    {
        treeView->AddSelectedItemChanged(new suic::RoutedPropChangedEventHandler(this, &MainWindow::OnTreeItemSelectedChanged));
  
        TreeGroup* treeGrp = new TreeGroup();
        TreeGroup* gjGroup = new TreeGroup();

        treeView->AddChild(treeGrp);
        treeGrp->SetName("亚洲");
        
        gjGroup->SetName("中国");
        gjGroup->AddItem(new TreeGuoJia("北京"));
        gjGroup->AddItem(new TreeGuoJia("上海"));
        gjGroup->AddItem(new TreeGuoJia("哈尔滨"));
        gjGroup->AddItem(new TreeGuoJia("广东深圳"));
        gjGroup->AddItem(new TreeGuoJia("乌鲁木齐"));

        treeView->SetItemIsExpanded(gjGroup, true);
        treeGrp->AddItem(gjGroup);

        gjGroup = new TreeGroup();
        gjGroup->SetName("韩国");
        gjGroup->AddItem(new TreeGuoJia("首尔"));

        treeGrp->AddItem(gjGroup);
        treeGrp->AddItem(new TreeGuoJia("朝鲜"));

        treeGrp = new TreeGroup();
        treeGrp->SetName("欧洲");

        gjGroup = new TreeGroup();
        gjGroup->SetName("德国");
        gjGroup->AddItem(new TreeGuoJia("柏林"));
        gjGroup->AddItem(new TreeGuoJia("柏林"));
        gjGroup->AddItem(new TreeGuoJia("柏林"));
        gjGroup->AddItem(new TreeGuoJia("柏林"));
        gjGroup->AddItem(new TreeGuoJia("柏林"));
        gjGroup->AddItem(new TreeGuoJia("柏林"));
        gjGroup->AddItem(new TreeGuoJia("柏林"));
        gjGroup->AddItem(new TreeGuoJia("柏林"));
        treeGrp->AddItem(gjGroup);

        gjGroup = new TreeGroup();
        gjGroup->SetName("英国");
        treeGrp->AddItem(gjGroup);

        treeView->AddChild(treeGrp);

        treeGrp = new TreeGroup();
        treeGrp->SetName("美洲");
        treeView->AddChild(treeGrp);
    }
}

void MainWindow::OnTreeItemSelectedChanged(suic::Element* sender, suic::RoutedPropChangedEventArg* e)
{
    suic::Object* unselectedItem = e->GetOldValue();
    suic::Object* selectedItem = e->GetNewValue();

    if (NULL != unselectedItem)
    {
        ;
    }

    // 当前选中项
    if (selectedItem != NULL)
    {
    }
}
