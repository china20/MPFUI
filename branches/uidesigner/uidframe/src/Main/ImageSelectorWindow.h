// 华勤科技版权所有 2010-2011
// 
// 文件名：ImageSelectorWindow.h
// 功  能：图像选择界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _IMAGESELECTORWINDOW_H_
# define _IMAGESELECTORWINDOW_H_

using namespace suic;

#include "XamlRootItem.h"
#include "ProjectTreeMenu.h"

class GridBk : public Grid
{
public:

    GridBk()
    {
        SetClipToBounds(true);
    }

    void OnRender(Drawing* drawing);
    void DrawBkgnd(Drawing* drawing);
};

class ImageTreeViewItem : public TreeViewItem
{
public:

    RTTIOfClass(ImageTreeViewItem)

    Size OnMeasure(const Size& constraint)
    {
        Object* obj = GetDataContext();
        FileRootItem* pItem = RTTICast<FileRootItem>(GetDataContext());
        if (NULL != pItem)
        {
            return Size();
        }
        else
        {
            return TreeViewItem::OnMeasure(constraint);
        }
    }

    Element* GetContainerForItemOverride()
    {
        return new ImageTreeViewItem();
    }

    bool IsItemItsOwnContainer(Object* item)
    {
        return (ImageTreeViewItem::RTTIType() == item->GetRTTIType());
    }
};

class ImageTreeView : public TreeView
{
public:

    RTTIOfClass(ImageTreeView)

    Element* GetContainerForItemOverride()
    {
        return new ImageTreeViewItem();
    }

    bool IsItemItsOwnContainer(Object* item)
    {
        return (ImageTreeViewItem::RTTIType() == item->GetRTTIType());
    }

    ImageTreeView();
    ~ImageTreeView();

    void InitPrjTreeMenu(SlnTreeManager* slnMana);

    void OnContextMenu(ContextMenuEventArg* e);

private:

    PrjTreeMenu* _prjTreeMenu;
};

class ImageSelectorWindow : public suic::Window
{
public:

    ImageSelectorWindow(SlnTreeManager* slnMana, ItemCollection* itemsColl, const String& strDir);
    ~ImageSelectorWindow();

    String GetSelectedImage() const 
    {
        return _strSelImage;
    }

    void InitTreeView();

    void OnOkClick(suic::Element* sender, suic::RoutedEventArg* e);
    void OnCancelClick(suic::Element* sender, suic::RoutedEventArg* e);
    void OnTreeItemSelectedChanged(suic::Element* sender, suic::RoutedPropChangedEventArg* e);
    void OnDbClickTree(Element* sender, MouseButtonEventArg* e);

protected:

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

protected:

    String _strDir;
    String _strSelImage;
    ImageTreeView* _treeView;

    SlnTreeManager* _slnMana;
    ItemCollection* _itemsColl;
};

#endif

