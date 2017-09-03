
#ifndef _PLAYMANAGER_H_
#define _PLAYMANAGER_H_

#include "PlayerView.h"

#include <src/Video/VideoReader.h>

class PlayManager : public suic::Object
{
public:

    PlayManager();
    ~PlayManager();

    void Init(PlayerView* pView);
    void OnInvoker(suic::Object* sender, suic::InvokerArg* e);

    void PlayVideo(suic::String filename);

private:

    PlayerView* _playView;
    VideoReaderThr* _vReaderThr;
    suic::InvokeProxy* _reflesh;
};

#endif
