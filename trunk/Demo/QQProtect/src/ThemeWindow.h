// 华勤科技版权所有 2010-2022
// 
// 文件名：ThemeWindow.h
// 功  能：主题界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _THEMEWINDOW_H_
#define _THEMEWINDOW_H_

class ThemeItem : public Object
{
public:

    ThemeItem() {}
    ThemeItem(ImageSource* thump, String title)
    {
        _thumb = thump;
        _title = title;
        _useCount = _U("2236888人使用");
        _provider = _U("由MPF提供");
    }

    BeginMember(ThemeItem, Object)
        MemberString(Title)
        MemberString(UseCount)
        MemberString(Provider)
        MemberObject(Thumb)
    EndMember()

    String GetTitle()
    {
        return _title;
    }

    void SetTitle(String val)
    {
        _title = val;
    }

    String GetUseCount()
    {
        return _useCount;
    }

    void SetUseCount(String val)
    {
        _useCount = val;
    }

    String GetProvider()
    {
        return _provider;
    }

    void SetProvider(String val)
    {
        _provider = val;
    }

    Object* GetThumb()
    {
        return _thumb.get();
    }

    void SetThumb(Object* val)
    {
        _thumb = val;
    }

    String GetThemePath()
    {
        return _themePath;
    }

    void SetThemePath(String val)
    {
        _themePath = val;
    }

private:

    String _title;
    String _useCount;
    String _provider;
    String _themePath;
    ImageSourcePtr _thumb;
};

class ThemeWindow : public suic::Window
{
public:

    ThemeWindow();
    ~ThemeWindow();

    void OnTimer(Object* sender, EventArg* e);
    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnColorThemeClick(Element* sender, RoutedEventArg* e);
    void OnThemeSelectionChanged(Element* sender, SelectionChangedEventArg* e);

protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

};

#endif

