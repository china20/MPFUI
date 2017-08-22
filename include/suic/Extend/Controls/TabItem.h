// 华勤科技版权所有 2010-2011
// 
// 文件名：TabItem.h
// 功  能：Tab控件项。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UITABITEM_H_
#define _UITABITEM_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/HeaderedContentControl.h>

namespace suic
{

class TabControl;

class SUIWGX_API TabItem : public HeaderedContentControl
{
public:

    enum BoolField
    {
        DefaultValue = 0,
        SetFocusOnContent = 0x10,
        SettingFocus = 0x20,
    };

    static DpProperty* IsSelectedProperty;
    static DpProperty* TabStripPlacementProperty;

    static void OnIsSelectedPropChanged(DpObject* d, DpPropChangedEventArg* e);

    TabItem();
    ~TabItem();

    RTTIOfClass(TabItem);

    bool SetFocus();

    bool IsSelected();
    void SetSelected(bool val);

    TabControl* GetTabControlParent();
    Dock GetTabStripPlacement();

protected:

    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnPreviewGotKeyboardFocus(KeyboardFocusEventArg* e);

    virtual void OnSelected(RoutedEventArg* e);
    virtual void OnUnselected(RoutedEventArg* e);

private:

    static bool StaticInit();
    static int CoerceTabStripPlacement(DpObject* d, Object* value);

    bool GetBoolField(BoolField field);
    void SetBoolField(BoolField field, bool val);

    void HandleIsSelectedChanged(bool newValue, RoutedEventArg* e);

    void OnContentChanged(Object* oldContent, Object* newContent);
    void OnContentTemplateChanged(DataTemplate* oldContentTemplate, DataTemplate* newContentTemplate);
    void OnContentTemplateSelectorChanged(DataTemplateSelector* oldContentTemplateSelector, DataTemplateSelector* newContentTemplateSelector);

    friend class TabControl;

private:

    suic::Byte _field;
};

}

#endif
