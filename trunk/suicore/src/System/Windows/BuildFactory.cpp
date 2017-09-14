
#include <System/Windows/BuildFactory.h>

#include <Framework/Controls/Content.h>
#include <Framework/Controls/ItemsPanelTemplate.h>
#include <Framework/Controls/Border.h>
#include <Framework/Controls/Menu.h>
#include <Framework/Controls/Separator.h>
//#include <Framework/Controls/Splitter.h>
//#include <Framework/Controls/TreeView.h>
//#include <Framework/Controls/TreeViewItem.h>
#include <Framework/Controls/Label.h>
#include <Framework/Controls/Button.h>
#include <Framework/Controls/Expander.h>
#include <Framework/Controls/Image.h>
#include <Framework/Controls/Track.h>
#include <Framework/Controls/ToolTip.h>
//#include <Framework/Controls/TextBox.h>
#include <Framework/Controls/TextBlock.h>
#include <Framework/Controls/TextElement.h>
#include <Framework/Controls/ToggleButton.h>
#include <Framework/Controls/RadioButton.h>
#include <Framework/Controls/CheckBox.h>
//#include <Framework/Controls/ComboBox.h>
#include <Framework/Controls/Slider.h>
#include <Framework/Controls/TickBar.h>
#include <Framework/Controls/ProgressBar.h>
//#include <Framework/Controls/ComboBoxItem.h>
#include <Framework/Controls/DatePicker.h>
#include <Framework/Presenter/ContentPresenter.h>
#include <Framework/Presenter/ItemsPresenter.h>
#include <Framework/Presenter/ScrollContentPresenter.h>
#include <Framework/Controls/ScrollBar.h>
#include <Framework/Controls/ScrollViewer.h>
#include <Framework/Controls/ControlTemplate.h>
#include <Framework/Controls/DataTemplate.h>
//#include <Framework/Controls/HierarchicalDataTemplate.h>
#include <Framework/Controls/Window.h>
#include <Framework/Controls/Application.h>
#include <Framework/Controls/CaretElement.h>
#include <Framework/Controls/ToolTip.h>
#include <Framework/Controls/HwndHost.h>
#include <Framework/Controls/VisualizeStackPanel.h>

/*#include <Framework/Controls/ToolBar.h>
#include <Framework/Controls/ToolBarPanel.h>

#include <Framework/Controls/TabPanel.h>
#include <Framework/Controls/TabControl.h>*/

//#include <Framework/Controls/GridSplitter.h>

#include <Framework/Controls/Grid.h>
#include <Framework/Controls/Canvas.h>
#include <Framework/Controls/DockPanel.h>
/*#include <Framework/Controls/WrapPanel.h>*/
#include <Framework/Controls/StackPanel.h>
#include <Framework/Controls/UniformGrid.h>

//#include <Framework/Controls/ListView.h>

#include <Framework/Controls/GridView.h>
#include <Framework/Controls/GridViewColumnHeader.h>
#include <Framework/Controls/GridViewRowPresenter.h>
#include <Framework/Controls/GridViewColumnCollection.h>
#include <Framework/Controls/GridViewHeaderRowPresenter.h>

#include <Framework/Controls/Line.h>
#include <Framework/Controls/Path.h>
#include <Framework/Controls/Ellipse.h>
#include <Framework/Controls/Rectangle.h>

#include <System/Windows/Effect.h>
#include <System/Windows/SystemEffect.h>

#include <System/Types/String.h>
#include <System/Resources/Style.h>
#include <System/Resources/ResourceDictionary.h>

#include <System/Input/KeyboardNavigation.h>

#include <Framework/Animation/AnimationAsSeries.h>

