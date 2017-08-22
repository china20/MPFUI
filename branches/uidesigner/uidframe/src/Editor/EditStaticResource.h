// 华勤科技版权所有 2010-2011
// 
// 文件名：EditStaticResource.h
// 功  能：绑定编辑界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _EDITSTATICRESOURCE_H_
# define _EDITSTATICRESOURCE_H_

#include <Main/FilterNode.h>
#include <Main/SlnTreeManager.h>
#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>

using namespace suic;

class EditStaticResourceWindow : public suic::Window
{
public:

    EditStaticResourceWindow(ResourceDictionaryNode* resDicNode, DesignElement* target, DpProperty* dp, ResNode* resNode);
    ~EditStaticResourceWindow();

    String GetStaticResource();

    void OnButtonClick(Element* sender, RoutedEventArg* e);
    void OnItemSelectionChanged(Element* sender, SelectionChangedEventArg* e);

protected:

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    SetterNode* HandlePropItem(SetterEditor* setterEditor, DpItem* dpItem, SetterNode* pSetter);

private:

    ResourceDictionaryNode* _resDicNode;
    ResNode* _resNode;

    DesignElement* _target;
    DpProperty* _dp;

    String _strResourceKey;
    EditorControl* _prevEditCtrl;

};

#endif
