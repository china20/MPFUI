//////////////////////////////////////////////////////////////////////////////
// DesignPanel.cpp

#include "stdafx.h"
#include "DesignPanel.h"
#include "Project.h"
#include <Framework/Controls/Grid.h>
#include <Framework/Controls/DockPanel.h>

void DragBox::UpdateDragRect(suic::Rect rect)
{
    _rect = rect;
}

void DragBox::DrawDrag(suic::Drawing* drawing)
{
    drawing->FillRect(suic::DrawCtx::DefDraw, suic::Colors::Green, _rect.TofRect());
}

bool DragBox::HitTest(suic::Point pt) const
{
    return _rect.PointIn(pt);
}

suic::Rect DragBox::GetRect() const
{
    return _rect;
}


DesignPanel::DesignPanel()
    : _dragMeta(NULL)
    , _root(NULL)
    , _bIsDown(false)
    , _needMeasure(true)
    , _designMode(SelectMode)
    , _docTree(NULL)
    , _isAddSucc(false)
    , _addParent(NULL)
    , _dragMetaCount(10)
    , _resTarget(NULL)
{
    _dragMeta = NULL;

    _margin.SetLTRB(20, 20, 0, 0);

    _elemType = _U("Button");
    SetFocusable(true);
    LinkDragMetaEvent();
}

DesignPanel::~DesignPanel()
{
}

void DesignPanel::UpdateDElement()
{
    _needMeasure = true;
    UpdateLayout();
}

void DesignPanel::InitEditPanelInfo(ElementRootItem* rootElem)
{
    GetPropWindow()->InitEditPanelInfo(rootElem, this);
}

void DesignPanel::ResetDesignPanel(bool bAsNull)
{
    if (NULL != GetPropWindow())
    {
        GetPropWindow()->SetDesignPanel(bAsNull ? NULL : this);
    }
}

DesignElement* DesignPanel::GetResourceElement() const
{
    return _resTarget;
}

void DesignPanel::SwitchRootElement(DesignElement* resTarget, ElementRootItem* root)
{
    if (_root != root)
    {
        if (_root != NULL)
        {
            RemoveVisualChild(_root->GetUIElement());
        }

        _resTarget = resTarget;
        SETREFOBJ(_root, root);

        if (root != NULL)
        {
            _root->Refresh();

            GetPropWindow()->SetDesignPanel(this);
            if (NULL != root->GetProject())
            {
                root->GetProject()->SwitchToCurrent();
            }

            if (NULL != _root->GetUIElement())
            {
                _root->GetUIElement()->InvalidateResourceReference(false);
            }

            if (NULL == _root->GetFocusElement())
            {
                SetFocusedElement(_root->GetRootElement(), true);
            }
            else
            {
                SetFocusedElement(_root->GetFocusElement(), true);
            }

            AddVisualChild(_root->GetUIElement());
        }
        else
        {
            SetFocusedElement(NULL, false);
        }

        UpdateDElement();
    }
}

void DesignPanel::SetTreeDoc(IElementTreeDoc* docMana)
{
    _docTree = docMana;
}

IElementTreeDoc* DesignPanel::GetTreeDoc() const
{
    return _docTree;
}

IPropertyWindow* DesignPanel::GetPropWindow() const
{
    if (NULL == GetTreeDoc())
    {
        return NULL;
    }
    else
    {
        return GetTreeDoc()->GetPropertyWindow();
    }
}

ElementRootItem* DesignPanel::GetRootElement() const
{
    return _root;
}

DesignElement* DesignPanel::HitTestElement(Point pt)
{
    if (!_root)
    {
        return NULL;
    }
    DesignElement* focus = _root->HitTestElement(pt);
    return focus;
}

void DesignPanel::RefleshDesignPanel()
{
    _needMeasure = true;
    if (NULL != GetParent())
    {
        GetParent()->UpdateLayout();
    }
}

void DesignPanel::SetElementWidth(DesignElement* elem, int wid)
{
    SetterNode* pSetter = ElementRootItem::SetElementValue(elem, FrameworkElement::WidthProperty, new Integer(wid));
    if (NULL != pSetter)
    {
        UpdateElementSetter(elem, pSetter);
    }
}

void DesignPanel::SetElementHeight(DesignElement* elem, int hei)
{
    SetterNode* pSetter = ElementRootItem::SetElementValue(elem, FrameworkElement::HeightProperty, new Integer(hei));
    if (NULL != pSetter)
    {
        UpdateElementSetter(elem, pSetter);
    }
}

void DesignPanel::SetElementMargin(DesignElement* elem, Rect rect)
{
    SetterNode* pSetter = ElementRootItem::SetElementValue(elem, FrameworkElement::MarginProperty, new ORect(rect));
    if (NULL != pSetter)
    {
        UpdateElementSetter(elem, pSetter);
    }
}

void DesignPanel::UpdateModified()
{
    if (NULL != GetPropWindow())
    {
        GetPropWindow()->UpdateModified();
    }

    if (NULL != _root)
    {
        _root->SetModified(true);
        RefleshDesignPanel();
    }
}

void DesignPanel::UpdateElementSetter(DesignElement* elem, SetterNode* pSetter)
{
    if (NULL != elem)
    {
        if (NULL != GetPropWindow())
        {
            GetPropWindow()->UpdateSetter(pSetter, elem->NeedNotifyChanged());
        }

        UpdateModified(); 
    }
}

void DesignPanel::SetFocusedElement(DesignElement* elem, bool bFromDesign)
{
    DesignElement* focusElem = elem;

    if (NULL != _root)
    {
        if (NULL == focusElem)
        {
            focusElem = _root->GetRootElement();
        }
        _root->SetFocusElement(focusElem);
    }

    if (NULL != GetPropWindow())
    {
        GetPropWindow()->SetCurrentElement(GetResourceElement(), focusElem);
    }

    if (bFromDesign)
    {
        _docTree->SetSelectedElement(focusElem);
    }
    else
    {
        InvalidateVisual();
    }
}

void DesignPanel::SetMouseOverElement(DesignElement* elem, bool bFromDesign)
{
    if (_root->GetHotElement() != elem)
    {
        _root->SetHotElement(elem);
        if (bFromDesign)
        {
            //if (_designMode == AddMode)
            {
                _docTree->SetMouseOverElement(elem);
            }
        }
        else
        {
            InvalidateVisual();
        }
    }
}

void DesignPanel::SetDesignMode(DesignMode mode, const String& elemType)
{
    if (mode != DesignMode::NoneMode)
    {
        _designMode = mode;
    }

    _elemType = elemType;

    if (_designMode == AddMode)
    {
        InvalidateVisual();
    }
}

