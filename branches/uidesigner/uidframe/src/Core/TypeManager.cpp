/////////////////////////////////////////////////////
// PropertyWindow.cpp

#include "stdafx.h"

#include <Core/TypeManager.h>

#include <System/Graphics/RadialGradientBrush.h>

#include <Framework/Controls/ScrollBar.h>
#include <Framework/Controls/DatePicker.h>
#include <Framework/Controls/Canvas.h>
#include <Framework/Controls/Path.h>
#include <Framework/Controls/Ellipse.h>
#include <Framework/Controls/UniformGrid.h>

#include <Extend/Controls/GridSplitter.h>

#include <Framework/Presenter/ContentPresenter.h>
#include <Framework/Presenter/ItemsPresenter.h>
#include <Framework/Presenter/ScrollContentPresenter.h>

ObservableCollection* TypePool::_panelTypes = NULL;
ObservableCollection* TypePool::_ctrlsTypes = NULL;
ObservableCollection* TypePool::_shapeTypes = NULL;
ObservableCollection* TypePool::_otherTypes = NULL;

//int TypePool::GetCount()
//{
//    return GetTypeItems()->GetCount();
//}
//
//RTTIOfInfo* TypePool::GetTypeItem(int index)
//{
//    TypeItem* item = dynamic_cast<TypeItem*>(GetTypeItems()->GetItem(index));
//    return item->GetType();
//}

