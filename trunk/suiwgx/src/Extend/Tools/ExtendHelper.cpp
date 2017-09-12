
#include "stdafx.h"

#include <Extend/Editor/TextBox.h>
#include <Extend/Editor/RichTextBox.h>
#include <Extend/Editor/PasswordBox.h>
#include <Extend/Editor/RichInterface.h>
#include <Extend/Editor/ImageEmbbed.h>

#include <Extend/ActiveX/FlashBox.h>
#include <Extend/ActiveX/WebBrowser.h>
#include <Extend/Controls/GifBox.h>
#include <Extend/Controls/PlayBox.h>
#include <Extend/Controls/HyperPanel.h>

#include <Extend/Controls/InfoBox.h>
#include <Extend/Controls/WebkitBox.h>
#include <Extend/Controls/TreeView.h>
#include <Extend/Controls/WrapPanel.h>

//#include <Extend/Controls/StackPanel.h>

#include <Extend/Controls/ToolBar.h>
#include <Extend/Controls/ToolBarPanel.h>

#include <Extend/Controls/TabPanel.h>
#include <Extend/Controls/TabControl.h>

#include <Extend/Controls/GridSplitter.h>

#include <Extend/Controls/ComboBox.h>
#include <Extend/Controls/ComboBoxItem.h>

#include <Extend/Controls/ListBox.h>
#include <Extend/Controls/ListBoxItem.h>

#include <Extend/Controls/ListView.h>
#include <Extend/Controls/ListViewItem.h>

#include <Extend/Controls/VisualizeWrapPanel.h>

namespace suic
{

static bool s_g_init = false;

class WgxAddType : public suic::IAddType
{
public:

