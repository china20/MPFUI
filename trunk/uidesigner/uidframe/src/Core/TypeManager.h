
#ifndef _IVALUEEDITOR_H_
#define _IVALUEEDITOR_H_

#include <Core/ResNode.h>
#include <Core/DpManager.h>
#include <Main/XamlRootItem.h>
#include <System/Types/Const.h>

class ResTypeItem : public Object
{
public:

    ResTypeItem(RTTIOfInfo* rttiInfo, const String& name)
    {
        _rttiInfo = rttiInfo;
        _name = name;
    }

    RTTIOfInfo* GetType() const
    {
        return _rttiInfo;
    }

    String ToString()
    {
        return _name;
    }

private:

    String _name;
    RTTIOfInfo* _rttiInfo;
};

class TypeItem : public Object
{
public:

    TypeItem(RTTIOfInfo* rttiInfo)
    {
        _rttiInfo = rttiInfo;
    }

    RTTIOfInfo* GetType() const
    {
        return _rttiInfo;
    }

    String ToString()
    {
        return _rttiInfo->typeName;
    }

private:

    RTTIOfInfo* _rttiInfo;
};

class TypePool
{
public:

    static void GetTypeItems(ItemCollection* itemColl);
    static void GetTypeItems(ItemCollection* itemColl, const String& grp);

    //static int GetCount();
    //static RTTIOfInfo* GetTypeItem(int index);

private:

    static ObservableCollection* _panelTypes;
    static ObservableCollection* _ctrlsTypes;
    static ObservableCollection* _shapeTypes;
    static ObservableCollection* _otherTypes;

    static void InitTypeItems();
    static void CompTypeItems(ObservableCollection* ori, ItemCollection* dest);

    TypePool() {}
};

class ResTypePool
{
public:

    static ItemCollection* GetTypeItems();
    static int GetCount();
    static RTTIOfInfo* GetTypeItem(int index);
    static String GetResTypeName(RTTIOfInfo* rttiInfo);

private:

    static ObservableCollection* _types;
    static ObjectDic<RTTIOfInfo*, String> _resTypeMap;

    ResTypePool() {}
};

