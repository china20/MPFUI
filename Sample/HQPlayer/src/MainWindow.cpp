/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

MainWindow::MainWindow()
{
    _playManager = NULL;
}

MainWindow::~MainWindow()
{
    if (NULL != _playManager)
    {
        _playManager->unref();
    }
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
        layPlayer->AddChild(playView);
        _playManager = new PlayManager();
        _playManager->Init(playView, this);
    }

    playView->unref();

    // 
    // ´°¿Ú¾ÓÖÐÏÔÊ¾
    //
    CenterWindow();
}


void MainWindow::OnClickOpenButton(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    suic::FileBrowser fb;
    if (NULL != _playManager && fb.Open())
    {
        _playManager->PlayVideo(fb.GetFilePath());

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
        FindName("btnPause")->SetVisibility(suic::Visibility::Visible);
    }
    else
    {
        FindName("btnPlay")->SetVisibility(suic::Visibility::Visible);
    }
}