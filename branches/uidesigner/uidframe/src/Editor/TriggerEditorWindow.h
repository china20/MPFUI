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

class TriggerEditorWindow : public SetterEditorWindow
{
public:

    TriggerEditorWindow(SetterCollectionNode* setterColl, TriggerCollectionNode* trgs, ThemeEditorWindow* owner, eTriggerType trgType);
    ~TriggerEditorWindow();

    ePropertyType GetPropertyType();

    void SetSetterColl(SetterCollectionNode* setterColl);
    void SetTriggerCollectionNode(TemplateRootItem* tempRoot, TriggerCollectionNode* trgs);
    void UpdateSelectedPropShow();

    void Reflesh();

    void OnEditStylePropClick(DpObject* sender, RoutedEventArg* e);

    void OnAddTriggerButton(Element* sender, RoutedEventArg* e);

    void OnOpTriggerButton(Element* sender, RoutedEventArg* e);

    void OnAddPropButton(Element* sender, RoutedEventArg* e);
    void OnDeletePropButton(Element* sender, RoutedEventArg* e);

    void OnSelTriggerChanged(Element* sender, RoutedPropChangedEventArg* e);
    void OnSelTriggerItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e);
    void OnDbClickTree(Element* sender, MouseButtonEventArg* e);

protected:

    TreeView* GetTreeView();

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);
    void OnIsVisibleChanged(suic::IsVisibleChangedEventArg* e);

    void ShowTriggerSetterNode(SetterNode* resNode);
    suic::RTTIOfInfo* GetOwnerRTTIInfo();

    void SelectToSetterNode(SetterNode* pSetterNode);
    SetterNode* HandlePropItem(SetterEditor* setterEditor, DpItem* dpItem, SetterNode* pSetter);

protected:

    SetterEditor* _prevSetterEditor;
    TreeView* _propTreeView;
    TriggerNode* _trigger;
    TemplateRootItem* _tempRootItem;
    TriggerCollectionNode* _triggers;
    SetterCollectionNode* _setterColl;

    suic::Button* _addProp;
    suic::Button* _delProp;
    suic::ComboBox* _propCmb;

    eTriggerType _triggerType;

    QueryDpCond _cond;
};

#endif

