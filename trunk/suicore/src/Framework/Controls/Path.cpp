
#include <Framework/Controls/Path.h>
#include <System/Graphics/SolidcolorBrush.h>
#include <Skia/core/SkPath.h>

namespace suic
{

ImplementRTTIOfClass(Path, Shape)

DpProperty* Path::DataProperty;

void Path::StaticInit()
{
    if (NULL == DataProperty)
    {
        Shape::StaticInit();

        DataProperty = DpProperty::Register(_T("Data"), RTTIType(), PathGeometry::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsParentMeasure, &Path::OnDataPropChanged));
    }
}

void Path::OnDataPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Path* pPath = RTTICast<Path>(d);
    PathGeometry* pg = RTTICast<PathGeometry>(e->GetNewValue());
    pPath->_path = new PathGeometry();

    pPath->_path->ref();

    if (NULL != pg)
    {
        pg->TransformPath(SystemParameters::GetTransform(), pPath->_path);
    }
}

Path::Path()
    : _path(NULL)
{
}

Path::~Path()
{
    if (NULL != _path)
    {
        _path->unref();
    }
}

PathGeometry* Path::GetData()
{
    return _path;
}

void Path::SetData(PathGeometry* data)
{
    SetValue(DataProperty, data);
}

Size Path::OnMeasure(const Size& size)
{
    if (NULL != _path)
    {
        return _path->GetBounds().ToRect().ToSize();
    }
    else
    {
        return Size();
    }
}

void Path::OnRender(Drawing * drawing)
{
    if (NULL != _path)
    {
        Pen pen;
        Brush* stroke = GetStroke();
        Float iStrokeThickness = GetStrokeThickness();

        if (FloatUtil::GreaterThan(iStrokeThickness, 0) && stroke != NULL)
        {
            InitPen(&pen, stroke, iStrokeThickness);
            drawing->DrawPath(DrawCtx::DefDraw, GetFill(), &pen, GetData()->GetPathFigure());
        }
        else
        {
            drawing->DrawPath(DrawCtx::DefDraw, GetFill(), NULL, GetData()->GetPathFigure());
        }
    }
}

}
