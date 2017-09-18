/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

ImplementRTTIOfClass(ZipListView, suic::ListView)

ImplementRTTIOfClass(ZipInfo, suic::NotifyPropChanged)
ImplementRTTIOfClass(ZipEntry, suic::NotifyPropChanged)

MainWindow::MainWindow()
{
    _timer = new suic::AssignerTimer();
    _timer->ref();

    _zipColl = new ZipFileColl();
    _zipColl->ref();

    _zipInfo = new ZipInfo();
    _zipInfo->ref();
}

MainWindow::~MainWindow()
{
    _timer->unref();
    _zipColl->unref();
    _zipInfo->unref();
}

void ZipListView::RenderChildren(suic::Drawing* drawing)
{
    suic::ListView::RenderChildren(drawing);
    suic::GridView* pView = suic::DynamicCast<suic::GridView>(GetView());
    suic::ScrollViewer* pScrollView = GetScrollHost();

    if (NULL != pView && NULL != pScrollView)
    {
        int iWid = pScrollView->GetHorizontalOffset() * -1;
        int iMaxWid = pScrollView->GetViewportWidth();
        int iMaxHei = pScrollView->GetViewportHeight();

        suic::GridViewColumnCollection* pColl = pView->GetColumns();
        for (int i = 0; i < pColl->GetCount(); ++i)
        {
            suic::GridViewColumn* pColumn = pColl->GetItem(i);
            iWid += pColumn->GetWidth();
            suic::fRect rect(iWid - 1, 25, 1, iMaxHei);

            if (rect.left > iMaxWid)
            {
                break;
            }
            drawing->FillRect(suic::DrawCtx::DefDraw, 0xFFD5E3EF, rect);
        }
    }
}

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::VirtualizingWrapPanel* pPanel = NULL;

    suic::Window::OnLoaded(e);

    CenterWindow();

    _zipInfo->GetDriverGrp()->InitRootItems();

    SetDataContext(_zipInfo);

    //FindElem<suic::TreeView>("tvDrivers")->SetItemsSource(_zipInfo->GetDriverGrp());

    _timer->SetTick(suic::EventHandler(this, &MainWindow::OnRefleshTimer));
    _timer->SetInterval(1000);
    //_timer->Start();

    _zipListView = FindElem<ZipListView>("lvZip");
    //_zipListView->SetItemsSource(_zipColl);

    _zipListBox = FindElem<suic::ListBox>("lbElem");
    _zipListBox->SetItemsSource(_zipColl);

    pPanel = suic::DynamicCast<suic::VirtualizingWrapPanel>(_zipListBox->GetItemsHost());

    pPanel->SetItemWidth(120);

    // 测试数据
    AddTestData();

    // 
    // 挂接菜单点击处理
    // 
    _mainMenu = FindElem<suic::Menu>("mainMenu");
    _mainMenu->AddHandler(suic::MenuItem::ClickEvent, new suic::ClickEventHandler(this, &MainWindow::OnMainMenuClick));

    _zipListView->AddHandler(suic::Button::ClickEvent, new suic::ClickEventHandler(this, &MainWindow::OnListViewColomnClicked));
    _zipListView->AddMouseDoubleClick(new suic::MouseButtonEventHandler(this, &MainWindow::OnListViewItemDbClck));
}

void MainWindow::UnzipFile(suic::String strPath)
{
    suic::WindowPtr pWnd = suic::XamlReader::LoadUri(NULL, new UnzipWindow(), "lpZip/Layout/UnzipWindow.xml").get();

    if (NULL != pWnd)
    {
        pWnd->ShowDialog();
    }
}

void MainWindow::HideListViewSortIcon()
{
    suic::GridViewHeaderRowPresenter* pColls = NULL;

    // 
    // 查找ListView的标题头控件（模板中对应的Name="lvHeader"）
    // 用来查找实际显示的列头信息
    // 
    pColls = suic::DynamicCast<suic::GridViewHeaderRowPresenter>(suic::VisualTreeOp::FindName(_zipListView, "lvHeader"));

    if (NULL != pColls)
    {
        for (int i = 0; i < pColls->GetVisualChildrenCount(); ++i)
        {
            suic::Element* pChild = pColls->GetVisualChild(i);
            suic::Element* sortIcon = suic::VisualTreeOp::FindName(pChild, "imgIcon");
            if (sortIcon != NULL)
            {
                sortIcon->SetVisibility(suic::Visibility::Collapsed);
            }
        }
    }
}

