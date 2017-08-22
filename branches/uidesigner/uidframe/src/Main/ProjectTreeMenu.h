// 华勤科技版权所有 2010-2011
// 
// 文件名：ProjectTreeMenu.h
// 功  能：元素层次树。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _PROJECTTREEMENU_H_
# define _PROJECTTREEMENU_H_

#include "Solution.h"

class SlnTreeManager;
class ObjTreeManager;
class PrjTreeMenu;

class MenuCommand : public ICommand
                    , public Object
{
public:

    BeginMember(MenuCommand, suic::NotifyPropChanged)
        MemberGetBool(Enabled)
        MemberGetBool(ShowItem)
    EndMember()

    MenuCommand()
    {
        _showItem = true;
        _enableItem = true;
    }

    bool GetEnabled()
    {
        return _enableItem;
    }

    void SetEnabled(bool val)
    {
        _enableItem = val;
    }

    bool GetShowItem()
    {
        return _showItem;
    }

    void SetShowItem(bool val)
    {
        _showItem = val;
    }

private:

    bool _showItem;
    bool _enableItem;
};


class RootElemMenuCmd : public MenuCommand
{
public:

    RootElemMenuCmd(PrjTreeMenu* tree, String name);

    bool CanExecute(Object* target, Object* parameter);
    void Execute(Object* target, Object* parameter);

    String ToString();

private:

    String _name;
    PrjTreeMenu* _tree;
};

class RootItemCmd : public MenuCommand
{
public:

    RootItemCmd(PrjTreeMenu* tree, String name);

    bool CanExecute(Object* target, Object* parameter);
    void Execute(Object* target, Object* parameter);

    void ApplyResourceTo(FileRootItem* resRoot);

    String ToString();

    void SetName(String name)
    {
        _name = name;
    }

    void UpdateTreeUI(bool objMana);

private:

    String _name;
    PrjTreeMenu* _tree;
};

class ImageNodeCmd : public MenuCommand
{
public:

    ImageNodeCmd(PrjTreeMenu* tree, String name);

    bool CanExecute(Object* target, Object* parameter);
    void Execute(Object* target, Object* parameter);

    String ToString();

private:

    String _name;
    PrjTreeMenu* _tree;
};

class FilterNodeCmd : public MenuCommand
{
public:

    FilterNodeCmd();
    FilterNodeCmd(PrjTreeMenu* tree, String name);

    bool CanExecute(Object* target, Object* parameter);
    void Execute(Object* target, Object* parameter);

    String ToString();

    void UpdateTreeUI(bool objMana);

    static void OpenPathOnNode(FilterNode* filterNode);

protected:

    void AddLayoutNode(FilterNode* pNode);
    void AddSubFilter(FilterNode* pNode);
    void AddThemeNode(FilterNode* pNode);
    void AddImageNode(FilterNode* pNode);

    bool IsAlpha(Char c) const;
    bool IsDigital(Char c) const;

    String GetAddName(const String& title, const String& name);
    bool CheckPath(const String& strPath);

private:

    String _name;
    PrjTreeMenu* _tree;
};

class ElementMenuItemCmd : public MenuCommand
{
public:

    ElementMenuItemCmd(String name);

    bool CanExecute(Object* target, Object* parameter);
    void Execute(Object* target, Object* parameter);

    void SetName(String name);

    String ToString();

    void OnMainReturnClick(suic::Object* sender, suic::EventArg* e);

private:

    String _name;
};

class ElemTreeMenu
{
public:

    ElemTreeMenu();
    ~ElemTreeMenu();

    void TrackElementMenu(ElementRootItem* rootElem, DesignElement* elem, Element* target);

private:

    suic::ObservableCollection* _elemMenu;

};

class PrjTreeMenu : public Object
{
public:

    PrjTreeMenu();
    ~PrjTreeMenu();

    SlnTreeManager* GetSlnTree();
    ObjTreeManager* GetObjTree();

    void UpdatePrjTree();
    void SelectTreeItem(Object* pItem);

    void Initialize(SlnTreeManager* stm, TreeView* tv);

    TreeView* treeView;
    SlnTreeManager* slnMana;

    suic::ObservableCollection* images;
    suic::ObservableCollection* rootElem;
    suic::ObservableCollection* filter;
};

#endif

