/////////////////////////////////////////////////////
// PlayManager.cpp

#include "stdafx.h"
#include "PlayManager.h"

PlayManager::PlayManager()
{
    _vReaderThr = NULL;
    _reflesh = NULL;
    _playIndex = -1;
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

void PlayManager::Init(PlayerView* pView, suic::Element* pRoot, PlayVideoCb cb)
{
    _rootView = pRoot;
    _playCb = cb;

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
    int64_t duration = d / 1000;

    secs = duration;
    us = duration % 1000;
    mins = secs / 60;
    secs %= 60;
    hours = mins/ 60;
    mins %= 60;

    return suic::String().Format(_U("%02d:%02d:%02d"), hours, mins, secs);
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
    if (NULL != pPB)
    {
        double dCur = bmp->curDuration / 1.0f;
        double dAll = bmp->duration / 1.0f;
        double dRatio = (double)dCur / (double)dAll;

        pPB->SetValue(dRatio * 100);
    }
}

void PlayManager::OnInvoker(suic::Object* sender, suic::InvokerArg* e)
{
    BmpInfo* bmp = (BmpInfo*)e->GetData();
    
    if (NULL != bmp)
    {
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
    else
    {
        _playCb(e->GetWhat() == 1);
        if (0 == e->GetWhat())
        {
            suic::ProgressBar* pPB = _rootView->FindElem<suic::ProgressBar>("playPB");
            if (NULL != pPB)
            {
                pPB->SetValue(100.0f);
            }
        }
    }
}

int PlayManager::CheckPlayFile(suic::String filename)
{
    for (int i = 0; i < _playLists.Length(); ++i)
    {
        suic::String strName = _playLists.GetItem(i);
        if (strName.CompareI(filename) == 0)
        {
            return i;
        }
    }

    return -1;
}

void PlayManager::PlayVideo(suic::String filename)
{
    int index = CheckPlayFile(filename);

    if (index == -1)
    {
        _playIndex = _playLists.Length();
        _playLists.Add(filename);
    }
    else
    {
        _playIndex = index;
    }

    PlayCurrentVideo();
}

void PlayManager::PlayCurrentVideo()
{
    suic::String filename;
    
    if (_playIndex >= 0)
    {
        filename = _playLists.GetItem(_playIndex);

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
}

bool PlayManager::IsPlaying() const
{
    if (!_vReaderThr || !_vReaderThr->IsPlaying())
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool PlayManager::IsPause() const
{
    if (!_vReaderThr || !_vReaderThr->IsPause())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void PlayManager::PauseVideo(bool bPause)
{
    if (NULL != _vReaderThr)
    {
        _vReaderThr->PausePlay(bPause);
    }
}

void PlayManager::StopVideo()
{
    if (NULL != _vReaderThr)
    {
        _vReaderThr->StopPlay();
        _vReaderThr->Join();
        _vReaderThr->unref();
        _vReaderThr = NULL;
    }
}
