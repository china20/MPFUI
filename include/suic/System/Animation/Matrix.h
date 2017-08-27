// 华勤科技版权所有 2010-2022
// 
// 文件名：Matrix.h
// 功  能：矩阵转换
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIMATRIX_H_
#define _UIMATRIX_H_

#include <System/Tools/Array.h>
#include <System/Windows/DpObject.h>

namespace suic
{

struct MatrixInfo;

class SUICORE_API Matrix
{
public:

    enum TypeMask 
    {
        tIdentity_Mask = 0,
        tTranslate_Mask = 0x01,
        tScale_Mask = 0x02,
        tAffine_Mask = 0x04, 
        tPerspective_Mask = 0x08,
    };

    Matrix();
    ~Matrix();

    MatrixInfo* GetMatrixInfo() const;

    float GetScaleX() const;
    float GetScaleY() const;
    float GetSkewY() const;
    float GetSkewX() const;
    float GetTranslateX() const;
    float GetTranslateY() const;
    float GetPerspX() const;
    float GetPerspY() const;

    void SetScaleX(float v);
    void SetScaleY(float v);
    void SetSkewX(float v);
    void SetSkewY(float v);
    void SetTranslateX(float v);
    void SetTranslateY(float v);
    void SetPerspX(float v);
    void SetPerspY(float v);

    void SetMatrix(const Matrix& m);
    void SetValue(int index, float value);

    void Reset();
    void SetIdentity() { Reset(); }

    void SetTranslate(float dx, float dy);
    void SetRotate(float degrees);
    void SetRotate(float px, float py, float degrees);
    void SetScale(float sx, float sy);
    void SetScale(float sx, float sy, float px, float py);
    void SetSkew(float kx, float ky);
    void SetSkew(float kx, float ky, float px, float py);

    void SetSinCos(float sinValue, float cosValue);
    void SetSinCos(float sinValue, float cosValue, float px, float py);

    void SetConcat(const Matrix& a, const Matrix& b);

    void PreTranslate(float dx, float dy);
    void PreScale(float sx, float sy, float px, float py);
    void PreScale(float sx, float sy);
    void PreRotate(float px, float py, float degrees);
    void PreRotate(float degrees);
    void PreSkew(float kx, float ky, float px, float py);
    void PreSkew(float kx, float ky);
    void PreConcat(const Matrix& other);

    void PostTranslate(float dx, float dy);
    void PostScale(float sx, float sy, float px, float py);
    void PostScale(float sx, float sy);

    void PostRotate(float px, float py, float degrees);
    void PostRotate(float degrees);
    void PostSkew(float kx, float ky, float px, float py);
    void PostSkew(float kx, float ky);
    void PostConcat(const Matrix& other);

    void ReadFrom(float* data, int size);

    float GetValue(int index) const;

    TypeMask GetType() const;
    bool IsIdentity() const;

    fRect TransformRect(fRect rect) const;
    fPoint TransformPoint(fPoint inPoint) const;

    friend bool operator==(const Matrix& a, const Matrix& b);
    friend bool operator!=(const Matrix& a, const Matrix& b) { return !(a == b); }

private:

    MatrixInfo* _info;
};

}

#endif
