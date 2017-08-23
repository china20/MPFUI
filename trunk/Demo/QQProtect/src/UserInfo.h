
#ifndef _USERINFO_H_
#define _USERINFO_H_

#include <Extend/RichText/TextBox.h>

using namespace suic;

// 这里继承至NotifyPropChanged，这样用户定义的业务类属性变化时
// 就会拥有通知界面改变的能力
// 继承ICommand接口，命令改变（比如按钮）时拥有命令执行能力
class UserInfo : public ICommand, public NotifyPropChanged
{
public:

    BeginMember(UserInfo, NotifyPropChanged)
        MemberString(UserName)
        MemberString(UserPwd)
        MemberBool(EnableGrey)
        MemberString(UserNumber)
        MemberGetBool(IsHidden)
        MemberString(UserMark)
        MemberGetObject(HeadIcon)
    EndMember()

    RTTIOfClass(UserInfo)

    UserInfo()
    {
        _isHidden = true;
        _isLogin = false;
    }

    UserInfo(String usrName, String usrPwd);

    UserInfo(String usrName, String strNumber, String userMark, suic::Object* val)
    {
        _usrName = usrName;
        _userNumber = strNumber;
        _userMark = userMark;
        _headIcon = val;
        _isHidden = true;
        _isLogin = false;
    }

    bool CanExecute(Object* target, Object* param);
    void Execute(Object* target, Object* param);

    String GetUserName();
    void SetUserName(String usrName);

    String GetUserPwd();
    void SetUserPwd(String usrPwd);

    String GetUserNumber()
    {
        return _userNumber;
    }

    void SetUserNumber(String val)
    {
        _userNumber = val;
    }

    String GetUserMark()
    {
        return _userMark;
    }

    void SetUserMark(String val)
    {
        _userMark = val;
    }

    bool GetIsHidden()
    {
        return _isHidden;
    }

    void SetIsHidden(bool val)
    {
        _isHidden = val;
    }

    bool GetEnableGrey()
    {
        return !_isLogin;
    }

    void SetEnableGrey(bool val)
    {
    }

    suic::Object* GetHeadIcon()
    {
        return _headIcon.get();
    }

    void SetHeadIcon(suic::Object* val)
    {
        _headIcon = val;
    }

    bool GetIsLogin() const
    {
        return _isLogin;
    }

    void SetIsLogin(bool val)
    {
        _isLogin = val;
    }

    void SetBubbleBk(Brush* brush)
    {
        _bubbleBk = brush;
    }

    Brush* GetBubbleBk()
    {
        return _bubbleBk.get();
    }

    suic::String UserInfo::ToString()
    {
        return L"登  录";
    }

private:

    // 用户显示名称
    String _usrName;
    // 用户密码(MD5加密)
    String _usrPwd;
    // 用户备注信息
    String _userMark;
    // 用户QQ号码
    String _userNumber;

    // 用户目前使用的主题标识
    String _userThemeId;

    // 用户会员级别（目前为1-7级）
    int _userVipLevel;
    
    // 用户是否安装摄像头
    bool _hasCamara;
    // 用户是否登录
    bool _isLogin;
    // 用户是否隐身
    bool _isHidden;
    
    // 用户头像
    BitmapPtr _headIcon;
    // 用户QQ秀头像
    BitmapPtr _qqShowHead;
    BrushPtr _bubbleBk;
};

#endif
