// 华勤科技版权所有 2008-2022
// 
// 文件名：PlayBox.h
// 功  能：实现控件轮播。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UIPLAYBOX_H_
#define _UIPLAYBOX_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/Content.h>
#include <System/Tools/ElementCollection.h>

namespace suic
{

enum PlayBehavior 
{
    // 重复次数
    pbIterationCount,
    // 不重复
    pbRepeatDuration,
    // 永远执行
    pbForever,
};

class SUIWGX_API PlayBox : public suic::FrameworkElement
                         , public suic::IAddChild
{
public:

    static suic::DpProperty* BackgroundProperty;
    static suic::DpProperty* PlayProperty;
    static suic::DpProperty* PlayIndexProperty;
    static suic::DpProperty* PlayBehaviorProperty;
    static suic::DpProperty* MousePlayProperty;
    static suic::DpProperty* ShowFrameProperty;

    static suic::DpProperty* PlayDelayProperty;

    static void OnPlayPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnPlayIndexPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static void StaticInit();

    PlayBox();
    virtual ~PlayBox();

    RTTIOfClass(PlayBox)

    static int GetPlayDelay(suic::Element* elem);
    static void SetPlayDelay(suic::Element* elem, int val);

    int GetPlayIndex();
    void SetPlayIndex(int val);

    bool GetShowFrame();
    void SetShowFrame(bool val);

    bool GetPlay();
    void SetPlay(bool val);

    bool GetMousePlay();
    void SetMousePlay(bool val);

    int GetPlayBehavior();
    void SetPlayBehavior(int val);

    suic::Brush* GetBackground();
    void SetBackground(suic::Brush* val);

    void AddChild(suic::Object* child);
    void AddText(suic::String val);
    void RemoveChild(Object* child);

    suic::ElementColl* GetChildCollection();

    int GetVisualChildrenCount();
    suic::Element* GetVisualChild(int index);

    suic::Element* GetLogicalChild(int index);
    int GetLogicalChildrenCount();

protected:

    void Play();
    int GetCurrentIndex();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    suic::Size OnMeasure(const suic::Size& size);
    void OnArrange(const suic::Size& arrangeSize);

    void OnRender(suic::Drawing* drawing);

    void OnMouseEnter(suic::MouseButtonEventArg* e);
    void OnMouseLeave(suic::MouseButtonEventArg* e);

private:

    bool _isReverse;
    suic::ElementColl* _elemColl;
};

}

#endif
