// 华勤科技版权所有 2008-2022
// 
// 文件名：uicontent.h
// 功  能：实现窗口的基本操作，包括窗口属性的存取。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UICONTENTCONTROL_H_
#define _UICONTENTCONTROL_H_

#include <Framework/Controls/Control.h>
#include <Framework/Controls/DataTemplateSelector.h>

namespace suic
{

/// <summary>
/// 内容界面元素类的基类，仅包含一个子元素，可以设置GetPadding().
/// 对其进行边距控制，有内容时，默认内容元素铺满整个区域
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
    ///  设置内容文本
    /// </summary>
    /// <remarks>
    ///  纯文本并不创建内容对象，而是采用自身作为容器
    /// </remarks>
    /// <param name="text">文本</param>
    /// <returns>内容对象</returns> 
    void SetText(const String & text);
    String ToString();

// 属性访问定义
public:

    /// <summary>
    ///     获取内容对象
    /// </summary>
    /// <remarks>
    ///     内容对象必须派生至FrameView
    /// </remarks>
    /// <returns>内容对象</returns> 
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
    ///  增加一个子元素
    /// </summary>
    /// <param name="index">插入索引位置</param>
    /// <param name="child">子元素</param>
    /// <returns>无</returns>
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
