// 华勤科技版权所有 2010-2011
// 
// 文件名：uicontentpresenter.h
// 功  能：内容渲染类，支持鼠标进入。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UICONTENTPRESENTER_H_
#define _UICONTENTPRESENTER_H_

#include <Framework/Controls/DataTemplate.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

class SUICORE_API ContentPresenter : public FrameworkElement
{
public:

    static DpProperty* ContentProperty;
    static DpProperty* ContentSourceProperty;
    static DpProperty* ContentTemplateProperty;
    static DpProperty* ContentTemplateSelectorProperty;

    static bool StaticInit();

    static void OnContentPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnContentTemplatePropChanged(DpObject* sender, DpPropChangedEventArg* e);
    static void OnContentTemplateSelectorPropChanged(DpObject* sender, DpPropChangedEventArg* e);

    ContentPresenter();
    virtual ~ContentPresenter();

    RTTIOfClass(ContentPresenter)

    void AddChild(Object* obj);
    void AddText(String val);

    Object* GetContent();
    void SetContent(Object* val);

    DataTemplate* GetContentTemplate();
    void SetContentTemplate(DataTemplate* val);

    DataTemplateSelector* GetContentTemplateSelector();
    void SetContentTemplateSelector(DataTemplateSelector* val);

    void PrepareContentPresenter(Object* item, DataTemplate* itemTemplate, DataTemplateSelector* itemTemplateSelector);

protected:

    void ClearContentPresenter(Object* item);

    virtual DataTemplate* ChooseTemplate();

    friend class ItemsControl;
    friend class ItemsControlEx;

protected:

    Size OnMeasure(const Size& constraitSize);
    void OnArrange(const Size& availableSize);

    void OnPreApplyTemplate();

    FrameworkTemplate* GetTemplateInternal();
    FrameworkTemplate* GetTemplateCache();

    void SetTemplateCache(FrameworkTemplate* temp);

    void OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate);

    virtual void OnContentChanged(Object* oldContent, Object* newContent);
    virtual void OnTemplateChanged(DataTemplate* oldTemplate, DataTemplate* newTemplate);
    virtual void OnContentTemplateChanged(DataTemplate* oldTemplate, DataTemplate* newTemplate);

private:

    void EnsureTemplate();

private:

    Object* _content;
    DataTemplate* _templateCache;
};

inline void ContentPresenter::SetContent(Object* val)
{
    SetValue(ContentProperty, val);
}

inline DataTemplate* ContentPresenter::GetContentTemplate()
{
    return _templateCache;
}

inline void ContentPresenter::SetContentTemplate(DataTemplate* val)
{
    SetValue(ContentTemplateProperty, val);
}

inline DataTemplateSelector* ContentPresenter::GetContentTemplateSelector()
{
    return RTTICast<DataTemplateSelector>(GetValue(ContentTemplateSelectorProperty));
}

inline void ContentPresenter::SetContentTemplateSelector(DataTemplateSelector* val)
{
    SetValue(ContentTemplateSelectorProperty, val);
}

inline FrameworkTemplate* ContentPresenter::GetTemplateCache()
{
    return _templateCache;
}

inline void ContentPresenter::SetTemplateCache(FrameworkTemplate* temp)
{
    SETREFOBJ(_templateCache, RTTICast<DataTemplate>(temp));
}

}

#endif
