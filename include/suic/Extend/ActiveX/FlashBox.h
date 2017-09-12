// 华勤科技版权所有 2010-2020
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
//
// MPF界面引擎采用业界成熟的体系架构，其属性系统、事件系统以
// 及模版在目前c++界面库是一大特点，真正做到样式和代码分离，底层
// 更采用了SSE2指令进行加速，不论在普通窗口还是半透明窗口下性能都
// 非常优秀，完善、灵活的动画接口，完全可以满足各种商业使用。
// 
// 如果需购买技术支持服务或咨询请联系
// china0851@foxmail.com；QQ:386666951。
// ====================================================================

#ifndef _UIFLASHBOX_H_
#define _UIFLASHBOX_H_

#include <Extend/ActiveX/ActiveX.h>

namespace suic
{

/// <summary>
///  FlashBox，封装Flash控件。
/// </summary>
class SUIWGX_API FlashBox : public ActiveX
{
public:

    static suic::DpProperty* SourceProperty;
    static suic::DpProperty* PlayProperty;

    static void StaticInit();

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
