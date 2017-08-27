/////////////////////////////////////////////////////////////////////////
// Geometry.cpp

#include <System/Graphics/Geometry.h>
#include <System/Graphics/Drawing.h>

#include <System/Interop/System.h>

#include <Skia/core/SkRRect.h>
#include <Skia/core/SkRegion.h>

#define RECT_TO_SKRECT(rect) SkRect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom)
#define RECT_TO_SKIRECT(rect) SkIRect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom)

namespace suic
{

ImplementRTTIOfClass(Geometry, Resource)
ImplementRTTIOfClass(PathGeometry, Geometry)
ImplementRTTIOfClass(LineGeometry, Geometry)
ImplementRTTIOfClass(RectangleGeometry, Geometry)
ImplementRTTIOfClass(EllipseGeometry, Geometry)

//const Float PathGeometry::lineMarker           = 100001.0f;
//const Float PathGeometry::moveMarker           = 100002.0f;
//const Float PathGeometry::quadMarker           = 100003.0f;
//const Float PathGeometry::cubicMarker          = 100004.0f;
//const Float PathGeometry::closeSubPathMarker   = 100005.0f;

PathGeometry::PathGeometry()
{
}

PathGeometry::PathGeometry(const String& data)
{
    Parse(data);
}

PathGeometry::~PathGeometry()
{
}

PathFigure* PathGeometry::GetPathFigure()
{
    return &_path;
}

void PathGeometry::Reset()
{
    _path.Reset();
}

void PathGeometry::Close()
{
    _path.Close();
}

bool PathGeometry::Contains(Point pt)
{
    return _path.Contains(pt);
}

void PathGeometry::MoveTo(Float x, Float y)
{
    _path.MoveTo(x, y);
}

void PathGeometry::LineTo(Float x, Float y)
{
    _path.LineTo(x, y);
}

void PathGeometry::AddOval(const fRect& oval, PathDirection dir)
{
    _path.AddOval(oval, dir);
}

void PathGeometry::AddCircle(Float x, Float y, Float radius, PathDirection dir)
{
    _path.AddCircle(x, y, radius, dir);
}

void PathGeometry::AddArc(const fRect& oval, Float startAngle, Float sweepAngle)
{
    _path.AddArc(oval, startAngle, sweepAngle);
}

void PathGeometry::AddArc(Float left, Float top, Float right, Float bottom, Float startAngle, Float sweepAngle)
{
    _path.AddArc(left, top, right, bottom, startAngle, sweepAngle);
}

void PathGeometry::AddArcTo(const fPoint p1, const fPoint p2, Float radius)
{
    _path.AddArcTo(p1, p2, radius);
}

void PathGeometry::AddRRectii(const fRRect& rect, PathDirection dir)
{
    _path.AddRRectii(rect, dir);
}

void PathGeometry::AddRRect(const fRect& rect, Float rx, Float ry, PathDirection dir)
{
    _path.AddRRect(rect, rx, ry, dir);
}

void PathGeometry::AddRect(const fRect& rect, PathDirection dir)
{
    _path.AddRect(rect.left, rect.top, rect.right, rect.bottom, dir);
}

void PathGeometry::AddRect(Float left, Float top, Float right, Float bottom, PathDirection dir)
{
    _path.AddRect(left, top, right, bottom, dir);
}

void PathGeometry::AddPath(const PathGeometry& src, Float dx, Float dy, AddPathMethod mode)
{
    _path.AddPath(src._path, dx, dy, mode);
}

void PathGeometry::Offset(Float dx, Float dy)
{
    _path.Offset(dx, dy);
}

void PathGeometry::Offset(Float dx, Float dy, PathGeometry* dst) const
{
    _path.Offset(dx, dy, dst->_path);
}

void PathGeometry::TransformPath(const Matrix& matrix)
{
    _path.TransformPath(matrix);
}

void PathGeometry::TransformPath(const Matrix& matrix, PathGeometry* dst) const
{
    _path.TransformPath(matrix, dst->_path);
}

bool PathGeometry::IsEmpty()
{
    return _path.IsEmpty();
}

fRect PathGeometry::GetBounds()
{
    return _path.GetBounds();
}

PathGeometry* PathGeometry::GetAsPathGeometry()
{
    ref();
    return this;
}

bool PathGeometry::Parse(const String& data)
{
    return _path.Parse(data);
}

//----------------------------------------------------------------------
// LineGeometry

LineGeometry::LineGeometry()
{
}

LineGeometry::LineGeometry(fPoint startPoint, fPoint endPoint)
    : _startPoint(startPoint)
    , _endPoint(endPoint)
{
}

LineGeometry::~LineGeometry()
{

}

PathGeometry* LineGeometry::GetAsPathGeometry()
{
    PathGeometry* pathClip = new PathGeometry();

    pathClip->ref();
    pathClip->MoveTo(GetStartPoint().x, GetStartPoint().y);
    pathClip->LineTo(GetEndPoint().x, GetEndPoint().y);

    return pathClip;
}

//void LineGeometry::Draw(Drawing* drawing, Brush* brush, Pen* pen)
//{
//    drawing->DrawLine(pen, GetStartPoint(), GetStartPoint());
//}

bool LineGeometry::Contains(Point pt)
{
    SkPath path;

    path.moveTo(*((SkPoint*)&_startPoint));
    path.lineTo(*((SkPoint*)&_endPoint));

    return path.contains(pt.x, pt.y);
}

fRect LineGeometry::GetBounds()
{
    fRect rect;
    return rect;
}

fPoint LineGeometry::GetStartPoint()
{
    return _startPoint;
}
void LineGeometry::SetStartPoint(fPoint pt)
{
    _startPoint = pt;
}

fPoint LineGeometry::GetEndPoint()
{
    return _endPoint;
}
void LineGeometry::SetEndPoint(fPoint pt)
{
    _endPoint = pt;
}

bool LineGeometry::Parse(const String& data)
{
    return false;
}

//----------------------------------------------------------------------
// RectangleGeometry

RectangleGeometry::RectangleGeometry()
{

}

RectangleGeometry::RectangleGeometry(fRect rect, Float radiusX, Float radiusY)
    : _rect(rect)
    , _radiusX(radiusX)
    , _radiusY(radiusY)
{

}

RectangleGeometry::~RectangleGeometry()
{

}

PathGeometry* RectangleGeometry::GetAsPathGeometry()
{
    PathGeometry* pathClip = new PathGeometry();

    pathClip->ref();
    pathClip->AddRRect(GetRect(), GetRadiusX(), GetRadiusY());

    return pathClip;
}

//void RectangleGeometry::Draw(Drawing* drawing, Brush* brush, Pen* pen)
//{
//    drawing->DrawRoundRect(brush, pen, GetRect(), GetRadiusX(), GetRadiusY());
//}

fRect RectangleGeometry::GetBounds()
{
    return _rect;
}

fRect RectangleGeometry::GetRect()
{
    return _rect;
}

void RectangleGeometry::SetRect(fRect rect)
{
    _rect = rect;
}

Float RectangleGeometry::GetRadiusX()
{
    return _radiusX;
}

void RectangleGeometry::SetRadiusX(Float val)
{
    _radiusX = val;
}

Float RectangleGeometry::GetRadiusY()
{
    return _radiusY;
}

void RectangleGeometry::SetRadiusY(Float val)
{
    _radiusY = val;
}

bool RectangleGeometry::Parse(const String& data)
{
    return false;
}

//----------------------------------------------------------------------
// EllipseGeometry

EllipseGeometry::EllipseGeometry()
{

}

EllipseGeometry::EllipseGeometry(fRect rect)
{
    _radiusX = rect.Width() / 2;
    _radiusY = rect.Height() / 2;
    _center.x = rect.left + _radiusX;
    _center.y = rect.top + _radiusY;
}

EllipseGeometry::EllipseGeometry(fPoint center, Float radiusX, Float radiusY)
    : _center(center)
    , _radiusX(radiusX)
    , _radiusY(radiusY)
{

}

EllipseGeometry::~EllipseGeometry()
{

}

void EllipseGeometry::GetRect(fRect& rect)
{
    rect.left = GetCenter().x - GetRadiusX();
    rect.top = GetCenter().y - GetRadiusY();
    rect.right = GetCenter().x + GetRadiusX();
    rect.bottom = GetCenter().y + GetRadiusY();
}

PathGeometry* EllipseGeometry::GetAsPathGeometry()
{
    PathGeometry* pathClip = new PathGeometry();
    fRect rect;

    GetRect(rect);
    pathClip->ref();
    pathClip->AddOval(rect);

    return pathClip;
}

//void EllipseGeometry::Draw(Drawing* drawing, Brush* brush, Pen* pen)
//{
//    fRect rect;
//   
//    GetRect(rect);
//    drawing->DrawEllipse(brush, pen, &rect);
//}

fRect EllipseGeometry::GetBounds()
{
    fRect rect;
    GetRect(rect);
    return rect;
}

bool EllipseGeometry::Contains(Point pt)
{
    SkPath path;
    fRect rect;

    GetRect(rect);
    path.addOval(*((SkRect*)&rect));
    return path.contains(pt.x, pt.y);
}

fPoint EllipseGeometry::GetCenter()
{
    return _center;
}

void EllipseGeometry::SetCenter(fPoint pt)
{
    _center = pt;
}

Float EllipseGeometry::GetRadiusX()
{
    return _radiusX;
}

void EllipseGeometry::SetRadiusX(Float val)
{
    _radiusX = val;
}

Float EllipseGeometry::GetRadiusY()
{
    return _radiusY;
}

void EllipseGeometry::SetRadiusY(Float val)
{
    _radiusY = val;
}

bool EllipseGeometry::Parse(const String& data)
{
    return false;
}

//----------------------------------------------------------------------
//
Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

bool Geometry::IsEmpty()
{
    return true;
}

fRect Geometry::GetBounds()
{
    return fRect();
}

bool Geometry::Contains(Point pt)
{
    return GetBounds().ToRect().PointIn(pt);
}

bool Geometry::IsEqual(Geometry* Other)
{
    return false;
}

PathGeometry* Geometry::GetAsPathGeometry()
{
    return NULL;
}

}
