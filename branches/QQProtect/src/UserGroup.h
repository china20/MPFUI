
#ifndef _USERGROUP_H_
#define _USERGROUP_H_

#include "UserInfo.h"

using namespace suic;

class UserGroup : public NotifyPropChanged
{
public:

    // 定义需要绑定到模版的属性
    // 这样数据会自动绑定到数据模版中进行显示
    // 比如UserGroup会和模版中的Binding Path=UserGroup对应
    BeginMember(UserGroup, NotifyPropChanged)
        MemberString(UserGroup)
        MemberGetObject(ItemsSource) // ItemsSource是固定的，用于拥有子项的树子项
    EndMember()

    RTTIOfClass(UserGroup)

    UserGroup(suic::String grpName)
    {
        _groupName = grpName;
        _userIds.setAutoDelete(false);
    }

    UserGroup()
    {

    }

    suic::String GetUserGroup()
    {
        return _groupName;
    }

    void SetUserGroup(suic::String val)
    {
        _groupName = val;
    }

    suic::Object* GetItemsSource()
    {
        return &_userIds;
    }

    void AddUserId(UserInfo* userId)
    {
        _userIds.AddItem(userId);
    }

private:

    ObservableCollection _userIds;
    // 用户分组名称
    String _groupName;
};

///
/// <Summary>
///  树子项模版选择器（因为用户分组和用户外观不一样，需要绑定不同的模版）
/// </Summary>
///
class UserTemplateSelector : public DataTemplateSelector
{
public:

    UserTemplateSelector(DataTemplate* t1, DataTemplate* t2);
    DataTemplate* SelectTemplate(Object* item, FrameworkElement* container);

private:

    // 用户分组数据模版
    DataTemplate* userGrp;
    // 用户信息模版
    DataTemplate* userId;
};

#endif