void DesignPanel::OnScrollChanged(suic::ScrollChangedEventArg& e)
{
    /*if (_root && _root->GetElement())
    {
        if (!e->IsOnlySet())
        {
            suic::Element* ePtr(_root->GetElement());
            suic::Point pt(ePtr->GetVisualOffset());

            pt.x += _horiOffset - e->GetHoriScrollOffset();
            pt.y += _vertOffset - e->GetVertScrollOffset();

            ePtr->SetVisualOffset(pt);
        }

        _horiOffset = e->GetHoriScrollOffset();
        _vertOffset = e->GetVertScrollOffset();

        // 需要重新布局
        e->SetIsArrange(true);
    }*/
}

int DesignPanel::GetVisualChildrenCount()
{
    if (NULL != _root && _root->GetUIElement() != NULL)
    {
        return 1;
    }
    return 0;
}

Element* DesignPanel::GetVisualChild(int index)
{
    return _root->GetUIElement();
}

Size DesignPanel::OnMeasure(const suic::Size& constraitSize)
{
    if (_root && _root->GetUIElement())
    {
        suic::FrameworkElement* pElem = _root->GetUIElement();
        Size size = pElem->GetDesiredSize();
        if (_needMeasure)
        {
            _needMeasure = false;
            suic::Window* pWnd = suic::RTTICast<suic::Window>(pElem);
            if (pWnd != NULL)
            {
                suic::Size szMeasure = constraitSize;
                if (pWnd->GetSizeToContent() == suic::SizeToContent::stcWidth)
                {
                    szMeasure.cx = suic::Numeric::MeasureInt;
                }
                else if (pWnd->GetSizeToContent() == suic::SizeToContent::stcHeight)
                {
                    szMeasure.cy = suic::Numeric::MeasureInt;
                }
                else if (pWnd->GetSizeToContent() == suic::SizeToContent::stcWidthAndHeight)
                {
                    szMeasure.cx = suic::Numeric::MeasureInt;
                    szMeasure.cy = suic::Numeric::MeasureInt;
                }

                pElem->Measure(szMeasure);
            }
            else
            {
                pElem->Measure(constraitSize);
            }
            size = pElem->GetDesiredSize();
        }

        size.cx += _margin.left + _margin.right;
        size.cy += _margin.top + _margin.bottom;

        return size;
    }
    else
    {
        return Size(0, 0);
    }
}

void DesignPanel::OnArrange(const suic::Size& availableSize)
{
    if (_root && _root->GetUIElement())
    {
        suic::Rect rcLay;
        suic::Element* pElem(_root->GetUIElement());

        rcLay.left = _margin.left;
        rcLay.top = _margin.top;
        rcLay.right = rcLay.left + pElem->GetDesiredSize().cx;
        rcLay.bottom = rcLay.top + pElem->GetDesiredSize().cy;

        pElem->Arrange(rcLay);
    }
}

int DesignPanel::HitTestFilter()
{
    return suic::HitTestFilterAction::ContinueSkipChildren;
}

void DesignPanel::DrawRectOnly(suic::Drawing* drawing, Rect rect, suic::Color clr, int iThickness)
{
    suic::Pen pen;
    pen.SetBrush(new suic::SolidColorBrush(clr));
    pen.SetThickness(iThickness);
    drawing->DrawRect(suic::DrawCtx::DefDraw, NULL, &pen, rect.TofRect());
}

Rect DesignPanel::GetRelativeRect(Element* Other)
{
    Point point = PointToScreen(Point());
    Point ptPanel = Other->PointToScreen(Point());

    ptPanel.x -= point.x;// - GetVisualOffset().x;
    ptPanel.y -= point.y;// - GetVisualOffset().y;

    return Rect(ptPanel, Other->GetRenderSize());
}

void DesignPanel::RenderInSelectMode(suic::Drawing* drawing)
{
    DesignElement* dfocused(_root->GetFocusElement());
    Rect rcDraw;

    if (!dfocused)
    {
        return;
    }
 
    suic::Element* curEle = dfocused->GetUIElement();
    // 如果选中的不是容器,则绘制容器边界
    if (!IsMain())
    {
        // 获取逻辑父元素
        DesignElement* pParent = dfocused->GetParentXamlElement();
        if (NULL != pParent)
        {
            suic::Rect rect = GetRelativeRect(pParent->GetUIElement());
            rect.Inflate(2, 2);
            DrawRectOnly(drawing, rect, ARGB(150,0x7B,0x97,0xE0), 2);
        }
    }

    if (_root->GetHotElement() && dfocused != _root->GetHotElement())
    {
        DrawDElement(drawing, _root->GetHotElement(), 3, ARGB(200,0x7B,0x97,0xE0));
    }

    rcDraw = GetRelativeRect(dfocused->GetUIElement());
 
    if (NULL == _dragMeta || (_dragMeta->index != 0 && _dragMeta->index != 9) || 
        (NULL == _dragMouseOver && !_bIsDown))
    {
        InitDragMeta(rcDraw);
        int iCount = _dragMetaCount;

        if (IsMain())
        {
            iCount -= 1;
        }

        for (int i = 1; i < iCount; ++i)
        {
            _dragMetas[i].OnRender(drawing);
        }
    }
    else
    {
        if (NULL != _dragMouseOver && _dragMouseOver != dfocused)
        {
            DesignElement* xamlParent = _dragMouseOver;
            suic::IAddChild* mvPanel = dynamic_cast<suic::IAddChild*>(_dragMouseOver->GetUIElement());
            
            if (NULL == mvPanel)
            {
                xamlParent = RTTICast<DesignElement>(_dragMouseOver->GetParent());
                mvPanel = dynamic_cast<suic::IAddChild*>(xamlParent->GetUIElement());
            }

            if (NULL != mvPanel && NULL != xamlParent->GetUIElement())
            {
                DrawDElement(drawing, xamlParent->GetUIElement(), 2, suic::Colors::Green);
            }
        }

        DrawDElement(drawing, dfocused, 1, suic::Colors::Red);

        int x = _trackDragPt.x - _startDragPt.x;
        int y = _trackDragPt.y - _startDragPt.y;

        rcDraw.Offset(x, y);
        DrawRectOnly(drawing, rcDraw, suic::Colors::Red, 2);

        InitDragMeta(rcDraw);
        _dragMetas[9].OnRender(drawing);
    }
}

void DesignPanel::RenderInMoveMode(suic::Drawing * drawing)
{
    DesignElement* dfocused(_root->GetFocusElement());
    if (dfocused)
    {
        DrawDElement(drawing, dfocused, 1, ARGB(200,0x7B,0x97,0xE0));
    }
}

void DesignPanel::DrawDElement(Drawing* drawing, DesignElement* delem, int t, Color clr)
{
    DrawDElement(drawing, delem->GetUIElement(), t, clr);
}

void DesignPanel::DrawDElement(Drawing* drawing, FrameworkElement* fe, int t, Color clr)
{
    if (NULL != fe)
    {
        Rect rect = GetRelativeRect(fe);
        rect.Inflate(t, t);
        DrawRectOnly(drawing, rect, clr, t);
    }
}

