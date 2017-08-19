// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uicontent.h
// ��  �ܣ�ʵ�ִ��ڵĻ��������������������ԵĴ�ȡ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UICONTENTCONTROL_H_
#define _UICONTENTCONTROL_H_

#include <Framework/Controls/Control.h>
#include <Framework/Controls/DataTemplateSelector.h>

namespace suic
{

/// <summary>
/// ���ݽ���Ԫ����Ļ��࣬������һ����Ԫ�أ���������GetPadding().
/// ������б߾���ƣ�������ʱ��Ĭ������Ԫ��������������
/// </summary>
class SUICORE_API ContentControl : public Control, public IAddChild
{
public:

    static DpProperty* ContentProperty;
    static DpProperty* ContentTemplateProperty;
    static DpProperty* ContentTemplateSelectorProperty;

    static DpProperty* HasContentProperty;

    static bool StaticInit();
    static void OnContentPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnContentTemplatePropChanged(DpObject* sender, DpPropChangedEventArg* e);
    static void OnContentTemplateSelectorPropChanged(DpObject* sender, DpPropChangedEventArg* e);

    RTTIOfClass(ContentControl)

    ContentControl();
    virtual ~ContentControl();

    ContentControl* Clone();

    bool HasContent();
    bool ContentIsLogical() const;
    bool ContentIsItem() const;

public:

    Element* GetLogicalChild(int index);
    int GetLogicalChildrenCount();

    /// <summary>
    ///  ���������ı�
    /// </summary>
    /// <remarks>
    ///  ���ı������������ݶ��󣬶��ǲ���������Ϊ����
    /// </remarks>
    /// <param name="text">�ı�</param>
    /// <returns>���ݶ���</returns> 
    void SetText(const String & text);
    String ToString();

// ���Է��ʶ���
public:

    /// <summary>
    ///     ��ȡ���ݶ���
    /// </summary>
    /// <remarks>
    ///     ���ݶ������������FrameView
    /// </remarks>
    /// <returns>���ݶ���</returns> 
    Object* GetContent();
    Object* SetContent(Object* val);

    DataTemplate* GetContentTemplate();
    void SetContentTemplate(DataTemplate* val);

    DataTemplateSelector* GetContentTemplateSelector();
    void SetContentTemplateSelector(DataTemplateSelector* val);

public:

    virtual void OnContentChanged(Object* oldContent, Object* newContent);
    virtual void OnContentTemplateChanged(DataTemplate* oldTemplate, DataTemplate* newTemplate);
    virtual void OnContentTemplateSelectorChanged(DataTemplateSelector* oldTemplateSelector, DataTemplateSelector* newTemplateSelector);

    /// <summary>
    ///  ����һ����Ԫ��
    /// </summary>
    /// <param name="index">��������λ��</param>
    /// <param name="child">��Ԫ��</param>
    /// <returns>��</returns>
    void AddChild(Object* obj);
    void AddText(String val);
    void RemoveChild(Object* child);

protected:

    void SetContentIsLogical(bool val);
    void SetContentIsItem(bool val);

    void ClearContentControl(Object* item);
    void PrepareContentControl(Object* item, DataTemplate* itemTemplate, DataTemplateSelector* itemTemplateSelector);

    friend class ItemsControl;
    friend class ItemsControlEx;

private:

    Object* _content;
    DataTemplate* _contentTemplate;
    DataTemplateSelector* _contentTemplateSelector;
};

inline bool ContentControl::ContentIsLogical() const
{
    return !ReadItemFlags(Control::ItemBoolFlags::ContentIsNotLogical);
}

inline bool ContentControl::ContentIsItem() const
{
    return ReadItemFlags(Control::ItemBoolFlags::ContentEqualItem);
}

inline bool ContentControl::HasContent()
{
    return ReadItemFlags(Control::ItemBoolFlags::HasContentFlag);
}

inline Object* ContentControl::GetContent()
{
    return _content;
}

inline DataTemplate* ContentControl::GetContentTemplate()
{
    return _contentTemplate;
}

inline void ContentControl::SetContentTemplate(DataTemplate* val)
{
    SetValue(ContentTemplateProperty, val);
}

inline DataTemplateSelector* ContentControl::GetContentTemplateSelector()
{
    return _contentTemplateSelector;
}

inline void ContentControl::SetContentTemplateSelector(DataTemplateSelector* val)
{
    SetValue(ContentTemplateSelectorProperty, val);
} 

};

#endif
