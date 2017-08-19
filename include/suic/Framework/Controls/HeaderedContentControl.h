// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����HeaderedContentControl.h
// ��  �ܣ�ʵ��Item��������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIHEADEREDCONTENTCONTROL_H_
#define _UIHEADEREDCONTENTCONTROL_H_

#include <Framework/Controls/Content.h>
#include <Framework/Controls/DataTemplateSelector.h>

namespace suic
{

/// <summary>
/// HeaderedContentControl��
/// </summary>
class SUICORE_API HeaderedContentControl : public ContentControl

{
public:

    static DpProperty* HasHeaderProperty;
    static DpProperty* HeaderProperty;
    static DpProperty* HeaderTemplateProperty;
    static DpProperty* HeaderTemplateSelectorProperty;

    static bool StaticInit();
    static void OnHeaderPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnHeaderTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e);

    HeaderedContentControl();
    virtual ~HeaderedContentControl();

    RTTIOfClass(HeaderedContentControl)

    Object* GetHeader();
    void SetHeader(Object* val);

    DataTemplate* GetHeaderTemplate();
    void SetHeaderTemplate(DataTemplate* val);

public:

    bool HasHeader();
    bool HeaderIsItem() const;
    bool HeaderIsLogical() const;

protected:

    void SetHeaderIsLogical(bool val);
    void SetHeaderIsItem(bool val);

    void ClearHeaderedContentControl(Object* item);
    void PrepareHeaderedContentControl(Object* item, DataTemplate* itemTemplate, DataTemplateSelector* itemTemplateSelector);

    virtual void OnHeaderChanged(Object* oldHeader, Object* newHeader);
    virtual void OnHeaderTemplateChanged(DataTemplate* oldTemplate, DataTemplate* newTemplate);

    friend class ItemsControl;
    friend class ItemsControlEx;
};

inline bool HeaderedContentControl::HasHeader()
{
    return GetValue(HasHeaderProperty)->ToInt() != 0;
}

inline bool HeaderedContentControl::HeaderIsItem() const
{
    return ReadItemFlags(Control::ItemBoolFlags::HeaderEqualItem);
}

inline bool HeaderedContentControl::HeaderIsLogical() const
{
    return !ReadItemFlags(Control::ItemBoolFlags::HeaderIsNotLogical);
}

inline Object* HeaderedContentControl::GetHeader()
{
    return GetValue(HeaderProperty);
}

inline void HeaderedContentControl::SetHeader(Object* val)
{
    SetValue(HeaderProperty, val);
}

inline DataTemplate* HeaderedContentControl::GetHeaderTemplate()
{
    return RTTICast<DataTemplate>(GetValue(HeaderTemplateProperty));
}

inline void HeaderedContentControl::SetHeaderTemplate(DataTemplate* val)
{
    SetValue(HeaderTemplateProperty, val);
}

}

#endif
