/////////////////////////////////////////////////////
// ThemeWindow.cpp

#include "stdafx.h"
#include "ThemeWindow.h"

ThemeWindow::ThemeWindow()
{
}

ThemeWindow::~ThemeWindow()
{

}

void ThemeWindow::OnTimer(Object* sender, EventArg* e)
{

}

bool ThemeWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void ThemeWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void ThemeWindow::OnColorThemeClick(Element* sender, RoutedEventArg* e)
{
    suic::Button* pRect = RTTICast<suic::Button>(e->GetOriginalSource());
    if (NULL != pRect && !pRect->GetName().Empty())
    {
        String strPath;
        strPath.Format(_U("QQ/skins/color/%s.xml"), pRect->GetName().c_str());

        ResourceDictionaryPtr res = XamlReader::LoadUri(NULL, NULL, strPath.c_str()).get();
        if (res.get() != NULL)
        {
            ListBox* list = FindElem<ListBox>(_U("ThemeList"));
            if (NULL != list)
            {
                list->UnselectAll();
            }

            ResourceDictionary* resDic = Application::Current()->GetResources();
            int iCount = resDic->GetMergedDictionariesCount();

            if (iCount == 2)
            {
                resDic->RemoveMergedDictionary(1);
            }
            resDic->AddMergedDictionary(res.get());
            Application::Current()->InvalidateResourceReference();
        }

        e->SetHandled(true);
    }
}

void ThemeWindow::OnThemeSelectionChanged(Element* sender, SelectionChangedEventArg* e)
{
    if (e->GetAddedItems()->GetCount() > 0)
    {
        ThemeItem* theItem = DynamicCast<ThemeItem>(e->GetAddedItems()->GetItem(0));
        ResourceDictionaryPtr res = XamlReader::LoadUri(NULL, NULL, theItem->GetThemePath().c_str()).get();
        if (res.get() != NULL)
        {
            ResourceDictionary* resDic = Application::Current()->GetResources();
            int iCount = resDic->GetMergedDictionariesCount();

            if (iCount == 2)
            {
                resDic->RemoveMergedDictionary(1);
            }
            resDic->AddMergedDictionary(res.get());
            Application::Current()->InvalidateResourceReference();
        }
    }
}

void ThemeWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
}

void ThemeWindow::OnLoaded(suic::LoadedEventArg* e)
{
    Window::OnLoaded(e);

    // 窗口居中显示
    CenterWindow();

    AddHandler(Button::ClickEvent, new ClickEventHandler(this, &ThemeWindow::OnColorThemeClick));

    //AddMouseLeftButtonDown(new MouseEventHandler(this, &ThemeWindow::OnMouseDown), true);

    // 初始化默认样式
    ListBox* list = FindElem<ListBox>(_U("ThemeList"));
    if (NULL != list)
    {
        ImageSourcePtr bmp;
        ThemeItem* themeItem = NULL;

        for (int i = 0; i < 12; ++i)
        {
            bmp = ImageSourcePtr(FindRes(String().Format(_U("preview_%d"), i + 1)));
            themeItem = new ThemeItem(bmp.get(), String().Format(_U("天气图片 - %d"), i + 1));

            themeItem->SetThemePath(String().Format(_U("QQ/skins/%d/theme.xml"), i + 1));
            list->AddChild(themeItem);
        }

        list->AddSelectionChanged(new SelectionChangedEventHandler(this, &ThemeWindow::OnThemeSelectionChanged));
    }

    BeginAnimation(OpacityProperty, NULL);
    Duration duration(180);
    DoubleAnimation* pAni = new DoubleAnimation(0.2f, 1.0f, duration, FillBehavior::fbHoldEnd);

    //pAni->SetAutoReverse(true);
    //pAni->SetAccelerationRatio(0.3);
    //pAni->SetDecelerationRatio(0.3);
    BeginAnimation(OpacityProperty, pAni);
}