/*
struct OwnerItem
{
    suic::RTTIOfInfo* owner;
    suic::DpProperty* prop;

    OwnerItem()
    {
        owner = NULL;
        prop = NULL;
    }

    OwnerItem(suic::RTTIOfInfo* o, suic::DpProperty* d)
    {
        owner = o;
        prop = d;
    }
};

#define OWNERITEM(prop,owner) OwnerItem(owner##::##RTTIType(), owner##::##prop##Property)
#define NEWPROPITEMDEF(prop,owner) new PropItem(#prop, _U(""), OWNERITEM(prop, owner))
#define NEWPROPITEM(prop,editor,owner) new PropItem(#prop, #editor, OWNERITEM(prop, owner))

struct PropItem : public Object
{
    suic::String name;
    // ÊôÐÔ±à¼­Æ÷Ãû×Ö
    suic::String editor;
    suic::Array<OwnerItem> owners;

    PropItem(const suic::String& prop, const suic::String& ename, OwnerItem owner)
        : Object(false)
    {
        name = prop;
        editor = ename + _U("_Prop");
        owners.Add(owner);
    }

    String ToString()
    {
        return name;
    }

    DpProperty* MapProperty(suic::RTTIOfInfo* rttiInfo);
    bool IsOwner(suic::RTTIOfInfo* rttiInfo);

    virtual suic::String ValueToString(ResNode* val)
    {
        return val->GetSingleXml();
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        ;
    }

    static PropItem* AddProperty(DpProperty* dp);

    static PropItem* FindPropItem(const String& name, RTTIOfInfo* rttiInfo);

    static PropItem* FindPropItem(const String& name);
    static PropItem* FindLayoutPropItem(const String& name);
    static PropItem* FindAttachPropItem(const String& name);

    static bool ContainsProp(suic::ItemCollection* pColl, const String& name);
    static void GetPropItems(suic::RTTIOfInfo* rttiInfo, suic::ItemCollection* pColl, suic::ItemCollection* pExistColl);

    static void InitControlProps();

    static suic::Array<PropItem*> props;
    static suic::Array<PropItem*> layoutProps;
    static suic::Array<PropItem*> attachProps;

    static suic::StringDic<int> baseTypes;

private:

    static PropItem* LookupPropItem(const String& name, suic::Array<PropItem*>& items);
};

struct ChoicePropItem : public PropItem
{
public:

    ChoicePropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : PropItem(prop, ename, dp)
    {
    }

    suic::String ValueToXml(ResNode* val)
    {
        return ValueToString(val);
    }

    int GetChoiceIndex(ResNode* val, Integer* pDef)
    {
        int iVal = 0;

        pDef->ref();

        if (NULL == val->GetValue())
        {
            val->SetValue(pDef);
        }

        pDef->unref();
        
        iVal = val->GetValue()->ToInt();
        return iVal;
    }
};

struct DockPropItem : public ChoicePropItem
{
public:

    DockPropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("Left"));
        sender->AddText(_U("Top"));
        sender->AddText(_U("Right"));
        sender->AddText(_U("Bottom"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, DockBox::LeftBox);

        if (iVal == Dock::dockTop)
        {
            return _U("Top");
        }
        else if (iVal == Dock::dockRight)
        {
            return _U("Right");
        }
        else if (iVal == Dock::dockBottom)
        {
            return _U("Bottom");
        }
        else
        {
            return _U("Left");
        }
    }
};

struct HoriAlignPropItem : public ChoicePropItem
{
public:

    HoriAlignPropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("Left"));
        sender->AddText(_U("Right"));
        sender->AddText(_U("Center"));
        sender->AddText(_U("Stretch"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, HoriAlignBox::StretchBox);

        if (iVal == HoriAlign::haLeft)
        {
            return _U("Left");
        }
        else if (iVal == HoriAlign::haRight)
        {
            return _U("Right");
        }
        else if (iVal == HoriAlign::haCenter)
        {
            return _U("Center");
        }
        else
        {
            return _U("Stretch");
        }
    }
};

struct VertAlignPropItem : public ChoicePropItem
{
public:

    VertAlignPropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("Top"));
        sender->AddText(_U("Bottom"));
        sender->AddText(_U("Center"));
        sender->AddText(_U("Stretch"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, VertAlignBox::StretchBox);

        if (iVal == VertAlign::vaTop)
        {
            return _U("Top");
        }
        else if (iVal == VertAlign::vaBottom)
        {
            return _U("Bottom");
        }
        else if (iVal == VertAlign::vaCenter)
        {
            return _U("Center");
        }
        else
        {
            return _U("Stretch");
        }
    }
};

struct OrientationPropItem : public ChoicePropItem
{
public:

    OrientationPropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("Horizontal"));
        sender->AddText(_U("Vertical"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, OrientationBox::HorizontalBox);

        if (iVal == Orientation::Horizontal)
        {
            return _U("Horizontal");
        }
        else
        {
            return _U("Vertical");
        }
    }
};

struct VisibilityPropItem : public ChoicePropItem
{
public:

    VisibilityPropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("Visible"));
        sender->AddText(_U("Hidden"));
        sender->AddText(_U("Collapsed"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, VisibilityBox::VisibleBox);

        if (iVal == Visibility::Collapsed)
        {
            return _U("Collapsed");
        }
        else if (iVal == Visibility::Hidden)
        {
            return _U("Hidden");
        }
        else
        {
            return _U("Visible");
        }
    }
};

struct WindowStatePropItem : public ChoicePropItem
{
public:

    WindowStatePropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("Normal"));
        sender->AddText(_U("Minimized"));
        sender->AddText(_U("Maximized"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, WindowStateBox::NormalBox);

        if (iVal == WindowState::wsMinimized)
        {
            return _U("Minimized");
        }
        else if (iVal == WindowState::wsMaximized)
        {
            return _U("Maximized");
        }
        else
        {
            return _U("Normal");
        }
    }
};

struct WindowStylePropItem : public ChoicePropItem
{
public:

    WindowStylePropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("None"));
        sender->AddText(_U("SingleBorderWindow"));
        sender->AddText(_U("ThreeDBorderWindow"));
        sender->AddText(_U("ToolWindow"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, WindowStyleBox::NoneBox);

        if (iVal == WindowStyle::wsSingleBorderWindow)
        {
            return _U("SingleBorderWindow");
        }
        else if (iVal == WindowStyle::wsThreeDBorderWindow)
        {
            return _U("ThreeDBorderWindow");
        }
        else if (iVal == WindowStyle::wsToolWindow)
        {
            return _U("ToolWindow");
        }
        else
        {
            return _U("None");
        }
    }
};

struct ResizeModePropItem : public ChoicePropItem
{
public:

    ResizeModePropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("NoResize"));
        sender->AddText(_U("CanMinimize"));
        sender->AddText(_U("CanResize"));
        sender->AddText(_U("CanResizeWithGrip"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, ResizeModeBox::NoResizeBox);

        if (iVal == ResizeMode::CanMinimize)
        {
            return _U("CanMinimize");
        }
        else if (iVal == ResizeMode::CanResize)
        {
            return _U("CanResize");
        }
        else if (iVal == ResizeMode::CanResizeWithGrip)
        {
            return _U("CanResizeWithGrip");
        }
        else
        {
            return _U("NoResize");
        }
    }
};

struct PlacementPropItem : public ChoicePropItem
{
public:

    PlacementPropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("Left"));
        sender->AddText(_U("Top"));
        sender->AddText(_U("Right"));
        sender->AddText(_U("Bottom"));

        sender->AddText(_U("Center"));
        sender->AddText(_U("Mouse"));
        sender->AddText(_U("MousePoint"));
        sender->AddText(_U("Custom"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, PlacementModeBox::MouseBox);

        if (iVal == PlacementMode::pmLeft)
        {
            return _U("Left");
        }
        else if (iVal == PlacementMode::pmTop)
        {
            return _U("Top");
        }
        else if (iVal == PlacementMode::pmRight)
        {
            return _U("Right");
        }
        else if (iVal == PlacementMode::pmBottom)
        {
            return _U("Bottom");
        }
        else if (iVal == PlacementMode::pmCenter)
        {
            return _U("Center");
        }
        else if (iVal == PlacementMode::pmMousePoint)
        {
            return _U("MousePoint");
        }
        else if (iVal == PlacementMode::pmCustom)
        {
            return _U("Custom");
        }
        else
        {
            return _U("Mouse");
        }
    }
};

struct TextAlignmentPropItem : public ChoicePropItem
{
public:

    TextAlignmentPropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("Left"));
        sender->AddText(_U("Right"));
        sender->AddText(_U("Center"));
        sender->AddText(_U("Justify"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, new Integer(TextAlignment::tLeft));

        if (iVal == TextAlignment::tLeft)
        {
            return _U("Left");
        }
        else if (iVal == TextAlignment::tRight)
        {
            return _U("Right");
        }
        else if (iVal == TextAlignment::tCenter)
        {
            return _U("Center");
        }
        else
        {
            return _U("Justify");
        }
    }
};

struct TextWrappingPropItem : public ChoicePropItem
{
public:

    TextWrappingPropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("NoWrap"));
        sender->AddText(_U("Wrap"));
        sender->AddText(_U("WrapWithOverflow"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, new Integer(TextWrapping::tNoWrap));

        if (iVal == TextWrapping::tWrap)
        {
            return _U("Wrap");
        }
        else if (iVal == TextWrapping::tWrapWithOverflow)
        {
            return _U("WrapWithOverflow");
        }
        else
        {
            return _U("NoWrap");
        }
    }
};

struct ScrollBarVisibilityPropItem : public ChoicePropItem
{
public:

    ScrollBarVisibilityPropItem(const suic::String& prop, const suic::String& ename, OwnerItem dp)
        : ChoicePropItem(prop, ename, dp)
    {
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        sender->AddText(_U("Disabled"));
        sender->AddText(_U("AutoScroll"));
        sender->AddText(_U("Hidden"));
        sender->AddText(_U("Visible"));
        sender->AddText(_U("Collapsed"));
    }

    suic::String ValueToString(ResNode* val)
    {
        int iVal = GetChoiceIndex(val, new Integer(ScrollBarVisibility::sbvAutoScroll));

        if (iVal == ScrollBarVisibility::sbvDisabled)
        {
            return _U("Disabled");
        }
        else if (iVal == ScrollBarVisibility::sbvHidden)
        {
            return _U("Hidden");
        }
        else if (iVal == ScrollBarVisibility::sbvVisible)
        {
            return _U("Visible");
        }
        else if (iVal == ScrollBarVisibility::sbvCollapsed)
        {
            return _U("Collapsed");
        }
        else
        {
            return _U("AutoScroll");
        }
    }
};*/

#endif
