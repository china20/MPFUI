// 华勤科技版权所有 2010-2011
// 
// 文件名：ElementTreeManager.h
// 功  能：管理元素树。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _ELEMENTTREEMANAGER_H_
# define _ELEMENTTREEMANAGER_H_

#include "ElementTree.h"
#include "ControlWindow.h"
#include "PropertyWindow.h"

class DesignPanel;

class ObjTreeManager : public IElementTreeDoc
{
public:

    ObjTreeManager();
    ~ObjTreeManager();

    // 得到设计面板
    DesignPanel* GetDesignPanel();
    // 获取控件接口
    IControlWindow* GetControlWindow();
    // 获取属性面板接口
    IPropertyWindow* GetPropertyWindow();

    // 获取控件对象树
    ElementTree* GetElementTree();

    void SetProject(Project* pPrj);
    Project* GetProject() const;

    /// <summary>
    ///   初始化编辑的控件
    /// </summary>
    /// <param name="mainWnd">控件对象</param>
    /// <returns>无</returns>
    void InitElementTree(FrameworkElement* mainWnd);
    void SetTargetElement(DesignElement* resElem, ElementRootItem* elem);

    void UpdateElementTree();
    void SetSelectedElement(Object* elem);
    void SetMouseOverElement(Object* elem);

    void OnControlNotify(DesignMode mode, const suic::String& name);
    void OnTreeMouseEnter(suic::Element* sender, suic::MouseButtonEventArg* e);

protected:

    void BindTreeEvent();

protected:

    // 当前编辑的控件所属的工程
    Project* _project;
    ElementTree* _elemTree;
    DesignPanel* _designPanel;

    ControlWindowOp _ctrlWindow;
    PropertyWindowOp _propWindow;
};

inline DesignPanel* ObjTreeManager::GetDesignPanel()
{
    return _designPanel;
}

inline IControlWindow* ObjTreeManager::GetControlWindow()
{
    return &_ctrlWindow;
}

inline IPropertyWindow* ObjTreeManager::GetPropertyWindow()
{
    return &_propWindow;
}

inline ElementTree* ObjTreeManager::GetElementTree()
{
    return _elemTree;
}

inline void ObjTreeManager::SetProject(Project* pPrj)
{
    _project = pPrj;
}

inline Project* ObjTreeManager::GetProject() const
{
    return _project;
}


#endif

