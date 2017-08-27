// 华勤科技版权所有 2010-2011
// 
// 文件名：ResEditorWindow.h
// 功  能：资源编辑界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _RESEDITORWINDOW_H_
# define _RESEDITORWINDOW_H_

#include <Main/EditNode.h>
#include <Main/SlnTreeManager.h>
#include <Core/TypeControls.h>
#include <Core/XamlRootElement.h>

using namespace suic;

class ResEditorWindow : public suic::Window
{
public:

    ResEditorWindow(ResourceDictionary* resElem);
    ~ResEditorWindow();

protected:

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

protected:

    ObjTreeManager* _objTree;
};

#endif

