// 华勤科技版权所有 2010-2011
// 
// 文件名：TriggerEditorWindow.h
// 功  能：Trigger编辑界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _TRIGGEREDITORWINDOW_H_
# define _TRIGGEREDITORWINDOW_H_

#include <Editor/SetterEditorWindow.h>

using namespace suic;

class EditPropItem : public Object
{
public:

    EditPropItem(DpProperty* dp)
    {
        _prop = dp;
    }

    String ToString()
    {
        return _prop->GetName();
    }

private:

    DpProperty* _prop;
};

class TriggerEditorWindow : public SetterEditorWindow
{
public:

    TriggerEditorWindow(SetterCollectionNode* setterColl, TriggerCollectionNode* trgs, ThemeEditorWindow* owner);
    ~TriggerEditorWindow();

    void SetSetterColl(SetterCollectionNode* setterColl);
    void SetTriggerCollectionNode(TriggerCollectionNode* trgs);
    void Reflesh();

    void OnEditStylePropClick(Element* sender, RoutedEventArg* e);

    void OnAddTriggerButton(Element* sender, RoutedEventArg* e);
    void OnAddMultiTriggerButton(Element* sender, RoutedEventArg* e);

    void OnOpTriggerButton(Element* sender, RoutedEventArg* e);

    void OnAddPropButton(Element* sender, RoutedEventArg* e);
    void OnDeletePropButton(Element* sender, RoutedEventArg* e);

    void OnSelItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e);
    void OnDbClickTree(Element* sender, MouseButtonEventArg* e);

protected:

    TreeView* GetTreeView();

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    SetterNode* HandlePropItem(SetterEditor* setterEditor, DpItem* dpItem, SetterNode* pSetter);

protected:

    ThemeEditorWindow* _themeWnd;
    SetterEditor* _prevSetterEditor;

    TreeView* _propTreeView;

    TriggerBaseNode* _currentTrigger;
    TriggerCollectionNode* _triggers;
    SetterCollectionNode* _setterColl;

    suic::Button* _addProp;
    suic::Button* _delProp;
    suic::ComboBox* _propCmb;
};

#endif

