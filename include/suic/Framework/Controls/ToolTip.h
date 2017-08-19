// 华勤科技版权所有 2010-2011
// 
// 文件名：uitooltip.h
// 功  能：实现提示窗口。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UITOOLTIP_H_
#define _UITOOLTIP_H_

#include <Framework/Controls/Popup.h>
#include <Framework/Controls/Content.h>

namespace suic
{

class SUICORE_API ToolTip : public ContentControl
{
public:

    static DpProperty* IsOpenProperty;
    static DpProperty* PlacementProperty;
    static DpProperty* PlacementRectangleProperty;
    static DpProperty* PlacementTargetProperty;
    static DpProperty* VerticalOffsetProperty;
    static DpProperty* HorizontalOffsetProperty;

    static bool StaticInit();

    ToolTip();
    ~ToolTip();

    static ToolTip* Ins();

    RTTIOfClass(ToolTip)

    void Show(Point pt);
    void Hide();
    void Close();

    void SetOwner(Element* owner);
    Element* GetOwner() const;

    PlacementMode GetPlacement();
    void SetPlacement(PlacementMode val);

    Rect GetPlacementRectangle();
    void GetPlacementRectangle(Rect val);

    Element* GetPlacementTarget();
    void SetPlacementTarget(Element* target);

    int GetVerticalOffset();
    void SetVerticalOffset(int val);

    int GetHorizontalOffset();
    void SetHorizontalOffset(int val);

    void SetToolTip(String tooltip);

    void OnPopupOpened(Object* sender, EventArg* e);
    void OnPopupClosed(Object* sender, EventArg* e);
    void OnClosedTimer(Object* sender, EventArg* e);

    void OnPopupClosing(Object* sender, EventArg* e);
    void OnPopupOpening(Object* sender, OpeningEventArg* e);

protected:

    void Clear();

    void OnHitTest(HitResultEventArg* e);
    void OnShowWindow();
    void OnContentChanged(Object* oldContent, Object* newContent);

protected:

    Popup* _popup;
    Element* _owner;
    AssignerTimer* _timer;
    static ToolTip* _toolTip;
};

}

#endif
