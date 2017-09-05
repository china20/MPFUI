/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

MainWindow::MainWindow()
{
    _playManager = NULL;
    _layBottom = NULL;
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

    // 
    // 窗口居中显示
    //
    CenterWindow();
    _layBottom = FindName("layBottom");
    UpdateLayBottomPos();

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
    e->SetHandled(true);
}

void MainWindow::OnCheckMouseMove(suic::Object* sender, suic::EventArg* e)
{
    // 
    // 关闭定时器
    // 
    _timer->Stop();
    if (AllowsFullScreen() && NULL != _layBottom)
    {
        _layBottom->SetVisibility(suic::Visibility::Collapsed);
    }
}

void MainWindow::UpdateLayBottomPos()
{
    if (NULL != _layBottom)
    {
        _layBotPos = suic::Rect(_layBottom->GetCanvasOffset(), _layBottom->GetRenderSize());
    }
}

void MainWindow::OnRenderSizeChanged(suic::SizeChangedInfo& sizeInfo)
{
    suic::Window::OnRenderSizeChanged(sizeInfo);

    UpdateLayBottomPos();
}

void MainWindow::OnPreviewMouseMove(suic::MouseButtonEventArg* e)
{
    suic::Window::OnPreviewMouseMove(e);

    if (_layBotPos.PointIn(e->GetMousePoint()))
    {
        _timer->Stop();
    }

    if (_lastMousePt != e->GetMousePoint())
    {
        if (NULL != _layBottom)
        {
            _layBottom->SetVisibility(suic::Visibility::Visible);
        }

        if (_layBotPos.PointIn(e->GetMousePoint()))
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

void MainWindow::PlayCallback(bool start)
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
}

void MainWindow::OnClickOpenButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    suic::FileBrowser fb;
    if (NULL != _playManager && fb.Open())
    {
        suic::String strPath = fb.GetFilePath();

        _playManager->PlayVideo(strPath);

        SetTitle(strPath);

        FindName("btnPause")->SetVisibility(suic::Visibility::Visible);
        FindName("btnPlay")->SetVisibility(suic::Visibility::Hidden);
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
    suic::FrameworkElement* pFull = suic::DynamicCast<suic::FrameworkElement>(sender);
    e->SetHandled(true);
    SetAllowsFullScreen(!AllowsFullScreen());

    if (AllowsFullScreen())
    {
        pFull->SetToolTip("退出全屏");
        FindName("layCaption")->SetVisibility(suic::Visibility::Collapsed);
    }
    else
    {
        _timer->Stop();
        pFull->SetToolTip("全屏显示");
        FindName("layCaption")->SetVisibility(suic::Visibility::Visible);
        if (NULL != _layBottom)
        {
            _layBottom->SetVisibility(suic::Visibility::Visible);
        }
    }
}

