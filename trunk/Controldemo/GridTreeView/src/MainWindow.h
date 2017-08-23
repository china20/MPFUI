// 华勤科技版权所有 2010-2011
// 
// 文件名：MainWindow.h
// 功  能：主界面窗口。
// 
// 作  者：Sharpui开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "ChildWindow.h"

class TLData : public suic::NotifyPropChanged
{
public:

    BeginMember(TLData, suic::NotifyPropChanged)
        MemberGetInt(ShowItem)
        MemberBool(Expanded)
        MemberGetInt(ShowOra)
        MemberGetInt(ShowDep)
    EndMember()

    RTTIOfClass(TLData)

    TLData()
    {
        _expanded = true;
        _parent = NULL;
    }

    ~TLData()
    {
        Clear();
    }

    void Clear()
    {
        for (int i = 0; i < _kids.Length(); ++i)
        {
            _kids.GetItem(i)->unref();
        }
        _kids.Clear();
    }

    void AddItem(TLData* val)
    {
        _kids.Add(val);
        val->_parent = this;
        val->ref();
    }

    int GetShowItem() 
    {
        if (IsNeedShow())
        {
            return suic::Visibility::Visible;
        }
        else
        {
            return suic::Visibility::Collapsed;
        }
    }

    int GetShowOra()
    {
        return GetShowTemp(0);
    }

    int GetShowDep()
    {
        return GetShowTemp(1);
    }

    bool GetExpanded()
    {
        return _expanded;
    }

    void SetExpanded(bool val)
    {
        _expanded = val;
        NotifyChildShow();
    }

    int GetCount() const
    {
        return _kids.GetCount();
    }

    bool IsNeedShow()
    {
        if (NULL == _parent)
        {
            return true;
        }
        else
        {
            TLData* p = _parent;
            while (NULL != p)
            {
                if (!p->_expanded)
                {
                    return false;
                }
                p = p->_parent;
            }
        }
        return true;
    }

    TLData* GetItem(int index) const
    {
        return _kids.GetItem(index);
    }

protected:

    void NotifyChildShow()
    {
        NotifyChanged("");
        for (int i = 0; i < GetCount(); ++i)
        {
            GetItem(i)->NotifyChildShow();
        }
    }

    virtual int GetShowTemp(int type)
    {
        return suic::Visibility::Hidden;
    }

protected:

    // 父元素是否展开
    bool _expanded;
    TLData* _parent;
    suic::Array<TLData*> _kids;
};

class OranizeData : public TLData
{
public:

    BeginMember(OranizeData, TLData)
        MemberGetString(Oranize)
    EndMember()

    OranizeData(suic::String val)
    {
        _oranize = val;
    }

    suic::String GetOranize()
    {
        return _oranize;
    }

    int GetShowTemp(int type)
    {
        if (0 == type)
        {
            return suic::Visibility::Visible;
        }
        return suic::Visibility::Hidden;
    }

private:

    suic::String _oranize;
};

class DepData : public TLData
{
public:

    BeginMember(DepData, TLData)
        MemberGetString(Dep)
    EndMember()

    DepData(suic::String val)
    {
        _dep = val;
    }

    suic::String GetDep()
    {
        return _dep;
    }

    int GetShowTemp(int type)
    {
        if (1 == type)
        {
            return suic::Visibility::Visible;
        }
        return suic::Visibility::Hidden;
    }

private:

    suic::String _dep;
};

class PersonData : public TLData
{
public:

    BeginMember(PersonData, TLData)
        MemberGetString(Name)
        MemberGetString(Addr)
    EndMember()

    PersonData(suic::String n, suic::String addr)
    {
        _name = n;
        _addr = addr;
    }

    suic::String GetName()
    {
        return _name;
    }

    suic::String GetAddr()
    {
        return _addr;
    }

private:

    suic::String _name;
    suic::String _addr;
};

class MainWindow : public suic::Window
{
public:

    MainWindow();
    ~MainWindow();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnButtonClicked(suic::Element* sender, suic::RoutedEventArg* e);
 
protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

private:

    void RegisterButtonEvent();
    void AddDataToListView(suic::ListView* pList, TLData* pData);

private:

    ChildWindow* _openWnd;
};

#endif

