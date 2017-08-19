// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ItemsPresenter.h
// ��  �ܣ�ʵ��Item��Ⱦ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIITEMPRESENTER_H_
#define _UIITEMPRESENTER_H_

#include <System/Windows/FrameworkElement.h>

#include <Framework/Controls/GroupItem.h>
#include <Framework/Controls/DataTemplate.h>

namespace suic
{

class Panel;
class ItemsControl;
class ItemContainerGenerator;
class ItemsPanelTemplate;

/// <summary>
/// ItemsPresenter����TemplateParentΪItemsControl��
/// </summary>
class SUICORE_API ItemsPresenter : public FrameworkElement
{
private:

    WeakRef _generator;
    ItemsPanelTemplate* _templateCache;
    ItemsControl* _owner;

public:

    RTTIOfClass(ItemsPresenter)

    static DpProperty* TemplateProperty;

    static bool StaticInit();
    static ItemsPresenter* FromPanel(Panel* panel);
    static void OnTemplateChanged(DpObject* d, DpPropChangedEventArg* e);

    ItemsPresenter();
    virtual ~ItemsPresenter();

    ItemContainerGenerator* GetGenerator() const;
    ItemsControl* GetOwner() const;

    void OnPanelChanged(Object* sender, EventArg* e);

protected:

    // ����
    FrameworkTemplate* GetTemplateCache();
    void SetTemplateCache(FrameworkTemplate* temp);

    FrameworkTemplate* GetTemplateInternal();

    Size OnMeasure(const Size& constraint);
    void OnArrange(const Size& arrangeSize);

    void OnTemplateChildChanged();
    void OnPreApplyTemplate();
    void OnApplyTemplate();

    void OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate);

private:

    void ClearPanel();
    void AttachToOwner();
    ItemsPresenter* FromGroupItem(GroupItem* groupItem);

    ItemsPanelTemplate* GetTemplate();
    void SetTemplate(ItemsPanelTemplate* val);

    void UseGenerator(ItemContainerGenerator* generator);
};

}

#endif
