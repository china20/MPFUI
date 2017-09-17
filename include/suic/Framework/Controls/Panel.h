// 华勤科技版权所有 2008-2022
// 
// 文件名：uipanel.h
// 功  能：所有面板的基类。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
// 
// ============================================================================

#ifndef _UIPANEL_H_
#define _UIPANEL_H_

#include <System/Windows/FrameworkElement.h>
#include <System/Tools/ElementCollection.h>

namespace suic
{

class ItemContainerGenerator;

enum eItemDirection
{
    idPrev,
    idCurr,
    idNext,
};

/// <summary>
/// 所有面板界面元素类的基类，实现一般界面元素的公共操作和接口.
/// </summary>
class SUICORE_API Panel : public FrameworkElement, public IAddChild
{
public:

    static DpProperty* IsItemsHostProperty;
    static DpProperty* ZIndexProperty;
    static DpProperty* BackgroundProperty;

    static void StaticInit();

    static void OnZIndexPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnBackgroundPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsItemsHostPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnInvalidateThunk(DpObject* sender, RoutedEventArg* e);

    static int GetZIndex(Element* element);
    static void SetZIndex(Element* element, int value);

    Panel();
    virtual ~Panel();

    Panel* Clone();
    void Dispose();

    RTTIOfAbstractClass(Panel);

    bool IsItemsHost();
    void SetIsItemsHost(bool val);

    ItemContainerGenerator* GetGenerator() const;

    ElementColl* GetChildren();

    bool IsVirtualizing();

    Brush* GetBackground();
    void SetBackground(Brush* val);

    virtual int GetVisibleStart() const;
    virtual int GetLogicalOrientation();
    virtual void NotifyItemChanged();

    virtual int ComputeOffsetFromItem(Object* item, eItemDirection id, int& offset, int& itemSize);
    virtual int ComputeOffsetFromIndex(int index, int& offset, int& itemSize);

public:

    void OnRender(Drawing* drawing);

    void AddChild(Object* value);
    void AddText(String text);
    void RemoveAt(int index);
    void RemoveChild(Object* child);

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    Element* GetLogicalChild(int index);
    int GetLogicalChildrenCount();

protected:

    void OnItemsChanged(Object* sender, ItemsChangedEventArg* e);
    void OnVisualChildrenChanged(DpObject* visualAdded, DpObject* visualRemoved);

    virtual void OnClearChildrenInternal();
    virtual void OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e);

    virtual void OnIsItemsHostChanged(bool oldIsItemsHost, bool newIsItemsHost);

    virtual void OnRequestBringIntoView(RequestBringIntoViewEventArg* e);

protected:

    virtual void GenerateChildren();

    void InvalidateZState();
    void EnsureGenerator();

    void RecomputeZState();

    ElementColl* GetInternalChildren();
    Element* GetInternalChild(int index);

    virtual ElementColl* CreateUIElementCollection(FrameworkElement* logicalParent);

private:

    bool CheckItemsHost();
    void ClearChildren();
    void ConnectToGenerator();

    void DisconnectFromGenerator();
    void EnsureEmptyChildren(FrameworkElement logicalParent);

    void AddChildren(int pos, int itemCount);
    void SwapChildren(int pos1, int pos2);
    void MoveChildren(int fromPos, int toPos, int containerCount);
    void RemoveChildren(int pos, int containerCount);
    void ReplaceChildren(int pos, int itemCount, int containerCount);
    void ResetChildren();
    void EnsureEmptyChildren(FrameworkElement* logicalParent);

    bool IsZStateDirty();
    void SetIsZStateDirty(bool val);

protected:

    enum BoolField : Byte
    {
        HasMeasuredField = 8,
        IsPixelBasedField = 0x10,
        IsVirtualizingField = 4,
        IsZStateDirtyField = 1,
        IsZStateDiverseField = 2
    };

    Size _round;
    Byte _flagField;

    Brush* _background;
    // 可视子元素
    ElementColl* _elemColl;

    // 计算后元素的Z顺序
    Array<int, false> _zIndexs;
    Array<int, false> _tIndexs;

    ItemContainerGenerator* _icGenerator;

    friend class ItemsControl;
    friend class ItemContainerGenerator;
};

typedef suic::shared<Panel> PanelPtr;

inline Brush* Panel::GetBackground()
{
    return _background;
}

inline ItemContainerGenerator* Panel::GetGenerator() const
{
    return _icGenerator;
}

inline void Panel::SetBackground(Brush* val)
{
    SetValue(BackgroundProperty, val);
}

inline ElementColl* Panel::GetChildren()
{
    return GetInternalChildren();
}

inline bool Panel::IsZStateDirty()
{
    return UITESTFLAG(_flagField, Panel::BoolField::IsZStateDirtyField);
}

inline bool Panel::IsItemsHost()
{
    return GetValue(IsItemsHostProperty)->ToBool();
}

inline void Panel::SetIsItemsHost(bool val)
{
    SetValue(IsItemsHostProperty, val ? Boolean::True : Boolean::False);
}

}

#endif
