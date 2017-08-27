// 华勤科技版权所有 2010-2011
// 
// 文件名：TemplateEditorWindow.h
// 功  能：模版编辑界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _TEMPLATEEDITORWINDOW_H_
# define _TEMPLATEEDITORWINDOW_H_

#include <Main/FilterNode.h>
#include <Main/SlnTreeManager.h>
#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>

using namespace suic;

class ThemeEditorWindow;

class TemplateEditorWindow : public suic::Window
{
public:

    TemplateEditorWindow(DesignElement* resElem, ThemeEditorWindow* owner);
    ~TemplateEditorWindow();

    void SetRootTemplateElement(TemplateRootItem* tempNode);
    void OnEditTempPropClick(DpObject* sender, RoutedEventArg* e);

    void OnIsVisibleChanged(IsVisibleChangedEventArg* e);

protected:

    void InitTreeDoc();

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

protected:

    ObjTreeManager* _objTree;
    ThemeEditorWindow* _themeWnd;
    TemplateRootItem* _tempElement;

    FrameworkElement* _templateParent;
};

#endif

