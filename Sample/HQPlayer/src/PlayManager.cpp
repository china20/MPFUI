/////////////////////////////////////////////////////
// PlayManager.cpp

#include "stdafx.h"
#include "PlayManager.h"

PlayManager::PlayManager()
{
    _vReaderThr = NULL;
    _reflesh = NULL;
}

PlayManager::~PlayManager()
{
    if (NULL != _reflesh)
    {
        _reflesh->unref();
    }

    if (NULL != _vReaderThr)
    {
        _vReaderThr->unref();
    }
}

void PlayManager::Init(PlayerView* pView, suic::Element* pRoot)
{
    _rootView = pRoot;

    if (NULL == _reflesh)
    {
        av_register_all();
        SDL_Delay(1);

        _playView = pView;
        _reflesh = new suic::InvokeProxy(pView);
        _reflesh->ref();
        _reflesh->Invoker += suic::InvokerHandler(this, &PlayManager::OnInvoker);
    }
}

suic::String PlayManager::FormatTime(int64_t d)
{
    int hours = 0;
    int mins = 0;
    int secs = 0;
    int us = 0;
    int64_t duration = d;

    secs = duration / 1000;
    us = duration % 1000;
    mins = secs / 60;
    secs %= 60;
    hours = mins/ 60;
    mins %= 60;

    return suic::String().Format(_U("%02d:%02d:%02d.%03d"), hours, mins, secs, us);
}

void PlayManager::UpdatePlayDate(BmpInfo* bmp)
{
    suic::TextBlock* pTxt = NULL;

    pTxt = _rootView->FindElem<suic::TextBlock>("curDuration");
    if (NULL != pTxt)
    {
        pTxt->SetText(FormatTime(bmp->curDuration));
    }

    pTxt = _rootView->FindElem<suic::TextBlock>("duration");
    if (NULL != pTxt)
    {
        pTxt->SetText(FormatTime(bmp->duration));
    }

    suic::ProgressBar* pPB = _rootView->FindElem<suic::ProgressBar>("playPB");
    if (NULL != pPB )
    {
        double dCur = (double)bmp->curDuration;
        double dAll = (double)bmp->duration;
        double dRatio = dCur / dAll;

        pPB->SetValue(dRatio * 100);
    }
}

void PlayManager::OnInvoker(suic::Object* sender, suic::InvokerArg* e)
{
    BmpInfo* bmp = (BmpInfo*)e->GetData();
    
    UpdatePlayDate(bmp);

    if (bmp->bmp.IsValid())
    {
        _playView->PostRender(bmp);
    }
    else
    {
        bmp->unref();
    }
}

void PlayManager::PlayVideo(suic::String filename)
{
    if (NULL != _reflesh)
    {
        if (_vReaderThr != NULL)
        {
            _vReaderThr->StopPlay();
            _vReaderThr->Join();
            _vReaderThr->unref();
        }

        _vReaderThr = new VideoReaderThr(filename, _reflesh);
        _vReaderThr->ref();
        _vReaderThr->Start();
    }    
}
