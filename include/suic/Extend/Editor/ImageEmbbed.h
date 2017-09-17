// 华勤科技版权所有 2008-2022
// 
// 文件名：ImageEmbbed.h
// 功  能：实现图像嵌入对象。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIIMAGEEMBBED_H_
#define _UIIMAGEEMBBED_H_

#include <Extend/Editor/Embbed.h>
#include <Framework/Animation/AnimationAsKeyFrame.h>

namespace suic
{

class RichTextBox;

class SUIWGX_API ImageEmbbed : public Embbed
{
public:

    RTTIOfClass(ImageEmbbed)

    ImageEmbbed();
    virtual ~ImageEmbbed();

    suic::Bitmap* GetBitmap();
    suic::ImageSource* GetImageSource();
    
    bool Load(const suic::String& path);
    int GetType();

protected:

    void OnMeasure(const suic::Size& contraint);
    void OnRender(suic::Drawing* drawing);
    void OnSave(suic::ISStream* strm);
    void OnRestore(int type, suic::ISStream* strm);

private:

    suic::Handle _handle;
    suic::ImageSource _bitmap;
};

class SUIWGX_API PlayImageEmbbed : public Embbed
{
public:

    RTTIOfClass(PlayImageEmbbed)

    PlayImageEmbbed();
    ~PlayImageEmbbed();

    void SetPlaySize(suic::Size size);
    void AddImage(suic::ImageSource* bmp, suic::Point offset, suic::Size size, int iDelay);

    void Clear();

protected:

    void OnRender(suic::Drawing* drawing);
    void OnSave(suic::ISStream* strm);
    void OnRestore(int type, suic::ISStream* strm);

    void OnTimer(suic::Object* sender, suic::EventArg* e);

private:

    struct ImFrame
    {
        int delay;
        suic::Size size;
        suic::Point offet;
        suic::ImageSource* bmp;
    };

    ImFrame* GetCurrentFrame();

    int _currentFrame;
    suic::Size _playSize;
    suic::AssignerTimer* _timer; 
    suic::Array<ImFrame, false> _aniFrames;
};

}

#endif
