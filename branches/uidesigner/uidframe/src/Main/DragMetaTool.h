
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
    suic::OCursor* cursor;
    suic::Brush* flBrush;

    int index;
    bool enable;
    bool dragdot;

    DragMetaEventHandler DragMetaAction;

    void OnRender(suic::Drawing * drawing);

    tagDragMeta(int idx=-1, suic::OCursor* cur=suic::OCursor::Arrow);
};

void InitDragMeta(suic::Array<DragMeta>& dragMetas, suic::Rect rect, bool isMain);

#endif
