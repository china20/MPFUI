// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����GifBox.h
// ��  �ܣ�ʵ��GIF�ļ����š�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
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

    static bool StaticInit();

    GifBox();
    virtual ~GifBox();

    RTTIOfClass(GifBox)

    suic::String GetUri();
    void SetUri(suic::String uri);

protected:

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);
    void OnTick(suic::Object* sender, suic::EventArg* e);

    suic::Size OnMeasure(const suic::Size& constraint);

    void OnRender(suic::Drawing* drawing);

private:

    suic::GIFParser* _parser;
    suic::AssignerTimer* _timer;
};

}

#endif