void TypePool::InitTypeItems()
{
    if (NULL == _panelTypes)
    {
        _panelTypes = new ObservableCollection();
        _ctrlsTypes = new ObservableCollection();
        _shapeTypes = new ObservableCollection();

        _panelTypes->ref();
        _ctrlsTypes->ref();
        _shapeTypes->ref();

        _panelTypes->AddItem(new TypeItem(Grid::RTTIType()));
        _panelTypes->AddItem(new TypeItem(Canvas::RTTIType()));
        _panelTypes->AddItem(new TypeItem(StackPanel::RTTIType()));
        _panelTypes->AddItem(new TypeItem(suic::HyperPanel::RTTIType()));
        _panelTypes->AddItem(new TypeItem(WrapPanel::RTTIType()));
        _panelTypes->AddItem(new TypeItem(DockPanel::RTTIType()));
        _panelTypes->AddItem(new TypeItem(UniformGrid::RTTIType()));
        _panelTypes->AddItem(new TypeItem(VirtualizingStackPanel::RTTIType()));

        _ctrlsTypes->AddItem(new TypeItem(Border::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(Button::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(RepeatButton::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(CheckBox::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(RadioButton::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(ToggleButton::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(Image::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(ComboBox::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(TextBlock::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(suic::TextBox::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(suic::PasswordBox::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(ProgressBar::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(TabControl::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(TreeView::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(ListBox::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(ListView::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(ScrollBar::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(ScrollViewer::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(DatePicker::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(Expander::RTTIType()));

        _ctrlsTypes->AddItem(new TypeItem(GridSplitter::RTTIType()));

        _ctrlsTypes->AddItem(new TypeItem(suic::PlayBox::RTTIType()));

        _ctrlsTypes->AddItem(new TypeItem(MinimizeButton::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(MaximizeButton::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(CloseButton::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(Window::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(ContentControl::RTTIType()));

        _ctrlsTypes->AddItem(new TypeItem(Thumb::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(Menu::RTTIType()));
        _ctrlsTypes->AddItem(new TypeItem(Expander::RTTIType()));

        _shapeTypes->AddItem(new TypeItem(Rectangle::RTTIType()));
        _shapeTypes->AddItem(new TypeItem(Ellipse::RTTIType()));
        _shapeTypes->AddItem(new TypeItem(Path::RTTIType()));

        _otherTypes = new ObservableCollection();
        _otherTypes->ref();

        _otherTypes->AddItem(new TypeItem(suic::ContentControl::RTTIType()));
        _otherTypes->AddItem(new TypeItem(suic::ContentPresenter::RTTIType()));
        _otherTypes->AddItem(new TypeItem(suic::ScrollContentPresenter::RTTIType()));
        _otherTypes->AddItem(new TypeItem(suic::ItemsPresenter::RTTIType()));
        //_otherTypes->AddItem(new TypeItem(suic::AdornerDecorator::RTTIType()));

        _otherTypes->AddItem(new TypeItem(TabItem::RTTIType()));
        _otherTypes->AddItem(new TypeItem(TreeViewItem::RTTIType()));
        _otherTypes->AddItem(new TypeItem(ListBoxItem::RTTIType()));
        _otherTypes->AddItem(new TypeItem(ListViewItem::RTTIType()));
        _otherTypes->AddItem(new TypeItem(ComboBoxItem::RTTIType()));
        _otherTypes->AddItem(new TypeItem(MenuItem::RTTIType()));
    }
}

void TypePool::CompTypeItems(ObservableCollection* ori, ItemCollection* dest)
{
    for (int i = 0; i < ori->GetCount(); ++i)
    {
        dest->AddItem(ori->GetItem(i));
    }
}

void TypePool::GetTypeItems(ItemCollection* itemColl)
{
    InitTypeItems();

    itemColl->Clear();

    CompTypeItems(_panelTypes, itemColl);
    CompTypeItems(_ctrlsTypes, itemColl);
    CompTypeItems(_shapeTypes, itemColl);
    CompTypeItems(_otherTypes, itemColl);
}

void TypePool::GetTypeItems(ItemCollection* itemColl, const String& grp)
{
    itemColl->Clear();

    if (grp.Equals(_U("All")))
    {
        GetTypeItems(itemColl);
    }
    else if (grp.Equals(_U("Panels")))
    {
        CompTypeItems(_panelTypes, itemColl);
    }
    else if (grp.Equals(_U("Controls")))
    {
        CompTypeItems(_ctrlsTypes, itemColl);
    }
    else if (grp.Equals(_U("Shapes")))
    {
        CompTypeItems(_shapeTypes, itemColl);
    }
}

//===============================================

ObservableCollection* ResTypePool::_types = NULL;
ObjectDic<RTTIOfInfo*, String> ResTypePool::_resTypeMap;

int ResTypePool::GetCount()
{
    return GetTypeItems()->GetCount();
}

RTTIOfInfo* ResTypePool::GetTypeItem(int index)
{
    TypeItem* item = dynamic_cast<TypeItem*>(GetTypeItems()->GetItem(index));
    return item->GetType();
}

String ResTypePool::GetResTypeName(RTTIOfInfo* rttiInfo)
{
    if (_resTypeMap.GetCount() == 0)
    {
        _resTypeMap.Add(suic::OString::RTTIType(), _U("String"));
        _resTypeMap.Add(suic::OFloat::RTTIType(), _U("Float"));
        _resTypeMap.Add(suic::OSize::RTTIType(), _U("Size"));
        _resTypeMap.Add(suic::ORect::RTTIType(), _U("Rect"));
        _resTypeMap.Add(suic::OfSize::RTTIType(), _U("fSize"));
        _resTypeMap.Add(suic::OfRect::RTTIType(), _U("fRect"));
    }
    String strName;
    _resTypeMap.TryGetValue(rttiInfo, strName);
    if (strName.Empty())
    {
        strName = rttiInfo->typeName;
    }
    return strName;
}

ItemCollection* ResTypePool::GetTypeItems()
{
    if (NULL == _types)
    {
        _types = new ObservableCollection();
        _types->ref();

        _types->AddItem(new ResTypeItem(ImageBrush::RTTIType(), _U("ImageBrush")));
        _types->AddItem(new ResTypeItem(SolidColorBrush::RTTIType(), _U("SolidColorBrush")));
        _types->AddItem(new ResTypeItem(LinearGradientBrush::RTTIType(), _U("LinearGradientBrush")));
        _types->AddItem(new ResTypeItem(RadialGradientBrush::RTTIType(), _U("RadialGradientBrush")));
        _types->AddItem(new ResTypeItem(Style::RTTIType(), _U("Style")));
        _types->AddItem(new ResTypeItem(DataTemplate::RTTIType(), _U("DataTemplate")));
        _types->AddItem(new ResTypeItem(ControlTemplate::RTTIType(), _U("ControlTemplate")));
        _types->AddItem(new ResTypeItem(OString::RTTIType(), _U("String")));
        _types->AddItem(new ResTypeItem(Integer::RTTIType(), _U("Integer")));
        _types->AddItem(new ResTypeItem(OFloat::RTTIType(), _U("Float")));
        //_types->AddItem(new ResTypeItem(OColor::RTTIType(), _U("Color")));
        _types->AddItem(new ResTypeItem(OSize::RTTIType(), _U("Size")));
        _types->AddItem(new ResTypeItem(OfSize::RTTIType(), _U("fSize")));
        _types->AddItem(new ResTypeItem(OPoint::RTTIType(), _U("Point")));
        _types->AddItem(new ResTypeItem(OfPoint::RTTIType(), _U("fPoint")));
        _types->AddItem(new ResTypeItem(ORect::RTTIType(), _U("Rect")));
        _types->AddItem(new ResTypeItem(OfRect::RTTIType(), _U("fRect")));

        _types->AddItem(new ResTypeItem(ImageSource::RTTIType(), _U("ImageSource")));
    }

    return _types;
}
