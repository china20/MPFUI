// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ItemsPanelTemplate.h
// ��  �ܣ�ItemsControl�ؼ�������ģ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIITEMSPANELTEMPLATE_H_
#define _UIITEMSPANELTEMPLATE_H_

#include <System/Windows/FrameworkTemplate.h>

namespace suic
{

class FEFactory;

/// <summary>
/// ItemsPanelTemplate����������������Panel�̳С�
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
