// 华勤科技版权所有 2010-2011
// 
// 文件名：MainWindow.h
// 功  能：主界面窗口。
// 
// 作  者：Sharpui开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _DRAWPANEL_H_
#define _DRAWPANEL_H_

class DrawPanel : public suic::Panel
{
public:

    RTTIOfClass(DrawPanel)

    DrawPanel();
    ~DrawPanel();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnButtonClicked(suic::Element* sender, suic::RoutedEventArg* e);

protected:

    void OnRender(suic::Drawing* drawing);
    void OnRenderSizeChanged(suic::SizeChangedInfo& e);

    void UpdateDrawArea();
    void ResetRenderContext();

private:

    suic::Drawing* _drawing;
    suic::RenderContext _renderCtx;
};

#endif

