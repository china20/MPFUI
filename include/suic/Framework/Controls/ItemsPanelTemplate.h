// 华勤科技版权所有 2010-2011
// 
// 文件名：ItemsPanelTemplate.h
// 功  能：ItemsControl控件项容器模板
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIITEMSPANELTEMPLATE_H_
#define _UIITEMSPANELTEMPLATE_H_

#include <System/Windows/FrameworkTemplate.h>

namespace suic
{

class FEFactory;

/// <summary>
/// ItemsPanelTemplate，其可视树根必须从Panel继承。
/// </summary>
class ItemsPanelTemplate : public FrameworkTemplate
{
public:

    RTTIOfClass(ItemsPanelTemplate)

    ItemsPanelTemplate();
    ItemsPanelTemplate(FEFactory* root);

    virtual ~ItemsPanelTemplate();

    void SetValue(DpProperty* dp, Object* val);

protected:

    RTTIOfInfo* GetTargetTypeInternal();

    void ProcessTemplateBeforeSeal();
    void ValidateTemplatedParent(FrameworkElement* templatedParent);

};

}

#endif
