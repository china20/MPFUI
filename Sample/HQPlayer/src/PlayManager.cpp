/////////////////////////////////////////////////////
// PlayManager.cpp

#include "stdafx.h"
#include "PlayManager.h"

ImplementRTTIOfClass(VideoItem, suic::NotifyPropChanged)

PlayCollection::PlayCollection()
{
}

PlayManager::PlayManager()
{
    _vReaderThr = NULL;
    _reflesh = NULL;
    _playIndex = -1;
    _volume = 100;
    _fromPlay = false;
    _playList = new PlayCollection();
    _playList->ref();
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

    _playList->unref();
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

    suic::Slider* pPB = _rootView->FindElem<suic::Slider>("playPB");
    if (NULL != pPB)
    {
        double dCur = bmp->curDuration / 1.0f;
        double dAll = bmp->duration / 1.0f;
        double dRatio = (double)dCur / (double)dAll;

        _fromPlay = true;
        pPB->SetValue(dRatio * 100);
        _fromPlay = false;
    }
}

VideoItem* PlayManager::GetPlayItem()
{
    return _playList->GetVideoItem(_playIndex);
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
        _playCb(e->GetWhat() == 1, _playIndex);
        if (0 == e->GetWhat())
        {
            suic::Slider* pPB = _rootView->FindElem<suic::Slider>("playPB");
            if (NULL != pPB)
            {
                _fromPlay = true;
                pPB->SetValue(100.0f);
                _fromPlay = false;
            }
        }
    }
}

int PlayManager::CheckPlayFile(suic::String filename)
{
    for (int i = 0; i < _playList->GetCount(); ++i)
    {
        VideoItem* pItem = _playList->GetVideoItem(i);
        if (filename.CompareI(pItem->GetFilePath()) == 0)
        {
            return i;
        }
    }

    return -1;
}

void PlayManager::AddVideo(suic::String filename)
{
    if (-1 == CheckPlayFile(filename))
    {
        VideoItem* pItem = new VideoItem();
        _playList->AddItem(pItem);
        pItem->SetFilePath(filename);
    }
}

void PlayManager::PlayVideo(suic::String filename)
{
    int index = CheckPlayFile(filename);

    if (index == -1)
    {
        VideoItem* pItem = new VideoItem();

        _playList->AddItem(pItem);
        pItem->SetFilePath(filename);

        _playIndex = _playList->GetCount() - 1;
    }
    else
    {
        _playIndex = index;
    }

    PlayCurrentVideo();
}

void PlayManager::SetPlayVolume(int volume)
{
    _volume = volume;

    if (NULL != _vReaderThr)
    {
        _vReaderThr->SetPlayVolume(_volume);
    }
}

void PlayManager::SetPlayProgress(float v)
{
    if (NULL != _vReaderThr && !_fromPlay)
    {
        _vReaderThr->SetPlayProgress(v);
    }
}

void PlayManager::PlayPrevVideo()
{
    if (_playIndex > 0)
    {
        --_playIndex;
        PlayCurrentVideo();
    }
}

void PlayManager::PlayNextVideo()
{
    if (_playIndex < _playList->GetCount() - 1)
    {
        ++_playIndex;
        PlayCurrentVideo();
    }
}

void PlayManager::PlayVideoItem(VideoItem* pItem)
{
    int index = _playList->IndexOf(pItem);
    if (index != -1)
    {
        if (!IsPlaying() || index != _playIndex)
        {
            _playIndex = index;
            PlayCurrentVideo();
        }
    }
}

void PlayManager::PlayCurrentVideo()
{
    VideoItem* pItem = NULL;
    
    if (_playIndex >= 0)
    {
        pItem = _playList->GetVideoItem(_playIndex);

        if (NULL != _reflesh)
        {
            if (_vReaderThr != NULL)
            {
                _vReaderThr->StopPlay();
                _vReaderThr->Join();
                _vReaderThr->unref();
            }

            _vReaderThr = new VideoReaderThr(pItem->GetFilePath(), _reflesh);
            _vReaderThr->ref();

            SetPlayVolume(_volume);
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