void DesignPanel::RenderInAddMode(suic::Drawing * drawing)
{
    DesignElement* dfocused(_root->GetFocusElement());
    DesignElement* dmouseover(_root->GetHotElement());

    if (dmouseover && !_bIsDown)
    {
        DrawDElement(drawing, dmouseover, 8, ARGB(200,0x7B,0x97,0xE0));
    }

    if (_bIsDown && dfocused)
    {
        DrawDElement(drawing, dfocused, 2, ARGB(200,0xEE,0x11,0x00));
    }
}

void DesignPanel::OnDrawDesign(suic::Drawing* drawing)
{
    if (_root)
    {
        Point ptSelf = PointToScreen(Point());

        switch (_designMode)
        {
        case SelectMode:
            RenderInSelectMode(drawing);
            break;

        case MoveMode:
            RenderInMoveMode(drawing);
            break;

        case AddMode:
            RenderInAddMode(drawing);
            break;
        }
    }
}

void DesignPanel::OnDrawBkgnd(suic::Drawing* drawing)
{
    const int RECSIZE = 8;
    suic::Rect rect(0, 0, GetRenderSize().cx, GetRenderSize().cy);
    int iBeg = RECSIZE;
    int index = 0;
    int indexY = 0;
    suic::Color clrs[2] = {0xFF2D2D2D, 0xFF2F2F2F};

    for (int y = 0; y < GetActualHeight();)
    {
        index = indexY;
        rect.left = 0;
        rect.top = y;
        rect.bottom = y + RECSIZE;

        for (int x = 0; x < GetActualWidth(); )
        {
            rect.right = rect.left + RECSIZE;
            drawing->FillRect(suic::DrawCtx::DefDraw, clrs[index], rect.TofRect());
            rect.left = rect.right;

            ++index;
            index = index % 2;
            x += RECSIZE;
        }

        ++indexY;
        indexY = indexY % 2;

        y += RECSIZE;
    }
}

void DesignPanel::OnLoaded(suic::LoadedEventArg* e)
{
    IDesignWindow::OnLoaded(e);
}

void DesignPanel::DispatchRender(Drawing * drawing)
{
    if (_root && _root->GetUIElement())
    {
        Element* pElem(_root->GetUIElement());
        // 绘制各种辅助线
        OnDrawDesign(drawing);
    }
}

void DesignPanel::OnRender(suic::Drawing* drawing)
{
    OnDrawBkgnd(drawing);
}

void DesignPanel::RenderChildren(suic::Drawing* drawing)
{
    IDesignWindow::RenderChildren(drawing);
    RenderDesignMeta(drawing);
}

void DesignPanel::RenderDesignMeta(suic::Drawing* drawing)
{
    // 绘制各种辅助线
    OnDrawDesign(drawing);

    suic::FormattedText fmtText;
    suic::String strText(_U("MPF 界面设计器Beta"));
    suic::fRect rect(Point(), GetRenderSize());

    fmtText.SetColor(0x1000FF00);

    drawing->DrawString(&fmtText, strText.c_str(), strText.Length(), &rect);
}

void DesignPanel::MouseMoveInSelectMode(suic::MouseButtonEventArg* e)
{
    DragMeta* pMeta = NULL;

    if (!_bIsDown)
    {
        pMeta = HittestDragMeta(e->GetMousePoint());

        if (pMeta)
        {
            pMeta->cursor->SetCursor();
        }

        DesignElement* oldMouseOver = _root->GetHotElement();
        DesignElement* mouseOver = HitTestElement(e->GetMousePoint());

        if (mouseOver != oldMouseOver)
        {
            SetMouseOverElement(mouseOver, true);
            InvalidateVisual();
        }
    }
    else if (_dragMeta)
    {
        _trackDragPt = e->GetMousePoint();
        _dragMeta->cursor->SetCursor();
        _dragMouseOver = HitTestElement(e->GetMousePoint());

        _dragMeta->DragMetaAction(*_dragMeta, eMouseAction::eMouseMove);
    }
}

void DesignPanel::MouseMoveInAddMode(suic::MouseButtonEventArg* e)
{
    _trackDragPt = e->GetMousePoint();

    if (!_bIsDown)
    {
        DesignElement* oldMouseOver = _root->GetHotElement();
        DesignElement* mouseOver = HitTestElement(e->GetMousePoint());

        if (mouseOver != oldMouseOver)
        {
            SetMouseOverElement(mouseOver, true);
            InvalidateVisual();
        }
    }
    else 
    {
        if (_addParent != NULL && !_addParent->IsLocked())
        {
            MLDownInAddMode(e);
        }

        DesignElement* fdelem = _root->GetFocusElement();

        if (fdelem != _addParent && fdelem && !fdelem->IsLocked() && _isAddSucc)
        {
            int x = _trackDragPt.x - _startDragPt.x;
            int y = _trackDragPt.y - _startDragPt.y;
            Rect rcMgr(_startDragMgr);

            _needMeasure = true;

            if (x < 0)
            {
                rcMgr.left += x;
            }

            if (y < 0)
            {
                rcMgr.top += y;
            }

            if (_addParent != NULL)
            {
                MLDownInAddMode(e);
            }

            if (_isAddSucc)
            {
                FrameworkElement* elem(CASTTOFE(fdelem->GetUIElement()));

                if (elem->GetHorizontalAlignment() >= HoriAlign::haCenter)
                {
                    //rcMgr.left = 0;
                }

                if (elem->GetVerticalAlignment() >= VertAlign::vaCenter)
                {
                    //rcMgr.top = 0;
                }

                SetElementMargin(fdelem, rcMgr);
                SetElementWidth(fdelem, abs(x));
                SetElementHeight(fdelem, abs(y));

                InvalidateMeasure();
                InvalidateArrange();
            }
        }

        InvalidateVisual();
    }

    if (_root->GetHotElement())
    {
        static OCursor* curs(RTTICast<OCursor>(FindRes(_U("addcursor"))));
        if (curs)
        {
            curs->SetCursor();
        }
        else
        {
            OCursor::Hand->SetCursor();
        }
    }
}

void DesignPanel::OnPreviewSetCursor(CursorEventArg* e)
{
    OnSetCursor(e);
    e->SetHandled(true);
}

void DesignPanel::OnSetCursor(CursorEventArg* e)
{
    if (_designMode == AddMode)
    {
        static OCursor* curs(RTTICast<OCursor>(FindRes(_U("addcursor"))));
        e->SetCursor(curs);
        e->SetHandled(true);
    }
}

