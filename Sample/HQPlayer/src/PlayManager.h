
#ifndef _PLAYMANAGER_H_
#define _PLAYMANAGER_H_

#include "PlayerView.h"

#include <src/Video/VideoReader.h>

class VideoItem : public suic::NotifyPropChanged 
{
public:

    BeginMember(VideoItem, suic::NotifyPropChanged)
        MemberString(FilePath)
    EndMember()

    RTTIOfClass(VideoItem)

    DefineString(FilePath);

    suic::String ToString()
    {
        return GetFilePath();
    }
};

class PlayCollection : public suic::ObservableCollection
{
public:

    PlayCollection();

    VideoItem* GetVideoItem(int index)
    {
        VideoItem* pItem = NULL;
        pItem = suic::DynamicCast<VideoItem>(GetItem(index));
        return pItem;
    }
};

class PlayManager : public suic::Object
{
public:

    PlayManager();
    ~PlayManager();

    void Init(PlayerView* pView, suic::Element* pRoot, PlayVideoCb cb);
    void OnInvoker(suic::Object* sender, suic::InvokerArg* e);

    void SetPlayVolume(int volume);
    void SetPlayProgress(float v);

    void PlayCurrentVideo();
    void PlayVideo(suic::String filename);
    void AddVideo(suic::String filename);

    void PauseVideo(bool bPause);
    void StopVideo();
    bool IsPlaying() const;
    bool IsPause() const;

    PlayCollection* GetPlayList()
    {
        return _playList;
    }

    VideoItem* GetPlayItem();

    void PlayPrevVideo();
    void PlayNextVideo();
    void PlayVideoItem(VideoItem* pItem);

private:

    void UpdatePlayDate(BmpInfo* bmp);
    suic::String FormatTime(int64_t d);
    
    int CheckPlayFile(suic::String filename);

private:

    PlayerView* _playView;
    suic::Element* _rootView;

    PlayVideoCb _playCb;
    VideoReaderThr* _vReaderThr;
    suic::InvokeProxy* _reflesh;

    int _volume;
    int _playIndex;
    bool _fromPlay;
    PlayCollection* _playList;
};

#endif
