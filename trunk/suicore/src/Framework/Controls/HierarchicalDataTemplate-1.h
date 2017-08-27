// 华勤科技版权所有 2010-2011
// 
// 文件名：HierarchicalDataTemplate.h
// 功  能：实现层次数据模版
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIHIERARCHICALDATATEMPLATE_
#define _UIHIERARCHICALDATATEMPLATE_

#include <Framework/Controls/DataTemplate.h>

namespace suic
{

/// <summary>
/// HierarchicalDataTemplate,层级数据模版。
/// </summary>
class SUICORE_API HierarchicalDataTemplate : public DataTemplate
{
public:

    RTTIOfClass(HierarchicalDataTemplate)

    HierarchicalDataTemplate();
    HierarchicalDataTemplate(RTTIOfInfo* targetType);
    virtual ~HierarchicalDataTemplate();

    Binding* GetItemSource() const;
    void SetItemSource(Binding* val);

    DataTemplate* GetItemTemplate() const;
    void SetItemTemplate(DataTemplate* val);

    DataTemplateSelector* GetItemTemplateSelector() const;
    void SetItemTemplateSelector(DataTemplateSelector* val);

private:

    void InitDefaultValue();

private:

    Binding* _itemSource;
    DataTemplate* _itemTemplate;
    DataTemplateSelector* _itemTempSelector;
};

inline Binding* HierarchicalDataTemplate::GetItemSource() const
{
    return _itemSource;
}

inline DataTemplate* HierarchicalDataTemplate::GetItemTemplate() const
{
    return _itemTemplate;
}

inline DataTemplateSelector* HierarchicalDataTemplate::GetItemTemplateSelector() const
{
    return _itemTempSelector;
}

}

#endif
