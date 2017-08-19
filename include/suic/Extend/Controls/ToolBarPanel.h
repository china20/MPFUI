// 华勤科技版权所有 2010-2011
// 
// 文件名：ToolBarPanel.h
// 功  能：工具条面板
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UITOOLBARPANEL_H_
#define _UITOOLBARPANEL_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/StackPanel.h>

namespace suic
{

class ToolBar;
class ToolBarPanel;

class SUIWGX_API ToolBarOverflowPanel : public Panel
{
public:

    static DpProperty* WrapWidthProperty;

    static bool StaticInit();

    static bool IsWrapWidthValid(Object* value);

    ToolBarOverflowPanel();

    int GetWrapWidth();
    void SetWrapWidth(int val);

private:

    void OnArrange(const Size& arrangeBounds);
    Size OnMeasure(const Size& constraint);

    ElementColl* CreateUIElementCollection(FrameworkElement* logicalParent);

    void ArrangeLine(int y, int lineHeight, int start, int end);

    ToolBar* GetToolBar();
    ToolBarPanel* GetToolBarPanel();

private:
    
    Size _panelSize;
    int _wrapWidth;
};

/// <summary>
/// ToolBarPanel。
/// </summary>
class SUIWGX_API ToolBarPanel : public StackPanel
{
public:

    static bool StaticInit();

    static void OnOrientationPropChanged(DpObject* d, DpPropChangedEventArg* e);

    ToolBarPanel();
    virtual ~ToolBarPanel();

    RTTIOfClass(ToolBarPanel)

    int GetMinLength() const;
    void SetMinLength(int val);

    int GetMaxLength() const;
    void SetMaxLength(int val);

protected:

    IScrollInfo* GetScrollInfo();
    bool IsScrolling();

    Array<Element*>& GetGeneratedItemsCollection();

    friend class ToolBarOverflowPanel;

protected:

    void OnPreApplyTemplate();

    Size OnMeasure(const Size& constraintSize);
    void OnArrange(const Size& arrangesize);

    void OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e);

private:

    ToolBar* GetToolBar();
    ToolBarOverflowPanel* GetToolBarOverflowPanel();
    bool MeasureGeneratedItems(bool asNeededPass, Size constraint, bool horizontal, int maxExtent, Size& panelDesiredSize, int& overflowExtent);

    void GenerateChildren();
    void RemoveAt(Element* child);
    void AddChildren(int pos, int itemCount);
    void MoveChildren(int fromPos, int toPos, int containerCount);
    void RemoveChildren(int pos, int containerCount);
    void ReplaceChildren(int pos, int itemCount, int containerCount);

private:

    int _minLength;
    int _maxLength;
    Array<Element*> _generatedItemsCollection;

};

}

#endif
