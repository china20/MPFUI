/////////////////////////////////////////////////////
// ImageSelectorWindow.cpp

#include "stdafx.h"
#include "FilterNode.h"
#include "ImageSelectorWindow.h"
#include <Framework/Controls/Rectangle.h>

ImplementRTTIOfClass(ImageTreeView, TreeView)
ImplementRTTIOfClass(ImageTreeViewItem, TreeViewItem)

void GridBk::DrawBkgnd(Drawing* drawing)
{
    const int RECSIZE = 8;
    fRect rect(0, 0, GetRenderSize().Width(), GetRenderSize().Height());
    int iBeg = RECSIZE;
    Color clrs[2] = {0xFFFFFFFF, 0xFFCCCCCC};

    drawing->EraseRect(suic::DrawCtx::DefDraw, clrs[0], &rect);

    for (int y = 0; y < (int)rect.bottom;)
    {
        for (int x = iBeg; x < (int)rect.right;)
        {
            fRect rect(x, y, RECSIZE, RECSIZE);
            drawing->EraseRect(suic::DrawCtx::DefDraw, clrs[1], &rect);
            x += 16;
        }
        y += RECSIZE;
        iBeg = iBeg == 0 ? RECSIZE : 0;
    }
}

void GridBk::OnRender(Drawing* drawing)
{
    DrawBkgnd(drawing);
}

ImageTreeView::ImageTreeView()
{
    _prjTreeMenu = NULL;
}

ImageTreeView::~ImageTreeView()
{
    if (NULL != _prjTreeMenu)
    {
        _prjTreeMenu->unref();
    }
}

void ImageTreeView::InitPrjTreeMenu(SlnTreeManager* slnMana)
{
    _prjTreeMenu = new PrjTreeMenu();
    _prjTreeMenu->Initialize(slnMana, this);
    _prjTreeMenu->ref();
}

void ImageTreeView::OnContextMenu(ContextMenuEventArg* e)
{
    suic::Element* elem = suic::RTTICast<suic::Element>(e->GetOriginalSource());
    FilterNode* filterNode = RTTICast<FilterNode>(elem->GetDataContext());
    suic::ObservableCollection* pMenu = NULL;
    RootItem* rootItem = RTTICast<RootItem>(filterNode);

    // 图像节点
    if (filterNode->GetRTTIType()->InheritFrom(ImageRootItem::RTTIType()))
    {
        pMenu = _prjTreeMenu->images;
    }
    // .xml节点，需要读取
    else if (rootItem != NULL)
    {
    }
    // 分组节点
    else
    {
        FilterNodeCmd* pItem = NULL;

        pMenu = _prjTreeMenu->filter;

        pItem = DynamicCast<FilterNodeCmd>(pMenu->GetItem(0));
        pItem->SetShowItem(false);

        pItem = DynamicCast<FilterNodeCmd>(pMenu->GetItem(1));
        pItem->SetShowItem(false);

        pItem = DynamicCast<FilterNodeCmd>(pMenu->GetItem(2));
        pItem->SetShowItem(false);
    }

    if (NULL != pMenu)
    {
        suic::ContextMenu* ctxMenu = new suic::ContextMenu();

        ctxMenu->ref();
        ctxMenu->SetItemsSource(pMenu);
        SetSelectedItem(filterNode);
        UpdateLayout();

        ctxMenu->SetTag(filterNode);
        ctxMenu->SetPlacementTarget(this);
        ctxMenu->TrackContextMenu();

        ctxMenu->unref();
    }
}

//========================================================
// ImageSelectorWindow

ImageSelectorWindow::ImageSelectorWindow(SlnTreeManager* slnMana, ItemCollection* itemsColl, const String& strDir)
{
    _strDir = strDir;
    _treeView = NULL;
    _slnMana = slnMana;
    _itemsColl = itemsColl;
}

ImageSelectorWindow::~ImageSelectorWindow()
{
}

bool ImageSelectorWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    suic::String strName = pNode->FindAttri(_U("Name"));
    if (strName.Equals(_U("ImgShowBk")))
    {
        obj = new GridBk();
        return true;
    }
    else
    {
        return false;
    }
}

void ImageSelectorWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void ImageSelectorWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);

    _treeView = FindElem<ImageTreeView>(_U("ImgsTree"));
    if (NULL != _treeView)
    {
        InitTreeView();
        _treeView->AddSelectedItemChanged(new suic::RoutedPropChangedEventHandler(this, &ImageSelectorWindow::OnTreeItemSelectedChanged));
        _treeView->AddPreviewMouseDoubleClick(new MouseButtonEventHandler(this, &ImageSelectorWindow::OnDbClickTree));
    }
}

void ImageSelectorWindow::InitTreeView()
{
    _treeView->InitPrjTreeMenu(_slnMana);
    _treeView->SetItemsSource(_itemsColl);
}

void ImageSelectorWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
    
    suic::Button* pOkBtn = FindElem<suic::Button>(_U("Ok"));
    if (NULL != pOkBtn)
    {
        pOkBtn->AddClick(new ClickEventHandler(this, &ImageSelectorWindow::OnOkClick));
        pOkBtn->Enable(false);
    }

    suic::Button* pCancelBtn = FindElem<suic::Button>(_U("Cancel"));
    if (NULL != pCancelBtn)
    {
        pCancelBtn->AddClick(new ClickEventHandler(this, &ImageSelectorWindow::OnCancelClick));
    }
}

void ImageSelectorWindow::OnOkClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    if (_strSelImage.IndexOf(_U(".")) == -1)
    {
        _strSelImage = _U("");
    }
    AsyncClose();
}

void ImageSelectorWindow::OnCancelClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    _strSelImage = _U("");
    AsyncClose();
}

void ImageSelectorWindow::OnTreeItemSelectedChanged(suic::Element* sender, suic::RoutedPropChangedEventArg* e)
{
    Object* selObj = e->GetNewValue();
    suic::Button* pOkBtn = FindElem<suic::Button>(_U("Ok"));
    ImageRootItem* selElem = suic::DynamicCast<ImageRootItem>(selObj);

    pOkBtn->Enable(false);

    if (NULL != selElem)
    {
        _strSelImage = selElem->GetRelativePath();
        String strLocalPath = selElem->GetFullPath();
        suic::Rectangle* pRect = FindElem<suic::Rectangle>(_U("ImgShow"));
        if (NULL != pRect)
        {
            ImageBrush* imgBrush = RTTICast<ImageBrush>(pRect->GetFill());
            if (NULL != imgBrush)
            {
                imgBrush->SetImageSource(new ImageSource(strLocalPath));
                pRect->UpdateLayout();
                suic::TextBlock* tipBlock = FindElem<suic::TextBlock>(_U("ImgPath"));
                if (NULL != tipBlock)
                {
                    tipBlock->SetText(_strSelImage);
                }
                pOkBtn->Enable(true);
            }
            else
            {
                _strSelImage = _U("");
            }
        }
    }

    e->SetHandled(true);
}

void ImageSelectorWindow::OnDbClickTree(Element* sender, MouseButtonEventArg* e)
{
    if (e->GetMouseButton() != suic::MouseButton::mbLeftButton)
    {
        return;
    }

    if (!_strSelImage.Empty() && FindElem<suic::Button>(_U("Ok"))->IsEnabled())
    {
        if (_strSelImage.IndexOf(_U(".")) == -1)
        {
            _strSelImage = _U("");
        }
        AsyncClose();
    }
}