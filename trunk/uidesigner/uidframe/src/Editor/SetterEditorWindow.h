// 华勤科技版权所有 2010-2011
// 
// 文件名：SetterEditorWindow.h
// 功  能：属性编辑界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _SETTEREDITORWINDOW_H_
# define _SETTEREDITORWINDOW_H_

#include <Main/FilterNode.h>
#include <Main/SlnTreeManager.h>

#include <Editor/PropSelectorWindow.h>

#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>
#include <Core/ValueEditor.h>
#include <Core/TypeManager.h>
#include <Core/TypeControls.h>

using namespace suic;

class SetterEditorWindow : public suic::Window
{
public:

    SetterEditorWindow(ThemeEditorWindow* themeWnd);
    ~SetterEditorWindow();

    SetterNode* SwitchToProperty(RTTIOfInfo* targetType, DpItem* dpItem, SetterNode* pSetter, String targetName, ePropertyType fromTrigger);
    bool OpenPropSelectorWindow(Element* sender, PropSelectorWindow* propWnd, PlacementMode mode);

    void ShowSetterNode(SetterNode* resNode, SetterCollectionNode* setterColl, ePropertyType fromTrigger);

protected:

    void HandlePrevSetterNode();

    virtual TreeView* GetTreeView();
    virtual void SelectToSetterNode(SetterNode* pSetterNode);
    virtual SetterNode* HandlePropItem(SetterEditor* setterEditor, DpItem* dpItem, SetterNode* pSetter);

protected:

    ThemeEditorWindow* _themeWnd;
    SetterEditor* _prevSetterEditor;
};

#endif

