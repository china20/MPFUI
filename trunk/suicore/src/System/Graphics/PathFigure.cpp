/////////////////////////////////////////////////////////////////////////
// PathFigure.cpp

#include <System/Graphics/PathFigure.h>
#include <System/Graphics/Drawing.h>

#include <System/Interop/System.h>

#include <Skia/core/SkRRect.h>
#include <Skia/core/SkRegion.h>

#define RECT_TO_SKRECT(rect) SkRect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom)
#define RECT_TO_SKIRECT(rect) SkIRect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom)

namespace suic
{

PathFigure::PathFigure()
{
    _info = new PathInfo();
}

PathFigure::~PathFigure()
{
    delete _info;
}

PathInfo* PathFigure::GetPathInfo() const
{
    return _info;
}

static SkPath& __path(const PathFigure* path)
{
    return path->GetPathInfo()->path;
}

void PathFigure::Reset()
{
    GetPathInfo()->Reset();
}

void PathFigure::Close()
{
    GetPathInfo()->path.close();
}

bool PathFigure::Contains(Point pt)
{
    return GetPathInfo()->path.contains(pt.x, pt.y);
}

void PathFigure::MoveTo(Float x, Float y)
{
    GetPathInfo()->path.moveTo(x, y);
}

void PathFigure::LineTo(Float x, Float y)
{
    GetPathInfo()->path.lineTo(x, y);
}

void PathFigure::AddOval(const fRect& oval, PathDirection dir)
{
    GetPathInfo()->path.addOval(*((SkRect*)(&oval)), (SkPath::Direction)dir);
}

void PathFigure::AddCircle(Float x, Float y, Float radius, PathDirection dir)
{
    GetPathInfo()->path.addCircle(x, y, radius, (SkPath::Direction)dir);
}

void PathFigure::AddArc(const fRect& oval, Float startAngle, Float sweepAngle)
{
    GetPathInfo()->path.addArc(RECT_TO_SKRECT(oval), startAngle, sweepAngle);
}

void PathFigure::AddArc(Float left, Float top, Float right, Float bottom, Float startAngle, Float sweepAngle)
{
    GetPathInfo()->path.addArc(SkRect::MakeLTRB(left, top, right, bottom), startAngle, sweepAngle);
}

void PathFigure::AddArcTo(const fPoint p1, const fPoint p2, Float radius)
{
    GetPathInfo()->path.arcTo(p1.x, p1.y, p2.x, p2.y, radius);
}

void PathFigure::AddRRectii(const fRRect& rect, PathDirection dir)
{
    SkRRect rrect;
    SkVector raddi[4];

    memcpy(&raddi[0], &rect.radii[0], sizeof(rect.radii));
    rrect.setRectRadii(RECT_TO_SKRECT(rect.rect), raddi);
    GetPathInfo()->path.addRRect(rrect, (SkPath::Direction)dir);
}

void PathFigure::AddRRect(const fRect& rect, Float rx, Float ry, PathDirection dir)
{
    GetPathInfo()->path.addRoundRect(RECT_TO_SKRECT(rect), rx, ry, (SkPath::Direction)dir);
}

void PathFigure::AddRect(const fRect& rect, PathDirection dir)
{
    GetPathInfo()->path.addRect(rect.left, rect.top, rect.right, rect.bottom, (SkPath::Direction)dir);
}

void PathFigure::AddRect(Float left, Float top, Float right, Float bottom, PathDirection dir)
{
    GetPathInfo()->path.addRect(left, top, right, bottom, (SkPath::Direction)dir);
}

void PathFigure::AddPath(const PathFigure& src, Float dx, Float dy, AddPathMethod mode)
{
    GetPathInfo()->path.addPath(src.GetPathInfo()->path, dx, dy, (SkPath::AddPathMode)mode);
}

void PathFigure::Offset(Float dx, Float dy)
{
    GetPathInfo()->path.offset(dx, dy);
}

void PathFigure::Offset(Float dx, Float dy, PathFigure& dst) const
{
    GetPathInfo()->path.offset(dx, dy, &(dst.GetPathInfo()->path));
}

void PathFigure::TransformPath(const Matrix& matrix)
{
    GetPathInfo()->path.transform(matrix.GetMatrixInfo()->m);
}

void PathFigure::TransformPath(const Matrix& matrix, PathFigure& dst) const
{
    GetPathInfo()->path.transform(matrix.GetMatrixInfo()->m, &(dst.GetPathInfo()->path));
}

bool PathFigure::IsEmpty()
{
    return GetPathInfo()->path.isEmpty();
}

fRect PathFigure::GetBounds()
{
    fRect frc;
    const SkRect& skrc = GetPathInfo()->path.getBounds();
    frc.SetLTRB(skrc.left(), skrc.top(), skrc.right(), skrc.bottom());
    return frc;
}

bool PathFigure::Parse(const String& data)
{
    // 上一个坐标点
    fPoint prevPoint;
    // 当前解析坐标点
    fPoint currPoint;

    // 1:M;2:L;3:Z
    int iTag = 0;
    int iOffset = 0;
    int closeNum = 0;
    bool isRelative = false;

    String str = data;

    str.Trim();

    Reset();

    for (int i = 0; i < str.Length(); ++i)
    {
        if (str[i] == 'M' || str[i] == 'm')
        {
            ++closeNum;
            iTag = 1;

            // 相对坐标点
            isRelative = (str[i] == 'm');
            iOffset = SplitPoint(str, i + 1, currPoint);

            if (iOffset < 0)
            {
                Reset();
                return false;
            }

            // 完成一段路径
            if (closeNum > 1)
            {
                Close();
                if (isRelative)
                {
                    currPoint += prevPoint;
                }
            }

            MoveTo(currPoint.x, currPoint.y);

            i = iOffset;
            prevPoint = currPoint;
        }
        // 贝塞尔曲线
        else if (str[i] == 'C' || str[i] == 'c')
        {
            // 少了初始点
            if (closeNum == 0)
            {
                Reset();
                return false;
            }

            fPoint fc1;
            fPoint fc2;
            fPoint fc3;

            // C3.4101491,13.5 0.5,10.589851 0.5,7 
            iOffset = SplitCircle(str, i + 1, fc1, fc2, fc3);

            i = iOffset;

            if (iOffset < 0)
            {
                Reset();
                return false;
            }
        }
        // 直线
        else if (str[i] == 'L' || str[i] == 'l')
        {
            // 少了初始点
            if (closeNum == 0)
            {
                Reset();
                return false;
            }

            // 相对坐标点
            isRelative = (str[i] == 'l');

            iOffset = SplitPoint(str, i + 1, currPoint);
            i = iOffset;

            if (iOffset < 0)
            {
                Reset();
                return false;
            }

            if (isRelative)
            {
                currPoint.x += prevPoint.x;
                currPoint.y += prevPoint.y;
            }

            LineTo(currPoint.x, currPoint.y);
            prevPoint = currPoint;
        }
        else if (str.Length() == i + 1 || str[i] == 'Z' || str[i] == 'z')
        {
            Close();
        }
    }

    return true;
}

int PathFigure::SplitValue(const String& str, int& iStart, String& strVal)
{
    int i = 0;
    int iEnd = -1;

    for (i = iStart; i < str.Length(); ++i)
    {
        if (str[i] != ' ')
        {
            iStart = i;
            break;
        }
    }

    ++i;

    for (; i < str.Length(); ++i)
    {
        int ch = tolower(str[i]);
        //if ((str[i] < '0' || str[i] > '9') && str[i] != ',' && str[i] != ' ')
        //if (str[i] == 'L' || str[i] == 'C' || str[i] == 'Z')
        if (ch >= 'a' && ch <= 'z')
        {
            iEnd = i - 1;
            strVal = str.Substring(iStart, iEnd - iStart + 1);
            strVal.Trim();

            break;
        }
        else if (str.Length() == i + 1)
        {
            iEnd = i;
            strVal = str.Substring(iStart, iEnd - iStart + 1);
            strVal.Trim();
            break;
        }
    }

    return iEnd;
}

int PathFigure::StringSplitToPoint(const String& strVal, fPoint& pt)
{
    StringArray tmp;

    tmp.SplitString(strVal, _U(","));

    if (tmp.Length() != 0)
    {
        return -1;
    }

    pt.x = tmp[0].ToFloat();
    pt.y = tmp[1].ToFloat();

    return 0;
}

int PathFigure::SplitCircle(const String& str, int iStart, fPoint& pt1, fPoint& pt2, fPoint& pt3)
{
    int iEnd = -1;
    String strVal;
    StringArray vec;

    iEnd = SplitValue(str, iStart, strVal);

    if (iEnd < 0)
    {
        return 0;
    }

    vec.SplitString(strVal, _U(" "));

    if (vec.Length() == 3)
    {
        if (StringSplitToPoint(vec[0], pt1) < 0)
        {
            return -1;
        }

        if (StringSplitToPoint(vec[1], pt2) < 0)
        {
            return -1;
        }

        if (StringSplitToPoint(vec[2], pt3) < 0)
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }

    return iEnd;
}

int PathFigure::SplitPoint(const String& str, int iStart, fPoint& pt)
{
    int iEnd = -1;
    String strVal;

    iEnd = SplitValue(str, iStart, strVal);

    if (iEnd < 0)
    {
        return 0;
    }

    iStart = strVal.IndexOf(_U(","), 0);

    if (-1 == iStart)
    {
        iStart = strVal.IndexOf(_U(" "), 0);
    }

    if (-1 == iStart)
    {
        return -1;
    }

    pt.x = strVal.Substring(0, iStart).ToFloat();
    pt.y = strVal.Substring(iStart + 1).ToFloat();

    return iEnd;
}

}
