/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"
#include <ShellAPI.h>

MainWindow::MainWindow()
{
    _isPendingHome = false;
    _isPendingCtrl = false;
    _currentMode = ViewMode::vmHome;
    _timer = new AssignerTimer();
    _timer->ref();
}

MainWindow::~MainWindow()
{
    _timer->unref();
}

GroupData* MainWindow::NewGroupData(suic::String name)
{
    GroupData* group = new GroupData(name);
    group->GetItems()->AddItem(new suic::OString(_U("Sub Group 1")));
    group->GetItems()->AddItem(new suic::OString(_U("Sub Group 2")));
    group->GetItems()->AddItem(new suic::OString(_U("Sub Group 3")));
    group->GetItems()->AddItem(new suic::OString(_U("Sub Group 4")));
    group->GetItems()->AddItem(new suic::OString(_U("Sub Group 5")));
    return group;
}

void MainWindow::InitGroupListData(suic::ListBox* pListBox)
{
    if (NULL != pListBox)
    {
        GroupData* group = NewGroupData(_U("一级分组 1"));
        GroupData* subGroup = new GroupData(_U("二级分组 1-1"));

        subGroup->GetItems()->AddItem(new suic::OString(_U("Sub Group 1-1")));
        subGroup->GetItems()->AddItem(new suic::OString(_U("Sub Group 1-2")));
        subGroup->GetItems()->AddItem(new suic::OString(_U("Sub Group 1-3")));

        group->GetItems()->AddItem(subGroup);

        pListBox->AddChild(new OString(_U("分组演示")));
        pListBox->AddChild(group);
        pListBox->AddChild(NewGroupData(_U("一级分组 2")));
        pListBox->AddChild(NewGroupData(_U("一级分组 3")));
    }

}

void MainWindow::SwitchToControl(String name)
{
    Panel* fe = FindElem<Panel>(_U("CtrlArea"));
    FrameworkElement* elem = NULL;
    _ctrls.TryGetValue(name, elem);

    if (elem == NULL && fe)
    {
        String strPath;
        strPath.Format(_U("resource/MPF/Controls/%s.xml"), name.c_str());
        ObjectPtr obj = NULL;
        
        obj = XamlReader::LoadUri(NULL, NULL, strPath.c_str());
        elem = RTTICast<FrameworkElement>(obj.get());

        if (NULL != elem)
        {
            _ctrls.Add(name, elem);
        }

        ListView* listView = RTTICast<ListView>(elem);
        if (listView)
        {
            for (int i = 0; i < 50000; ++i)
            {
                NameField* pName = new NameField(i + 1, _U("等雨停了"));
                pName->SetSexy(_U("男"));
                listView->AddChild(pName);
            }
        }

        if (name.Equals(_U("ListBox")))
        {
            elem->ApplyTemplate();
            suic::TabControl* tabCtrl = elem->FindElem<suic::TabControl>(_U("TabList"));
            if (NULL != tabCtrl && tabCtrl->GetCount() >= 2)
            {
                suic::TabItem* tabItem = RTTICast<suic::TabItem>(tabCtrl->GetItem(1));
                suic::ListBox* pListBox = RTTICast<suic::ListBox>(tabItem->GetContent());
                InitGroupListData(pListBox);
            }
        }
    }

    if (NULL != elem)
    {
        fe->GetChildren()->Clear();
        fe->AddChild(elem);
        elem->UpdateLayout();

        Button* pBtn = elem->FindElem<Button>(_U("SetPosBtn"));
        if (pBtn != NULL)
        {
            pBtn->AddClick(new ClickEventHandler(this, &MainWindow::OnSetPosBtnClicked));
        }
        else
        {
            RepeatButton* pRepeatBtn = elem->FindElem<RepeatButton>(_U("RepeatBtn"));
            if (pRepeatBtn)
            {
                pRepeatBtn->AddClick(new ClickEventHandler(this, &MainWindow::OnSetPosBtnClicked));
            }
        }
    }

    UpdateLayout();
}