namespace suic
{

BuildFactory* BuildFactory::_ins = NULL;
LocalTypes* LocalTypes::_ins = NULL;

LocalTypes* LocalTypes::Ins()
{
    if (_ins == NULL)
    {
        _ins = new LocalTypes();
    }

    return _ins;
}

LocalTypes::LocalTypes()
{

}

RTTIOfInfo* LocalTypes::Lookup(const String& strType)
{
    RTTIOfInfo* cls = NULL;
    _classes.TryGetValue(strType, cls);
    return cls;
}

bool LocalTypes::Contains(const String& strType)
{
    return (NULL != Lookup(strType));
}

bool LocalTypes::Contains(RTTIOfInfo* cls)
{
    return Contains(cls->typeName);
}

void LocalTypes::Remove(const String& strType)
{
    _classes.Remove(strType);
}

void LocalTypes::Remove(RTTIOfInfo* cls)
{
    Remove(cls->typeName);
}

void LocalTypes::Add(RTTIOfInfo* cls)
{
    if (Contains(cls->typeName))
    {
        throw InvalidOperationException(_U("has contains the type"), __FILELINE__);
    }
    _classes.Add(cls->typeName, cls);
}

//========================================================
// BuildFactory

BuildFactory* BuildFactory::Ins()
{
    if (_ins == NULL)
    {
        _ins = new BuildFactory();
    }

    return _ins;
}

RTTIOfInfo* BuildFactory::Lookup(const String& strType)
{
    RTTIOfInfo* cls = LookupType(strType);

    if (NULL == cls)
    {
        cls = AddByType(strType);
    }

    return cls;
}

RTTIOfInfo* BuildFactory::LookupType(const String& strType)
{
    RTTIOfInfo* cls = NULL;

    if (strType.Length() > 0 && !_classes.TryGetValue(strType, cls))
    {
        if (!_sysClasses.TryGetValue(strType, cls))
        {
            ; // Î´ÕÒµ½
        }
    }

    return cls;
}

RTTIOfInfo* BuildFactory::AddByType(const String& strType)
{
    RTTIOfInfo* cls = NULL;

    cls = AddSystmeType(strType);

    if (NULL == cls)
    {
        cls = AddType(strType);
    }

    return cls;
}

bool BuildFactory::Contains(const String& strType)
{
    return (NULL != LookupType(strType));
}

bool BuildFactory::Contains(RTTIOfInfo* cls)
{
    return Contains(cls->typeName);
}

void BuildFactory::Remove(const String& strType)
{
    _classes.Remove(strType);
}

void BuildFactory::Remove(RTTIOfInfo* cls)
{
    Remove(cls->typeName);
}

void BuildFactory::Add(RTTIOfInfo* cls)
{
    if (Contains(cls->typeName))
    {
        Remove(cls);
        InvalidOperationException e(_U("has contains the type"), __FILELINE__);
    }
    else
    {
        //cls->staticInit();
    }
    _classes.Add(cls->typeName, cls);
}

void BuildFactory::RemoveAddType(IAddType* addType)
{
    for (int i = 0; i < _addTypes.GetCount(); ++i)
    {
        if (_addTypes.GetItem(i) == addType)
        {
            _addTypes.RemoveAt(i);
            break;
        }
    }
}

void BuildFactory::RegisterAddType(IAddType* addType)
{
    for (int i = 0; i < _addTypes.GetCount(); ++i)
    {
        if (_addTypes.GetItem(i) == addType)
        {
            return;
        }
    }
    _addTypes.Add(addType);
}

RTTIOfInfo* BuildFactory::AddType(const String& strType)
{
    RTTIOfInfo* rttiInfo = NULL;
    for (int i = 0; i < _addTypes.GetCount(); ++i)
    {
        rttiInfo = _addTypes.GetItem(i)->AddType(strType);
        if (NULL != rttiInfo)
        {
            //rttiInfo->staticInit();
            _classes.Add(rttiInfo->typeName, rttiInfo);
            break;
        }
    }
    return rttiInfo;
}

RTTIOfInfo* BuildFactory::AddSystmeType(const String& strType)
{
    String strName = strType;
    RTTIOfInfo* rttiInfo = NULL;

    switch (strType.Length())
    {
    case 3:
        if (strType.Equals(_U("Run")))
        {
            rttiInfo = Run::RTTIType();
        }
        break;
    case 4:
        if (strType.Equals(_U("Span")))
        {
            rttiInfo = Span::RTTIType();
        }
        else if (strType.Equals(_U("Bold")))
        {
            rttiInfo = Bold::RTTIType();
        }
        else if (strType.Equals(_U("Rect")))
        {
            rttiInfo = ORect::RTTIType();
        }
        else if (strType.Equals(_U("Size")))
        {
            rttiInfo = OSize::RTTIType();
        }
        else if (strType.Equals(_U("Line")))
        {
            rttiInfo = Line::RTTIType();
        }
        else if (strType.Equals(_U("Path")))
        {
            rttiInfo = Path::RTTIType();
        }
        else if (strType.Equals(_U("Grid")))
        {
            rttiInfo = Grid::RTTIType();
        }
        else if (strType.Equals(_U("Menu")))
        {
            rttiInfo = Menu::RTTIType();
        }
        break;

    case 5:
        if (strType.Equals(_U("fRect")))
        {
            rttiInfo = OfRect::RTTIType();
        }
        else if (strType.Equals(_U("fSize")))
        {
            rttiInfo = OfSize::RTTIType();
        }
        else if (strType.Equals(_U("Float")))
        {
            rttiInfo = OFloat::RTTIType();
        }
        else if (strType.Equals(_U("Point")))
        {
            rttiInfo = OPoint::RTTIType();
        }
        else if (strType.Equals(_U("Color")))
        {
            rttiInfo = OColor::RTTIType();
        }
        else if (strType.Equals(_U("Style")))
        {
            rttiInfo = Style::RTTIType();
        }
        else if (strType.Equals(_U("Array")))
        {
            rttiInfo = XArray::RTTIType();
        }
        else if (strType.Equals(_U("Image")))
        {
            rttiInfo = Image::RTTIType();
        }
        else if (strType.Equals(_U("Panel")))
        {
            rttiInfo = Panel::RTTIType();
        }
        else if (strType.Equals(_U("Shape")))
        {
            rttiInfo = Shape::RTTIType();
        }
        else if (strType.Equals(_U("Thumb")))
        {
            rttiInfo = Thumb::RTTIType();
        }
        else if (strType.Equals(_U("Track")))
        {
            rttiInfo = Track::RTTIType();
        }
        else if (strType.Equals(_U("Label")))
        {
            rttiInfo = Label::RTTIType();
        }
        else if (strType.Equals(_U("Popup")))
        {
            rttiInfo = Popup::RTTIType();
        }
        /*else if (strType.Equals(_U("Toast")))
        {
            rttiInfo = Toast::RTTIType();
        }*/
        break;

    case 6:
        if (strType.Equals(_U("Border")))
        {
            rttiInfo = Border::RTTIType();
        }
        else if (strType.Equals(_U("ImageSource")))
        {
            rttiInfo = ImageSource::RTTIType();
        }
        else if (strType.Equals(_U("String")))
        {
            rttiInfo = OString::RTTIType();
        }
        else if (strType.Equals(_U("Italic")))
        {
            rttiInfo = Italic::RTTIType();
        }
        else if (strType.Equals(_U("Window")))
        {
            rttiInfo = Window::RTTIType();
        }
        else if (strType.Equals(_U("Button")))
        {
            rttiInfo = Button::RTTIType();
        }
        else if (strType.Equals(_U("Slider")))
        {
            rttiInfo = Slider::RTTIType();
        }
        else if (strType.Equals(_U("Canvas")))
        {
            rttiInfo = Canvas::RTTIType();
        }
        break;

    case 7:
        if (strType.Equals(_U("Element")))
        {
            rttiInfo = Element::RTTIType();
        }
        else if (strType.Equals(_U("Control")))
        {
            rttiInfo = Control::RTTIType();
        }
        else if (strType.Equals(_U("Integer")))
        {
            rttiInfo = Integer::RTTIType();
        }
        else if (strType.Equals(_U("Boolean")))
        {
            rttiInfo = Boolean::RTTIType();
        }        
        else if (strType.Equals(_U("TickBar")))
        {
            rttiInfo = TickBar::RTTIType();
        }
        else if (strType.Equals(_U("Ellipse")))
        {
            rttiInfo = Ellipse::RTTIType();
        }
        /*else if (strType.Equals(_U("InfoBox")))
        {
            rttiInfo = InfoBox::RTTIType();
        }*/
        else if (strType.Equals(_U("ToolTip")))
        {
            rttiInfo = ToolTip::RTTIType();
        }
        break;

    case 8:
        if (strType.Equals(_U("GridView")))
        {
            rttiInfo = GridView::RTTIType();
        }
        else if (strType.Equals(_U("CheckBox")))
        {
            rttiInfo = CheckBox::RTTIType();
        }
        else if (strType.Equals(_U("Expander")))
        {
            rttiInfo = Expander::RTTIType();
        }
        else if (strType.Equals(_U("HwndHost")))
        {
            rttiInfo = HwndHost::RTTIType();
        }
        else if (strType.Equals(_U("Selector")))
        {
            rttiInfo = Selector::RTTIType();
        }
        else if (strType.Equals(_U("MenuItem")))
        {
            rttiInfo = MenuItem::RTTIType();
        }
        else if (strType.Equals(_U("Calendar")))
        {
            rttiInfo = Calendar::RTTIType();
        }
        break;

    case 9:
        if (strType.Equals(_U("Hyperlink")))
        {
            rttiInfo = Hyperlink::RTTIType();
        }
        else if (strType.Equals(_U("Underline")))
        {
            rttiInfo = Underline::RTTIType();
        }
        else if (strType.Equals(_U("LineBreak")))
        {
            rttiInfo = LineBreak::RTTIType();
        }
        else if (strType.Equals(_U("TextBlock")))
        {
            rttiInfo = TextBlock::RTTIType();
        }
        else if (strType.Equals(_U("ViewGroup")))
        {
            rttiInfo = ViewGroup::RTTIType();
        }
        else if (strType.Equals(_U("GroupItem")))
        {
            rttiInfo = GroupItem::RTTIType();
        }
        else if (strType.Equals(_U("DockPanel")))
        {
            rttiInfo = DockPanel::RTTIType();
        }
        else if (strType.Equals(_U("ScrollBar")))
        {
            rttiInfo = ScrollBar::RTTIType();
        }
        else if (strType.Equals(_U("Rectangle")))
        {
            rttiInfo = Rectangle::RTTIType();
        }
        else if (strType.Equals(_U("Separator")))
        {
            rttiInfo = Separator::RTTIType();
        }
        break;

    case 10:
        if (strType.Equals(_U("GroupStyle")))
        {
            rttiInfo = GroupStyle::RTTIType();
        }
        else if (strType.Equals(_U("Underline")))
        {
            rttiInfo = Underline::RTTIType();
        }
        else if (strType.Equals(_U("StackPanel")))
        {
            rttiInfo = StackPanel::RTTIType();
        }
        else if (strType.Equals(_U("Dictionary")))
        {
            rttiInfo = XDictionary::RTTIType();
        }
        else if (strType.Equals(_U("ResizeGrip")))
        {
            rttiInfo = ResizeGrip::RTTIType();
        }
        else if (strType.Equals(_U("DatePicker")))
        {
            rttiInfo = DatePicker::RTTIType();
        }
        break;

    case 11:
        if (strType.Equals(_U("RadioButton")))
        {
            rttiInfo = RadioButton::RTTIType();
        }
        else if (strType.Equals(_U("CloseButton")))
        {
            rttiInfo = CloseButton::RTTIType();
        }
        else if (strType.Equals(_U("Application")))
        {
            rttiInfo = Application::RTTIType();
        }
        else if (strType.Equals(_U("TextElement")))
        {
            rttiInfo = TextElement::RTTIType();
        }
        else if (strType.Equals(_U("ProgressBar")))
        {
            rttiInfo = ProgressBar::RTTIType();
        }
        else if (strType.Equals(_U("UniformGrid")))
        {
            rttiInfo = UniformGrid::RTTIType();
        }
        else if (strType.Equals(_U("ContextMenu")))
        {
            rttiInfo = ContextMenu::RTTIType();
        }
        break;

    case 12:
        if (strType.Equals(_U("RepeatButton")))
        {
            rttiInfo = RepeatButton::RTTIType();
        }
        else if (strType.Equals(_U("ToggleButton")))
        {
            rttiInfo = ToggleButton::RTTIType();
        }
        else if (strType.Equals(_U("ItemsControl")))
        {
            rttiInfo = ItemsControl::RTTIType();
        }
        else if (strType.Equals(_U("ScrollViewer")))
        {
            rttiInfo = ScrollViewer::RTTIType();
        }
        else if (strType.Equals(_U("CaretElement")))
        {
            rttiInfo = CaretElement::RTTIType();
        }
        break;

    case 13:
        if (strType.Equals(_U("RowDefinition")))
        {
            rttiInfo = RowDefinition::RTTIType();
        }
        break;

    case 14:
        if (strType.Equals(_U("MinimizeButton")))
        {
            rttiInfo = MinimizeButton::RTTIType();
        }
        else if (strType.Equals(_U("MaximizeButton")))
        {
            rttiInfo = MaximizeButton::RTTIType();
        }
        else if (strType.Equals(_U("ContentControl")))
        {
            rttiInfo = ContentControl::RTTIType();
        }
        else if (strType.Equals(_U("GridViewColumn")))
        {
            rttiInfo = GridViewColumn::RTTIType();
        }
        else if (strType.Equals(_U("CalendarButton")))
        {
            rttiInfo = CalendarButton::RTTIType();
        }
        break;

    case 16:
        if (strType.Equals(_U("FrameworkElement")))
        {
            rttiInfo = FrameworkElement::RTTIType();
        }
        else if (strType.Equals(_U("AdornerDecorator")))
        {
            rttiInfo = AdornerDecorator::RTTIType();
        }
        else if (strType.Equals(_U("ContentPresenter")))
        {
            rttiInfo = ContentPresenter::RTTIType();
        }
        else if (strType.Equals(_U("ColumnDefinition")))
        {
            rttiInfo = ColumnDefinition::RTTIType();
        }
        break;

    case 17:
        if (strType.Equals(_U("InlineUIContainer")))
        {
            rttiInfo = InlineUIContainer::RTTIType();
        }
        else if (strType.Equals(_U("CalendarDayButton")))
        {
            rttiInfo = CalendarDayButton::RTTIType();
        }
        break;

    case 18:
        if (strType.Equals(_U("ItemsPanelTemplate")))
        {
            rttiInfo = ItemsPanelTemplate::RTTIType();
        }
        else if (strType.Equals(_U("FocusVisualAdorner")))
        {
            rttiInfo = FocusVisualAdorner::RTTIType();
        }
        break;

    case 20:
        if (strType.Equals(_U("GridViewRowPresenter")))
        {
            rttiInfo = GridViewRowPresenter::RTTIType();
        }
        else if (strType.Equals(_U("GridViewColumnHeader")))
        {
            rttiInfo = GridViewColumnHeader::RTTIType();
        }
        break;

    case 22:
        if (strType.Equals(_U("ScrollContentPresenter")))
        {
            rttiInfo = ScrollContentPresenter::RTTIType();
        }
        else if (strType.Equals(_U("VirtualizingStackPanel")))
        {
            rttiInfo = VirtualizingStackPanel::RTTIType();
        }
        break;

    case 24:
        if (strType.Equals(_U("GridViewColumnCollection")))
        {
            rttiInfo = GridViewColumnCollection::RTTIType();
        }
        break;

    case 26:
        if (strType.Equals(_U("GridViewHeaderRowPresenter")))
        {
            rttiInfo = GridViewHeaderRowPresenter::RTTIType();
        }
        break;
    }

    if (NULL != rttiInfo)
    {
        // rttiInfo->staticInit();
        _sysClasses.Add(strName, rttiInfo);
    }

    return rttiInfo;
}

BuildFactory::BuildFactory()
{
    _sysClasses.Add(_U("ResourceDictionary"), ResourceDictionary::RTTIType());
    _sysClasses.Add(_U("ItemsPresenter"), ItemsPresenter::RTTIType());
 
    _sysClasses.Add(_U("ControlTemplate"), ControlTemplate::RTTIType());
    _sysClasses.Add(_U("DataTemplate"), DataTemplate::RTTIType());

    _sysClasses.Add(_U("BlurEffect"), BlurEffect::RTTIType());
    _sysClasses.Add(_U("LightingEffect"), LightingEffect::RTTIType());
    _sysClasses.Add(_U("DropShadowEffect"), DropShadowEffect::RTTIType());

    _sysClasses.Add(_U("DoubleAnimation"), DoubleAnimation::RTTIType());
}

//===============================================================
//

class BToV : public IBindConverter
{
public:

