/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

#include <Framework/Controls/DatePicker.h>
#include <Framework/Controls/GridView.h>
#include <Framework/Controls/GridViewColumnHeader.h>

ImplementRTTIOfClass(NameField, suic::NotifyPropChanged)

MainWindow::MainWindow()
{
    _timer = new suic::AssignerTimer();
    _timer->ref();
}

MainWindow::~MainWindow()
{
    _timer->ref();
}

bool MainWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void MainWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void MainWindow::OnTimer(suic::Object* sender, suic::EventArg* e)
{
    static int count = 1;
    int iCheckCount = 8;
    suic::ListView* listView = FindElem<suic::ListView>("ListView");

    if (listView != NULL && listView->GetItemsSource()->GetCount() >= iCheckCount)
    {
        for (int i = 2; i < iCheckCount; ++i)
        {
            NameField* field = suic::DynamicCast<NameField>(listView->GetItem(i));
            if (NULL != field)
            {
                field->SetSexy(suic::String().Format(_U("name_%d"), count++));
            }
        }
    }
}

void MainWindow::OnInitialized(suic::EventArg* e)
{
    suic::Window::OnInitialized(e);
    _timer->SetInterval(1000);
    _timer->SetTick(suic::EventHandler(this, &MainWindow::OnTimer));
    _timer->Start();
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

void MainWindow::OnBtnClicked(suic::Element* sender, suic::RoutedEventArg* e)
{
    suic::Object* pItem = NULL;
    suic::Element* item = suic::DynamicCast<suic::Element>(e->GetOriginalSource());
    suic::GridViewColumnHeader* colHeader = suic::DynamicCast<suic::GridViewColumnHeader>(item);
    if (NULL != colHeader)
    {
        suic::ListView* pListView = suic::DynamicCast<suic::ListView>(sender);
        suic::GridView* pView = suic::DynamicCast<suic::GridView>(pListView->GetView());
        suic::GridViewColumnCollection* pColl = pView->GetColumns();
        suic::GridViewColumn* pColumn = colHeader->GetColumn();
        suic::String strInfo;

        strInfo.Format(_U("点击第 %d 列；Header：%s"), pColl->IndexOf(pColumn), pColumn->GetHeader()->ToString().c_str());

        suic::InfoBox::Show(strInfo, "提示", suic::InfoBox::ButtonType::IB_OK);
        return;
    }

    /*if (item)
    {
        pItem = item->GetDataContext();
    }
    if (pItem != NULL)
    {
        suic::InfoBox::Show(pItem->ToString(), "单击Button提示", suic::InfoBox::ButtonType::IB_OK);
    }
    else
    {
        suic::InfoBox::Show("点击单元格的Button控件!", "提示", suic::InfoBox::ButtonType::IB_OK);
    }*/
}

void MainWindow::OnItemDbClck(suic::Element* sender, suic::MouseButtonEventArg* e)
{
    suic::Object* pItem = NULL;
    suic::Element* item = suic::DynamicCast<suic::Element>(e->GetOriginalSource());
    if (item)
    {
        pItem = item->GetDataContext();
    }
    if (pItem != NULL)
    {
        suic::InfoBox::Show(pItem->ToString(), "双击提示", suic::InfoBox::ButtonType::IB_OK);
    }
    else
    {
        suic::InfoBox::Show("点击单元格的Button控件!", "双击提示", suic::InfoBox::ButtonType::IB_OK);
    }
    e->SetHandled(true);
}

void MainWindow::OnDeleteListButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    suic::ListView* listView = FindElem<suic::ListView>("ListView");

    e->SetHandled(true);

    if (listView && listView->GetItemsSource()->GetCount() > 1)
    {
        listView->GetItemsSource()->RemoveItemAt(listView->GetItemsSource()->GetCount() - 1);
    }
}

void MainWindow::OnResetListButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    suic::ListView* listView = FindElem<suic::ListView>("ListView");

    e->SetHandled(true);

    if (listView)
    {
        NameField* pName = new NameField(listView->GetItemsSource()->GetCount() + 1, _U("等雨停了"));
        pName->SetSexy(_U("男"));
        listView->AddChild(pName);
        InsertUserItem(listView);
    }
}

void MainWindow::OnClipListView(suic::Element* sender, suic::RoutedEventArg* e)
{
    suic::ListView* listView = FindElem<suic::ListView>("ListView");

    e->SetHandled(true);

    if (listView)
    {
        suic::TextEditor* centerX = FindElem<suic::TextEditor>("CenterX");
        suic::TextEditor* centerY = FindElem<suic::TextEditor>("CenterY");
        suic::TextEditor* radiusX = FindElem<suic::TextEditor>("RadiusX");
        suic::TextEditor* radiusY = FindElem<suic::TextEditor>("RadiusY");

        suic::fPoint fCenter;
        suic::Float fRadiusX;
        suic::Float fRadiusY;

        fCenter.x = centerX->GetText().ToFloat();
        fCenter.y = centerY->GetText().ToFloat();
        fRadiusX = radiusX->GetText().ToFloat();
        fRadiusY = radiusY->GetText().ToFloat();

        if (fRadiusX >= 0 && fRadiusY >= 0)
        {
            listView->SetClip(new suic::EllipseGeometry(fCenter, fRadiusX, fRadiusY));
        }
        else
        {
            listView->SetClip(NULL);
        }
    }
}

void MainWindow::OnItemSelectionChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    static int num = 0;
    suic::Debug::Trace(_U("MainWindow::OnItemSelectionChanged -> %d"), ++num);
}

void MainWindow::InsertUserItem(suic::ListView* listView)
{
    suic::ListViewItem* testItem = new suic::ListViewItem();
    listView->AddChild(testItem);
    suic::Style* pSty = suic::RTTICast<suic::Style>(FindRes(_U("TestViewStyle")));

    testItem->SetDataContext(new NameField(-1, _U("等雨停了888")));
    testItem->SetStyle(pSty);
}

void MainWindow::RegisterButtonEvent()
{
    suic::ListView* listView = FindElem<suic::ListView>("ListView");

    if (listView)
    {
        listView->AddHandler(suic::Button::ClickEvent, new suic::ClickEventHandler(this, &MainWindow::OnBtnClicked));
        listView->AddMouseDoubleClick(new suic::MouseButtonEventHandler(this, &MainWindow::OnItemDbClck));

        InsertUserItem(listView);

        for (int i = 0; i < 0; ++i)
        {
            NameField* pName = new NameField(i + 1, _U("等雨停了"));
            pName->SetSexy(_U("男"));
            listView->AddChild(pName);
        }

        listView->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &MainWindow::OnItemSelectionChanged));
    }

    suic::Button* pBtn = FindElem<suic::Button>("ClipList");
    if (NULL != pBtn)
    {
        pBtn->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnClipListView));
    }
}