void MainWindow::OnCtrlAreaStart(Object* sender, EventArg* e)
{
    Element* fe = FindName(_U("CtrlArea"));
    fe->SetVisibility(Visibility::Visible);
}

void MainWindow::OnLeftAniEnd(Object* sender, EventArg* e)
{
    Element* feLeft = FindName(_U("DemonPanel"));
    feLeft->SetVisibility(Visibility::Collapsed);
}

void MainWindow::OnTimer(Object* sender, EventArg* e)
{
    ListBox* listObj = FindElem<ListBox>(_U("CtrlPanel"));
    ElementPtr listItem = DynamicCast<Element>(listObj->GetSelectedItem());
    if (listItem.get() != DpProperty::UnsetValue())
    {
        listObj->ScrollIntoView(listItem.get());
    }
    _timer->SetEnabled(false);
}

void MainWindow::OnRightAniEnd(Object* sender, EventArg* e)
{
    if (_currentMode == ViewMode::vmControl)
    {
        /*_timer->SetInterval(1000);
        _timer->SetTick(EventHandler(this, &MainWindow::OnTimer));
        _timer->Start();*/

        // 
        // 找到控件演示区域的面板控件
        //
        Element* fe = FindName(_U("CtrlArea"));
        Int32Animation* xAni = new Int32Animation(120, 0, Duration(120), FillBehavior::fbStop);
        DoubleAnimation* opaAni = new DoubleAnimation(0.2f, 1, Duration(120), FillBehavior::fbStop);
        TransformGroup* transGrp = new TransformGroup();
        TranslateTransform* trans = new TranslateTransform();

        //
        // 把偏移转换增加到TransformGroup
        //
        transGrp->AddChild(trans);
        // 
        // 前面40%匀速，后面60%减速前进
        //
        opaAni->SetDecelerationRatio(0.6f);

        //
        // 增加回调侦听动画状态变化（比如从活动变为停止）
        //
        opaAni->StateInvalidated += EventHandler(this, &MainWindow::OnCtrlAreaStart);

        //
        // 开始动画，把动画应用到控件元素上
        //
        fe->BeginAnimation(OpacityProperty, opaAni);

        //
        // 增加控件元素的渲染变换对象
        // 控件元素渲染完成时会使用此变换对象进行转换然后再显示
        //
        fe->SetRenderTransform(transGrp);
        trans->BeginAnimation(TranslateTransform::YProperty, xAni);
    }

    UpdateLayout();
}

void MainWindow::OnControlMouseEnter(Element* sender, MouseButtonEventArg* e)
{
    ListBoxItem* pItem = DynamicCast<ListBoxItem>(sender);
    if (NULL != pItem)
    {
        //DoubleAnimation* pAni = new DoubleAnimation(0.3, 1, Duration(300), FillBehavior::fbStop);
        //pAni->SetAutoReverse(true);
        //pItem->BeginAnimation(OpacityProperty, pAni);
        //pItem->GetAssigner()->GetTimeManager()->Tick();
    }
}

void MainWindow::OnControlSwitchComplete(Object* sender, EventArg* e)
{
    Clock* clock = DynamicCast<Clock>(sender);
    if (NULL == clock || clock->GetCurrentState() == Clock::ClockState::Active)
    {
        return;
    }
    
    ListBox* listObj = FindElem<ListBox>(_U("CtrlPanel"));
    Object* itemHost = listObj->GetSelectedItem();
    ElementPtr listItem = itemHost;
    if (NULL == listItem.get() || listItem.get() == DpProperty::UnsetValue())
    {
        return;
    }
    String ctrlName = listItem->GetName();

    //
    // 切换指定的演示控件页面
    //
    SwitchToControl(ctrlName);
    FrameworkElement* elem = NULL;
    _ctrls.TryGetValue(ctrlName, elem);

    if (NULL != elem)
    {
        DoubleAnimation* opaAni = new DoubleAnimation(0.1f, 1, Duration(200), FillBehavior::fbStop);
        elem->BeginAnimation(OpacityProperty, opaAni);

        /*ScaleTransform* trans = new ScaleTransform(1, 1, 0.5, 0.5);
        DoubleAnimation* scaleAni = new DoubleAnimation(0, 1, Duration(200), FillBehavior::fbHoldEnd);

        elem->SetRenderTransform(trans);
        trans->BeginAnimation(ScaleTransform::ScaleXProperty, scaleAni);
        trans->BeginAnimation(ScaleTransform::ScaleYProperty, new DoubleAnimation(0, 1, Duration(200), FillBehavior::fbHoldEnd));*/

        //
        // 调用此句代码是立即同步动画状态（这样可以根据动画当前时间改变控件的属性值）
        //
        GetAssigner()->GetTimeManager()->Tick();
    }
}

