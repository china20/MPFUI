// 华勤科技版权所有 2010-2011
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
 
class CollectionViewGroup : public suic::NotifyPropChanged
{
public:

	CollectionViewGroup();
	~CollectionViewGroup();

	int GetItemCount() const
	{
		return _items->GetCount();
	}

	ObservableCollection* GetItems()
	{
		return _items;
	}

private:

	ObservableCollection* _items;
};

class GroupStyle : public suic::DpObject
{
public:

    RTTIOfClass(GroupStyle)

	GroupStyle()
	{
		_hidesIfEmpty = false;
	}

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

	void SetContainerStyle(Style* val)
	{
		_containerStyle = val;
	}

	StyleSelector* GetContainerStyleSelector()
	{
		return _containerStyleSelector;
	}

	void SetContainerStyleSelector(StyleSelector* val)
	{
		_containerStyleSelector = val;
	}

	DataTemplate* GetHeaderTemplate()
	{
		return _headerTemplate;
	}

	void SetHeaderTemplate(DataTemplate* val)
	{
		_headerTemplate = val;
	}

	DataTemplateSelector* GetHeaderTemplateSelector()
	{
		return _headerTemplateSelector;
	}

	void SetHeaderTemplateSelector(DataTemplateSelector* val)
	{
		_headerTemplateSelector = val;
	}

	ItemsPanelTemplate* GetPanelTemplate()
	{
		if (NULL == DefaultGroupPanel)
		{
			//ItemsPanelTemplate temp = new ItemsPanelTemplate(new FEFactory(StackPanel::RTTIType()));
			//temp->ref();
			//temp->Seal();
			//DefaultGroupPanel = temp;
			s_DefaultGroupStyle = new GroupStyle();
		}
		return _panel;
	}

	void SetPanelTemplate(ItemsPanelTemplate* val)
	{
		_panel = val;
	}

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
    virtual GroupStyle* SelectStyle(CollectionViewGroup* group, int level) = 0;
};

}

#endif
