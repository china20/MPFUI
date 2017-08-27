// 华勤科技版权所有 2010-2011
// 
// 文件名：ResSelectorWindow.h
// 功  能：资源类型选择界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _RESTYPEWINDOW_H_
# define _RESTYPEWINDOW_H_

#include <Main/FilterNode.h>
#include <Main/SlnTreeManager.h>
#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>

using namespace suic;

class ResSelectorWindow : public suic::Window
{
public:

    ResSelectorWindow();
    ~ResSelectorWindow();

    ResTypeItem* GetSelectedItem();
    TypeItem* GetSelectedControl();

    bool IsFromTypeChecked();

    void OnDbListClick(Element* sender, MouseButtonEventArg* e);
    void OnCtrlListDbClick(Element* sender, MouseButtonEventArg* e);

    void OnSelectedChanged(suic::Element* sender, suic::SelectionChangedEventArg* e);

protected:

    void InitTreeDoc();

    int IsCanClose(ResTypeItem* resItem);
    ResTypeItem* GetCurrentSelItem();

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

private:

    bool _isDbClicked;
};

#endif