void MainWindow::OnAppSelected(Element* sender, MouseButtonEventArg* e)
{
    //
    // 获取MPF独立的演示程序列表对象（比如QQDemon、SuiDemon等）
    //
    ListBox* listApp = FindElem<ListBox>(_U("AppList"));
    if (listApp)
    {
        //
        // 获取当前选中的应用条目（这里ListBox是单选）
        //
        ElementPtr item = listApp->GetSelectedItem();
        if (item.get())
        {
            String strPath;
            strPath.Format(_U("%s.exe"), item->GetName().c_str());
            strPath = FileDir::CalculatePath(strPath);
            ShellExecuteW(NULL, _U("open"), strPath.c_str(), NULL, NULL, SW_SHOW);
        }
        e->SetHandled(true);
    }
}

void MainWindow::OnControlSelected(Element* sender, SelectionChangedEventArg* e)
{
    if (_isPendingHome && _currentMode == ViewMode::vmHome)
    {
        return;
    }

    ElementPtr item;

    //
    // 显示模式处于主页模式
    //
    if (_currentMode == ViewMode::vmHome)
    {
        _currentMode = ViewMode::vmControl;
        if (e->GetAddedItems()->GetCount() == 1)
        {
            item = e->GetAddedItems()->GetItem(0);
            LeftPanelAnimate(false);
            RightPanelAnimate(false);
            SwitchToControl(item->GetName());
            UpdateLayout();
        }
    }
    else
    {
        //
        // 如果点击了主页按钮，则需要切换状态到vmHome
        // 其他情况是点击了控件列表项（这里仅仅切换需要显示的控件）
        //
        if (_isPendingHome)
        {
            _currentMode = ViewMode::vmHome;
        }
        if (e->GetRemovedItems()->GetCount() == 1)
        {
            ElementPtr oitem = e->GetRemovedItems()->GetItem(0);
            FrameworkElement* elem = NULL;
            _ctrls.TryGetValue(oitem->GetName(), elem);

            if (NULL != elem)
            {
                //ScaleTransform* trans = new ScaleTransform(1, 1, 0.5, 0.5);
                DoubleAnimation* opaAni = new DoubleAnimation(1, 0, Duration(200), FillBehavior::fbStop);
                opaAni->Completed += EventHandler(this, &MainWindow::OnControlSwitchComplete);

                //elem->SetRenderTransform(trans);
                //elem->BeginAnimation(OpacityProperty, opaAni);
                //trans->BeginAnimation(ScaleTransform::ScaleXProperty, opaAni);
                //trans->BeginAnimation(ScaleTransform::ScaleYProperty, new DoubleAnimation(1, 0, Duration(200), FillBehavior::fbStop));
                elem->BeginAnimation(OpacityProperty, opaAni);
            }
            else if (e->GetAddedItems()->GetCount() == 1)
            {
                item = e->GetAddedItems()->GetItem(0);
                SwitchToControl(item->GetName());
            }
            UpdateLayout();
        }
    }

    e->SetHandled(true);
}

