// 华勤科技版权所有 2010-2011
// 
// 文件名：ImageEditPanel.h
// 功  能：绑定编辑界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _IMAGEEDITPANEL_H_
# define _IMAGEEDITPANEL_H_

#include <Main/FilterNode.h>
#include <Main/SlnTreeManager.h>
#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>

using namespace suic;

class DImageGrid : public suic::Grid
{
public:

    RTTIOfClass(DImageGrid)

    DImageGrid();
    ~DImageGrid();

protected:

    void OnRender(Drawing* drawing);
};

enum ImageDragMode
{
    idmNone,
    idmImgLeft,
    idmImgTop,
    idmImgRight,
    idmImgBottom,

    idmGapLeft,
    idmGapTop,
    idmGapRight,
    idmGapBottom,
};

class ImageEditPanel : public suic::Control
{
public:

    RTTIOfClass(ImageEditPanel)

    ImageEditPanel();
    ~ImageEditPanel();

    void SetEditBitmap(suic::ImageBrush* bmp);
    suic::ImageBrush* GetEditBitmap() const;

    void Commit();
    void Restore();

    suic::fRect UpdateViewbox(suic::fRect rect, int iMode);
    suic::fRect UpdateViewcorner(suic::fRect rect, int iMode);

    void CoercedViewbox(int iMode);
    void CoercedViewcorner(int iMode);

protected:

    void OnInitialized(EventArg* e);
    void OnRender(Drawing* drawing);

    void OnSetCursor(CursorEventArg* e);
    void OnMouseMove(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);

    void DrawPanelBk(Drawing* drawing);
    void DrawEditImage(Drawing* drawing);
    void DrawMetas(Drawing* drawing);

    void CheckViewbox();
    void CheckViewCorner();

    suic::fRect GetViewbox();
    suic::fRect GetImageDrawRect();

    void CalcDrawRectMeta();

    fRect CalcRealViewbox();

    fPoint ConvertPoint(fPoint pt);

    fPoint GetHoriUp(fRect rect);
    fPoint GetHoriDown(fRect rect);
    fPoint GetVertLeft(fRect rect);
    fPoint GetVertRight(fRect rect);

    fRect GetDrawViewbox();

    PathGeometry* CreatePathGeometry(Point p0, Point p1, Point p2);

    fRect ConvertDrawRect(fRect val);
    fPoint ConvertDrawPoint(fPoint val);
    Float ConvertDrawCoor(Float val);

private:

    // 当前编辑的图像
    suic::ImageBrush* _editBmp;

    // 当前图像缩放比例
    suic::Float _zoomRatio;

    suic::fRect _drawRect;

    bool _dragTracking;
    int _startValue;
    Point _startPoint;

    ImageDragMode _dragMode;

    suic::fRect _cornerRect;
    suic::fRect _viewBox;
    suic::fRect _viewPort;

    // 分别为左上右下
    suic::Rect _drawMetas[4];

    suic::PathGeometry* _horiGap;
    suic::PathGeometry* _vertGap;

    //----------------------------
    suic::fRect _rcViewbox;
    suic::fRect _rcViewCorner;
    suic::Byte _viewboxMode;
};

#endif
