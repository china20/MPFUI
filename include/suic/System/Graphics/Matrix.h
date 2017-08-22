// 华勤科技版权所有 2010-2011
// 
// 文件名：uimatrix.h
// 功  能：定义核心库的三维转换矩阵。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIMATRIX_H_
#define _UIMATRIX_H_

#include <suic/uicoreconfig.h>

class SkString;

namespace suic
{

class SUICORE_API Matrix 
{
public:

    enum TypeMask 
    {
        kIdentity_Mask      = 0,
        kTranslate_Mask     = 0x01,  //!< set if the matrix has translation
        kScale_Mask         = 0x02,  //!< set if the matrix has X or Y scale
        kAffine_Mask        = 0x04,  //!< set if the matrix skews or rotates
        kPerspective_Mask   = 0x08   //!< set if the matrix is in perspective
    };

    enum 
    {
        kMScaleX,
        kMSkewX,
        kMTransX,
        kMSkewY,
        kMScaleY,
        kMTransY,
        kMPersp0,
        kMPersp1,
        kMPersp2
    };

    TypeMask getType() const 
    {
        if (fTypeMask & kUnknown_Mask) 
        {
            fTypeMask = this->computeTypeMask();
        }

        return (TypeMask)(fTypeMask & 0xF);
    }

    bool isIdentity() const 
    {
        return this->getType() == 0;
    }

    bool rectStaysRect() const 
    {
        if (fTypeMask & kUnknown_Mask) 
        {
            fTypeMask = this->computeTypeMask();
        }
        return (fTypeMask & kRectStaysRect_Mask) != 0;
    }

    bool preservesAxisAlignment() const { return this->rectStaysRect(); }

    bool hasPerspective() const 
    {
        return SkToBool(this->getType() & kPerspective_Mask);
    }
 
    double operator[](int index) const 
    {
        Debug::Assert((unsigned)index < 9);
        return fMat[index];
    }
    
    double get(int index) const 
    {
        Debug::Assert((unsigned)index < 9);
        return fMat[index];
    }
    
    double getScaleX() const { return fMat[kMScaleX]; }
    double getScaleY() const { return fMat[kMScaleY]; }
    double getSkewY() const { return fMat[kMSkewY]; }
    double getSkewX() const { return fMat[kMSkewX]; }
    double getTranslateX() const { return fMat[kMTransX]; }
    double getTranslateY() const { return fMat[kMTransY]; }
    double getPerspX() const { return fMat[kMPersp0]; }
    double getPerspY() const { return fMat[kMPersp1]; }

    double& operator[](int index) 
    {
        Debug::Assert((unsigned)index < 9);
        this->setTypeMask(kUnknown_Mask);
        return fMat[index];
    }

    void set(int index, double value) 
    {
        Debug::Assert((unsigned)index < 9);
        fMat[index] = value;
        this->setTypeMask(kUnknown_Mask);
    }

    void setScaleX(double v) { this->set(kMScaleX, v); }
    void setScaleY(double v) { this->set(kMScaleY, v); }
    void setSkewY(double v) { this->set(kMSkewY, v); }
    void setSkewX(double v) { this->set(kMSkewX, v); }
    void setTranslateX(double v) { this->set(kMTransX, v); }
    void setTranslateY(double v) { this->set(kMTransY, v); }
    void setPerspX(double v) { this->set(kMPersp0, v); }
    void setPerspY(double v) { this->set(kMPersp1, v); }

    void setAll(double scaleX, double skewX, double transX,
                double skewY, double scaleY, double transY,
                double persp0, double persp1, double persp2) 
    {
        fMat[kMScaleX] = scaleX;
        fMat[kMSkewX]  = skewX;
        fMat[kMTransX] = transX;
        fMat[kMSkewY]  = skewY;
        fMat[kMScaleY] = scaleY;
        fMat[kMTransY] = transY;
        fMat[kMPersp0] = persp0;
        fMat[kMPersp1] = persp1;
        fMat[kMPersp2] = persp2;
        this->setTypeMask(kUnknown_Mask);
    }
        
    void reset();
    void setIdentity() { this->reset(); }

    void setTranslate(double dx, double dy);

    void setScale(double sx, double sy, double px, double py);
    void setScale(double sx, double sy);

    void setRotate(double degrees, double px, double py);
    void setRotate(double degrees);

    void setSinCos(double sinValue, double cosValue,
                   double px, double py);
    void setSinCos(double sinValue, double cosValue);

    void setSkew(double kx, double ky, double px, double py);
    void setSkew(double kx, double ky);

    bool setConcat(const Matrix& a, const Matrix& b);

    bool preTranslate(double dx, double dy);
    bool preScale(double sx, double sy, double px, double py);
    bool preScale(double sx, double sy);
    bool preRotate(double degrees, double px, double py);
    bool preRotate(double degrees);
    bool preSkew(double kx, double ky, double px, double py);
    bool preSkew(double kx, double ky);
    bool preConcat(const Matrix& other);
    bool postTranslate(double dx, double dy);
    bool postScale(double sx, double sy, double px, double py);
    bool postScale(double sx, double sy);
    bool postIDiv(int divx, int divy);
    bool postRotate(double degrees, double px, double py);
    bool postRotate(double degrees);
    bool postSkew(double kx, double ky, double px, double py);
    bool postSkew(double kx, double ky);
    bool postConcat(const Matrix& other);

