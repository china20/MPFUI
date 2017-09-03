
#ifndef _PLAYMANAGER_H_
#define _PLAYMANAGER_H_

#include "PlayerView.h"

#include <src/Video/VideoReader.h>

class PlayManager : public suic::Object
{
public:

    PlayManager();
    ~PlayManager();

    void Init(PlayerView* pView, suic::Element* pRoot);
    void OnInvoker(suic::Object* sender, suic::InvokerArg* e);

    void PlayVideo(suic::String filename);

private:

    void UpdatePlayDate(BmpInfo* bmp);
    suic::String FormatTime(int64_t d);

private:

    PlayerView* _playView;
    suic::Element* _rootView;

    VideoReaderThr* _vReaderThr;
    suic::InvokeProxy* _reflesh;
};

#endif
