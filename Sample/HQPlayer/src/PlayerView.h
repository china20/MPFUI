// 华勤科技版权所有 2010-2011
// 
// 文件名：PlayerView.h
// 功  能：播放界面的控件。
// 
// 作  者：MPF开发组
// 时  间：2017-01-02
// 
// ============================================================================

#ifndef _PLAYERVIEW_H_
#define _PLAYERVIEW_H_

#include <src/Video/VideoReader.h>

class PlayerView : public suic::FrameworkElement
{
public:

    PlayerView();
    ~PlayerView();

    void PostRender(BmpInfo* bmp);

protected:

    void Dispose();

    void OnRender(suic::Drawing* drawing);

private:

    void DrawVideo(suic::Drawing* drawing, suic::Bitmap* bmp);

private:

    BmpInfo* _bmp;
};

#endif
