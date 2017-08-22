// 华勤科技版权所有 2010-2012
// 
// 文件名：PathFigure.h
// 功  能：边界对象区域。
// 
// 作  者：MPF开发组
// 时  间：2010-05-01
// 
// ======================================================

#ifndef _UIPATHFIGURE_H_
#define _UIPATHFIGURE_H_

#include <System/Animation/Matrix.h>
#include <System/Graphics/FormattedText.h>

namespace suic
{

class Drawing;
struct PathInfo;

enum ClipOp
{
    OpDifference,
    OpIntersect,
    OpUnion,
    OpXOR,
    OpReverseDifference,
    OpReplace,
};

enum AddPathMethod
{
    apmAppend,
    apmExtend,
};

enum PathDirection
{
    pdWhere,
    pdCW,
    pdCCW,
};

class SUICORE_API PathFigure
{
public:

    PathFigure();
    ~PathFigure();

    void Reset();
    void Close();

    void MoveTo(Float x, Float y);
    void LineTo(Float x, Float y);

    void AddOval(const fRect& oval, PathDirection dir = PathDirection::pdCW);
    void AddCircle(Float x, Float y, Float radius, PathDirection dir = PathDirection::pdCW);
    void AddArc(const fRect& oval, Float startAngle, Float sweepAngle);
    void AddArc(Float left, Float top, Float right, Float bottom, Float startAngle, Float sweepAngle);
    void AddArcTo(const fPoint p1, const fPoint p2, Float radius);
    void AddRRectii(const fRRect& rect, PathDirection dir = PathDirection::pdCW);
    void AddRRect(const fRect& rect, Float rx, Float ry, PathDirection dir = PathDirection::pdCW);
    void AddRect(const fRect& rect, PathDirection dir = PathDirection::pdCW);
    void AddRect(Float left, Float top, Float right, Float bottom, PathDirection dir = PathDirection::pdCW);

    void AddPath(const PathFigure& src, Float dx, Float dy,
        AddPathMethod mode = AddPathMethod::apmAppend);

    void Offset(Float dx, Float dy);
    void Offset(Float dx, Float dy, PathFigure& dst) const;

    void TransformPath(const Matrix& matrix);
    void TransformPath(const Matrix& matrix, PathFigure& dst) const;

    PathInfo* GetPathInfo() const;

    bool IsEmpty();
    fRect GetBounds();
    bool Contains(Point pt);

    bool Parse(const String& data);

private:

    int SplitValue(const String& str, int& iStart, String& strVal);
    int StringSplitToPoint(const String& strVal, fPoint& pt);
    int SplitPoint(const String& str, int iStart, fPoint& pt);
    int SplitCircle(const String& str, int iStart, fPoint& pt1, fPoint& pt2, fPoint& pt3);

private:

    PathInfo* _info;
};

}

#endif
