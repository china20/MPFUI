// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uipanel.h
// ��  �ܣ��������Ļ��ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-11-02
// 
// ============================================================================

#ifndef _UIPANEL_H_
#define _UIPANEL_H_

#include <System/Windows/FrameworkElement.h>
#include <System/Tools/ElementCollection.h>

namespace suic
{

class ItemContainerGenerator;

/// <summary>
/// ����������Ԫ����Ļ��࣬ʵ��һ�����Ԫ�صĹ��������ͽӿ�.
/// </summary>
class SUICORE_API Panel : public FrameworkElement, public IAddChild
{
public:

    static DpProperty* IsItemsHostProperty;
    static DpProperty* ZIndexProperty;
    static DpProperty* BackgroundProperty;

    static bool StaticInit();

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
    // ������Ԫ��
    ElementColl* _elemColl;

    // �����Ԫ�ص�Z˳��
    Array<int> _zIndexs;
    Array<int> _tIndexs;

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