void DesignPanel::OnMouseMove(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    if (!IsEnabled())
    {
        return;
    }
    
    if (_designMode == MoveMode)
    {
        if (!_root)
        {
            return;
        }

        if (_bIsDown)
        {
            _trackDragPt = e->GetMousePoint();
            int iOffsetX = _trackDragPt.x - _startDragPt.x;
            int iOffsetY = _trackDragPt.y - _startDragPt.y;

            if (1 || iOffsetX > 0)
            {
                _margin.left = _startMoveMgr.left + iOffsetX;
            }
            else
            {
                _margin.right = _startMoveMgr.right - iOffsetX;
            }
            
            if (1 || iOffsetY > 0)
            {
                _margin.top = _startMoveMgr.top + iOffsetY;
            }
            else
            {
                _margin.bottom = _startMoveMgr.bottom - iOffsetY;
            }

            RefleshDesignPanel();
        }

        return;
    }

    if (_root)
    {
        if (_designMode == SelectMode)
        {
            MouseMoveInSelectMode(e);
        }
        else if (_designMode == AddMode)
        {
            MouseMoveInAddMode(e);
        }
    }
}

void DesignPanel::OnMouseEnter(suic::MouseButtonEventArg* e)
{
    OnMouseMove(e);
}

void DesignPanel::OnMouseLeave(suic::MouseButtonEventArg* e)
{
    if (_root && IsEnabled())
    {
        SetMouseOverElement(NULL, true);
        InvalidateVisual();
    }
}

void DesignPanel::MLDownInSelectMode(suic::MouseButtonEventArg* e)
{
    DesignElement* mouseOver = _root->GetHotElement();
    _dragMeta = HittestDragMeta(e->GetMousePoint());

    if (!_dragMeta || !_dragMeta->dragdot)
    {
        DesignElement* selPtr = _root->GetFocusElement();
        if (selPtr != mouseOver)
        {
            SetFocusedElement(mouseOver, true);
            InvalidateVisual();
            _dragMeta = HittestDragMeta(e->GetMousePoint());
        }
    }

    if (!_root->GetFocusElement())
    {
        SetFocusedElement(mouseOver, true);
    }

    if (!_root->GetFocusElement())
    {
        SetFocusedElement(_root->GetRootElement(), true);
    }

    if (_dragMeta && _root->GetFocusElement())
    {
        FrameworkElement* ele(_root->GetFocusElement()->GetUIElement());
        _startDragRc = CalcElementRect(_root->GetFocusElement());
        _startDragMgr = ele->GetMargin();

        _dragMeta->cursor->SetCursor();
    }
}

