// 华勤科技版权所有 2008-2022
// 
// 文件名：GifBox.h
// 功  能：实现GIF文件播放。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UIGIFBOX_H_
#define _UIGIFBOX_H_

#include <Extend/suiwgxconfig.h>
#include <System/Graphics/GIFParser.h>
#include <Framework/Controls/Content.h>

namespace suic
{

class SUIWGX_API GifBox : public suic::FrameworkElement
{
public:

    static suic::DpProperty* UriProperty;
    static suic::DpProperty* PlayProperty;

    static void OnUriPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnPlayPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static void StaticInit();

    GifBox();
    virtual ~GifBox();

    RTTIOfClass(GifBox)

    suic::String GetUri();
    void SetUri(suic::String uri);

    bool IsPlay();
    bool IsValid() const;

protected:

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);
    void OnTick(suic::Object* sender, suic::EventArg* e);

    suic::Size OnMeasure(const suic::Size& constraint);
    void OnArrange(const suic::Size& arrangeSize);

    void OnRender(suic::Drawing* drawing);

    void StartGif();

private:

    suic::Bitmap _bitmap;
    suic::GIFParser* _parser;
    suic::AssignerTimer* _timer;
};

inline bool GifBox::IsValid() const
{
    if (NULL == _parser || !_parser->IsLoaded())
    {
        return false;
    }
    else
    {
        return true;
    }
}

}

#endif