void MainWindow::OnButtonClicked(Element* sender, RoutedEventArg* e)
{
    ElementPtr elem = e->GetOriginalSource();
    if (elem)
    {
        if (_currentMode == ViewMode::vmHome && elem->GetName().Equals(_U("CtrlBtn")))
        {
            _isPendingCtrl = true;
            ListBox* listObj = FindElem<ListBox>(_U("CtrlPanel"));
            listObj->SetSelectedItem(listObj->GetItem(0));
            _isPendingCtrl = false;
        }
        else if (_currentMode == ViewMode::vmControl && elem->GetName().Equals(_U("HomeBtn")))
        {
            _isPendingHome = true;
            ListBox* listObj = FindElem<ListBox>(_U("CtrlPanel"));

            //
            // 回到主页页面需要取消控件选择项（否点击同一个控件时不能切换到控件演示页面）
            // 这里会触发OnControlSelected回调，状态由此函数改变
            //
            listObj->UnselectAll();

            LeftPanelAnimate(true);
            RightPanelAnimate(true);

            Element* fe = FindName(_U("CtrlArea"));
            //
            // 隐藏控件演示区域
            //
            fe->SetVisibility(Visibility::Collapsed);
            _isPendingHome = false;
        }

        UpdateLayout();
        e->SetHandled(true);
    }
}

void MainWindow::OnSetPosBtnClicked(Element* sender, RoutedEventArg* e)
{
    ElementPtr elem = e->GetOriginalSource();
    if (elem->GetName().Equals(_U("RepeatBtn")))
    {
        suic::TextBlock* pText = FindElem<suic::TextBlock>(_U("RepeatText"));
        if (pText)
        {
            static int num = 0;
            String str;
            str.Format(_U("%d"), ++num);
            pText->SetText(str);
        }
        return;
    }

    ProgressBar* pBar = FindElem<ProgressBar>(_U("ProgressBar03"));
    if (pBar)
    {
        Int32Animation* barAni = new Int32Animation(25, 60, Duration(200), FillBehavior::fbStop);
        pBar->BeginAnimation(ProgressBar::ValueProperty, barAni);
        UpdateLayout();
    }
}

bool MainWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void MainWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void MainWindow::OnInitialized(EventArg* e)
{
    Window::OnInitialized(e);
}

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    Window::OnLoaded(e);

    // 窗口居中显示
    CenterWindow();

    AddHandler(Button::ClickEvent, new ClickEventHandler(this, &MainWindow::OnButtonClicked), true);

    ListBox* listObj = FindElem<ListBox>(_U("CtrlPanel"));
    if (listObj)
    {
        listObj->AddSelectionChanged(new SelectionChangedEventHandler(this, &MainWindow::OnControlSelected));
    }

    ListBox* listApp = FindElem<ListBox>(_U("AppList"));
    if (listApp)
    {
        //listApp->AddMouseLeftButtonUp(new MouseEventHandler(this, &MainWindow::OnAppSelected), true);
    }
}

void MainWindow::LeftPanelAnimate(bool bHome)
{
    Element* feLeft = FindName(_U("DemonPanel"));
    Int32Animation* widAni = NULL;
    DoubleAnimation* opaAni = NULL;

    if (bHome)
    {
        widAni = new Int32Animation(0, 183, Duration(160), FillBehavior::fbHoldEnd);
    }
    else
    {
        widAni = new Int32Animation(183, 0, Duration(160), FillBehavior::fbHoldEnd);
    }

    //
    // 增加宽度变化动画
    //
    feLeft->BeginAnimation(WidthProperty, widAni);
}

void MainWindow::RightPanelAnimate(bool bHome)
{
    Element* feRight = FindName(_U("CtrlPanel"));
    Int32Animation* widAni = NULL;
    
    if (bHome)
    {
        widAni = new Int32Animation(150, 610, Duration(100), FillBehavior::fbHoldEnd);
    }
    else
    {
        widAni = new Int32Animation(610, 150, Duration(100), FillBehavior::fbHoldEnd);
    }

    //
    // 控件列表动画结束时，我们需要启动控件演示区域的显示动画
    // 因此，这里增加动画完成的回调
    //
    widAni->Completed += EventHandler(this, &MainWindow::OnRightAniEnd);

    widAni->SetAccelerationRatio(0.3f);
    feRight->BeginAnimation(WidthProperty, widAni);
}
