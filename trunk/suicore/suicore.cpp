// suicore.cpp : Defines the entry point for the DLL application.
//


#include <System/Interop/System.h>
#include <System/Interop/InternalOper.h>

#include <System/DragDrop/DragDrop.h>

#include <System/Windows/EventManager.h>
#include <System/Windows/SystemParameters.h>

#include <Framework/Controls/Label.h>
#include <Framework/Controls/Button.h>
#include <Framework/Controls/Border.h>
//#include <Framework/Controls/InfoBox.h>
#include <Framework/Controls/Image.h>
#include <Framework/Controls/Popup.h>
//#include <Framework/Controls/ToolBar.h>
//#include <Framework/Controls/Splitter.h>
#include <Framework/Controls/CaretElement.h>
//#include <Framework/Controls/ToolBarPanel.h>
//#include <Framework/Controls/ToolTip.h>
//#include <Framework/Controls/ComboBox.h>
//#include <Framework/Controls/ComboBoxItem.h>
#include <Framework/Controls/Expander.h>
#include <Framework/Controls/RangeBase.h>
#include <Framework/Controls/Track.h>
#include <Framework/Controls/RadioButton.h>
//#include <Framework/Controls/TextBox.h>
#include <Framework/Controls/TextBlock.h>
#include <Framework/Controls/ScrollBar.h>
//#include <Framework/Controls/TextSimple.h>
#include <Framework/Controls/TextElement.h>
#include <Framework/Controls/ScrollViewer.h>
//#include <Framework/Controls/ListView.h>
//#include <Framework/Controls/TreeView.h>
#include <Framework/Controls/HeaderedItemsControl.h>

#include <Framework/Controls/Menu.h>
#include <Framework/Controls/Grid.h>
#include <Framework/Controls/TickBar.h>
#include <Framework/Controls/Slider.h>
#include <Framework/Controls/Canvas.h>
//#include <Framework/Controls/WrapPanel.h>
#include <Framework/Controls/DockPanel.h>
#include <Framework/Controls/DatePicker.h>
#include <Framework/Controls/StackPanel.h>
#include <Framework/Controls/UniformGrid.h>
#include <Framework/Controls/ProgressBar.h>
#include <Framework/Controls/VisualizeStackPanel.h>

#include <Framework/Controls/GridView.h>
#include <Framework/Controls/GridViewColumnHeader.h>
#include <Framework/Controls/GridViewRowPresenter.h>
#include <Framework/Controls/GridViewColumnCollection.h>
#include <Framework/Controls/GridViewHeaderRowPresenter.h>

#include <Framework/Controls/Line.h>
#include <Framework/Controls/Path.h>
#include <Framework/Controls/Ellipse.h>
#include <Framework/Controls/Rectangle.h>
#include <Framework/Presenter/ContentPresenter.h>

/*#include <Framework/Controls/TabPanel.h>
#include <Framework/Controls/TabControl.h>*/

//#include <Framework/Controls/GridSplitter.h>

#include <System/Graphics/SolidColorBrush.h>
#include <System/Windows/DpProperty.h>
#include <System/Tools/CoreHelper.h>
#include <System/Tools/StyleHelper.h>
#include <System/Input/KeyboardNavigation.h>

#include <System/Interop/System.h>
#include <System/Input/FocusManager.h>
#include <System/Render/Skia/SkiaDrawing.h>

#include <Framework/Animation/AnimationAsSeries.h>

#include <Skia/core/SkImageDecoder.h>

HMODULE s_g_handle = NULL;

static bool s_g_initcore = false;
static bool s_g_initctrls = false;

void SkiaStaticInit();

#ifdef _MANAGED
#pragma managed(push, off)
#endif

namespace suic
{
    //void __suiInitFontCache();
    void suiInternalClear();
    bool StaticInitCursor();
    void InitializeSystemParameters();

