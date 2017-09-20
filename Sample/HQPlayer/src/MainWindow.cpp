/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

MainWindow::MainWindow()
{
    _playManager = NULL;
    _layBottom = NULL;
    _playListBox = NULL;
    _timer = new suic::AssignerTimer();
    _timer->ref();
}

MainWindow::~MainWindow()
{
    if (NULL != _playManager)
    {
        _playManager->unref();
    }

    _timer->unref();
}

void MainWindow::Dispose()
{
    suic::Window::Dispose();
}

bool MainWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void MainWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    PlayerView* playView = new PlayerView();
    suic::Panel* layPlayer = FindElem<suic::Panel>("layPlayer");

    playView->ref();

    if (NULL != layPlayer)
    {
        PlayVideoCb cb(this, &MainWindow::PlayCallback);
        layPlayer->AddChild(playView);
        _playManager = new PlayManager();
        _playManager->Init(playView, this, cb);
    }

    playView->unref();

    SetAllowDrop(true);

    // 
    // 窗口居中显示
    //
    CenterWindow();
    _layBottom = FindElem<suic::FrameworkElement>("layBottom");
    _playArea = FindElem<suic::FrameworkElement>("layPlayer");
    _layRight = FindElem<suic::FrameworkElement>("rightPanel");
    _layPlayer = FindElem<suic::FrameworkElement>("layPlayer");

    _layPlayer->SetMargin(suic::Rect(0, 0, 0, 52));

    // 
    // 注册音量按钮的回调
    // 
    suic::Slider* pSlider = FindElem<suic::Slider>("volumeSlider");
    if (NULL != pSlider)
    {
        pSlider->AddValueChanged(new suic::FloatPropChangedEventHandler(this, &MainWindow::OnVolumeChanged));
        pSlider->SetValue(100.0f);
    }

    suic::Slider* pPB = FindElem<suic::Slider>("playPB");
    if (NULL != pPB)
    {
        pPB->AddValueChanged(new suic::FloatPropChangedEventHandler(this, &MainWindow::OnPlayProgressChanged));
        pPB->SetValue(0);
    }

    _playListBox = FindElem<suic::ListBox>("playList");
    if (NULL != _playListBox)
    {
        _playListBox->SetItemsSource(_playManager->GetPlayList());
        //_playListBox->AddMouseDoubleClick(new suic::MouseButtonEventHandler(this, &MainWindow::OnDblPlayListClick));
        //_playListBox->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &MainWindow::OnPlayListSelectionChanged));
    }
}

void MainWindow::OnDblPlayListClick(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    //
    // 鼠标双击事件
    //
    suic::Element* pElem = suic::DynamicCast<suic::Element>(e->GetOriginalSource());
    VideoItem* pItem = suic::DynamicCast<VideoItem>(pElem->GetDataContext());

    if (NULL != pItem)
    {
        _playManager->PlayVideoItem(pItem);
    }

    e->SetHandled(true);
}

void MainWindow::OnPlayListSelectionChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    e->SetHandled(true);
    if (e->GetAddedItems()->GetCount() > 0)
    {
        VideoItem* pItem = NULL;
        
        pItem = suic::DynamicCast<VideoItem>(e->GetAddedItems()->GetItem(0));
        _playManager->PlayVideoItem(pItem);
    }
}

void MainWindow::OnKeyDown(suic::KeyboardEventArg* e)
{
    if (e->GetKey() == suic::Key::kSpace && NULL != _playManager)
    {
        _playManager->PauseVideo(!_playManager->IsPause());
    }
    else if (e->GetKey() == suic::Key::kEscape && AllowsFullScreen())
    {
        SetFullScreenMode(false);
    }
}

void MainWindow::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    suic::Window::OnMouseLeftButtonDown(e);

    suic::Rect playPos(_playArea->GetCanvasOffset(), _playArea->GetRenderSize());
    if (playPos.PointIn(e->GetMousePoint()) && NULL != _playManager)
    {
        Focus();
    }

    _prevMouseDown = e->GetMousePoint();
    e->SetHandled(true);
}

void MainWindow::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    suic::Window::OnMouseLeftButtonUp(e);

    if (_prevMouseDown == e->GetMousePoint())
    {
        suic::Rect playPos(_playArea->GetCanvasOffset(), _playArea->GetRenderSize());
        if (playPos.PointIn(e->GetMousePoint()) && NULL != _playManager)
        {
            _playManager->PauseVideo(!_playManager->IsPause());
        }
    }

    e->SetHandled(true);
}