    RTTIOfClass(BToV)

    virtual ~BToV() {}

    AutoObj Convert(Object* inVal, String param)
    {
        bool bReverse = !param.Empty();

        if (inVal->ToBool())
        {
            return (bReverse ? VisibilityBox::CollapsedBox : VisibilityBox::VisibleBox);
        }
        else
        {
            return (bReverse ? VisibilityBox::VisibleBox : VisibilityBox::CollapsedBox);
        }
    }

    AutoObj ConvertBack(Object* inVal, String param)
    {
        bool bReverse = !param.Empty();
        Visibility iVal = (Visibility)inVal->ToInt();

        if (Visibility::Visible == iVal)
        {
            return bReverse ? Boolean::False : Boolean::True;
        }
        else
        {
            return (bReverse ? Boolean::True : Boolean::False);
        }
    }
};

ImplementRTTIOfClass(BToV, IBindConverter)

//===============================================================
//

class IntToRect : public IBindConverter
{
public:

    RTTIOfClass(IntToRect)

    virtual ~IntToRect() {}

    AutoObj Convert(Object* inVal, String param)
    {
        int iVal = inVal->ToInt();

        if (param.Equals(_U("Top")))
        {
            return new ORect(Rect(0, iVal, 0, -iVal));
        }
        else if (param.Equals(_U("Right")))
        {
            return new ORect(Rect(0, 0, iVal, 0));
        }
        else if (param.Equals(_U("Bottom")))
        {
            return new ORect(Rect(0, 0, 0, iVal));
        }
        else
        {
            return new ORect(Rect(iVal, 0, -iVal, 0));
        }
    }