    enum ScaleToFit 
    {
        kFill_ScaleToFit,
        kStart_ScaleToFit,
        kCenter_ScaleToFit,
        kEnd_ScaleToFit
    };

    bool setRectToRect(const SkRect& src, const SkRect& dst, ScaleToFit stf); 
    bool setPolyToPoly(const Point src[], const Point dst[], int count);
    bool invert(Matrix* inverse) const;
    bool pdfTransform(double transform[6]) const;

    void mapPoints(Point dst[], const Point src[], int count) const;

    void mapPoints(Point pts[], int count) const 
    {
        this->mapPoints(pts, pts, count);
    }
    
    void mapXY(double x, double y, Point* result) const 
    {
        Debug::Assert(result);
        //this->getMapXYProc()(*this, x, y, result);
    }

    void mapVectors(SkVector dst[], const SkVector src[], int count) const;

    void mapVectors(SkVector vecs[], int count) const 
    {
        this->mapVectors(vecs, vecs, count);
    }

    bool mapRect(SkRect* dst, const SkRect& src) const;

    bool mapRect(SkRect* rect) const 
    {
        return this->mapRect(rect, *rect);
    }

    void mapPointsWithStride(Point pts[], size_t stride, int count) const 
    {
        for (int i = 0; i < count; ++i) 
        {            
            this->mapPoints(pts, pts, 1);
            pts = (Point*)((intptr_t)pts + stride);
        }
    }
    
    double mapRadius(double radius) const;

    bool fixedStepInX(double y, SkFixed* stepX, SkFixed* stepY) const;

    friend bool operator==(const Matrix& a, const Matrix& b) 
    {
        return memcmp(a.fMat, b.fMat, sizeof(a.fMat)) == 0;
    }

    friend bool operator!=(const Matrix& a, const Matrix& b) 
    {
        return memcmp(a.fMat, b.fMat, sizeof(a.fMat)) != 0;
    }

    enum 
    {
        // flatten/unflatten will never return a value larger than this
        kMaxFlattenSize = 9 * sizeof(double) + sizeof(uint32_t)
    };

    // return the number of bytes written, whether or not buffer is null
    uint32_t flatten(void* buffer) const;
    // return the number of bytes read
    uint32_t unflatten(const void* buffer);
    
    void dump() const;
    void toDumpString(SkString*) const;

    double getMaxStretch() const;

    static const Matrix& I();
    static const Matrix& InvalidMatrix();

    void dirtyMatrixTypeCache() 
    {
        this->setTypeMask(kUnknown_Mask);
    }

private:

    enum 
    {
        /** Set if the matrix will map a rectangle to another rectangle. This
            can be true if the matrix is scale-only, or rotates a multiple of
            90 degrees. This bit is not set if the matrix is identity.
             
            This bit will be set on identity matrices
        */
        kRectStaysRect_Mask = 0x10,

        kUnknown_Mask = 0x80,

        kORableMasks =  kTranslate_Mask |
                        kScale_Mask |
                        kAffine_Mask |
                        kPerspective_Mask,

        kAllMasks = kTranslate_Mask |
                    kScale_Mask |
                    kAffine_Mask |
                    kPerspective_Mask |
                    kRectStaysRect_Mask
    };

    double fMat[9];
    mutable Uint8 fTypeMask;

    Uint8 computeTypeMask() const;

    void setTypeMask(int mask) 
    {
        // allow kUnknown or a valid mask
        Debug::Assert(kUnknown_Mask == mask || (mask & kAllMasks) == mask);
        fTypeMask = SkToU8(mask);
    }

    void orTypeMask(int mask) 
    {
        Debug::Assert((mask & kORableMasks) == mask);
        fTypeMask = SkToU8(fTypeMask | mask);
    }

    void clearTypeMask(int mask) 
    {
        // only allow a valid mask
        Debug::Assert((mask & kAllMasks) == mask);
        fTypeMask &= ~mask;
    }
    
    static bool Poly2Proc(const Point[], Matrix*, const Point& scale);
    static bool Poly3Proc(const Point[], Matrix*, const Point& scale);
    static bool Poly4Proc(const Point[], Matrix*, const Point& scale);

    static void Identity_xy(const Matrix&, double, double, Point*);
    static void Trans_xy(const Matrix&, double, double, Point*);
    static void Scale_xy(const Matrix&, double, double, Point*);
    static void ScaleTrans_xy(const Matrix&, double, double, Point*);
    static void Rot_xy(const Matrix&, double, double, Point*);
    static void RotTrans_xy(const Matrix&, double, double, Point*);
    static void Persp_xy(const Matrix&, double, double, Point*);

    static void Identity_pts(const Matrix&, Point[], const Point[], int);
    static void Trans_pts(const Matrix&, Point dst[], const Point[], int);
    static void Scale_pts(const Matrix&, Point dst[], const Point[], int);
    static void ScaleTrans_pts(const Matrix&, Point dst[], const Point[],
                               int count);
    static void Rot_pts(const Matrix&, Point dst[], const Point[], int);
    static void RotTrans_pts(const Matrix&, Point dst[], const Point[],
                             int count);
    static void Persp_pts(const Matrix&, Point dst[], const Point[], int);
};

}

#endif
