// 华勤科技版权所有 2010-2011
// 
// 文件名：HeaderedItemsControl.h
// 功  能：实现Item容器的基类
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIHEADEREDITEMSCONTROL_H_
#define _UIHEADEREDITEMSCONTROL_H_

#include <Framework/Controls/ItemsControl.h>

namespace suic
{

/// <summary>
/// HeaderedItemsControl。
/// </summary>
class SUICORE_API HeaderedItemsControl : public ItemsControl
{
public:

    static DpProperty* HasHeaderProperty;
    static DpProperty* HeaderProperty;
    static DpProperty* HeaderTemplateProperty;
    static DpProperty* HeaderTemplateSelectorProperty;

    static bool StaticInit();
    static void OnHasHeaderPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnHeaderPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnHeaderTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e);

    HeaderedItemsControl();
    virtual ~HeaderedItemsControl();

    RTTIOfClass(HeaderedItemsControl)

    Object* GetHeader();
    void SetHeader(Object* val);

    Element* GetHeaderElement();

    int GetHeaderSize(bool isHorizontal);

    bool HasHeader();
    bool HeaderIsItem() const;
    bool HeaderIsLogical() const;

    DataTemplate* GetHeaderTemplate();
    void SetHeaderTemplate(DataTemplate* val);

    DataTemplateSelector* GetHeaderTemplateSelector();
    void SetHeaderTemplateSelector(DataTemplateSelector* val);

    String ToString();

protected:

    void OnApplyTemplate();
    void SetHeaderIsLogical(bool val);
    void SetHeaderIsItem(bool val);

    void PrepareHierarchy(ItemEntry* item, ItemsControl* parentItemsControl);

    virtual void ClearHeaderedItemsControl(ItemEntry* item);
    virtual void PrepareHeaderedItemsControl(ItemEntry* item, ItemsControl* parentItemsControl);
    virtual void OnHeaderChanged(Object* oldHeader, Object* newHeader);
    virtual void OnHeaderTemplateChanged(DataTemplate* oldTemplate, DataTemplate* newTemplate);

    friend class ItemsControl;
    friend class ItemsControlEx;

private:

    Object* _header;
    Element* _headerElement;
};

inline bool HeaderedItemsControl::HasHeader()
{
    return ReadItemFlags(HasHeaderFlag);
}

inline bool HeaderedItemsControl::HeaderIsItem() const
{
    return ReadItemFlags(Control::ItemBoolFlags::HeaderEqualItem);
}

inline bool HeaderedItemsControl::HeaderIsLogical() const
{
    return !ReadItemFlags(Control::ItemBoolFlags::HeaderIsNotLogical);
}

inline Object* HeaderedItemsControl::GetHeader()
{
    return _header;
}

inline void HeaderedItemsControl::SetHeader(Object* val)
{
    SetValue(HeaderProperty, val);
}

inline Element* HeaderedItemsControl::GetHeaderElement()
{
    return _headerElement;
}

inline DataTemplate* HeaderedItemsControl::GetHeaderTemplate()
{
    return RTTICast<DataTemplate>(GetValue(HeaderTemplateProperty));
}

inline void HeaderedItemsControl::SetHeaderTemplate(DataTemplate* val)
{
    SetValue(HeaderTemplateProperty, val);
}

inline DataTemplateSelector* HeaderedItemsControl::GetHeaderTemplateSelector()
{
    return RTTICast<DataTemplateSelector>(GetValue(HeaderTemplateSelectorProperty));
}

inline void HeaderedItemsControl::SetHeaderTemplateSelector(DataTemplateSelector* val)
{
    SetValue(HeaderTemplateSelectorProperty, val);
}

}

#endif
