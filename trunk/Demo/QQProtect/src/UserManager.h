
#ifndef _USERMANAGER_H_
#define _USERMANAGER_H_

#include <System/Graphics/Bitmap.h>

class UseId : public suic::NotifyPropChanged
{
public:

    BeginMember(UseId, NotifyPropChanged)
        MemberString(UserNumber)
    EndMember()

    RTTIOfClass(UseId)

    UseId();
    virtual ~UseId();

    suic::String GetUserId() const;

    suic::String GetUserNumber();
    void SetUserNumber(suic::String val);

    int GetUserType() const;
    void SetUserType(int val);

private:
    
    int _userType;
    bool _isContact;
    suic::String _userNumber;
};

class UserUseId : public UseId
{
public:

    BeginMember(UserUseId, UseId)
        MemberString(UserName)
        MemberString(UserPwd)
        MemberGetBool(EnableGrey)
        MemberString(UserMark)
        MemberGetObject(HeadIcon)
    EndMember()

    RTTIOfClass(UserUseId)

    UserUseId();
    UserUseId(ImageSource* headIcon);
    ~UserUseId();

    String GetUserName();
    void SetUserName(String usrName);

    String GetUserPwd();
    void SetUserPwd(String usrPwd);

    String GetUserMark();
    void SetUserMark(String val);

    bool GetEnableGrey();

    suic::Object* GetHeadIcon();
    void SetHeadIcon(suic::Object* val);

    bool IsLogin()
    {
        return _isLogin;
    }

    void SetIsLogin(bool val)
    {
        _isLogin = val;
    }

private:

    // 用户显示名称
    String _userName;
    // 用户密码(MD5加密)
    String _userPwd;
    // 用户备注信息
    String _userMark;

    // 用户目前使用的主题标识
    String _userThemeId;

    // 用户会员级别（目前为1-7级）
    int _userVipLevel;

    // 用户是否安装摄像头
    bool _hasCamara;
    // 用户是否在线
    bool _isOnLine;
    // 用户是否登录
    bool _isLogin;

    // 用户头像
    ImageSourcePtr _headIcon;
};

class GroupUseId : public UseId
{
public:

    RTTIOfClass(GroupUseId)

    GroupUseId();
    ~GroupUseId();
};

class HierarchyGroup : public suic::HierarchicalItem
{
public:

    // 定义需要绑定到模版的属性
    // 这样数据会自动绑定到数据模版中进行显示
    // 比如UserGroup会和模版中的Binding Path=UserGroup对应
    BeginMember(HierarchyGroup, HierarchicalItem)
        MemberString(UserGroup)
    EndMember()

    RTTIOfClass(HierarchyGroup)

    HierarchyGroup(suic::String name);
    HierarchyGroup();

    suic::String GetUserGroup();
    void SetUserGroup(suic::String val);

private:

    // 分组名称
    suic::String _groupName;
};

///
/// <Summary>
///  分类分组，比如一般用户；群；讨论组等
/// <Summary/>
///
class ClassifyGroup : public suic::ObservableCollection
{
public:

    ClassifyGroup();
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

    // 用户信息模版
    DataTemplate* userId;
    // 用户分组数据模版
    DataTemplate* userGrp;
};

class UserManager
{
public:

    static UserManager* Ins();

    UseId* FindUserId(suic::String userId);
    void AddUserId(UseId* userId);

    UserUseId* GetCurrentUser() const;

    ClassifyGroup* GetFriendGroup();
    ClassifyGroup* GetGroupGroup();
    ClassifyGroup* GetDiscussGroup();

private:

    // 测试数据
    void InitTestUserMarks();
    void InitTestUsers();
    void InitTestUserGroup(String groupName, int iCount);

private:

    UserManager();
    ~UserManager();

    // 当前登录用户信息
    UserUseId* _currUser;
    ClassifyGroup* _friendGrp;

    // 所有用户信息，包括好友和陌生人
    suic::StringAutoDic<UseId> _users;

    // 测试数据
    String _userMarks[10];
};

#endif