int DesignPanel::CalcStackPanelPos(StackPanel* parent, Point pt, Size& size)
{
    Point ptself(PointToScreen(Point()));
    pt.x = pt.x - ptself.x;
    pt.y = pt.y - ptself.y;

    ptself = parent->PointToScreen(Point());

    if (parent->GetOrientation() == Orientation::Horizontal)
    {
        for (int i = 0; i < parent->GetVisualChildrenCount(); ++i)
        {
            FrameworkElementPtr child(parent->GetVisualChild(i));
            if (child)
            {
                Rect rcChild(GetRenderBound(child.get()));
                if (pt.x >= rcChild.left)
                {
                    if (i < parent->GetVisualChildrenCount() - 1)
                    {
                        child = parent->GetVisualChild(i + 1);
                        if (child.get() && pt.x > GetRenderBound(child.get()).left)
                        {
                            continue;
                        }
                    }

                    size.cx = pt.x - rcChild.right;
                    return i + 1;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < parent->GetVisualChildrenCount(); ++i)
        {
            FrameworkElementPtr child(parent->GetVisualChild(i));
            if (child.get())
            {
                Rect rcChild(GetRenderBound(child.get()));
                if (pt.y >= rcChild.top)
                {
                    if (i < parent->GetVisualChildrenCount() - 1)
                    {
                        child = parent->GetVisualChild(i + 1);
                        if (child.get() && pt.y > GetRenderBound(child.get()).top)
                        {
                            continue;
                        }
                    }

                    size.cy = pt.y - rcChild.bottom;
                    return i + 1;
                }
            }
        }
    }

    return 0;
}

Rect DesignPanel::GetInsertElemMargin(DesignElement* parent, Point curPt)
{
    if (!parent)
    {
        return Rect();
    }

    // 设置边距
    Point pt(parent->GetUIElement()->PointToScreen(Point()));
    Rect rcMgr;

    rcMgr.left = curPt.x - pt.x;
    rcMgr.top = curPt.y - pt.y;

    return rcMgr;
}

void DesignPanel::MLDownInAddMode(suic::MouseButtonEventArg* e)
{
    DesignElement* delem(_addParent);

    if (!_addParent)
    {
        return;
    }

    // 可以增加元素
    if (CheckCanAddElement(delem) && !delem->IsLocked())
    {
        // 创建新元素
        String strPath;
        strPath.Format(_U("/mpfuid;/resource/uidesign/Controls/%s.control"), _elemType.c_str());
        DesignElementPtr child;

        if (delem->CreateElement(child, strPath, _elemType))
        {
            _root->AddChileElement(delem, child.get());

            // 设置边距
            Rect rcMgr(GetInsertElemMargin(delem, e->GetMousePoint()));

            suic::Grid* pGrid = RTTICast<suic::Grid>(delem->GetUIElement());
            suic::Canvas* pCanvas = RTTICast<suic::Canvas>(delem->GetUIElement());

            StackPanel* stackPanel = RTTICast<StackPanel>(delem->GetUIElement());
 
            _startDragMgr = rcMgr;

            if (stackPanel != NULL)
            {
                if (stackPanel->GetOrientation() == Orientation::Horizontal)
                {
                    _startDragMgr.left = 0;
                }
                else
                {
                    _startDragMgr.top = 0;
                }
            }
            else if (pGrid == NULL && pCanvas == NULL)
            {
                _startDragMgr.left = 0;
                _startDragMgr.top = 0;
            }
            else if (pCanvas != NULL)
            {
                ;
            }
 
            if (!_startDragMgr.IsZero())
            {
                SetElementMargin(child.get(), rcMgr);
            }

            SetElementWidth(child.get(), 0);
            SetElementHeight(child.get(), 0);

            SetFocusedElement(child.get(), true);

            _needMeasure = true;
            UpdateLayout();
        }
        else
        {
            _isAddSucc = false;
        }
    }
    else
    {
        _isAddSucc = false;
    }
    _addParent = NULL;
}

bool DesignPanel::CheckCanAddElement(DesignElement* delem)
{
    if (!delem)
    {
        return false;
    }
    else
    {
        // 这是模版容器对象，始终返回true
        if (delem->IsTemplate() && delem->IsTemplateTaget())
        {
            return true;
        }

        suic::IAddChild* pAddChild = DynamicCast<suic::IAddChild>(delem->GetUIElement());
        if (NULL != pAddChild)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void DesignPanel::CompleteAddElement()
{
    if (NULL != _root->GetFocusElement())
    {
        DesignElement* fdelem = _root->GetFocusElement();
        FrameworkElement* elem(CASTTOFE(fdelem->GetUIElement()));

        if (elem->GetWidth() < 6 && elem->GetHeight() < 6)
        {
            RemoveElement(fdelem);
        }
    }
}

void DesignPanel::OnPreviewMouseDown(suic::MouseButtonEventArg* e)
{
    if (e->GetMouseButton() == suic::MouseButton::mbRightButton)
    {
        
    }
    else if (e->GetMouseButton() == suic::MouseButton::mbLeftButton)
    {
        e->SetHandled(true);
        OnMouseLeftButtonDown(e);
    }
}

void DesignPanel::OnPreviewMouseUp(suic::MouseButtonEventArg* e)
{
    if (e->GetMouseButton() == suic::MouseButton::mbLeftButton)
    {
        e->SetHandled(true);
        OnMouseLeftButtonUp(e);
    }
}

void DesignPanel::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    _isAddSucc = false;

    if (!_root)
    {
        return;
    }

    SetCaptureMouse();
    Focus();

    _bIsDown = true;
    _dragMouseOver = NULL;

    e->SetHandled(true);

    _startDragPt = e->GetMousePoint();
    _trackDragPt = _startDragPt;
    _startMoveMgr = _margin;

    if (_designMode == MoveMode)
    {
        Element* root(_root->GetUIElement());
        Point pt(root->PointToScreen(Point()));
        Point rpt(e->GetMousePoint());

        Rect rect(pt.x, pt.y, root->GetActualWidth(), root->GetActualHeight());
        if (rect.PointIn(rpt))
        {
            OCursor::Hand->SetCursor();
        }
        else
        {
            _bIsDown = false;
        }

        return;
    }

    _dragMeta = NULL;
    _mouseOverRc.SetEmpty();

    DesignElement* mouseOver = HitTestElement(e->GetMousePoint());
    SetMouseOverElement(mouseOver, true);
    
    // 选择模式下
    if (_designMode == SelectMode)
    {
        MLDownInSelectMode(e);
    }
    // 增加模式下
    else if (_designMode == AddMode)
    {
        _addParent = mouseOver;
        _isAddSucc = true;

        static OCursor* curs(RTTICast<OCursor>(FindRes(_U("addcursor"))));
        if (curs)
        {
            curs->SetCursor();
        }
    }
}

void DesignPanel::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    if (!_root)
    {
        return;
    }

    e->SetHandled(true); 

    if (_dragMeta)
    {
        _trackDragPt = e->GetMousePoint();
        _dragMeta->DragMetaAction(*_dragMeta, eMouseAction::eMouseLeftButtonUp);
    }

    _bIsDown = false;

    if (_designMode == AddMode)
    {
        if (_isAddSucc)
        {
            CompleteAddElement();
        }

        // 恢复到选择模式
        if (_docTree != NULL)
        {
            _docTree->SetMouseOverElement(NULL);
        }
    }
    else if (_designMode == SelectMode)
    {
        if (_root != NULL && _root->GetFocusElement() != NULL)
        {
            FrameworkElement* feFucus = _root->GetFocusElement()->GetUIElement();
            suic::ToggleButton* pToggleBtn = RTTICast<suic::ToggleButton>(feFucus);
            if (NULL != pToggleBtn)
            {
                if (pToggleBtn->IsChecked())
                {
                    pToggleBtn->SetChecked(suic::Boolean::False);
                }
                else
                {
                    pToggleBtn->SetChecked(suic::Boolean::True);
                }
            }

            // 如果ctrl键压下
            if (suic::Keyboard::IsControlDown())
            {
                // 处理拖动复制
                ;
            }
        }
    }

    _addParent = NULL;
    _dragMeta = NULL;
    _dragMouseOver = NULL;

    InvalidateVisual();
    ReleaseCaptureMouse();
}

void DesignPanel::OnContextMenu(ContextMenuEventArg* e)
{
    e->SetHandled(true);

    if (_root)
    {
        suic::Point point(e->GetCursorLeft(), e->GetCursorTop());
        DesignElement* focus = HitTestElement(point);
        if (focus != NULL)
        {
            SetFocusedElement(focus, true);
            InvalidateVisual();

            // 右键点击了布局元素
            _elemMenu.TrackElementMenu(_root, focus, this);
        }
    }
}

void DesignPanel::OnMouseRightButtonDown(suic::MouseButtonEventArg* e)
{
    
}

void DesignPanel::RemoveElement(DesignElement* dfocus)
{
    if (!dfocus)
    {
        return;
    }

    DesignElement* pParent = RTTICast<DesignElement>(dfocus->GetParent());
    if (pParent && !dfocus->IsLocked())
    {
       GetRootElement()->RemoveChileElement(pParent, dfocus);
    }
}

void DesignPanel::OnKeyDown(suic::KeyboardEventArg* e)
{
    if (_designMode != SelectMode || !_root || !_root->GetFocusElement())
    {
        return;
    }

    // 主窗口不移动
    if (_root->GetFocusElement() == _root->GetRootElement())
    {
        return;
    }

    DesignElement* dfocus(_root->GetFocusElement());
    FrameworkElementPtr felem(dfocus->GetUIElement());
    suic::Rect rcMgr = felem->GetMargin();

    e->SetHandled(true);

    if (e->GetKey() == Key::kDelete)
    {
        RemoveElement(dfocus);
        return;
    }
    
    if (felem->GetHorizontalAlignment() == HoriAlign::haLeft)
    {
        if (e->GetKey() == Key::kLeft)
        {
            rcMgr.left -= 1;
        }
        else if (e->GetKey() == Key::kRight)
        {
            rcMgr.left += 1;
        }
    }
    else if (felem->GetHorizontalAlignment() == HoriAlign::haRight)
    {
        if (e->GetKey() == Key::kLeft)
        {
            rcMgr.right += 1;
        }
        else if (e->GetKey() == Key::kRight)
        {
            rcMgr.right -= 1;
        }
    }

    if (felem->GetVerticalAlignment() == VertAlign::vaTop)
    {
        if (e->GetKey() == Key::kUp)
        {
            rcMgr.top -= 1;
        }
        else if (e->GetKey() == Key::kDown)
        {
            rcMgr.top += 1;
        }
    }
    else if (felem->GetVerticalAlignment() == VertAlign::vaBottom)
    {
        if (e->GetKey() == Key::kUp)
        {
            rcMgr.bottom += 1;
        }
        else if (e->GetKey() == Key::kDown)
        {
            rcMgr.bottom -= 1;
        }
    }
    else
    {
        return;
    }

    SetElementMargin(dfocus, rcMgr);
    felem->InvalidateMeasure();    
    //_docTree->UpdateDragProp(GetDElement(), dfocus);

    InvalidateArrange();
    InvalidateVisual();
}


void DesignPanel::OnTextInput(suic::KeyboardEventArg* e)
{
    e->SetHandled(true);

    if (e->GetKey() == 'z' && e->GetModifierKey() == suic::ModifierKey::mkControl)
    {
        //GetRootElement()->GetRedoUndoMana()->Undo();
        return;
    }
    else if (e->GetKey() == 'y' && e->GetModifierKey() == suic::ModifierKey::mkControl)
    {
        //GetRootElement()->GetRedoUndoMana()->Redo();
        return;
    }
}

void DesignPanel::OnDragLeft(DragMeta& meta, int iAction)
{
    if (meta.enable)
    {
        suic::Rect rect;
        InternalHandleDrag(rect, (int*)&rect.left, NULL);
    }
}

void DesignPanel::OnDragLeftTop(DragMeta& meta, int iAction)
{
    if (meta.enable)
    {
        suic::Rect rect;
        InternalHandleDrag(rect, (int*)&rect.left, (int*)&rect.top);
    }
}

void DesignPanel::OnDragTop(DragMeta& meta, int iAction)
{
    if (meta.enable)
    {
        suic::Rect rect;
        InternalHandleDrag(rect, NULL, (int*)&rect.top);
    }   
}

void DesignPanel::OnDragRightTop(DragMeta& meta, int iAction)
{
    if (meta.enable)
    {
        suic::Rect rect;
        InternalHandleDrag(rect, (int*)&rect.right, (int*)&rect.top);
    } 
}

void DesignPanel::OnDragRight(DragMeta& meta, int iAction)
{
    suic::Rect rect;
    InternalHandleDrag(rect, (int*)&rect.right, NULL);
}

void DesignPanel::OnDragRightBottom(DragMeta& meta, int iAction)
{
    suic::Rect rect;
    InternalHandleDrag(rect, (int*)&rect.right, (int*)&rect.bottom);
}

void DesignPanel::OnDragBottom(DragMeta& meta, int iAction)
{
    suic::Rect rect;
    InternalHandleDrag(rect, NULL, (int*)&rect.bottom);
}

void DesignPanel::OnDragLeftBottom(DragMeta& meta, int iAction)
{
    if (meta.enable)
    {
        suic::Rect rect;
        InternalHandleDrag(rect, (int*)&rect.left, (int*)&rect.bottom);
    }
}

void DesignPanel::OnDragBox(DragMeta& meta, int iAction)
{
    if (meta.enable)
    {
        OnDragMove(meta, iAction);
    }
}

bool DesignPanel::GetDragingRect(suic::Rect& rcMgr)
{
    DesignElement* dfocus(_root->GetFocusElement());
    FrameworkElementPtr focused = dfocus->GetUIElement();

    int x = _trackDragPt.x - _startDragPt.x;
    int y = _trackDragPt.y - _startDragPt.y;

    rcMgr = _startDragMgr;

    if (x != 0 || y != 0)
    {
        int iOk = 2;

        // 根据对其方式调整margin
        if (focused->GetHorizontalAlignment() == HoriAlign::haLeft)
        {
            rcMgr.left += x;
        }
        else if (focused->GetHorizontalAlignment() == HoriAlign::haRight)
        {
            rcMgr.right -= x;
        }
        else
        {
            --iOk;
        }

        if (focused->GetVerticalAlignment() == VertAlign::vaTop)
        {
            rcMgr.top += y;
        }
        else if (focused->GetVerticalAlignment() == VertAlign::vaBottom)
        {
            rcMgr.bottom -= y;
        }
        else
        {
            --iOk;
        }

        if (iOk == 0)
        {
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

void DesignPanel::DragMoveFinish(DesignElement* dfocus, DesignElement* dparent)
{
    // 根元素不做移动处理
    if (_root->GetRootElement() == dfocus)
    {
        return;
    }

    FrameworkElementPtr focused = dfocus->GetUIElement();

    if (NULL == dparent)
    {
        return;
    }

    PanelPtr grid(dparent->GetUIElement());
    ContentControl* cont(DynamicCast<ContentControl>(dparent->GetUIElement()));

    if (!grid && !cont)
    {
        return;
    }

    if (dfocus->IsLocked())
    {
        return;
    }

    suic::Rect rcMgr;

    if (GetDragingRect(rcMgr))
    {
        SetElementMargin(dfocus, rcMgr);

        focused->InvalidateMeasure();
        if (NULL != focused->GetParent())
        {
            focused->GetParent()->InvalidateArrange();
        }

        InvalidateVisual();
    }
}

bool DesignPanel::IsAncestor(DesignElement* child, DesignElement* ancestor)
{
    if (NULL == child || NULL == ancestor)
    {
        return false;
    }
    else
    {
        return ancestor->GetUIElement()->IsAncestorOf(child->GetUIElement());
    }
}

DesignElement* DesignPanel::GetSameParentElement(DesignElement* dfocus, DesignElement* dMouse)
{
    DesignElement* dFocusParent = dfocus->GetParentXamlElement();
    DesignElement* dMouseParent = dMouse->GetParentXamlElement();
    DesignElement* dPrevMouse = dMouse;

    while (NULL != dMouseParent)
    {
        if (dMouseParent == dFocusParent)
        {
            break;
        }

        dPrevMouse = dMouseParent;
        dMouseParent = dMouseParent->GetParentXamlElement();
    }

    return dPrevMouse;
}

void DesignPanel::HandleCopyElement(DesignElement* dfocus, suic::IAddChild* mvPanel)
{
    ///
    /// 进入设计模式，让内核切换资源等
    ///
    DesignHelper::EnterDesignMode();

    ///
    /// 拷贝一个元素
    ///
    ResNodePtr cloneElem;
    dfocus->CloneNode(cloneElem);
    DesignElement* pCloneElem = RTTICast<DesignElement>(cloneElem.get());
    if (NULL != pCloneElem)
    {
        pCloneElem->InstantiateDesignElement(_dragMouseOver, NULL, false);
        _root->AddChileElement(_dragMouseOver, pCloneElem, false);
        _dragMouseOver->AddXamlElement(pCloneElem);

        suic::Rect rcDraw;
        suic::Grid* newGrid = dynamic_cast<suic::Grid*>(mvPanel);
        suic::Canvas* newCanvas = dynamic_cast<suic::Canvas*>(mvPanel);

        // 如果是Grid或Canvas，计算其相对坐标
        // 其它布局面板直接增加
        if (NULL != newGrid || NULL != newCanvas)
        {
            rcDraw = GetRelativeRect(dfocus->GetUIElement());
            int x = _trackDragPt.x - _startDragPt.x;
            int y = _trackDragPt.y - _startDragPt.y;
            suic::Point ptDesign = PointToScreen(suic::Point());
            suic::Point ptParent = _dragMouseOver->GetUIElement()->PointToScreen(suic::Point());

            x += ptDesign.x - ptParent.x;
            y += ptDesign.y - ptParent.y;

            rcDraw.Offset(x, y);

            rcDraw.right = 0;
            rcDraw.bottom = 0;
        }

        SetElementMargin(pCloneElem, rcDraw);
        SetFocusedElement(pCloneElem, true);
    }

    DesignHelper::ExitDesignMode();
}

void DesignPanel::OnDragMove(DragMeta& meta, int iAction)
{
    // 根元素不做移动处理
    if (_root->GetRootElement() == _root->GetFocusElement())
    {
        return;
    }

    // 当前焦点元素处于锁定状态，直接返回
    if (_root->GetFocusElement() && _root->GetFocusElement()->GetLockElement())
    {
        return;
    }

    // 鼠标移动中
    if (iAction == eMouseAction::eMouseMove)
    {
        InvalidateVisual();
    }
    // 释放鼠标左键
    else if (eMouseAction::eMouseLeftButtonUp == iAction && NULL != _dragMouseOver)
    {
        DesignElement* dfocus(_root->GetFocusElement());
        // 当前焦点元素的父元素
        DesignElement* dparent(DynamicCast<DesignElement>(dfocus->GetParent()));
        // 当前鼠标所在元素的父元素
        DesignElement* xamlParent = RTTICast<DesignElement>(_dragMouseOver->GetParent());
        // 当前鼠标所在元素是否是一个Panel布局面板
        suic::IAddChild* mvPanel = dynamic_cast<suic::IAddChild*>(_dragMouseOver->GetUIElement());

        // 当前鼠标所在元素的父元素是否是一个Panel布局面板
        suic::IAddChild* mvParent = NULL;

        ///
        /// 如果ctrl键压下，做复制元素操作
        ///
        if (suic::Keyboard::IsControlDown() && !suic::Keyboard::IsAltDown() && !suic::Keyboard::IsShiftDown())
        {
            if (NULL != mvPanel && dfocus != _dragMouseOver)
            {
                HandleCopyElement(dfocus, mvPanel);
            }
            return;
        }

        if (NULL != xamlParent)
        {
            mvParent = dynamic_cast<suic::IAddChild*>(xamlParent->GetUIElement());
        }

        if (suic::Keyboard::IsControlDown() && suic::Keyboard::IsAltDown())
        {
            suic::TextBlock* overTxtBlock = suic::RTTICast<suic::TextBlock>(_dragMouseOver->GetUIElement());
            if (NULL != overTxtBlock)
            {
                return;
            }

            if ((mvParent != NULL || NULL != mvPanel) && 
                dparent != _dragMouseOver && !IsAncestor(_dragMouseOver, dfocus))
            {
                DesignElement* newParent = _dragMouseOver;
                suic::IAddChild* newPanel = mvPanel;

                dfocus->ref();
                dparent->RemoveElement(dfocus);
                dparent->GetUIElement()->UpdateArrange();

                if (NULL == newPanel)
                {
                    newParent = xamlParent;
                    newPanel = mvParent;
                }

                suic::Grid* newGrid = dynamic_cast<suic::Grid*>(newPanel);
                suic::Canvas* newCanvas = dynamic_cast<suic::Canvas*>(newPanel);

                // 如果是Grid或Canvas，计算其相对坐标
                // 其它布局面板直接增加
                if (NULL != newGrid || NULL != newCanvas)
                {
                    suic::Rect rcDraw = GetRelativeRect(dfocus->GetUIElement());
                    int x = _trackDragPt.x - _startDragPt.x;
                    int y = _trackDragPt.y - _startDragPt.y;
                    suic::Point ptDesign = PointToScreen(suic::Point());
                    suic::Point ptParent = newParent->GetUIElement()->PointToScreen(suic::Point());

                    x += ptDesign.x - ptParent.x;
                    y += ptDesign.y - ptParent.y;

                    rcDraw.Offset(x, y);

                    rcDraw.right = 0;
                    rcDraw.bottom = 0;

                    newParent->AddElement(dfocus);
                    SetElementMargin(dfocus, rcDraw);
                }
                else
                {
                    suic::Panel* tmPanel = dynamic_cast<suic::Panel*>(newParent->GetUIElement());
                    if (NULL == tmPanel)
                    {
                        newParent->AddElement(dfocus);
                    }
                    else
                    {
                        suic::Rect hitRect;
                        int newIndex = newParent->IndexOf(_trackDragPt, hitRect);
                        if (-1 == newIndex)
                        {
                            newParent->AddElement(dfocus);
                        }
                        else
                        {
                            newParent->InsertElement(newIndex, dfocus);
                        }
                    }

                    SetElementMargin(dfocus, Rect());
                }

                dfocus->unref();

                _root->SetModified(true);
                newParent->GetUIElement()->UpdateLayout();
                UpdateDElement();
                _docTree->UpdateElementTree();
            }
        }
        else if (!suic::Keyboard::IsControlDown())
        {
            DesignElement* dSameMouse = GetSameParentElement(dfocus, _dragMouseOver);
            // 有相同的父元素
            if (NULL != dSameMouse && dfocus != _dragMouseOver)
            {
                xamlParent = dSameMouse->GetParentXamlElement();
                if (NULL != xamlParent)
                {
                    suic::Panel* dSamePanel = dynamic_cast<suic::Panel*>(xamlParent->GetUIElement());
                    if (NULL != dSamePanel)
                    {
                        xamlParent->MoveElement(dfocus, dSameMouse);
                        UpdateModified();
                        _docTree->UpdateElementTree();
                        UpdateLayout();
                        return;
                    }
                }
            }

            //if (_dragMouseOver == dparent)
            {
                DragMoveFinish(dfocus, dparent);
            }
        }
    }
}

//================================================================
// 辅助函数

void tagDragMeta::OnRender(suic::Drawing * drawing)
{
    if (index < 9)
    {
        drawing->FillRect(suic::DrawCtx::DefDraw, flColor, rect.TofRect());
    }
    else
    {
        drawing->DrawRect(suic::DrawCtx::DefDraw, flBrush, NULL, rect.TofRect());
    }
}

tagDragMeta::tagDragMeta(int idx, suic::OCursor* cur)
{
    flColor = ARGB(255,255,0,0);
    cursor = cur;
    enable = true;
    dragdot = true;
    index = idx;
    flBrush = NULL;
}

bool DesignPanel::IsMain() const
{
    return (_root && _root->GetRootElement() == _root->GetFocusElement());
}

suic::Rect DesignPanel::CalcElementRect(DesignElement* elem)
{
    Point pt = PointFromScreen(Point());
    suic::Rect rect = suic::GetRenderBound(elem->GetUIElement());

    rect.Offset(pt);

    return rect;
}

DragMeta* DesignPanel::HittestDragMeta(suic::Point pt)
{
    // 现转换鼠标坐标
    pt = PointFromScreen(pt);
    
    int iCount = _dragMetaCount;

    if (IsMain())
    {
        iCount -= 1;
    }

    for (int i = 1; i < iCount; ++i)
    {
        if (_dragMetas[i].rect.PointIn(pt))
        {
            return &_dragMetas[i];
        }
    }

    if (_dragMetas[0].rect.PointIn(pt))
    {
        return &_dragMetas[0];
    }

    return NULL;
}

void DesignPanel::InitDragMeta(suic::Rect rect)
{
    static const int DRAGSIZE = 6;
    double radio = 1;

    //Point point(PointToScreen(Point()));

    //rect.Offset(-point.x, -point.y);

    _dragMetas[0].rect = rect;
    rect.Inflate(DRAGSIZE / radio, DRAGSIZE / radio);

    _dragMetas[1].rect = rect;
    _dragMetas[1].rect.top += (rect.Height() - DRAGSIZE) / 2;
    _dragMetas[1].rect.bottom = _dragMetas[1].rect.top + DRAGSIZE;
    _dragMetas[1].rect.right = _dragMetas[1].rect.left + DRAGSIZE;

    suic::Rect rrr = _dragMetas[1].rect;

    _dragMetas[2].rect = rect;
    _dragMetas[2].rect.bottom = rect.top + DRAGSIZE;
    _dragMetas[2].rect.right = rect.left + DRAGSIZE;

    rrr = _dragMetas[2].rect;

    _dragMetas[3].rect = rect;
    _dragMetas[3].rect.left += (rect.Width() - DRAGSIZE) / 2;
    _dragMetas[3].rect.right = _dragMetas[3].rect.left + DRAGSIZE;
    _dragMetas[3].rect.bottom = rect.top + DRAGSIZE;

    _dragMetas[4].rect = rect;
    _dragMetas[4].rect.left = rect.right - DRAGSIZE;
    _dragMetas[4].rect.bottom = _dragMetas[4].rect.top + DRAGSIZE;

    // Right
    _dragMetas[5].rect = rect;
    _dragMetas[5].rect.top += (rect.Height() - DRAGSIZE) / 2;
    _dragMetas[5].rect.bottom = _dragMetas[5].rect.top + DRAGSIZE;
    _dragMetas[5].rect.left = rect.right - DRAGSIZE;

    // RightBottom
    _dragMetas[6].rect = rect;
    _dragMetas[6].rect.top = rect.bottom - DRAGSIZE;
    _dragMetas[6].rect.left = rect.right - DRAGSIZE;

    // Bottom
    _dragMetas[7].rect = rect;
    _dragMetas[7].rect.left += (rect.Width() - DRAGSIZE) / 2;
    _dragMetas[7].rect.right = _dragMetas[7].rect.left + DRAGSIZE;
    _dragMetas[7].rect.top = rect.bottom - DRAGSIZE;

    // LeftBottom
    _dragMetas[8].rect = rect;
    _dragMetas[8].rect.right = rect.left + DRAGSIZE;
    _dragMetas[8].rect.top = rect.bottom - DRAGSIZE;

    // DragMode
    _dragMetas[9].rect.right = _dragMetas[2].rect.left;
    _dragMetas[9].rect.bottom = _dragMetas[2].rect.top;
    _dragMetas[9].rect.left = _dragMetas[2].rect.left - 10;
    _dragMetas[9].rect.top = _dragMetas[2].rect.top - 10;
    _dragMetas[9].flColor = suic::Colors::Green;

    _dragMetas[9].flBrush = RTTICast<suic::Brush>(FindRes(_U("drag_move")));

    bool isMain = IsMain();

    _dragMetas[1].enable = !isMain;
    _dragMetas[2].enable = !isMain;
    _dragMetas[3].enable = !isMain;
    _dragMetas[4].enable = !isMain;
    _dragMetas[8].enable = !isMain;
    _dragMetas[9].enable = !isMain;
}

void DesignPanel::HandleDragRect(suic::Rect& rect)
{
    if (rect.left > rect.right)
    {
        int iRight = rect.left;
        rect.left = rect.right;
        rect.right = iRight;;
    }

    if (rect.top > rect.bottom)
    {
        int iBottom = rect.top;
        rect.top = rect.bottom;
        rect.bottom = iBottom;
    }
}

void DesignPanel::InternalHandleDrag(suic::Rect& rect, int* pHori, int* pVert)
{
    // 锁定不能编辑
    if (_root->GetFocusElement()->IsLocked())
    {
        return;
    }

    int x = (pHori ? (_trackDragPt.x - _startDragPt.x) : 0);
    int y = (pVert ? (_trackDragPt.y - _startDragPt.y) : 0);

    rect = _startDragRc;

    if (pHori)
    {
        *pHori += x;
    }

    if (pVert)
    {
        *pVert += y;
    }

    DesignElement* dfocus(_root->GetFocusElement());
    FrameworkElement* ele = CASTTOFE(dfocus->GetUIElement());

    if (!IsMain())
    {
        HandleDragRect(rect);

        suic::Rect rcMgr = ele->GetMargin();
        rcMgr.left = _startDragMgr.left + rect.left - _startDragRc.left;
        rcMgr.top = _startDragMgr.top + rect.top - _startDragRc.top;

        SetElementMargin(dfocus, rcMgr);
    }

    if (pHori)
    {
        SetElementWidth(dfocus, rect.Width());
    }

    if (pVert)
    {
        SetElementHeight(dfocus, rect.Height());
    }

    _needMeasure = true;
    RefleshDesignPanel();
}

void DesignPanel::LinkDragMetaEvent()
{
    _dragMetas[0] = DragMeta(0, OCursor::Hand);
    _dragMetas[0].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragMove);
    _dragMetas[0].dragdot = false;

    _dragMetas[1] = DragMeta(1, OCursor::SizeWe);
    _dragMetas[1].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragLeft);

    _dragMetas[2] = DragMeta(2, OCursor::SizeNwse);
    _dragMetas[2].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragLeftTop);

    _dragMetas[3] = DragMeta(3, OCursor::SizeNs);
    _dragMetas[3].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragTop);

    _dragMetas[4] = DragMeta(4, OCursor::SizeNesw);
    _dragMetas[4].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragRightTop);

    _dragMetas[5] = DragMeta(5, OCursor::SizeWe);
    _dragMetas[5].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragRight);

    _dragMetas[6] = DragMeta(6, OCursor::SizeNwse);
    _dragMetas[6].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragRightBottom);

    _dragMetas[7] = DragMeta(7, OCursor::SizeNs);
    _dragMetas[7].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragBottom);

    _dragMetas[8] = DragMeta(8, OCursor::SizeNesw);
    _dragMetas[8].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragLeftBottom);

    _dragMetas[9] = DragMeta(9, OCursor::Hand);
    _dragMetas[9].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragBox);
}