void MainWindow::OnPlayProgressChanged(suic::Element*, suic::FloatPropChangedEventArg* e)
{
    if (NULL != _playManager)
    {
        _playManager->SetPlayProgress(e->GetNewValue() / 100.0f);
    }
    e->SetHandled(true);
}

void MainWindow::OnVolumeChanged(suic::Element*, suic::FloatPropChangedEventArg* e)
{
    if (NULL != _playManager)
    {
        _playManager->SetPlayVolume(e->GetNewValue());
    }
    UpdateVolumeStatus(true);
    e->SetHandled(true);
}

void MainWindow::OnCheckMouseMove(suic::Object* sender, suic::EventArg* e)
{
    // 
    // 关闭定时器
    // 
    _timer->Stop();

    // 
    // 处于全屏状态，开始隐藏底部状态栏
    // 
    if (AllowsFullScreen())
    {
        ShowBottomStatus(false);
    }
}

void MainWindow::OnPreviewDrop(suic::DragEventArg* e)
{
    suic::FileDragInfo* fi = NULL;
    suic::IDataStore* ds = suic::DynamicCast<suic::IDataStore>(e->GetData());
    if (NULL != ds && suic::eDropType::dtFiles == e->GetDropType())
    {
        e->SetHandled(true);
        fi = suic::DynamicCast<suic::FileDragInfo>(ds->GetData("Object"));
        if (NULL != fi && fi->GetCount() > 0)
        {
            PlayFile(fi->GetFilePath(0));

            for (int i = 1; i < fi->GetCount(); ++i)
            {
                _playManager->AddVideo(fi->GetFilePath(i));
            }
        }
    }
}

void MainWindow::ShowBottomStatus(bool bShow)
{
    suic::RectAnimation* mgrAni = NULL;
    suic::Rect fromMargin = _layBottom->GetMargin();
    suic::Rect toMargin = fromMargin;

    if (bShow)
    {
        toMargin.bottom = 0;
    }
    else
    {
        toMargin.bottom = _layBottom->GetHeight() * -1;
    }

    // 
    // 这里使用RectAnimation类型的动画，对suic::Rect这种类型的属性执行动画变换
    // 为了使得底部状态栏产生显示和隐藏的动画效果，对MarginProperty属性执行动画变换
    // 
    mgrAni = new suic::RectAnimation(fromMargin, toMargin, suic::Duration(300), suic::FillBehavior::fbHoldEnd);
    _layBottom->BeginAnimation(suic::FrameworkElement::MarginProperty, mgrAni);
}

void MainWindow::ShowRightPlayPanel(bool bShow)
{
    suic::RectAnimation* mgrAni = NULL;
    suic::Rect fromMargin = _layRight->GetMargin();
    suic::Rect toMargin = fromMargin;

    if (bShow)
    {
        toMargin.right = 0;
    }
    else
    {
        toMargin.right = _layRight->GetWidth() * -1;
    }
 
    mgrAni = new suic::RectAnimation(fromMargin, toMargin, suic::Duration(200), suic::FillBehavior::fbHoldEnd);
    _layRight->BeginAnimation(suic::FrameworkElement::MarginProperty, mgrAni);
}

void MainWindow::OnRenderSizeChanged(suic::SizeChangedInfo& sizeInfo)
{
    suic::Window::OnRenderSizeChanged(sizeInfo);
}

void MainWindow::OnPreviewMouseMove(suic::MouseButtonEventArg* e)
{
    suic::Window::OnPreviewMouseMove(e);

    suic::Rect layBotPos(_layBottom->GetCanvasOffset(), _layBottom->GetRenderSize());

    if (layBotPos.PointIn(e->GetMousePoint()))
    {
        _timer->Stop();
    }

    // 
    // 是否移动了鼠标，移动了需要显示状态栏
    // 同时启动一个定时监测鼠标超过一定时间未动时隐藏底部和上部的状态显示
    // 
    if (_lastMousePt != e->GetMousePoint())
    {
        if (NULL != _layBottom)
        {
            ShowBottomStatus(true);
        }

        if (layBotPos.PointIn(e->GetMousePoint()))
        {
            _timer->Stop();
        }
        else if (AllowsFullScreen())
        {
            _timer->Stop();
            _timer->SetInterval(2000);
            _timer->SetTick(suic::EventHandler(this, &MainWindow::OnCheckMouseMove));
            _timer->Start();
        }
    }

    _lastMousePt = e->GetMousePoint();
}

