
#ifndef _DESIGNPANEL_H_
#define _DESIGNPANEL_H_

#include "DElement.h"
#include "DragMetaTool.h"
#include <sui/ScrollPresenter.h>

class DesignPanel : public ui::ScrollPresenter
{
public:

    DesignPanel();
    virtual ~DesignPanel();

    void OnInitialized();

    void OnLoaded(suic::LoadedEventArg& e);
    void OnRender(suic::Drawing* drawing);

    void OnMouseMove(suic::MouseEventArg& e);

    void OnMouseLeftButtonDown(suic::MouseEventArg& e);
    void OnMouseLeftButtonUp(suic::MouseEventArg& e);

    void OnMouseRightButtonDown(suic::MouseEventArg& e);

    void OnTextInput(suic::KeyboardEventArg& e);

public:

    // 拖动左边
    void OnDragLeft(DragMeta& meta, int iAction);

    // 拖动左上角
    void OnDragLeftTop(DragMeta& meta, int iAction);

    // 拖动上边
    void OnDragTop(DragMeta& meta, int iAction);

    // 拖动右上角
    void OnDragRightTop(DragMeta& meta, int iAction);

    // 拖动右边
    void OnDragRight(DragMeta& meta, int iAction);

    // 拖动右下角
    void OnDragRightBottom(DragMeta& meta, int iAction);

    // 拖动下边
    void OnDragBottom(DragMeta& meta, int iAction);

    // 拖动左下角
    void OnDragLeftBottom(DragMeta& meta, int iAction);

    // 移动
    void OnDragMove(DragMeta& meta, int iAction);

protected:

    bool IsMain() const;
    void LinkDragMetaEvent();
    DragMeta* HittestDragMeta(suic::Point pt);
    void HandleDragRect(suic::Rect& rect);
    void InternalHandleDrag(suic::Rect& rect, int* pNori, int* pVert);

protected:

    suic::Vector<DragMeta> _dragMetas;

    // 当前处理的拖动点(鼠标压下有效)
    DragMeta* _dragMeta;

    // 当前设计的根元素
    DElement* _root;

    // 鼠标在上面的元素
    suic::Rect _mouseOverRc;
    // 开始拖动时的点
    suic::Point _startDragPt;
    // 开始拖动时元素区域大小
    suic::Rect _startDragRc;
    suic::Rect _startDragMgr;

    // 当前鼠标点
    suic::Point _trackDragPt;
};

bool InitUIDCore(suic::Element* main);

#endif
