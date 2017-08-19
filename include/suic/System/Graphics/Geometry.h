// 华勤科技版权所有 2010-2012
// 
// 文件名：Geometry.h
// 功  能：边界对象区域。
// 
// 作  者：MPF开发组
// 时  间：2010-05-01
// 
// ======================================================

#ifndef _UIGEOMETRY_H_
#define _UIGEOMETRY_H_

#include <System/Graphics/PathFigure.h>

namespace suic
{

class Drawing;
class PathGeometry;

class SUICORE_API Geometry : public Resource
{
public:

    Geometry();
    ~Geometry();

    RTTIOfAbstractClass(Geometry)

    virtual bool IsEmpty();
    virtual fRect GetBounds();
    virtual bool Contains(Point pt);
    virtual bool IsEqual(Geometry* Other);
    virtual PathGeometry* GetAsPathGeometry();

    virtual bool Parse(const String& data) = 0;
};

class SUICORE_API PathGeometry : public Geometry
{
public:

    PathGeometry();
    PathGeometry(const String& data);
    ~PathGeometry();

    RTTIOfClass(PathGeometry)

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

    void AddPath(const PathGeometry& src, Float dx, Float dy,
                 AddPathMethod mode = AddPathMethod::apmAppend);

    void Offset(Float dx, Float dy);
    void Offset(Float dx, Float dy, PathGeometry* dst) const;

    void TransformPath(const Matrix& matrix);
    void TransformPath(const Matrix& matrix, PathGeometry* dst) const;

    PathFigure* GetPathFigure();

    bool IsEmpty();
    fRect GetBounds();
    bool Contains(Point pt);

    // 此方法调用了ref，外部使用后应该调用unref
    PathGeometry* GetAsPathGeometry();

    bool Parse(const String& data);

private:

    PathFigure _path;
};

class SUICORE_API LineGeometry : public Geometry
{
public:

    LineGeometry();
    LineGeometry(fPoint startPoint, fPoint endPoint);

    ~LineGeometry();

    RTTIOfClass(LineGeometry)

    PathGeometry* GetAsPathGeometry();

    fPoint GetStartPoint();
    void SetStartPoint(fPoint pt);

    fPoint GetEndPoint();
    void SetEndPoint(fPoint pt);

    fRect GetBounds();
    bool Contains(Point pt);

    bool Parse(const String& data);

private:

    fPoint _endPoint;
    fPoint _startPoint;
};

class SUICORE_API RectangleGeometry : public Geometry
{
public:

    RectangleGeometry();
    RectangleGeometry(fRect rect, Float radiusX, Float radiusY);
    ~RectangleGeometry();

    RTTIOfClass(RectangleGeometry)

    PathGeometry* GetAsPathGeometry();

    fRect GetRect();
    void SetRect(fRect rect);

    Float GetRadiusX();
    void SetRadiusX(Float val);

    Float GetRadiusY();
    void SetRadiusY(Float val);

    fRect GetBounds();

    bool Parse(const String& data);

private:

    fRect _rect;
    Float _radiusX;
    Float _radiusY;
};

class SUICORE_API EllipseGeometry : public Geometry
{
public:

    EllipseGeometry();
    EllipseGeometry(fRect rect);
    EllipseGeometry(fPoint center, Float radiusX, Float radiusY);

    ~EllipseGeometry();

    RTTIOfClass(EllipseGeometry)

    PathGeometry* GetAsPathGeometry();

    fPoint GetCenter();
    void SetCenter(fPoint pt);

    Float GetRadiusX();
    void SetRadiusX(Float val);

    Float GetRadiusY();
    void SetRadiusY(Float val);

    fRect GetBounds();
    bool Contains(Point pt);

    bool Parse(const String& data);

private:

    void GetRect(fRect& rect);

private:

    fPoint _center;
    Float _radiusX;
    Float _radiusY;
};

}

#endif
