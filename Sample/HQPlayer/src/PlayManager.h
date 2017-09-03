
#ifndef _PLAYMANAGER_H_
#define _PLAYMANAGER_H_

#include "PlayerView.h"

#include <src/Video/VideoReader.h>

class PlayManager : public suic::Object
{
public:

    PlayManager();
    ~PlayManager();

    void Init(PlayerView* pView, suic::Element* pRoot, PlayVideoCb cb);
    void OnInvoker(suic::Object* sender, suic::InvokerArg* e);

    void PlayVideo(suic::String filename);
    void PauseVideo(bool bPause);
    void StopVideo();
    bool IsPlaying() const;
    bool IsPause() const;

private:

    void UpdatePlayDate(BmpInfo* bmp);
    suic::String FormatTime(int64_t d);

private:

    PlayerView* _playView;
    suic::Element* _rootView;

    PlayVideoCb _playCb;
    VideoReaderThr* _vReaderThr;
    suic::InvokeProxy* _reflesh;
};

#endif
