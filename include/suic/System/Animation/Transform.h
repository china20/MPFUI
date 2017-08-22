// 华勤科技版权所有 2010-2011
// 
// 文件名：Transform.h
// 功  能：矩阵转换
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UITRANSFORM_H_
#define _UITRANSFORM_H_

#include <System/Animation/Matrix.h>

namespace suic
{

class Animatable;
class AnimationClock;
class AnimationTimeline;

typedef delegate<void(Animatable*,DpProperty*)> AnimateNofityHandler;

class SUICORE_API Animatable : public DpObject
{
public:

    AnimateNofityHandler AnimateNofity;

    RTTIOfAbstractClass(Animatable)

    ~Animatable();

    Object* GetAnimationBaseValue(DpProperty* dp);
    void ApplyAnimationClock(DpProperty* dp, AnimationClock* clock);
    void BeginAnimation(DpProperty* dp, AnimationTimeline* animation);

public:

    virtual void PropertyChanged(DpProperty* dp);
};

class SUICORE_API OMatrix : public Object
{
public:

    RTTIOfClass(OMatrix)

    OMatrix();
    OMatrix(const Matrix& m);
    ~OMatrix();

    Matrix* GetMatrix() { return &_matrix; }

private:

    Matrix _matrix;
};

class TransformGroup;

class SUICORE_API Transform : public Animatable
{
public:

    static void OnPropNotifyChanged(DpObject* d, DpPropChangedEventArg* e);
    static bool StaticInit();

    static Transform* GetIdentity();

    RTTIOfAbstractClass(Transform)

    Transform() : _parentGroup(NULL) {}
    virtual ~Transform();

    static Size TransformSize(const Matrix& m, Size size);

    bool IsIdentity();

    void PropertyChanged(DpProperty* dp); 

    virtual void GetMatrix(Matrix& matrix) = 0;
    virtual void ComputeMatrix(Matrix& matrix, fRect rect) = 0;
    
private:

    TransformGroup* _parentGroup;
    static Transform* s_Identity;

    friend class TransformGroup;
};

class SUICORE_API TransformGroup : public Transform, public IAddChild
{
public:

    RTTIOfClass(TransformGroup)

    TransformGroup();
    ~TransformGroup();

    void AddChild(Object* trans);
    void AddText(String text);

    void RemoveChild(Object* child);
    void InsertChild(int pos, Object* child);

    void Add(Transform* trans);
    void RemoveAt(int index);
    void Clear();

    int IndexOf(Transform* trans);
    int GetCount();
    Transform* GetTransform(int index);

    void GetMatrix(Matrix& matrix);
    void ComputeMatrix(Matrix& matrix, fRect rect);

private:

    Matrix* _matrixCache;
    Array<Transform*> _transforms;
};

class SUICORE_API ScaleTransform : public Transform
{
public:

    static DpProperty* CenterXProperty;
    static DpProperty* CenterYProperty;
    static DpProperty* ScaleXProperty;
    static DpProperty* ScaleYProperty;

    RTTIOfClass(ScaleTransform)

    ScaleTransform();
    ScaleTransform(Float scaleX, Float scaleY);
    ScaleTransform(Float scaleX, Float scaleY, Float centerX, Float centerY);
    ~ScaleTransform();

    Float GetCenterX();
    void SetCenterX(Float val);

    Float GetCenterY();
    void SetCenterY(Float val);

    Float GetScaleX();
    void SetScaleX(Float val);

    Float GetScaleY();
    void SetScaleY(Float val);

    void GetMatrix(Matrix& matrix);
    void ComputeMatrix(Matrix& matrix, fRect rect);
};

class SUICORE_API SkewTransform : public Transform
{
public:

    static DpProperty* CenterXProperty;
    static DpProperty* CenterYProperty;
    static DpProperty* AngleXProperty;
    static DpProperty* AngleYProperty;

    RTTIOfClass(SkewTransform)

    SkewTransform();
    SkewTransform(Float angleX, Float angleY);
    SkewTransform(Float angleX, Float angleY, Float centerX, Float centerY);
    ~SkewTransform();

    Float GetCenterX();
    void SetCenterX(Float val);

    Float GetCenterY();
    void SetCenterY(Float val);

    Float GetAngleX();
    void SetAngleX(Float val);

    Float GetAngleY();
    void SetAngleY(Float val);

    void GetMatrix(Matrix& matrix);
    void ComputeMatrix(Matrix& matrix, fRect rect);
};

class SUICORE_API TranslateTransform : public Transform
{
public:

    static DpProperty* XProperty;
    static DpProperty* YProperty;

    RTTIOfClass(TranslateTransform)

    TranslateTransform();
    TranslateTransform(Float x, Float y);
    ~TranslateTransform();

    void GetMatrix(Matrix& matrix);
    void ComputeMatrix(Matrix& matrix, fRect rect);

    Float GetX();
    void SetX(Float val);
    Float GetY();
    void SetY(Float val);
};

class SUICORE_API RotateTransform : public Transform
{
public:

    static DpProperty* AngleProperty;
    static DpProperty* CenterXProperty;
    static DpProperty* CenterYProperty;

    RTTIOfClass(RotateTransform)

    RotateTransform();
    RotateTransform(Float angle);
    RotateTransform(Float angle, Float centerX, Float centerY);

    ~RotateTransform();

    void GetMatrix(Matrix& matrix);
    void ComputeMatrix(Matrix& matrix, fRect rect);

    Float GetCenterX();
    void SetCenterX(Float val);

    Float GetCenterY();
    void SetCenterY(Float val);

    Float GetAngle();
    void SetAngle(Float val);
};

class SUICORE_API Rotate3DTransform : public Transform
{
public:

    static DpProperty* AngleXProperty;
    static DpProperty* AngleYProperty;
    static DpProperty* AngleZProperty;
    static DpProperty* AutoScaleProperty;

    RTTIOfClass(Rotate3DTransform)

    Rotate3DTransform();
    Rotate3DTransform(Float angleY);
    Rotate3DTransform(Float angleX, Float angleY, Float angleZ);

    ~Rotate3DTransform();

    void GetMatrix(Matrix& matrix);
    void ComputeMatrix(Matrix& matrix, fRect rect);

    Float GetAngleX();
    void SetAngleX(Float val);

    Float GetAngleY();
    void SetAngleY(Float val);

    Float GetAngleZ();
    void SetAngleZ(Float val);

    bool GetAutoScale();
    void SetAutoScale(bool val);
};

class SUICORE_API MatrixTransform : public Transform
{
public:

    static DpProperty* MatrixProperty;

    RTTIOfClass(MatrixTransform)

    MatrixTransform();
    MatrixTransform(const Matrix& matrix);
    ~MatrixTransform();

    void GetMatrix(Matrix& matrix);
    void ComputeMatrix(Matrix& matrix, fRect rect);

    Matrix* GetMatrix();
    void SetMatrix(const Matrix& val);
};

}

#endif