    void UIInitCore()
    {
        if (!s_g_initcore)
        {
            //SkImageDecoder::SkiaStaticInit();

            DpProperty::StaticInit();

            suic::DpProperty::UnsetValue()->ref();
            suic::Expression::NoValue->ref();

            suic::StaticInitCursor();
            InitializeSystemParameters();
            SolidColorBrush::StaticInit();

            Transform::GetIdentity()->ref();
            DpProperty::NullValue->ref();

            SkiaDrawing::InitDrawBrushes();

            Button::StaticInit();
            Border::StaticInit();
            RadioButton::StaticInit();

            CaretElement::StaticInit();
            TextDecoration::StaticInit();

            Line::StaticInit();
            Path::StaticInit();
            Ellipse::StaticInit();
            Rectangle::StaticInit();
            ContentPresenter::StaticInit();
            RangeBase::StaticInit();
            Track::StaticInit();
            Thumb::StaticInit();

            ScrollBar::StaticInit();
            ScrollViewer::StaticInit();
            Run::StaticInit();
            Bold::StaticInit();
            Hyperlink::StaticInit();
            TextBlock::StaticInit();

            RowDefinition::StaticInit();
            ColumnDefinition::StaticInit();
            Grid::StaticInit();
            UniformGrid::StaticInit();

            Canvas::StaticInit();
            DockPanel::StaticInit();        
            StackPanel::StaticInit();
            VirtualizingStackPanel::StaticInit();

            GridView::StaticInit();

            GridViewColumn::StaticInit();
            GridViewColumnHeader::StaticInit();
            GridViewColumnCollection::StaticInit();
            GridViewRowPresenterBase::StaticInit();
            GridViewRowPresenter::StaticInit();
            GridViewHeaderRowPresenter::StaticInit();

            Selector::StaticInit();
            ToolTip::StaticInit();

            Window::StaticInit();

            s_g_initcore = true;
        }
    }

    void UIInitPlugin()
    {
        suic::String strPath = suic::FileDir::CalculatePath(_U("suicore.config"));

        suic::ColorManager::Ins();
        suic::PlugManager::Ins()->LoadPlugXml(strPath);
        suic::PlugManager::Ins()->InitPlugs();
    }

    void StaticUIInit()
    {
        if (!s_g_initctrls)
        {
            UIInitCore();

            Uint32 dwTime = ::GetTickCount();

            // 下面初始化元素依赖属性
    
            EasingFuncBase::StaticInit();
            ValueAnimation::StaticInit();

            RepeatButton::StaticInit();

            //TextSimple::StaticInit();
            //UIEdit::StaticInit();

            HeaderedContentControl::StaticInit();

            TickBar::StaticInit();
            Expander::StaticInit();

            TextEditor::StaticInit();
            Image::StaticInit();

            Calendar::StaticInit();
            DatePicker::StaticInit();

            HeaderedItemsControl::StaticInit();

            Slider::StaticInit();
            ProgressBar::StaticInit();

            //InfoBox::StaticInit();

            SysButton::StaticInit();
            MaximizeButton::StaticInit();

            Label::StaticInit();

            MenuItem::StaticInit();
            Menu::StaticInit();
            ContextMenu::StaticInit();

            UIInitPlugin();

            s_g_initctrls = true;
        }
    }
}

DWORD dwTime = suic::Environment::GetSystemTick();

#ifndef SUICORE_LIB

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{    
    s_g_handle = hModule;

    if (DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        //suic::__suiInitFontCache();
        suic::StaticUIInit();
        //suic::UISystem::Ins()->GetSystemLog()->Warn(suic::String().Format(_U("Initialize Cost : %ld, Count:%d\n")
        //    , Environment::GetSystemTick()-dwTime, suic::TypeofClass::classCount));
    }
    else if (DLL_PROCESS_DETACH == ul_reason_for_call)
    {
        //SystemLog()->Warn(suic::String().Format(_U("DllMain Cost : %ld\n"), Environment::GetSystemTick()-dwTime));
        suic::suiInternalClear();
    }

    return TRUE;
}

#endif

#ifdef _MANAGED
#pragma managed(pop)
#endif

