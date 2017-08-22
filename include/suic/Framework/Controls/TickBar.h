// 华勤科技版权所有 2010-2011
// 
// 文件名：TickBar.h
// 功  能：刻度控件。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UITICKBAR_H_
#define _UITICKBAR_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

class SUICORE_API TickBar : public FrameworkElement
{
public:

    static DpProperty* FillProperty;

    static DpProperty* IsDirectionReversedProperty;
    static DpProperty* IsSelectionRangeEnabledProperty;
    static DpProperty* MaximumProperty;
    static DpProperty* MinimumProperty;

    static DpProperty* PlacementProperty;
    static DpProperty* ReservedSpaceProperty;
    static DpProperty* SelectionEndProperty;
    static DpProperty* SelectionStartProperty;
    
    static DpProperty* TicksProperty;
    static DpProperty* TickFrequencyProperty;

    static bool StaticInit();

    TickBar();
    virtual ~TickBar();

    RTTIOfAbstractClass(TickBar);

    Brush* GetFill();
    void SetFill(Brush* val);

    bool IsDirectionReversed();
    void SetIsDirectionReversed(bool val);

    bool IsSelectionRangeEnabled();
    void SetIsSelectionRangeEnabled(bool val);

    int GetMinimum();
    void SetMinimum(int val);

    int GetMaximum();
    void SetMaximum(int val);

    Float GetReservedSpace();
    void SetReservedSpace(Float val);

    Float GetSelectionEnd();
    void SetSelectionEnd(Float val);

    Float GetSelectionStart();
    void SetSelectionStart(Float val);

    Float GetTickFrequency();
    void SetTickFrequency(Float val);

    IntAutoArray* GetTicks();
    void SetTicks(IntAutoArray* Float);

protected:

    void OnPreApplyTemplate();
    void OnRender(Drawing* drawing);

    void BindToTemplatedParent(DpProperty* target, DpProperty* source);
};

inline Brush* TickBar::GetFill()
{
    return RTTICast<Brush>(GetValue(FillProperty));
}

inline void TickBar::SetFill(Brush* val)
{
    SetValue(FillProperty, val);
}

}

#endif
