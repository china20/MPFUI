// 华勤科技版权所有 2010-2011
// 
// 文件名：TabControl.h
// 功  能：Tab控件。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UITABCONTROL_H_
#define _UITABCONTROL_H_

#include <Extend/suiwgxconfig.h>
#include <Extend/Controls/TabItem.h>
#include <Extend/Controls/TabPanel.h>
#include <Framework/Controls/Selector.h>
#include <Framework/Presenter/ContentPresenter.h>

namespace suic
{

class SUIWGX_API TabControl : public Selector
{
public:

    static DpProperty* ContentTemplateProperty;
    static DpProperty* ContentTemplateSelectorProperty;
    static DpProperty* SelectedContentProperty;
    static DpProperty* SelectedContentTemplateProperty;
    static DpProperty* SelectedContentTemplateSelectorProperty;
    
    static DpProperty* TabArrangeModeProperty;
    static DpProperty* TabStripPlacementProperty;

    static void OnTabArrangeModePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnTabStripPlacementPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static bool StaticInit();

    TabControl();
    ~TabControl();

    RTTIOfClass(TabControl);

    TabItem* GetSelectedTabItem();

    DataTemplate* GetContentTemplate();
    void SetContentTemplate(DataTemplate* val);

    DataTemplateSelector* GetContentTemplateSelector();
    void SetContentTemplateSelector(DataTemplateSelector* val);

    Object* GetSelectedContent();
    void SetSelectedContent(Object* val);

    ContentPresenter* GetSelectedContentPresenter();

    TabArrangeMode GetTabArrangeMode();
    void SetTabArrangeMode(TabArrangeMode val);

    DataTemplate* GetSelectedContentTemplate();
    void SetSelectedContentTemplate(DataTemplate* val);

    void SetSelectedContentTemplateSelector(DataTemplateSelector* val);
    DataTemplateSelector* GetSelectedContentTemplateSelector();

    Dock GetTabStripPlacement();
    void SetTabStripPlacement(Dock val);

    Element* GetContainerForItemOverride();
    bool IsItemItsOwnContainer(Object* item);

    Element* FindName(const String& name);

protected:

    void OnApplyTemplate();
    void OnInitialized(EventArg* e);

    void OnArrange(const Size& arrangeSize);

    void OnKeyDown(KeyboardEventArg* e);
    void OnMouseWheel(MouseWheelEventArg* e);

    void OnItemsChanged(NotifyCollChangedEventArg* e);
    void OnSelectionChanged(SelectionChangedEventArg* e);

    void PrepareContainerForItemOverride(DpObject* elem, ItemEntry* item);
    void OnTabNavigateClick(Element* sender, RoutedEventArg* e);
    
private:

    void UpdateSelectedContent();
    void AddButtonClick(const String& name);
    void CoerceTabItemStripPlacement();

    TabItem* FindNextTabItem(int startIndex, int direction);

private:

    TabPanel* _tabPanel;
    friend class TabPanel;
};

}

#endif