    AutoObj ConvertBack(Object* inVal, String param)
    {
        Rect rect = ORect::FromObj(inVal);

        if (param.Equals(_U("Top")))
        {
            return new Integer(rect.top);
        }
        else if (param.Equals(_U("Right")))
        {
            return new Integer(rect.right);
        }
        else if (param.Equals(_U("Bottom")))
        {
            return new Integer(rect.bottom);
        }
        else
        {
            return new Integer(rect.left);
        }
    }
};

ImplementRTTIOfClass(IntToRect, IBindConverter)

BindConverter* BindConverter::_ins = NULL;

BindConverter* BindConverter::Ins()
{
    if (_ins == NULL)
    {
        _ins = new BindConverter();
    }

    return _ins;
}

IBindConverter* BindConverter::Lookup(const String& strName)
{
    Object* obj = NULL;
    IBindConverter* converter = NULL;

    if (!_converters.TryGetValue(strName, obj))
    {
        _sysConverters.TryGetValue(strName, obj);
    }

    return (IBindConverter*)obj;
}

bool BindConverter::Contains(const String& strName)
{
    return (NULL != Lookup(strName));
}

void BindConverter::Remove(const String& strName)
{
    _converters.Remove(strName);
}

void BindConverter::Clear(const String& strType)
{

}

void BindConverter::Add(const String& strName, IBindConverter* val)
{
    if (Contains(strName))
    {
        throw InvalidOperationException(_U("has contains the type"), __FILELINE__);
    }
    _converters.Add(strName, val);
}

BindConverter::BindConverter()
{
    _sysConverters.Add(_U("BToV"), new BToV());
    _sysConverters.Add(_U("IntToRect"), new IntToRect());
}

}