void MainWindow::PlayCallback(bool start, int index)
{
    FindName("btnPause")->Enable(true);
    FindName("btnPlay")->Enable(true);
    FindName("btnStop")->Enable(start);

    if (start)
    {
        FindName("btnPause")->SetVisibility(suic::Visibility::Visible);
        FindName("btnPlay")->SetVisibility(suic::Visibility::Hidden);
    }
    else
    {
        FindName("btnPause")->SetVisibility(suic::Visibility::Hidden);
        FindName("btnPlay")->SetVisibility(suic::Visibility::Visible);
    }

    _playListBox->SetSelectedIndex(index);
}

void MainWindow::PlayFile(suic::String strFile)
{
    _playManager->PlayVideo(strFile);

    SetTitle(strFile);

    FindName("btnPause")->SetVisibility(suic::Visibility::Visible);
    FindName("btnPlay")->SetVisibility(suic::Visibility::Hidden);
}

void MainWindow::OnClickOpenButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    suic::FileBrowser fb;
    if (NULL != _playManager && fb.Open())
    {
        suic::String strPath = fb.GetFilePath();

        PlayFile(strPath);
    }
}

void MainWindow::OnClickButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    suic::Element* pElem = suic::DynamicCast<suic::Element>(sender);
    suic::String name = pElem->GetName();

    pElem->SetVisibility(suic::Visibility::Hidden);
   
    if (name.Equals("btnPlay"))
    {
        // 
        // 播放已经暂停，重新播放当前视频
        // 
        if (!_playManager->IsPlaying())
        {
            _playManager->PlayCurrentVideo();
        }
        else
        {
            _playManager->PauseVideo(false);
        }

        FindName("btnPause")->SetVisibility(suic::Visibility::Visible);
    }
    else
    {
        _playManager->PauseVideo(true);
        FindName("btnPlay")->SetVisibility(suic::Visibility::Visible);
    }
}

void MainWindow::OnClickStopButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    _playManager->StopVideo();
}

void MainWindow::OnClickFullButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    SetFullScreenMode(!AllowsFullScreen());
}

void MainWindow::OnClickBrowserButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    
    suic::Rect rcMargin = _layRight->GetMargin();

    if (rcMargin.right == 0)
    {
        ShowRightPlayPanel(false);
    }
    else
    {
        ShowRightPlayPanel(true);
    }
}

void MainWindow::OnClickPrevButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    if (NULL != _playManager)
    {
        _playManager->PlayPrevVideo();
    }
}

void MainWindow::OnClickNextButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    if (NULL != _playManager)
    {
        _playManager->PlayNextVideo();
    }
}

void MainWindow::UpdateVolumeStatus(bool bOpen)
{
    if (!bOpen)
    {
        FindName("openVolume")->SetVisibility(suic::Visibility::Visible);
        FindName("closeVolume")->SetVisibility(suic::Visibility::Collapsed);
    }
    else
    {
        FindName("openVolume")->SetVisibility(suic::Visibility::Collapsed);
        FindName("closeVolume")->SetVisibility(suic::Visibility::Visible);
    }
}


void MainWindow::OnClickOpenVolume(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    
    UpdateVolumeStatus(true);

    suic::Slider* pSlider = FindElem<suic::Slider>("volumeSlider");
    if (NULL != pSlider)
    {
        if (NULL != _playManager)
        {
            _playManager->SetPlayVolume(pSlider->GetValue());
        }
    }
}

void MainWindow::OnClickCloseVolume(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    UpdateVolumeStatus(false);

    if (NULL != _playManager)
    {
        _playManager->SetPlayVolume(0);
    }
}

void MainWindow::SetFullScreenMode(bool bFull)
{
    suic::FrameworkElement* pFull = FindElem<suic::FrameworkElement>("btnFull");

    SetAllowsFullScreen(bFull);

    if (AllowsFullScreen())
    {
        pFull->SetToolTip("退出全屏");
        _layPlayer->SetMargin(suic::Rect());
        FindName("layCaption")->SetVisibility(suic::Visibility::Collapsed);
    }
    else
    {
        _timer->Stop();
        pFull->SetToolTip("全屏显示");
        _layPlayer->SetMargin(suic::Rect(0, 0, 0, 52));
        FindName("layCaption")->SetVisibility(suic::Visibility::Visible);
       
        if (NULL != _layBottom)
        {
            _layBottom->SetMargin(suic::Rect());
            _layBottom->SetVisibility(suic::Visibility::Visible);
        }
    }
}

