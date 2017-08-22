
#ifndef _DRAGMETATOOL_H_
#define _DRAGMETATOOL_H_

struct tagDragMeta;
typedef tagDragMeta DragMeta;

typedef delegate<void(DragMeta&, int)> DragMetaEventHandler;

struct tagDragMeta
{
    suic::Rect rect;
    suic::Color brColor;
    suic::Color flColor;
    suic::Int32 cursor;
    bool enable;
    bool dragdot;

    DragMetaEventHandler DragMetaAction;

    void OnRender(suic::Drawing * drawing);

    tagDragMeta(suic::Int32 val=0);
};

void InitDragMeta(suic::Vector<DragMeta>& dragMetas, suic::Rect rect, bool isMain);

#endif
