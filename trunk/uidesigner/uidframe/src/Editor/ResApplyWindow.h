// 华勤科技版权所有 2010-2011
// 
// 文件名：ResApplyWindow.h
// 功  能：资源类型选择界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _RESAPPLYWINDOW_H_
# define _RESAPPLYWINDOW_H_

#include <Main/FilterNode.h>
#include <Main/SlnTreeManager.h>
#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>

using namespace suic;

class ResApplyItem : public suic::Object
{
public:

    BeginMember(ResApplyItem,suic::Object)
        MemberGetString(Name)
        MemberGetString(Path)
    EndMember()

    ResApplyItem(ResourceDicRootItem* resRootItem)
    {
        _resRootItem = resRootItem;
    }

    suic::String GetName()
    {
        return _resRootItem->GetName();
    }

    suic::String GetPath()
    {
        return _resRootItem->GetRelativePath();
    }

    ResourceDicRootItem* GetResourceDicRootItem() const
    {
        return _resRootItem;
    }

private:

    ResourceDicRootItem* _resRootItem;
};

class ResApplyWindow : public suic::Window
{
public:

    BeginRoutedEvent(ResApplyWindow, suic::Window)
        MemberRouted(OnApplyButtonClick)
        MemberRouted(OnRemoveButtonClick)
        MemberRouted(OnUpMoveButtonClick)
        MemberRouted(OnDownMoveButtonClick)
        MemberRouted(OnOkButtonClick)
        MemberRouted(OnCancelButtonClick)
    EndRoutedEvent()

    ResApplyWindow(FileRootItem* rootItem);
    ~ResApplyWindow();

protected:

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void InitAppliedResource();
    bool ExistResourceDicRootItem(ResourceDicRootItem* dicRootItem);

    void OnApplyButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnRemoveButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnUpMoveButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnDownMoveButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnOkButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnCancelButtonClick(suic::DpObject* sender, suic::RoutedEventArg* e);

private:

    FileRootItem* _rootItem;
    suic::ListBox* _listBox;
    suic::ListBox* _listBoxApply;
    ResourceDictionaryNode* _resDicNode;

};

#endif
