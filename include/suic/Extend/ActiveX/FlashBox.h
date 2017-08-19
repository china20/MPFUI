// ���ڿƼ���Ȩ���� 2010-2020
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
//
// SHARPUI�����������ҵ��������ϵ�ܹ���������ϵͳ���¼�ϵͳ��
// ��ģ����Ŀǰc++�������һ���ص㣬����������ʽ�ʹ�����룬�ײ�
// ��������SSE2ָ����м��٣���������ͨ���ڻ��ǰ�͸�����������ܶ�
// �ǳ����㣬���ơ����Ķ����ӿڣ���ȫ�������������ҵʹ�á�
// 
// ����蹺����֧�ַ������ѯ����ϵ
// china0851@foxmail.com��QQ:386666951��
// ====================================================================

#ifndef _UIFLASHBOX_H_
#define _UIFLASHBOX_H_

#include <Extend/ActiveX/ActiveX.h>

namespace suic
{

/// <summary>
///  FlashBox����װFlash�ؼ���
/// </summary>
class SUIWGX_API FlashBox : public ActiveX
{
public:

    static suic::DpProperty* SourceProperty;
    static suic::DpProperty* PlayProperty;

    static bool StaticInit();

    static void OnPlayPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    FlashBox();
    virtual ~FlashBox();

    RTTIOfClass(FlashBox)

    bool LoadMovie(suic::String uri);
    void SetTransparent(bool bTransparent);

    suic::String GetSource() const;
    void SetSource(suic::String source);

    void Play();
    void Stop();
    void Pause();

protected:

    bool OpenOverride();

    void OnInitialized(suic::EventArg* e);
    void OnRender(suic::Drawing * drawing);
    void OnLoaded(suic::LoadedEventArg* e);
    void OnUnloaded(suic::LoadedEventArg* e);

    void OnRenderSizeChanged(suic::SizeChangedInfo& e);
    void OnVisibilityChanged();

protected:

    suic::String _source;
};

typedef suic::shared<FlashBox> FlashBoxPtr;

inline suic::String FlashBox::GetSource() const
{
    return _source;
}

inline void FlashBox::SetSource(suic::String source)
{
    _source = source;
}

};

#endif