    suic::RTTIOfInfo* AddType(const String& strType)
    {
        suic::RTTIOfInfo* rttiInfo = NULL;
        switch (strType.Length())
        {
        case 5:
            if (strType.Equals(_U("Toast")))
            {
                rttiInfo = Toast::RTTIType();
            }
            break;

        case 6:
            if (strType.Equals(_U("GifBox")))
            {
                rttiInfo = GifBox::RTTIType();
                GifBox::StaticInit();
            }
            break;

        case 7:
            if (strType.Equals(_U("TextBox")))
            {
                rttiInfo = TextBox::RTTIType();
                TextBoxBase::StaticInit();
                TextBox::StaticInit();
            }
            else if (strType.Equals(_U("PlayBox")))
            {
                rttiInfo = PlayBox::RTTIType();
                PlayBox::StaticInit();
            }
            else if (strType.Equals(_U("InfoBox")))
            {
                rttiInfo = InfoBox::RTTIType();
                InfoBox::StaticInit();
            }
            else if (strType.Equals(_U("ToolBar")))
            {
                rttiInfo = ToolBar::RTTIType();
                suic::ToolBar::StaticInit();
            }
            else if (strType.Equals(_U("TabItem")))
            {
                rttiInfo = TabItem::RTTIType();
                suic::TabControl::StaticInit();
            }
            else if (strType.Equals(_U("ListBox")))
            {
                rttiInfo = ListBox::RTTIType();
                suic::ListBox::StaticInit();
            }
            break;

        case 8:
            if (strType.Equals(_U("FlashBox")))
            {
                rttiInfo = FlashBox::RTTIType();
                FlashBox::StaticInit();
            }
            else if (strType.Equals(_U("TabPanel")))
            {
                rttiInfo = TabPanel::RTTIType();
                suic::TabControl::StaticInit();
            }
            else if (strType.Equals(_U("TreeView")))
            {
                rttiInfo = TreeView::RTTIType();
                suic::TreeView::StaticInit();
            }
            else if (strType.Equals(_U("ListView")))
            {
                rttiInfo = ListView::RTTIType();
                suic::ListView::StaticInit();
            }
            else if (strType.Equals(_U("ComboBox")))
            {
                rttiInfo = ComboBox::RTTIType();
                suic::ComboBox::StaticInit();
            }
            break;

        case 9:
            if (strType.Equals(_U("WebkitBox")))
            {
                rttiInfo = WebkitBox::RTTIType();
                suic::WebkitBox::StaticInit();
            }
            else if (strType.Equals(_U("WrapPanel")))
            {
                rttiInfo = WrapPanel::RTTIType();
                suic::WrapPanel::StaticInit();
            }
            break;

        case 10:
            if (strType.Equals(_U("WebBrowser")))
            {
                rttiInfo = WebBrowser::RTTIType();
                WebBrowser::StaticInit();
            }
            else if (strType.Equals(_U("HyperPanel")))
            {
                rttiInfo = HyperPanel::RTTIType();
                HyperPanel::StaticInit();
            }
            else if (strType.Equals(_U("TabControl")))
            {
                rttiInfo = TabControl::RTTIType();
                suic::TabControl::StaticInit();
            }
            break;

        case 11:
            if (strType.Equals(_U("PasswordBox")))
            {
                rttiInfo = PasswordBox::RTTIType();
                PasswordBox::StaticInit();
            }
            else if (strType.Equals(_U("RichTextBox")))
            {
                rttiInfo = RichTextBox::RTTIType();
                TextBoxBase::StaticInit();
                RichTextBox::StaticInit();
            }
            else if (strType.Equals(_U("ImageEmbbed")))
            {
                rttiInfo = ImageEmbbed::RTTIType();
            }
            else if (strType.Equals(_U("ListBoxItem")))
            {
                rttiInfo = ListBoxItem::RTTIType();
                suic::ListBox::StaticInit();
            }
            break;

        case 12:
            if (strType.Equals(_U("ToolBarPanel")))
            {
                rttiInfo = ToolBarPanel::RTTIType();
                suic::TabControl::StaticInit();
            }
            else if (strType.Equals(_U("GridSplitter")))
            {
                rttiInfo = GridSplitter::RTTIType();
                suic::GridSplitter::StaticInit();
            }
            else if (strType.Equals(_U("TreeViewItem")))
            {
                rttiInfo = TreeViewItem::RTTIType();
                suic::TreeView::StaticInit();
            }
            else if (strType.Equals(_U("ListViewItem")))
            {
                rttiInfo = ListViewItem::RTTIType();
                suic::ListView::StaticInit();
            }
            else if (strType.Equals(_U("ComboBoxItem")))
            {
                rttiInfo = ComboBoxItem::RTTIType();
                suic::ComboBox::StaticInit();
            }
            break;

        case 13:
            if (strType.Equals(_U("TextContainer")))
            {
                rttiInfo = TextContainer::RTTIType();
                suic::TextBox::StaticInit();
            }
            break;
            
        case 15:
            if (strType.Equals(_U("PlayImageEmbbed")))
            {
                rttiInfo = PlayImageEmbbed::RTTIType();
            }
            break;

        case 21:
            if (strType.Equals(_U("VirtualizingWrapPanel")))
            {
                rttiInfo = VirtualizingWrapPanel::RTTIType();
                suic::VirtualizingWrapPanel::StaticInit();
            }
            break;
        }

        return rttiInfo;
    }
};

void __stdcall InitUIControls()
{
    suic::GifBox::StaticInit();
    suic::TextBoxBase::StaticInit();
    suic::TextBox::StaticInit();
    suic::PlayBox::StaticInit();
    suic::InfoBox::StaticInit();
    suic::ToolBar::StaticInit();
    suic::TabControl::StaticInit();
    suic::ListBox::StaticInit();
    suic::FlashBox::StaticInit();
    suic::TabControl::StaticInit();
    suic::TreeView::StaticInit();
    suic::ListView::StaticInit();
    suic::ComboBox::StaticInit();
    suic::WebkitBox::StaticInit();
    suic::WrapPanel::StaticInit();
    suic::WebBrowser::StaticInit();
    suic::HyperPanel::StaticInit();
    suic::TabControl::StaticInit();
    suic::PasswordBox::StaticInit();
    suic::RichTextBox::StaticInit();
    suic::ListBox::StaticInit();
    suic::TabControl::StaticInit();
    suic::GridSplitter::StaticInit();
    suic::TreeView::StaticInit();
    suic::ListView::StaticInit();
    suic::ComboBox::StaticInit();
    suic::TextBox::StaticInit();
    suic::VirtualizingWrapPanel::StaticInit();
}

SUIWGX_API bool __stdcall InitUIWgxs(bool initWgx)
{
    if (!s_g_init)
    {
        suic::BuildFactory::Ins()->RegisterAddType(new WgxAddType());
        suic::InitUICore();
        ::OleInitialize(NULL);
        //suic::BuildFactory::Ins()->Add();
        suic::InfoBox::StaticInit();
        if (initWgx)
        {
            InitUIControls();
        }
        s_g_init = true;
    }

    return true;
}

SUIWGX_API bool __stdcall InitUIWgx()
{
    return InitUIWgxs(false);
}

SUIWGX_API void __stdcall ExitUIWgx()
{
    if (s_g_init)
    {
        suic::ExitUICore();
        ::OleUninitialize();
    }
}

}
