// 华勤科技版权所有 2010-2011
// 
// 文件名：PropSelectorWindow.h
// 功  能：属性选择界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _PROPSELECTORWINDOW_H_
# define _PROPSELECTORWINDOW_H_

#include <Main/FilterNode.h>
#include <Main/SlnTreeManager.h>
#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>

using namespace suic;

class TargetNameItem : public Object
{
public:

    TargetNameItem(const String& name, RTTIOfInfo* targetType)
    {
        _targetName = name;
        _targetType = targetType;
    }

    String ToString()
    {
        return _targetName;//String().Format(_U("%s:%s"), _targetName.c_str(), _targetType->typeName);
    }

    suic::String GetTargetName() const
    {
        return _targetName;
    }

    RTTIOfInfo* GetTargetType() const
    {
        return _targetType;
    }

private:

    suic::String _targetName;
    RTTIOfInfo* _targetType;
};

class PropSelectorWindow : public suic::Window
{
public:

    PropSelectorWindow();
    ~PropSelectorWindow();

    void SetQueryDpCond(const QueryDpCond& cond);

    String GetTargetName();
    void SetTargetElements(suic::ItemCollection* itemColl);

    DpItem* GetSelectedItem();
    void OnDbListClick(Element* sender, MouseButtonEventArg* e);

protected:

    void InitTreeDoc();
    void ChangedTargetNameItem(TargetNameItem* targetItem);

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnTargetNameChanged(Element* sender, SelectionChangedEventArg* e);

private:

    QueryDpCond _cond;
    suic::ItemCollection* _targetElements;
};

#endif
