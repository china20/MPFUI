// 华勤科技版权所有 2008-2022
// 
// 文件名：CollectionViewGroup.h
// 功  能：ItemsControl分组对象
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UICOLLECTIONVIEWGROUP_H_
#define _UICOLLECTIONVIEWGROUP_H_

#include <System/Windows/Binding.h>
#include <System/Resources/StyleSelector.h>

#include <Framework/Controls/ItemSource.h>
#include <Framework/Controls/DataTemplate.h>
#include <Framework/Controls/ItemsPanelTemplate.h>

namespace suic
{

class SUICORE_API ViewGroup : public suic::NotifyPropChanged
{
public:

    BeginMember(ViewGroup, suic::NotifyPropChanged)
        MemberBool(Expanded)
    EndMember()

    RTTIOfAbstractClass(ViewGroup)

	ViewGroup();
	~ViewGroup();

    bool GetExpanded()
    {
        return _expanded;
    }

    void SetExpanded(bool val)
    {
        _expanded = val;
    }

	int GetItemCount() const;
	ObservableCollection* GetItems();

    virtual bool IsBottomLevel() = 0;

private:

    bool _expanded;
	ObservableCollection* _items;
};

inline int ViewGroup::GetItemCount() const
{
    return _items->GetCount();
}

inline ObservableCollection* ViewGroup::GetItems()
{
    return _items;
}

class SUICORE_API GroupStyle : public suic::DpObject
{
public:

    RTTIOfClass(GroupStyle)

	GroupStyle();
    ~GroupStyle();

    static GroupStyle* Default();

	bool HidesIfEmpty() const
	{
		return _hidesIfEmpty;
	}

	void SetHidesIfEmpty(bool val)
	{
		_hidesIfEmpty = val;
	}

	Style* GetContainerStyle() const
	{
		return _containerStyle;
	}

	void SetContainerStyle(Style* val);

	StyleSelector* GetContainerStyleSelector()
	{
		return _containerStyleSelector;
	}

	void SetContainerStyleSelector(StyleSelector* val);

	DataTemplate* GetHeaderTemplate()
	{
		return _headerTemplate;
	}

	void SetHeaderTemplate(DataTemplate* val);

	DataTemplateSelector* GetHeaderTemplateSelector()
	{
		return _headerTemplateSelector;
	}

	void SetHeaderTemplateSelector(DataTemplateSelector* val);

	ItemsPanelTemplate* GetPanel();
	void SetPanel(ItemsPanelTemplate* val);

	bool GetIsAlternationCountSet() const
	{
		return _isAlternationCountSet;
	}

	int GetAlternationCount() const
	{
		return _alternationCount;
	}

	void SetAlternationCount(int val)
	{
		_alternationCount = val;
	}

private:

	bool _hidesIfEmpty;
    bool _isAlternationCountSet;
	int _alternationCount;

    Style* _containerStyle;
	ItemsPanelTemplate* _panel;
	DataTemplate* _headerTemplate;
    StyleSelector* _containerStyleSelector; 
    DataTemplateSelector* _headerTemplateSelector;

	static GroupStyle* s_DefaultGroupStyle;
	static ItemsPanelTemplate* DefaultGroupPanel;
};

class SUICORE_API GroupStyleSelector : public DpObject
{
public:

    RTTIOfAbstractClass(GroupStyleSelector)

    virtual ~GroupStyleSelector() {}
    virtual GroupStyle* SelectStyle(ViewGroup* group, int level) = 0;
};

}

#endif
