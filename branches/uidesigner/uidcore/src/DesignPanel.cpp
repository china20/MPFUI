//////////////////////////////////////////////////////////////////////////////
// DesignPanel.cpp

#include "stdafx.h"
#include "DesignPanel.h"

bool InitUIDCore(suic::Element* main)
{
    suic::Element* pParent(FindElem<suic::Element>(main, _U("DesignViewer")));
    if (pParent)
    {
        DesignPanel* pElem = new DesignPanel();
        pParent->AddChild(pElem);
        
        pElem->SetHoriAlign(HoriAlign::Stretch);
        pElem->SetVertAlign(VertAlign::Stretch);

        return true;
    }
    else
    {
        return false;
    }
}

DesignPanel::DesignPanel()
    : _dragMeta(NULL)
    , _root(NULL)
{
    _dragMeta = NULL;

    // 初始化拖动数据
    _dragMetas.resize(10);

    LinkDragMetaEvent();
}

DesignPanel::~DesignPanel()
{
}

void DesignPanel::OnInitialized()
{

}

void DesignPanel::OnLoaded(suic::LoadedEventArg& e)
{

}

void DesignPanel::OnRender(suic::Drawing* drawing)
{
    //return;
    const int RECSIZE = 8;
    suic::Rect rect(0, 0, GetRenderSize().cx, GetRenderSize().cy);
    int iBeg = RECSIZE;
    suic::Color clrs[2] = {0xff595959, 0xff555555};

    drawing->FillRect(rect, 0xff696969);

    /*for (int y = 0; y < rect.bottom;)
    {
        for (int x = iBeg; x < rect.right;)
        {
            suic::Rect rect(x, y, RECSIZE, RECSIZE);
            drawing->FillRect(rect, clrs[1]);
            x += 16;
        }

        y += RECSIZE;
        iBeg = iBeg == 0 ? RECSIZE : 0;
    }*/
}

void DesignPanel::OnMouseMove(suic::MouseEventArg& e)
{

}

void DesignPanel::OnMouseLeftButtonDown(suic::MouseEventArg& e)
{
    SetCaptureMouse();
    SetFocus();
}

void DesignPanel::OnMouseLeftButtonUp(suic::MouseEventArg& e)
{
    ReleaseCaptureMouse();
    e.SetHandled(true);
}

void DesignPanel::OnMouseRightButtonDown(suic::MouseEventArg& e)
{

}

void DesignPanel::OnTextInput(suic::KeyboardEventArg& e)
{

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

void DesignPanel::OnDragMove(DragMeta& meta, int iAction)
{
}

//================================================================
// 辅助函数

void tagDragMeta::OnRender(suic::Drawing * drawing)
{
    drawing->FillRect(rect, flColor);
}

tagDragMeta::tagDragMeta(suic::Int32 val)
{
    flColor = ARGB(255,255,0,0);
    cursor = val;
    enable = true;
    dragdot = true;
}

bool DesignPanel::IsMain() const
{
    return false;
}

DragMeta* DesignPanel::HittestDragMeta(suic::Point pt)
{
    // 现转换鼠标坐标
    pt = PointFromScreen(pt);

    for (int i = 1; i < (int)_dragMetas.size(); ++i)
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
        rect.bottom = iBottom;;
    }
}

void DesignPanel::InternalHandleDrag(suic::Rect& rect, int* pNori, int* pVert)
{
    int x = (pNori ? (_trackDragPt.x - _startDragPt.x) : 0);
    int y = (pVert ? (_trackDragPt.y - _startDragPt.y) : 0);

    rect = _startDragRc;

    if (pNori)
    {
        *pNori += x;
    }

    if (pVert)
    {
        *pVert += y;
    }

    if (!IsMain())
    {
        HandleDragRect(rect);
    }

    GetParent()->UpdateLayout();
}

void DesignPanel::LinkDragMetaEvent()
{
    _dragMetas[0] = DragMeta(32649);
    _dragMetas[0].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragMove);
    _dragMetas[0].dragdot = false;

    _dragMetas[1] = DragMeta(32644);
    _dragMetas[1].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragLeft);

    _dragMetas[2] = DragMeta(32642);
    _dragMetas[2].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragLeftTop);

    _dragMetas[3] = DragMeta(32645);
    _dragMetas[3].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragTop);

    _dragMetas[4] = DragMeta(32643);
    _dragMetas[4].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragRightTop);

    _dragMetas[5] = DragMeta(32644);
    _dragMetas[5].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragRight);

    _dragMetas[6] = DragMeta(32642);
    _dragMetas[6].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragRightBottom);

    _dragMetas[7] = DragMeta(32645);
    _dragMetas[7].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragBottom);

    _dragMetas[8] = DragMeta(32643);
    _dragMetas[8].DragMetaAction = DragMetaEventHandler(this, &DesignPanel::OnDragLeftBottom);
}
