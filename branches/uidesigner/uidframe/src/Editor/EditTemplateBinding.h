// 华勤科技版权所有 2010-2011
// 
// 文件名：EditTemplateBinding..h
// 功  能：绑定编辑界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _EDITTEMPLATEBINDING_H_
# define _EDITTEMPLATEBINDING_H_

#include <Main/FilterNode.h>
#include <Main/SlnTreeManager.h>
#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>

using namespace suic;

class EditTemplateBindingWindow : public suic::Window
{
public:

    EditTemplateBindingWindow(RTTIOfInfo* rttiInfo, DpProperty* dp);
    ~EditTemplateBindingWindow();

    String GetBindingProp() const;

    void OnButtonClick(Element* sender, RoutedEventArg* e);
    void OnItemSelectionChanged(Element* sender, SelectionChangedEventArg* e);

protected:

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

private:

    DpProperty* _dp;
    RTTIOfInfo* _rttiInfo;

    String _bindingProp;
};

#endif
