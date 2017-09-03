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

void PlayManager::Init(PlayerView* pView)
{
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

void PlayManager::OnInvoker(suic::Object* sender, suic::InvokerArg* e)
{
    BmpInfo* bmp = (BmpInfo*)e->GetData();
    //bmp->unref();
    _playView->PostRender(bmp);
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
