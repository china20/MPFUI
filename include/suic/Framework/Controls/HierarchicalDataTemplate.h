// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����HierarchicalDataTemplate.h
// ��  �ܣ�ʵ�ֲ������ģ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIHIERARCHICALDATATEMPLATE_
#define _UIHIERARCHICALDATATEMPLATE_

#include <Framework/Controls/DataTemplate.h>

namespace suic
{

/// <summary>
/// HierarchicalDataTemplate,�㼶����ģ�档
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
