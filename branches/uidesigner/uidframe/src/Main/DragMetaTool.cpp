//////////////////////////////////////////////////////////////////////////////
// DragMetaTool.cpp

#include "stdafx.h"
#include "DragMetaTool.h"

void InitDragMeta(suic::Array<DragMeta>& dragMetas, suic::Rect rect, bool isMain)
{
    static const int DRAGSIZE = 6;
    dragMetas[0].rect = rect;
    double radio = 1;

    rect.Inflate(DRAGSIZE / radio, DRAGSIZE / radio);

    dragMetas[1].rect = rect;
    dragMetas[1].rect.top += (rect.Height() - DRAGSIZE) / 2;
    dragMetas[1].rect.bottom = dragMetas[1].rect.top + DRAGSIZE;
    dragMetas[1].rect.right = dragMetas[1].rect.left + DRAGSIZE;

    suic::Rect rrr = dragMetas[1].rect;

    dragMetas[2].rect = rect;
    dragMetas[2].rect.bottom = rect.top + DRAGSIZE;
    dragMetas[2].rect.right = rect.left + DRAGSIZE;

    rrr = dragMetas[2].rect;

    dragMetas[3].rect = rect;
    dragMetas[3].rect.left += (rect.Width() - DRAGSIZE) / 2;
    dragMetas[3].rect.right = dragMetas[3].rect.left + DRAGSIZE;
    dragMetas[3].rect.bottom = rect.top + DRAGSIZE;

    dragMetas[4].rect = rect;
    dragMetas[4].rect.left = rect.right - DRAGSIZE;
    dragMetas[4].rect.bottom = dragMetas[4].rect.top + DRAGSIZE;

    // Right
    dragMetas[5].rect = rect;
    dragMetas[5].rect.top += (rect.Height() - DRAGSIZE) / 2;
    dragMetas[5].rect.bottom = dragMetas[5].rect.top + DRAGSIZE;
    dragMetas[5].rect.left = rect.right - DRAGSIZE;

    // RightBottom
    dragMetas[6].rect = rect;
    dragMetas[6].rect.top = rect.bottom - DRAGSIZE;
    dragMetas[6].rect.left = rect.right - DRAGSIZE;

    // Bottom
    dragMetas[7].rect = rect;
    dragMetas[7].rect.left += (rect.Width() - DRAGSIZE) / 2;
    dragMetas[7].rect.right = dragMetas[7].rect.left + DRAGSIZE;
    dragMetas[7].rect.top = rect.bottom - DRAGSIZE;

    // LeftBottom
    dragMetas[8].rect = rect;
    dragMetas[8].rect.right = rect.left + DRAGSIZE;
    dragMetas[8].rect.top = rect.bottom - DRAGSIZE;

    dragMetas[1].enable = !isMain;
    dragMetas[2].enable = !isMain;
    dragMetas[3].enable = !isMain;
    dragMetas[4].enable = !isMain;
    dragMetas[8].enable = !isMain;
}