/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

ImplementRTTIOfClass(TLData, suic::NotifyPropChanged)

MainWindow::MainWindow()
    : _openWnd(NULL)
{
}

MainWindow::~MainWindow()
{
    if (NULL != _openWnd)
    {
        _openWnd->unref();
    }
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
    CenterWindow(0);

    //
    // 注册按钮事件处理
    //
    RegisterButtonEvent();
}

void MainWindow::OnButtonClicked(suic::Element* sender, suic::RoutedEventArg* e)
{
}

void MainWindow::AddDataToListView(suic::ListView* pList, TLData* pData)
{
    pList->AddChild(pData);
    for (int i = 0; i < pData->GetCount(); ++i)
    {
        AddDataToListView(pList, pData->GetItem(i));
    }
}

void MainWindow::RegisterButtonEvent()
{
    suic::ListView* pListView = FindElem<suic::ListView>("lvTree");
    if (NULL != pListView)
    {
        OranizeData* pData = new OranizeData("总部");
        DepData* pDep = new DepData("研发中心");

        pDep->AddItem(new PersonData("李辉", "深圳南山"));
        pDep->AddItem(new PersonData("郭强", "福田"));

        pData->AddItem(pDep);

        pDep = new DepData("人事部");
        pDep->AddItem(new PersonData("小芳", "西乡"));
        pData->AddItem(pDep);

        AddDataToListView(pListView, pData);
    }
}
