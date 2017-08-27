// 华勤科技版权所有 2010-2011
// 
// 文件名：BindingEditorWindow.h
// 功  能：绑定编辑界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _BINDINGEDITORWINDOW_H_
# define _BINDINGEDITORWINDOW_H_

#include <Main/FilterNode.h>
#include <Main/SlnTreeManager.h>
#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>

using namespace suic;

class BindingEditorWindow : public suic::Window
{
public:

    BindingEditorWindow(DesignElement* target, DpProperty* dp, ResourceDictionaryNode* resDicNode, ResNode* resNode);
    ~BindingEditorWindow();

    ResNode* GetResNode() const;
    void SetTemplateParent(TemplateRootItem* tempParent);
    void SetResourceElement(DesignElement* target);

protected:

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnButtonClick(Element* sender, RoutedEventArg* e);
    void OnItemSelectionChanged(Element* sender, SelectionChangedEventArg* e);

    void InitStaticResource();
    void InitTemplateBinding();

    void FilterResources(ItemCollection* itemColl);

    void SwitchToResNode(ResNode* resNode);

    void InitBindingValue();
    bool UpdateBindingValue();

    void ResetStaticResource(suic::ListBox* pListBox);

private:

    DpProperty* _targetDp;
    DesignElement* _target;
    DesignElement* _resTarget;

    TemplateRootItem* _tempParent;

    ResNode* _resNode;
    ResourceDictionaryNode* _resDicNode;
    
    EditorControl* _prevEditCtrl;

    RadioButton* _binding;
    RadioButton* _templateBinding;
    RadioButton* _staticResource;
    RadioButton* _dynamicResource;

    suic::TextBox* _pathText;
    suic::TextBox* _dynamicText;

};

#endif
