// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ComboBoxItem.h
// ��  �ܣ�ʵ����Ͽ����߼���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================


#ifndef _UICOMBOBOXITEM_H_
#define _UICOMBOBOXITEM_H_

#include <Extend/suiwgxconfig.h>
#include <Extend/Controls/ListBoxItem.h>

namespace suic
{

/// <summary>
/// ��Ͽ������ΪListBox�������
/// </summary>
class SUIWGX_API ComboBoxItem : public ListBoxItem
{
public:

    ComboBoxItem();
    virtual ~ComboBoxItem();

    RTTIOfClass(ComboBoxItem)

protected:

    static bool StaticInit();

    void OnMouseMove(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);

    friend class ComboBox;
};

}

#endif
