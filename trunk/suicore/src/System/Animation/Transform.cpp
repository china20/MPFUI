
#include <Skia/utils/skcamera.h>

#include <System/Interop/System.h>
#include <System/Windows/CoreTool.h>
#include <System/Animation/Transform.h>
#include <System/Animation/AnimationStorage.h>

#define SK_ScalarPI 3.14159265f
#define SkDegreesToRadians(degrees) ((degrees) * (SK_ScalarPI / 180))

namespace suic
{

class IdentityTransform : public Transform
{
public:

    void GetMatrix(Matrix& matrix)
    {

    }

    void ComputeMatrix(Matrix& matrix, fRect rect)
    {

    }
};

Transform* Transform::s_Identity = new IdentityTransform();

Animatable::~Animatable()
{

}

OMatrix::~OMatrix()
{

}

Transform::~Transform()
{

}

ScaleTransform::~ScaleTransform()
{

}

SkewTransform::~SkewTransform()
{

}

TranslateTransform::~TranslateTransform()
{

}

RotateTransform::~RotateTransform()
{

}

Rotate3DTransform::~Rotate3DTransform()
{

}

MatrixTransform::~MatrixTransform()
{

}

void Animatable::ApplyAnimationClock(DpProperty* dp, AnimationClock* clock)
{

}

void Animatable::BeginAnimation(DpProperty* dp, AnimationTimeline* animation)
{
    AnimationStorage::BeginAnimation(this, dp, animation);
}

Object* Animatable::GetAnimationBaseValue(DpProperty* dp)
{
    return NULL;
}

void Animatable::PropertyChanged(DpProperty* dp)
{
    if (AnimateNofity.safe_bool())
    {
        AnimateNofity(this, dp);
    }
}

///////////////////////////////////////////////////
// Matrix

Matrix::Matrix()
{
    _info = new MatrixInfo();
}

Matrix::~Matrix()
{
    delete _info;
}

MatrixInfo* Matrix::GetMatrixInfo() const
{
    return _info;
}

void Matrix::SetValue(int index, float value) 
{
    GetMatrixInfo()->m.set(index, value);
}

float Matrix::GetScaleX() const 
{ 
    return GetMatrixInfo()->m.getScaleX(); 
}

float Matrix::GetScaleY() const 
{ 
    return GetMatrixInfo()->m.getScaleY();
}

float Matrix::GetSkewY() const 
{ 
    return GetMatrixInfo()->m.getSkewY();
}

float Matrix::GetSkewX() const 
{ 
    return GetMatrixInfo()->m.getSkewX();
}

float Matrix::GetTranslateX() const 
{ 
    return GetMatrixInfo()->m.getTranslateX();
}

float Matrix::GetTranslateY() const 
{ 
    return GetMatrixInfo()->m.getTranslateY();
}

float Matrix::GetPerspX() const 
{ 
    return GetMatrixInfo()->m.getPerspX();
}

float Matrix::GetPerspY() const 
{ 
    return GetMatrixInfo()->m.getPerspY();
}

void Matrix::SetScaleX(float v) 
{ 
    GetMatrixInfo()->m.setScaleX(v);
}

void Matrix::SetScaleY(float v) 
{ 
    GetMatrixInfo()->m.setScaleY(v);
}

void Matrix::SetSkewX(float v) 
{ 
    GetMatrixInfo()->m.setSkewX(v);
}

void Matrix::SetSkewY(float v) 
{ 
    GetMatrixInfo()->m.setSkewY(v);
}

void Matrix::SetTranslateX(float v) 
{ 
    GetMatrixInfo()->m.setTranslateX(v);
}

void Matrix::SetTranslateY(float v) 
{ 
    GetMatrixInfo()->m.setTranslateY(v);
}

void Matrix::SetPerspX(float v) 
{ 
    GetMatrixInfo()->m.setPerspX(v);
}

void Matrix::SetPerspY(float v) 
{ 
    GetMatrixInfo()->m.setPerspY(v);
}

void Matrix::SetMatrix(const Matrix& m)
{
    SkMatrix& mOther = m.GetMatrixInfo()->m;
    SkScalar fMat[9];
    mOther.writeToMemory(&fMat);
    GetMatrixInfo()->m.readFromMemory(fMat, 9 * sizeof(SkScalar));
}

void Matrix::Reset()
{
    GetMatrixInfo()->m.reset();
}

void Matrix::SetTranslate(float dx, float dy)
{
    GetMatrixInfo()->m.setTranslate(dx, dy);
}

void Matrix::SetRotate(float degrees)
{
    GetMatrixInfo()->m.setRotate(degrees);
}

void Matrix::SetRotate(float px, float py, float degrees)
{
    GetMatrixInfo()->m.setRotate(degrees, px, py);
}

void Matrix::SetScale(float sx, float sy, float px, float py)
{
    GetMatrixInfo()->m.setScale(sx, sy, px, py);
}

void Matrix::SetScale(float sx, float sy)
{
    GetMatrixInfo()->m.setScale(sx, sy);
}

void Matrix::SetSkew(float kx, float ky)
{
    GetMatrixInfo()->m.setSkew(kx, ky);
}

void Matrix::SetSkew(float kx, float ky, float px, float py)
{
    GetMatrixInfo()->m.setSkew(kx, ky, px, py);
}

void Matrix::SetSinCos(float sinV, float cosV)
{
    GetMatrixInfo()->m.setSinCos(sinV, cosV);
}

void Matrix::SetSinCos(float sinV, float cosV, float px, float py)
{
    GetMatrixInfo()->m.setSinCos(sinV, cosV, px, py);
}

void Matrix::SetConcat(const Matrix& a, const Matrix& b)
{
    GetMatrixInfo()->m.setConcat(a.GetMatrixInfo()->m, b.GetMatrixInfo()->m);
}

void Matrix::PreTranslate(float dx, float dy)
{
    GetMatrixInfo()->m.preTranslate(dx, dy);
}

void Matrix::PreScale(float sx, float sy, float px, float py)
{
    GetMatrixInfo()->m.preScale(sx, sy, px, py);
}

void Matrix::PreScale(float sx, float sy)
{
    GetMatrixInfo()->m.preScale(sx, sy);
}

void Matrix::PreRotate(float px, float py, float degrees)
{
    GetMatrixInfo()->m.preRotate(degrees, px, py);
}

void Matrix::PreRotate(float degrees)
{
    GetMatrixInfo()->m.preRotate(degrees);
}

void Matrix::PreSkew(float kx, float ky, float px, float py)
{
    GetMatrixInfo()->m.preSkew(kx, ky, px, py);
}

void Matrix::PreSkew(float kx, float ky)
{
    GetMatrixInfo()->m.preSkew(kx, ky);
}

Matrix::TypeMask Matrix::GetType() const 
{
    return (Matrix::TypeMask)GetMatrixInfo()->m.getType();
}

bool Matrix::IsIdentity() const 
{
    return GetMatrixInfo()->m.isIdentity();
}

void Matrix::PreConcat(const Matrix& other)
{
    GetMatrixInfo()->m.preConcat(other.GetMatrixInfo()->m);
}

void Matrix::PostTranslate(float dx, float dy)
{
    GetMatrixInfo()->m.postTranslate(dx, dy);
}

void Matrix::PostScale(float sx, float sy, float px, float py)
{
    GetMatrixInfo()->m.postScale(sx, sy, px, py);
}

void Matrix::PostScale(float sx, float sy)
{
    GetMatrixInfo()->m.postScale(sx, sy);
}

void Matrix::PostRotate(float px, float py, float degrees)
{
    GetMatrixInfo()->m.postRotate(degrees, px, py);
}

void Matrix::PostRotate(float degrees)
{
    GetMatrixInfo()->m.postRotate(degrees);
}

void Matrix::PostSkew(float kx, float ky, float px, float py)
{
    GetMatrixInfo()->m.postSkew(kx, ky, px, py);
}

void Matrix::PostSkew(float kx, float ky)
{
    GetMatrixInfo()->m.postSkew(kx, ky);
}

void Matrix::PostConcat(const Matrix& other)
{
    GetMatrixInfo()->m.postConcat(other.GetMatrixInfo()->m);
}

void Matrix::ReadFrom(float* data, int size)
{
    GetMatrixInfo()->m.readFromMemory(data, size);
}

float Matrix::GetValue(int index) const
{
    return GetMatrixInfo()->m[index];
}

fRect Matrix::TransformRect(fRect rect) const
{
    SkRect* srcRect = (SkRect*)(&rect);
    GetMatrixInfo()->m.mapRect(srcRect);
    return rect;
}

fPoint Matrix::TransformPoint(fPoint inPoint) const
{
    SkPoint pt;
    GetMatrixInfo()->m.mapXY(inPoint.x, inPoint.y, &pt);
    return fPoint(pt.fX, pt.fY);
}

bool operator==(const Matrix& a, const Matrix& b)
{
    return a.GetMatrixInfo()->m == b.GetMatrixInfo()->m;
}

ImplementRTTIOfClass(Animatable, DpObject)
ImplementRTTIOfClass(OMatrix, Object)

ImplementRTTIOfClass(Transform, Animatable)
ImplementRTTIOfClass(TransformGroup, Transform)
ImplementRTTIOfClass(ScaleTransform, Transform)
ImplementRTTIOfClass(SkewTransform, Transform)
ImplementRTTIOfClass(TranslateTransform, Transform)
ImplementRTTIOfClass(RotateTransform, Transform)
ImplementRTTIOfClass(Rotate3DTransform, Transform)
ImplementRTTIOfClass(MatrixTransform, Transform)

DpProperty* TranslateTransform::XProperty;
DpProperty* TranslateTransform::YProperty;

DpProperty* RotateTransform::AngleProperty;
DpProperty* RotateTransform::CenterXProperty;
DpProperty* RotateTransform::CenterYProperty;

DpProperty* MatrixTransform::MatrixProperty;

//=============================================================
//
void Transform::StaticInit()
{
    if (NULL == ScaleTransform::CenterXProperty)
    {
        ScaleTransform::CenterXProperty = DpProperty::Register(_T("CenterX"), ScaleTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(0.5), PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        ScaleTransform::CenterYProperty = DpProperty::Register(_T("CenterY"), ScaleTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(0.5), PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        ScaleTransform::ScaleXProperty = DpProperty::Register(_T("ScaleX"), ScaleTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(1), PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        ScaleTransform::ScaleYProperty = DpProperty::Register(_T("ScaleY"), ScaleTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(1), PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));

        TranslateTransform::XProperty = DpProperty::Register(_T("X"), TranslateTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        TranslateTransform::YProperty = DpProperty::Register(_T("Y"), TranslateTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));

        RotateTransform::AngleProperty = DpProperty::Register(_T("Angle"), RotateTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        RotateTransform::CenterXProperty = DpProperty::Register(_T("CenterX"), RotateTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(0.5), PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        RotateTransform::CenterYProperty = DpProperty::Register(_T("CenterY"), RotateTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(new OFloat(0.5), PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));

        MatrixTransform::MatrixProperty = DpProperty::Register(_T("Matrix"), MatrixTransform::RTTIType(), OMatrix::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));

        SkewTransform::CenterXProperty = DpProperty::Register(_T("CenterX"), SkewTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        SkewTransform::CenterYProperty = DpProperty::Register(_T("CenterY"), SkewTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        SkewTransform::AngleXProperty = DpProperty::Register(_T("AngleX"), SkewTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        SkewTransform::AngleYProperty = DpProperty::Register(_T("AngleY"), SkewTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));

        Rotate3DTransform::AngleXProperty = DpProperty::Register(_T("AngleX"), Rotate3DTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(OFloat::GetZeroFloat(), PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        Rotate3DTransform::AngleYProperty = DpProperty::Register(_T("AngleY"), Rotate3DTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(OFloat::GetZeroFloat(), PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        Rotate3DTransform::AngleZProperty = DpProperty::Register(_T("AngleZ"), Rotate3DTransform::RTTIType(), OFloat::RTTIType(), DpPropMemory::GetPropMeta(OFloat::GetZeroFloat(), PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
        Rotate3DTransform::AutoScaleProperty = DpProperty::Register(_T("AutoScale"), Rotate3DTransform::RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsNone, &Transform::OnPropNotifyChanged));
    }
}

void Transform::OnPropNotifyChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Animatable* elem = DynamicCast<Animatable>(d);
    if (elem != NULL)
    {
        elem->PropertyChanged(e->GetProp());
    }
}

OMatrix::OMatrix()
{
    _matrix.Reset();
}

OMatrix::OMatrix(const Matrix& m)
{
    _matrix.SetMatrix(m);
}

Transform* Transform::GetIdentity()
{
    return s_Identity;
}

void Transform::PropertyChanged(DpProperty* dp)
{
    if (_parentGroup != NULL && _parentGroup != this)
    {
        _parentGroup->PropertyChanged(dp);
    }
    Animatable::PropertyChanged(dp);
}

TransformGroup::TransformGroup()
{
    _matrixCache = NULL;
}

TransformGroup::~TransformGroup()
{
    Clear();
}

void TransformGroup::AddChild(Object* trans)
{
    trans->ref();
    Add(RTTICast<Transform>(trans));
    trans->unref();
}

void TransformGroup::AddText(String text)
{

}

void TransformGroup::Add(Transform* trans)
{
    if (NULL != trans)
    {
        trans->ref();
        _transforms.Add(trans);
        trans->_parentGroup = this;
    }
}

void TransformGroup::RemoveAt(int index)
{
    if (index >= 0 && index < GetCount())
    {
        Transform* trans = _transforms[index];
        if (trans->_parentGroup == this)
        {
            trans->_parentGroup = NULL;
        }
        trans->unref();
        _transforms.RemoveAt(index);
    }
}

void TransformGroup::RemoveChild(Object* child)
{
    Transform* trans = RTTICast<Transform>(child);
    if (NULL != trans)
    {
        RemoveAt(IndexOf(trans));
    }
}

void TransformGroup::Clear()
{
    for (int i = 0; i < _transforms.Length(); ++i)
    {
        if (_transforms[i]->_parentGroup == this)
        {
            _transforms[i]->_parentGroup = NULL;
        }
        _transforms[i]->unref();
    }
    _transforms.Clear();
}

int TransformGroup::IndexOf(Transform* trans)
{
    for (int i = 0; i < _transforms.Length(); ++i)
    {
        if (_transforms[i] == trans)
        {
            return i;
        }
    }
    return -1;
}

int TransformGroup::GetCount()
{
    return _transforms.Length();
}

Transform* TransformGroup::GetTransform(int index)
{
    return _transforms[index];
}

void TransformGroup::GetMatrix(Matrix& matrix)
{
    if (_transforms.Length() == 0)
    {
        return;
    }

    _transforms[0]->GetMatrix(matrix);

    for (int i = 1; i < _transforms.Length(); ++i)
    {
        Matrix m;
        _transforms[i]->GetMatrix(m);
        matrix.PreConcat(m);
    }
}

void TransformGroup::ComputeMatrix(Matrix& matrix, fRect rect)
{
    if (_transforms.Length() == 0)
    {
        return;
    }

    _transforms[0]->ComputeMatrix(matrix, rect);

    for (int i = 1; i < _transforms.Length(); ++i)
    {
        Matrix m;
        _transforms[i]->ComputeMatrix(m, rect);
        matrix.PreConcat(m);
    }
}

DpProperty* ScaleTransform::CenterXProperty;
DpProperty* ScaleTransform::CenterYProperty;
DpProperty* ScaleTransform::ScaleXProperty;
DpProperty* ScaleTransform::ScaleYProperty;

ScaleTransform::ScaleTransform()
{

}

ScaleTransform::ScaleTransform(Float scaleX, Float scaleY)
{
    SetScaleX(scaleX);
    SetScaleY(scaleY);
}

ScaleTransform::ScaleTransform(Float scaleX, Float scaleY, Float centerX, Float centerY)
{
    SetScaleX(scaleX);
    SetScaleY(scaleY);
    SetCenterX(centerX);
    SetCenterY(centerY);
}

Float ScaleTransform::GetCenterX()
{
    return GetValue(CenterXProperty)->ToFloat();
}

void ScaleTransform::SetCenterX(Float val)
{
    SetValue(CenterXProperty, new OFloat(val));
}

Float ScaleTransform::GetCenterY()
{
    return GetValue(CenterYProperty)->ToFloat();
}

void ScaleTransform::SetCenterY(Float val)
{
    SetValue(CenterYProperty, new OFloat(val));
}

Float ScaleTransform::GetScaleX()
{
    return GetValue(ScaleXProperty)->ToFloat();
}

void ScaleTransform::SetScaleX(Float val)
{
    SetValue(ScaleXProperty, new OFloat(val));
}

Float ScaleTransform::GetScaleY()
{
    return GetValue(ScaleYProperty)->ToFloat();
}

void ScaleTransform::SetScaleY(Float val)
{
    SetValue(ScaleYProperty, new OFloat(val));
}

bool Transform::IsIdentity()
{
    return this == Transform::GetIdentity();
}

void ScaleTransform::GetMatrix(Matrix& matrix)
{
    matrix.SetScale(GetScaleX(), GetScaleY(), GetCenterX(), GetCenterY());
}

void ScaleTransform::ComputeMatrix(Matrix& matrix, fRect rect)
{
    GetMatrix(matrix);
}

//=================================================
//

DpProperty* SkewTransform::CenterXProperty;
DpProperty* SkewTransform::CenterYProperty;
DpProperty* SkewTransform::AngleXProperty;
DpProperty* SkewTransform::AngleYProperty;

SkewTransform::SkewTransform()
{

}

SkewTransform::SkewTransform(Float angleX, Float angleY)
{
    SetAngleX(angleX);
    SetAngleY(angleY);
}

SkewTransform::SkewTransform(Float angleX, Float angleY, Float centerX, Float centerY)
{
    SetAngleX(angleX);
    SetAngleY(angleY);
    SetCenterX(centerX);
    SetCenterY(centerY);
}

Float SkewTransform::GetCenterX()
{
    return GetValue(CenterXProperty)->ToFloat();
}

void SkewTransform::SetCenterX(Float val)
{
    SetValue(CenterXProperty, new OFloat(val));
}

Float SkewTransform::GetCenterY()
{
    return GetValue(CenterYProperty)->ToFloat();
}

void SkewTransform::SetCenterY(Float val)
{
    SetValue(CenterYProperty, new OFloat(val));
}

Float SkewTransform::GetAngleX()
{
    return GetValue(AngleXProperty)->ToFloat();
}

void SkewTransform::SetAngleX(Float val)
{
    SetValue(AngleXProperty, new OFloat(val));
}

Float SkewTransform::GetAngleY()
{
    return GetValue(AngleYProperty)->ToFloat();
}

void SkewTransform::SetAngleY(Float val)
{
    SetValue(AngleYProperty, new OFloat(val));
}

void SkewTransform::GetMatrix(Matrix& matrix)
{
    matrix.SetSkew(GetAngleX(), GetAngleY(), GetCenterX(), GetCenterY());
}

void SkewTransform::ComputeMatrix(Matrix& matrix, fRect rect)
{
    GetMatrix(matrix);
}

//=================================================
//

TranslateTransform::TranslateTransform()
{

}

TranslateTransform::TranslateTransform(Float offsetX, Float offsetY)
{
    SetX(offsetX);
    SetY(offsetY);
}

void TranslateTransform::GetMatrix(Matrix& matrix)
{
    matrix.SetTranslate(GetX(), GetY());
}

void TranslateTransform::ComputeMatrix(Matrix& matrix, fRect rect)
{
    GetMatrix(matrix);
}

Float TranslateTransform::GetX()
{
    return GetValue(XProperty)->ToFloat();
}

void TranslateTransform::SetX(Float val)
{
    SetValue(XProperty, new OFloat(val));
}

Float TranslateTransform::GetY()
{
    return GetValue(YProperty)->ToFloat();
}

void TranslateTransform::SetY(Float val)
{
    SetValue(YProperty, new OFloat(val));
}

//=================================================
//

RotateTransform::RotateTransform()
{

}

RotateTransform::RotateTransform(Float angle)
{
    SetAngle(angle);
}

RotateTransform::RotateTransform(Float angle, Float centerX, Float centerY)
{
    SetCenterX(centerX);
    SetCenterY(centerY);
    SetAngle(angle);
}

void RotateTransform::GetMatrix(Matrix& matrix)
{
    matrix.SetRotate(GetCenterX(), GetCenterY(), GetAngle());
}

void RotateTransform::ComputeMatrix(Matrix& matrix, fRect rect)
{
    GetMatrix(matrix);
}

Float RotateTransform::GetCenterX()
{
    return GetValue(CenterXProperty)->ToFloat();
}

void RotateTransform::SetCenterX(Float val)
{
    SetValue(CenterXProperty, new OFloat(val));
}

Float RotateTransform::GetCenterY()
{
    return GetValue(CenterYProperty)->ToFloat();
}

void RotateTransform::SetCenterY(Float val)
{
    SetValue(CenterYProperty, new OFloat(val));
}

Float RotateTransform::GetAngle()
{
    return GetValue(AngleProperty)->ToFloat();
}

void RotateTransform::SetAngle(Float val)
{
    SetValue(AngleProperty, new OFloat(val));
}

//=================================================
//
DpProperty* Rotate3DTransform::AngleXProperty;
DpProperty* Rotate3DTransform::AngleYProperty;
DpProperty* Rotate3DTransform::AngleZProperty;
DpProperty* Rotate3DTransform::AutoScaleProperty;

Rotate3DTransform::Rotate3DTransform()
{

}

Rotate3DTransform::Rotate3DTransform(Float angleY)
{
    SetAngleY(angleY);
}

Rotate3DTransform::Rotate3DTransform(Float angleX, Float angleY, Float angleZ)
{
    SetAngleY(angleX);
    SetAngleY(angleY);
    SetAngleY(angleZ);
}

void Rotate3DTransform::GetMatrix(Matrix& m)
{
    Sk3DView view;

    view.rotateX(GetAngleX());
    view.rotateY(GetAngleY());
    view.rotateZ(GetAngleZ());

    view.getMatrix(&(m.GetMatrixInfo()->m));
}

void Rotate3DTransform::ComputeMatrix(Matrix& m, fRect rect)
{
    Sk3DView view;
    SkMatrix& skm = m.GetMatrixInfo()->m;
    Float fX = GetAngleX();
    Float fY = GetAngleY();

    view.rotateX(fX);
    view.rotateY(fY);
    view.getMatrix(&skm);

    skm.preTranslate(-rect.Width() * 0.5, -rect.Height() * 0.5);
    skm.postTranslate(rect.Width() * 0.5, rect.Height() * 0.5);

    if (GetAutoScale())
    {
        fRect newRect;
        skm.mapRect((SkRect*)&newRect, *((SkRect*)&rect));
        float fx = rect.Width() / Math::Abs(newRect.Width());
        float fy = rect.Height() / Math::Abs(newRect.Height());

        if (fx > fy)
        {
            fx = fy;
        }

        if (fx < 1.0)
        {
            view.getMatrix(&skm);

            skm.preTranslate(-rect.Width() * 0.5, -rect.Height() * 0.5);
            skm.postScale(fx, fx);
            skm.postTranslate(rect.Width() * 0.5, rect.Height() * 0.5);
        }
    }
}

Float Rotate3DTransform::GetAngleX()
{
    return GetValue(AngleXProperty)->ToFloat();
}

void Rotate3DTransform::SetAngleX(Float val)
{
    SetValue(AngleXProperty, new OFloat(val));
}

Float Rotate3DTransform::GetAngleY()
{
    return GetValue(AngleYProperty)->ToFloat();
}

void Rotate3DTransform::SetAngleY(Float val)
{
    SetValue(AngleYProperty, new OFloat(val));
}

Float Rotate3DTransform::GetAngleZ()
{
    return GetValue(AngleZProperty)->ToFloat();
}

void Rotate3DTransform::SetAngleZ(Float val)
{
    SetValue(AngleZProperty, new OFloat(val));
}

bool Rotate3DTransform::GetAutoScale()
{
    return GetValue(AutoScaleProperty)->ToBool();
}

void Rotate3DTransform::SetAutoScale(bool val)
{
    SetValue(AutoScaleProperty, BOOLTOBOOLEAN(val));
}

//=================================================
//

MatrixTransform::MatrixTransform()
{

}

MatrixTransform::MatrixTransform(const Matrix& matrix)
{
    SetMatrix(matrix);
}

void MatrixTransform::GetMatrix(Matrix& matrix)
{
    Matrix* m = GetMatrix();
    if (m != NULL)
    {
        matrix.SetMatrix(*m);
    }
    else
    {
        matrix.Reset();
    }
}

void MatrixTransform::ComputeMatrix(Matrix& matrix, fRect rect)
{
    GetMatrix(matrix);
}

Matrix* MatrixTransform::GetMatrix()
{
    OMatrix* matrix = DynamicCast<OMatrix>(GetValue(MatrixProperty));
    if (NULL != matrix)
    {
        return matrix->GetMatrix();
    }
    else
    {
        return NULL;
    }
}

void MatrixTransform::SetMatrix(const Matrix& val)
{
    SetValue(MatrixProperty, new OMatrix(val));
}

}