void MainWindow::SortListViewColumn(suic::GridViewColumnHeader* colHeader)
{
    suic::GridViewColumn* pColumn = colHeader->GetColumn();
    suic::Rectangle* sortIcon = NULL;

    static int desc = -1;
    static CompareInfo ci;

    // 
    // 此排序的比较函数请自行根据业务更改
    // desc小于0降序
    // 
    ZipEntryCompare zipCompare;

    // 
    // 这里一定要在可视树上查找（模板里元素用逻辑树找不到）
    // 
    sortIcon = suic::DynamicCast<suic::Rectangle>(suic::VisualTreeOp::FindName(colHeader, "imgIcon"));

    if (NULL != sortIcon)
    {
        // 
        // 隐藏其它列的排序图标显示
        // 
        HideListViewSortIcon();


        ci.iDesc = desc;
        ci.iColumn = pColumn->GetColumnIndex();
        ci.name = pColumn->GetHeader()->ToString();

        _zipColl->Sort<ZipEntryCompare>(&zipCompare, &ci);

        sortIcon->SetVisibility(suic::Visibility::Visible);

        if (desc < 1)
        {
            sortIcon->SetFill(suic::DynamicCast<suic::Brush>(FindRes("DownArrow")));
        }
        else
        {
            sortIcon->SetFill(suic::DynamicCast<suic::Brush>(FindRes("UpArrow")));
        }
    }

    desc = desc * -1;

    _zipListView->UpdateLayout();
}

void MainWindow::OnListViewColomnClicked(suic::Element* sender, suic::RoutedEventArg* e)
{
    suic::Element* tvItem = suic::DynamicCast<suic::Element>(e->GetOriginalSource());
    suic::GridViewColumnHeader* colHeader = suic::DynamicCast<suic::GridViewColumnHeader>(tvItem);

    // 
    // 点击了列表头，可以做排序之类的处理
    // 
    if (NULL != colHeader && _zipListView == sender)
    {
        SortListViewColumn(colHeader);

        e->SetHandled(true);
    }
}

void MainWindow::OnListViewItemDbClck(suic::Element* sender, suic::MouseButtonEventArg* e)
{
    ZipEntry* pItem = NULL;
    suic::Element* tvItem = suic::DynamicCast<suic::Element>(e->GetOriginalSource());

    if (NULL != tvItem)
    {
        pItem = suic::DynamicCast<ZipEntry>(tvItem->GetDataContext());
    }

    // 
    // 双击了我们的列表，进行业务逻辑处理
    // 
    if (pItem != NULL)
    {
        e->SetHandled(true);
    }
}

void MainWindow::OnMainMenuClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    suic::MenuItem* pItem = NULL;
    e->SetHandled(true);

    // 
    // 在这里根据资源定义的MenuItem的Name分发菜单的事件处理
    // 
    pItem = suic::DynamicCast<suic::MenuItem>(e->GetOriginalSource());
    if (NULL != pItem)
    {
        suic::String strName(pItem->GetName());
        if (strName.Equals("miUnzip"))
        {
            UnzipFile(""); 
        }
        else if (strName.Equals("miExit"))
        {
            AsyncClose();
        }
        else
        {
            suic::InfoBox::Show(pItem->GetName(), "ClickMenu");
        }
    }
}

void MainWindow::AddTestData()
{
    ZipEntry* pItem = NULL;

    suic::Object* pIcon = FindRes("TitleIcon");

    for (int i = 0; i < 10000; ++i)
    {
        pItem = new ZipEntry();
        pItem->SetName(suic::String().Format(_U("test_%d.cpp"), i + 1));
        pItem->SetIcon(pIcon);
        pItem->SetZipType(suic::String().Format(_U("%d"), i + 1));

        _zipColl->AddItem(pItem);
    }
}

bool MainWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void MainWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void MainWindow::OnRefleshTimer(suic::Object* sender, suic::EventArg* e)
{

}

void MainWindow::OnClickAddToButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    suic::WindowPtr pWnd = suic::XamlReader::LoadUri(NULL, new ZipWindow(), "lpZip/Layout/ZipWindow.xml").get();

    if (NULL != pWnd)
    {
        pWnd->ShowDialog();
    }

    
    
}

void MainWindow::OnClickUnzipButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    UnzipFile("");
}

void MainWindow::OnClickOnOneUnzipButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
}

void MainWindow::OnClickDeleteButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
}

void MainWindow::OnClickPwdButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    suic::WindowPtr pWnd = suic::XamlReader::LoadUri(NULL, new ZipSetWindow(), "lpZip/Layout/SettingWindow.xml").get();

    if (NULL != pWnd)
    {
        pWnd->ShowDialog();
    }
}

void MainWindow::OnClickUpLevelButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::WindowPtr pWnd = suic::XamlReader::LoadUri(NULL, new UpdateAppWindow(), "lpZip/Layout/UpdateWindow.xml").get();

    if (NULL != pWnd)
    {
        pWnd->ShowDialog();
    }
}

void MainWindow::OnClickIconModeButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    if (_zipListView->GetVisibility() == suic::Visibility::Visible)
    {
        _zipListView->SetVisibility(suic::Visibility::Collapsed);
        _zipListBox->SetVisibility(suic::Visibility::Visible);
        _zipListView->SetItemsSource(NULL);
        _zipListBox->SetItemsSource(_zipColl);
    }
}

void MainWindow::OnClickListModeButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    if (_zipListView->GetVisibility() != suic::Visibility::Visible)
    {
        _zipListView->SetVisibility(suic::Visibility::Visible);
        _zipListBox->SetVisibility(suic::Visibility::Collapsed);
        _zipListView->SetItemsSource(_zipColl);
        _zipListBox->SetItemsSource(NULL);
    }
}
